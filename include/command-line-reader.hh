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

#ifndef command_line_reader_hh
#define command_line_reader_hh command_line_reader_hh

#include<string>


/*** class command_line_reader ***********************************************/

class command_line_reader{

 private:
    uint16_t command_line_length;
    uint16_t token_size;
    uint8_t capacity; // max number of arguments
    uint8_t noa; // current number of args
    char **token;
    char ch;
    uint8_t offset;
    uint8_t __i;
    uint16_t __j;
    //char command;

	std::string prompt;
	std::string history; // readline history file

 public:

    command_line_reader(uint16_t max_command_line_length,
                        uint8_t max_number_of_args, std::string propmt, std::string history);

    ~command_line_reader();

    inline uint8_t get_number_of_args(){
        return(noa);
    }//get_number_of_args

    bool is_dec(uint8_t i); // refers to i-th argument
    bool is_not_dec(uint8_t i);
    bool is_pos_dec(uint8_t i);
    bool is_not_pos_dec(uint8_t i);

    inline uintmax_t get_num(uint8_t i){
        return strtoull(token[i + offset], NULL, 10);
    }//get_num

    inline const char *get_string(uint8_t i){
        return token[i + offset];
    }//get_string

    void operator>>(char &command);
};

#endif

/* aczutro ************************************************************* end */
