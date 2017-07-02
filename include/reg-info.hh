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

#ifndef reg_info_hh
#define reg_info_hh reg_info_hh

#include <map>
#include <queue>


/*** data types **************************************************************/

struct field_data{
    uint8_t width;
    std::string name;
    field_data *next;

    field_data(): width(0), name(""), next(NULL){}
};


typedef std::map<std::string, field_data*> register_collection;
typedef register_collection::const_iterator register_data;


class reg_info_exception : public std::exception{
private:
    std::string error_message;

public:
    reg_info_exception(){
        error_message = "unknown error cause";
    }//reg_info_exception

    reg_info_exception(const std::string &a){
        error_message = a;
    }//reg_info_exception

    reg_info_exception(std::initializer_list<const char*> a){
        error_message = "";
        for(const char *b : a){
            error_message.append(b);
        }//
    }//reg_info_exception

    const char *what() const noexcept{
        return error_message.c_str();
    }//what
};//reg_info_exception;


/*** class declaration *******************************************************/

class reg_info{

private:
    register_collection *__RD; // register data
    uint8_t __max_number_of_fields;
    std::queue<field_data*> memory_to_free;

    void delete_RD();

public:
    reg_info(const char *filename);

    ~reg_info();

    const register_collection &RD(){
        return *__RD;
    }//RD

    uint8_t max_number_of_fields(){
        return __max_number_of_fields;
    }//max_number_of_fields
};

#endif

/* aczutro ************************************************************* end */
