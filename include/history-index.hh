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

#ifndef history_index_hh
#define history_index_hh history_index_hh

#include <stdint.h>


/*** class declaration *******************************************************/

class history_index{

private:
    uint8_t idx;

    uint8_t normalise(int16_t a);

public:
    static const uint8_t MAX_HISTORY_SIZE = UINT8_MAX;

    /* If size is != 0, update the history size.
     * In any case, returns the current (or updated) history size.
     * size must not be greater than MAX_HISTORY_SIZE (not tested). */
    static uint8_t history_size(uint8_t size=0){
        static uint8_t hist_size = MAX_HISTORY_SIZE;
        if(size){
            hist_size = size;
        }//if
        return hist_size;
    }//history_size

    history_index &operator=(int16_t a);

    inline history_index &operator++(int){ /* argument makes it postfix */
        *this = *this + 1;
        return *this;
    }//operator++

    inline history_index &operator--(int){ /* argument makes it postfix */
        *this = *this - 1;
        return *this;
    }//operator++

    history_index operator+(int16_t a);

    inline history_index operator-(int16_t a){
        return *this + -a;
    }//operator-

    inline bool operator==(history_index &a){
        return (this->idx == a.idx);
    }//operator==

    inline bool operator!=(history_index &a){
        return (this->idx != a.idx);
    }//operator!=

    inline uint8_t operator()(){
        return(idx);
    }//operator()
};

#endif

/* aczutro ************************************************************* end */
