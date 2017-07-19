/* aczutro -*- c-basic-offset:4 -*-
 * vim: set sw=4 ts=4 sts=4 expandtab :
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
#include <limits>

#include <cstring>

#include <exceptions.hh>
#include <command-line-reader.hh>

using namespace std;


/*** class command_line_reader functions *************************************/

command_line_reader::command_line_reader(uint16_t max_command_line_length,
                                         uint8_t max_number_of_args, std::string propmt, std::string history){
    command_line_length = max_command_line_length;
    token_size = command_line_length + 1;
    capacity = max_number_of_args;
    token = new char*[capacity + 1];
    for(__i = 0; __i <= capacity; __i++){
        token[__i] = new char[token_size];
    }//for
    noa = 0;
}//command_line_reader

/*****************************************************************/

command_line_reader::~command_line_reader(){
    for(__i = 0; __i <= capacity; __i++){
        delete[] token[__i];
    }//for
    delete[] token;
}//~command_line_reader

/*****************************************************************/

void command_line_reader::operator>>(char &command){
    for(__i = 0; __i <= capacity; __i++){
        memset(token[__i], 0, token_size);
    }//for
    __i = 0;
    __j = 0;
    while(true){
        cin.get(ch);
        if(cin.eof()){
            throw(exceptions::EOF_COMMAND);
        }//if
        switch(ch){
        case '\n':
            if(__j){
                __i++;
            }//if
            goto out_of_while;
        case ' ':
        case '\t':
            if(__j){
                __i++;
                __j = 0;
            }//if
            break;
        default:
            // only write into token if there is space; otherwise discard ch
            if(__i <= capacity && __j < command_line_length){
                token[__i][__j] = ch;
                __j++;
            }//if
        }//switch
    }//while
 out_of_while:

    if(token[0][0] == 0){
        throw(exceptions::EMPTY_COMMAND);
    }//if

    if(token[0][1] || isxdigit(token[0][0])){ /* self-insert command */
        command = 0;
        offset = 0;
        noa = 1;
    }else{
        command = token[0][0];
        offset = 1;
        noa = __i - 1;
    }//else
}//operator>>

/*****************************************************************/

bool command_line_reader::is_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! (isdigit(*c) || *c == '-')){
            return false;
        }//if
    }//for
    return true;
}//is_num

/*****************************************************************/

bool command_line_reader::is_not_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! (isdigit(*c) || *c == '-')){
            return true;
        }//if
    }//for
    return false;
}//is_not_num

/*****************************************************************/

bool command_line_reader::is_pos_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! isdigit(*c)){
            return false;
        }//if
    }//for
    return true;
}//is_pos_num

/*****************************************************************/

bool command_line_reader::is_not_pos_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! isdigit(*c)){
            return true;
        }//if
    }//for
    return false;
}//is_not_pos_num

/* aczutro ************************************************************* end */
