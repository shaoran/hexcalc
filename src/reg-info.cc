/* aczutro -*- c-basic-offset:4 -*-
 *
 * hexcalc - a handy hex calculator and register contents visualiser
 *           for assembly programmers
 *
 * Copyright 2014 - 2017 Alexander Czutro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licence as published by
 * the Free Software Foundation, either version 3 of the Licence, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public Licence for more details.
 *
 * You should have received a copy of the GNU General Public Licence
 * along with this program.  If not, see <http://www.gnu.org/licences/>.
 *
 ******************************************************************* aczutro */

#include <iostream>
#include <fstream>

#include <cstring>

#include <reg-info.hh>

using namespace std;


/*** macros ************************************************************/

#define BUFFER_SIZE 1000

#define delete_config()  {config->close(); delete config;}

/*** class reg_info functions ******************************************/


reg_info::reg_info(const char *filename){
    ifstream *config = new ifstream(filename);
    if(! config || ! *config){
        throw(reg_info_exception({"cannot open file '", filename, "'"}));
    }//if

    __RD = new register_collection;
    __max_number_of_fields = 0;

    uint8_t num;
    string name;
    char buffer[BUFFER_SIZE];
    char *token;
    field_data *current = NULL;
    field_data *first = NULL;
    uint8_t field_counter = 0;
    bool group_started = false;
    char line[6];
    uint16_t l = 0;
    goto starting_point;
    while(! config->eof()){
        if(buffer[0] && buffer[0] != '#'){ // ignore lines starting with #
            token = strtok(buffer, " \t");
            if((! token) || token[0] == '#'){ // line is empty
                goto starting_point;
            }//if
            for(char *ch = token; *ch; ch++){
                if(! isdigit(*ch)){
                    delete_config();
                    delete_RD();
                    throw(reg_info_exception({filename, ":", line, ": ",
                                    "expected a positive number, but read '",
                                    token, "'"}));
                }//if
            }//for
            num = atoi(token);
            if(num == 0){
                if(group_started){
                    token = strtok(NULL, " \t");
                    if(token && token[0] != '#'){
                        // if there is more on the line and it's not a comment
                        delete_config();
                        delete_RD();
                        throw(reg_info_exception({filename, ":", line, ": ",
                                        "unexpected token '", token, "'"}));
                    }//if
                    // close group
                    if(first->width % 4){
                        delete_config();
                        delete_RD();
                        throw(reg_info_exception({filename, ":", line, ": ",
                                        "total width of register ",
                                        first->name.c_str(),
                                        " (", to_string(first->width).c_str(),
                                        ") is not divisible by 4"}));
                    }//if
                    group_started = false;
                    if(field_counter > __max_number_of_fields){
                        __max_number_of_fields = field_counter;
                    }//if
                }else{
                    token = strtok(NULL, " \t");
                    if((! token) || token[0] == '#'){
                        // if register name is not present
                        delete_config();
                        delete_RD();
                        throw(reg_info_exception({filename, ":", line, ": ",
                                        " expected register name"}));
                    }else{
                        // open group
                        group_started = true;
                        current = new field_data;
                        memory_to_free.push(current);
                        first = current;
                        current->name = token;
                        (*__RD)[token] = current;
                        field_counter = 0;
                    }//else
                    token = strtok(NULL, " \t");
                    if(token && token[0] != '#'){
                        // if there is more on the line and it's not a comment
                        delete_config();
                        delete_RD();
                        throw(reg_info_exception({filename, ":", line, ": ",
                                        " unexpected token '", token, "'"}));
                    }//if
                }//else
            }else{ // num is the width of a bit field
                if(group_started){
                    token = strtok(NULL, " \t");
                    if((! token) || token[0] != '#'){
                        // new field / new unused block
                        current->next = new field_data;
                        memory_to_free.push(current->next);
                        current = current->next;
                        current->width = num;
                        if(token){
                            current->name = token;
                        }//if
                        first->width += num;
                        field_counter++;
                    }else{ // i.e. token[0] == '#'
                        delete_config();
                        delete_RD();
                        throw(reg_info_exception({filename, ":", line, ": ",
                                        " expected field name"}));
                    }//else
                    token = strtok(NULL, " \t");
                    if(token && token[0] != '#'){
                        // if there is more on the line and it's not a comment
                        delete_config();
                        delete_RD();
                        throw(reg_info_exception({filename, ":", line, ": ",
                                        " unexpected token '", token, "'"}));
                    }//if
                }else{
                    // field definition not expected because register name
                    // hasn't been declared yet
                    delete_config();
                    delete_RD();
                    throw(reg_info_exception({filename, ":", line, ": ",
                                    "expected new register declaration starting with 0"}));
                }//else
            }//else
        }//if
    starting_point:
        l++;
        sprintf(line, "%d", l);
        config->getline(buffer, BUFFER_SIZE);
    }//while

    if(group_started){
        delete_config();
        delete_RD();
        throw(reg_info_exception({filename, ": ", "unexpected end of file; ",
                        "last register definition is incomplete"}));
    }//

    delete_config();
}//reg_info

/*****************************************************************/

void reg_info::delete_RD(){
    while(memory_to_free.size()){
        delete memory_to_free.front();
        memory_to_free.pop();
    }//while
    delete __RD;
}//delete_RD


reg_info::~reg_info(){
    delete_RD();
}//~reg_info

/* aczutro ************************************************************* end */
