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

#include <core-state.hh>

using namespace std;


/*** class core_state functions ****************************************/

core_state &core_state::adjust_to_width(){
    if(__width){
        if(__width > __hex.length()){
            __hex.insert(0, string(__width - __hex.length(), '0'));
        }else{
            __hex.erase(0, __hex.length() - __width);
        }//else
    }else{
        index = __hex.find_first_not_of('0');
        if(index == string::npos){ // i.e. a is all-0s
            __hex = "0";
        }else{
            __hex.erase(0, index);
        }//else
    }//else
    return *this;
}//adjust_to_width

/*****************************************************************/

core_state &core_state::operator=(const string &a){
    __hex = a;
    adjust_to_width();
    if(__perm_hilite){
        set_perm_hilite(__perm_hilite_min, __perm_hilite_max);
    }//
    return *this;
}//operator=

/*****************************************************************/

core_state &core_state::set_width(uint8_t a){
    __width = a;
    adjust_to_width();
    if(__perm_hilite){
        set_perm_hilite(__perm_hilite_min, __perm_hilite_max);
    }//
    return *this;
}//set_width

/*****************************************************************/

core_state &core_state::set_perm_hilite(uint8_t lo, uint8_t hi){
    uint16_t bits = number_of_bits();
    if(lo >= bits || hi >= bits){
        __perm_hilite = false;
    }else{
        __perm_hilite = true;
        if(lo < hi){
            __perm_hilite_min = lo;
            __perm_hilite_max = hi;
        }else{
            __perm_hilite_min = hi;
            __perm_hilite_max = lo;
        }//else
    }//else
    return *this;
}//set_perm_hilite

/* aczutro ************************************************************* end */
