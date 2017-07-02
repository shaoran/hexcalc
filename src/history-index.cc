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

#include <history-index.hh>


/*** class history_index functions *************************************/

uint8_t history_index::normalise(int16_t a){
    if(a < 0){
        a = (a % history_size()) + history_size();
    }//if
    return a % history_size();
}//normalise

/*****************************************************************/

history_index &history_index::operator=(int16_t a){
    idx = normalise(a);
    return *this;
}//operator=

/*****************************************************************/

history_index history_index::operator+(int16_t a){
    history_index response;
    return (response = (int16_t)idx + a);
}//operator+

/* aczutro ************************************************************* end */
