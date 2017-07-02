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

#ifndef core_state_hh
#define core_state_hh core_state_hh

#include <stdint.h>
#include <string>


/*** class to hold snapshot of accumulator status ****************************/

class core_state{

private:
    std::string __hex;
    uint8_t __width;
    bool __perm_hilite;
    uint8_t __perm_hilite_min;
    uint8_t __perm_hilite_max;
    size_t index;

public:
    static const uint16_t MAX_NUMBER_OF_BITS = UINT8_MAX + 1;
    static const uint8_t MAX_WIDTH = MAX_NUMBER_OF_BITS / 4;

    bool show_indices;

    inline core_state(){
        __hex.reserve(MAX_WIDTH);
        __hex             = "0";
        __width           = 0;         // 0 is variable number of hex digits
        show_indices      = true;
        __perm_hilite     = false;
        __perm_hilite_min = UINT8_MAX; // invalid on purpose, as perm_hilite is false
        __perm_hilite_max = UINT8_MAX; // invalid on purpose, as perm_hilite is false
    }

    inline const std::string &hex()           {return __hex;}
    inline uint8_t           width()          {return __width;}
    inline uint16_t          number_of_bits() {return __hex.length() * 4;}
    inline bool              perm_hilite()    {return __perm_hilite;}
    inline uint8_t           perm_hilite_min(){return __perm_hilite_min;}
    inline uint8_t           perm_hilite_max(){return __perm_hilite_max;}

    /* MAKES NO SECURITY TESTS, i.e. a MUST be hexadecimal and not wider than
     * WIDTH if width is fixed.  If it is wider, it will be truncated
     * (silently). */
    core_state &operator=(const std::string &a);

    /* MAKES NO SECURITY TESTS, i.e. i MUST be < width(), and a MUST be
     * a hexadecimal digit. */
    inline core_state &replace_nibble(uint8_t i, char a){
        __hex[i] = a;
        return *this;
    }//replace_nibble

    /* MAKES NO SECURITY TESTS, i.e. a MUST be <= MAX_WIDTH;
     * also, if a is too short, the current value is truncated (silently) */
    core_state &set_width(uint8_t a);

    /* Matches width. */
    core_state &adjust_to_width();

    /* if hi/lo are out of range, turns permanent highlighting off (silently) */
    core_state &set_perm_hilite(uint8_t lo, uint8_t hi);

    /* removes permanent highlighting */
    core_state &reset_perm_hilite(){
        __perm_hilite = false;
        return *this;
    }//reset_perm_hilite

    inline void print(){
        printf("%s  wd(%d)", __hex.c_str(), __width);
        if(show_indices){
            printf("  idx");
        }//if
        if(__perm_hilite){
            printf("  hl(%d..%d)", __perm_hilite_max, __perm_hilite_min);
        }//if
    }//print
};

#endif

/* aczutro ************************************************************* end */
