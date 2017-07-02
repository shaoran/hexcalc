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

#ifndef exceptions_hh
#define exceptions_hh exceptions_hh


/*** namespace with exceptions and error messages ****************************/

namespace exceptions{

    static const char errmsg[][64] = {
        /* a */ "binary string contains illegal characters",
        /* b */ "hexadecimal string contains illegal characters",
        /* c */ "binary string is empty",
        /* d */ "decimal string is empty",
        /* e */ "hexadecimal string is empty",
        /* f */ "decimal string contains illegal characters",
        /* g */ "highlighting limits are out of range",
        /* h */ "value too large for current accumulator width",
        /* i */ "value too large for width of highlighted bit field",
        /* j */ "current version supports only up to width 16 (64 bit)",
        /* k */ "inversion limits are out of range",
        /* l */ "no permanently highlighted bits",
        /* m */ "no more undo history",
        /* n */ "no more redo history",
        /* o */ "history size too small",
        /* p */ "history size too large",
        /* q */ "argument(s) must be positive integer(s)",
        /* r */ "unknown register name",
        /* s */ "requested register's width mismatches current accumulator width",
        /* t */ "",
        /* u */ ""
    };

    enum signal{
        /* a */ BAD_BIN_STRING,
        /* b */ BAD_HEX_STRING,
        /* c */ EMPTY_BIN_STRING,
        /* d */ EMPTY_DEC_STRING,
        /* e */ EMPTY_HEX_STRING,
        /* f */ BAD_DEC_STRING,
        /* g */ BAD_HILITE_LIMITS,
        /* h */ BAD_VALUE_FOR_WIDTH,
        /* i */ BAD_VALUE_FOR_PERM_WIDTH,
        /* j */ UNSUPPORTED_WIDTH,
        /* k */ BAD_INV_LIMITS,
        /* l */ NO_HILITE,
        /* m */ EMPTY_UNDO_HISTORY,
        /* n */ EMPTY_REDO_HISTORY,
        /* o */ HISTORY_SIZE_SMALL,
        /* p */ HISTORY_SIZE_LARGE,
        /* q */ IS_NOT_POS_DEC,
        /* r */ UNKNOWN_REG_DEF,
        /* s */ INCOMP_REG_WIDTH,
        /* t */ EMPTY_COMMAND,
        /* u */ EOF_COMMAND
    };

}//exceptions

#endif

/* aczutro ************************************************************* end */
