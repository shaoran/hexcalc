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

#ifndef faces_hh
#define faces_hh faces_hh


/*** namespace with string constants for cout ********************************/

namespace faces{

    static const char DEFF[] = "[m";
    static const char BOLD[] = "[1m";
    static const char ULNE[] = "[4m";

    static const char RED[]     = "[31m";
    static const char GREEN[]   = "[32m";
    static const char YELLOW[]  = "[33m";
    static const char BLUE[]    = "[34m";
    static const char MAGENTA[] = "[35m";
    static const char CYAN[]    = "[36m";

}//faces

#endif

/* aczutro ************************************************************* end */
