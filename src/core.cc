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

#include <cassert>
#include <cstring>

#include <colours.hh>
#include <exceptions.hh>
#include <core.hh>

using namespace std;
using namespace exceptions;


/*** macros ************************************************************/

#define DEFAULT_WIDTH 8
#define MAX_WIDTH 16
#define MAX_NUMBER_OF_BITS (MAX_WIDTH * 4)
#define DEFAULT_HISTORY_CAPACITY 16
#define SEPARATOR ' '


/*** help functions and constants for conversions **********************/

static const uintmax_t __pow[] = {
    1u,                   // 2 ^ 0
    2u,                   // 2 ^ 1
    4u,                   // 2 ^ 2
    8u,                   // 2 ^ 3
    16u,                  // 2 ^ 4
    32u,                  // 2 ^ 5
    64u,                  // 2 ^ 6
    128u,                 // 2 ^ 7
    256u,                 // 2 ^ 8
    512u,                 // 2 ^ 9
    1024u,                // 2 ^ 10
    2048u,                // 2 ^ 11
    4096u,                // 2 ^ 12
    8192u,                // 2 ^ 13
    16384u,               // 2 ^ 14
    32768u,               // 2 ^ 15
    65536u,               // 2 ^ 16
    131072u,              // 2 ^ 17
    262144u,              // 2 ^ 18
    524288u,              // 2 ^ 19
    1048576u,             // 2 ^ 20
    2097152u,             // 2 ^ 21
    4194304u,             // 2 ^ 22
    8388608u,             // 2 ^ 23
    16777216u,            // 2 ^ 24
    33554432u,            // 2 ^ 25
    67108864u,            // 2 ^ 26
    134217728u,           // 2 ^ 27
    268435456u,           // 2 ^ 28
    536870912u,           // 2 ^ 29
    1073741824u,          // 2 ^ 30
    2147483648u,          // 2 ^ 31
    4294967296u,          // 2 ^ 32
    8589934592u,          // 2 ^ 33
    17179869184u,         // 2 ^ 34
    34359738368u,         // 2 ^ 35
    68719476736u,         // 2 ^ 36
    137438953472u,        // 2 ^ 37
    274877906944u,        // 2 ^ 38
    549755813888u,        // 2 ^ 39
    1099511627776u,       // 2 ^ 40
    2199023255552u,       // 2 ^ 41
    4398046511104u,       // 2 ^ 42
    8796093022208u,       // 2 ^ 43
    17592186044416u,      // 2 ^ 44
    35184372088832u,      // 2 ^ 45
    70368744177664u,      // 2 ^ 46
    140737488355328u,     // 2 ^ 47
    281474976710656u,     // 2 ^ 48
    562949953421312u,     // 2 ^ 49
    1125899906842624u,    // 2 ^ 50
    2251799813685248u,    // 2 ^ 51
    4503599627370496u,    // 2 ^ 52
    9007199254740992u,    // 2 ^ 53
    18014398509481984u,   // 2 ^ 54
    36028797018963968u,   // 2 ^ 55
    72057594037927936u,   // 2 ^ 56
    144115188075855872u,  // 2 ^ 57
    288230376151711744u,  // 2 ^ 58
    576460752303423488u,  // 2 ^ 59
    1152921504606846976u, // 2 ^ 60
    2305843009213693952u, // 2 ^ 61
    4611686018427387904u, // 2 ^ 62
    9223372036854775808u  // 2 ^ 63
};

static uintmax_t pow(uint8_t exp){
    if(exp < 64){
        return __pow[exp];
    }else{
        return 2 * pow(exp - 1);
    }//else
}//pow

/* returns number of decimal digits needed to represent a */
static uint8_t log(uint8_t a){
    if(a < 10){
        return 1;
    }else if(a < 100){
        return 2;
    }else{
        return 3;
    }//else
}//log

inline static uint8_t hex2dec(char a){
    return a - (a > '9' ? 'a' - 10 : '0');
}//hex2dec

static const char dec2hex[] = "0123456789abcdef";

static const char hex2bin[][5] = {
  "0000",
  "0001",
  "0010",
  "0011",
  "0100",
  "0101",
  "0110",
  "0111",
  "1000",
  "1001",
  "1010",
  "1011",
  "1100",
  "1101",
  "1110",
  "1111"
};

inline static char bool2char(bool a){
    return a ? '1' : '0';
}//bool2char


/*** class core functions **********************************************/

void core::init_history(){
    delete[] H;
    H = new core_state[history_index::history_size()];
    bottom = 0;
    top = 0;
    history_push();
}//init_history

/*****************************************************************/

void core::history_push(){
    if((top + 1) == bottom){
        bottom++;
    }//if
    H[top()] = C;
    top++;
    last_push = top;
    no_redo = true;
    //@debug __print_history();
}//history_push

/*****************************************************************/

void core::history_pop(){
    if((top - 1) == bottom){
        throw(EMPTY_UNDO_HISTORY);
    }//if
    top--;
    C = H[(top - 1)()];
    no_redo = false;
    //@debug __print_history();
}//history_pop

/*****************************************************************/

void core::history_unpop(){
    if(no_redo || top == last_push){
        throw(EMPTY_REDO_HISTORY);
    }//if
    top++;
    C = H[(top - 1)()];
    //@debug __print_history();
}//history_unpop

/*****************************************************************/

void core::__print_history(){
    cout << "HISTORY" << endl;
    for(uint8_t i = 0; i < history_index::history_size(); i++){
        if(i == bottom() && i == top()){
            cout << "|> ";
        }else if(i == bottom()){
            cout << "|- ";
        }else if(i == top()){
            cout << "-> ";
        }else if(i == last_push()){
            cout << " . ";
        }else{
            cout << "   ";
        }//else
        cout << i << ": ";
        H[i].print();
        cout << endl;
    }//for
    cout << "END" << endl;
}//__print_history

/*****************************************************************/

void core::print_history(){
    if(bottom == last_push){
        cout << "no undo history";
        return;
    } /* end if(bottom == last_push) */
    cout << "undo history:";
    for(history_index i = bottom; i != last_push; i++){
        cout << endl;
        if((i + 1) == top)
            cout << "-> ";
        else
            cout << "   ";
        H[i()].print();
    }//for
}//print_history

/*****************************************************************/

core::core(){
    C.show_indices = true;
    C.reset_perm_hilite();
    C.set_width(DEFAULT_WIDTH);
    C = "0";

    H = NULL;
    history_index::history_size(DEFAULT_HISTORY_CAPACITY);
    init_history();

    tmp_hex = new char[MAX_WIDTH + 1];
    line1.reserve(MAX_NUMBER_OF_BITS);
    line2.reserve(MAX_NUMBER_OF_BITS);
    line3.reserve(MAX_NUMBER_OF_BITS);
    line4.reserve(MAX_NUMBER_OF_BITS);
    line5.reserve(MAX_NUMBER_OF_BITS);
}//core

/*****************************************************************/

core::~core(){
    delete[] tmp_hex;
    delete[] H;
}//~core

/*****************************************************************/

void core::turn_on_perm_hilite(uint16_t a, uint16_t b, bool push){
    if(a >= C.number_of_bits() || b >= C.number_of_bits()){
        throw(BAD_HILITE_LIMITS);
    }//if
    C.set_perm_hilite(a, b);
    if(push){
        history_push();
    }//if
}//turn_on_perm_hilite

/*****************************************************************/

core &core::set_to(char mode, const string &a){
    switch(mode){
    case 'b':
        if(a.length() > MAX_NUMBER_OF_BITS){
            sprintf(tmp_hex, "%llx",
                    strtoull(a.substr(a.length() - MAX_NUMBER_OF_BITS).c_str(),
                             NULL, 2)
                    );
        }else{
            sprintf(tmp_hex, "%llx", strtoull(a.c_str(), NULL, 2));
        }//else
        tmp_string = tmp_hex;
        break;
    case 'd':
        sprintf(tmp_hex, "%llx", strtoull(a.c_str(), NULL, 10));
        if(C.width() && C.width() < strlen(tmp_hex)){
            throw(BAD_VALUE_FOR_WIDTH);
        }//if
        tmp_string = tmp_hex;
        break;
    default: // assuming 'h'
        tmp_string = a;
        break;
    }//switch

    if(tmp_string.length() > MAX_WIDTH){
        C = tmp_string.substr(tmp_string.length() - MAX_WIDTH);
    }else{
        C = tmp_string;
    }//else
    history_push();
    return *this;
}//set_to

/*****************************************************************/

void core::resize_history(uint8_t a){
    if(a < 1){
        throw(HISTORY_SIZE_SMALL);
    }//if
    if(a + 2 > history_index::MAX_HISTORY_SIZE){
        throw(HISTORY_SIZE_LARGE);
    }//if
    history_index::history_size(a + 2);
    init_history();
}//resize_history

/*****************************************************************/

core &core::set_width(uint8_t a){
    if(a == C.width()){
        return *this;
    }//if
    if(a > MAX_WIDTH){
        throw(UNSUPPORTED_WIDTH);
    }//if
    C.set_width(a);
    history_push();
    return *this;
}//set_width

/*****************************************************************/

core &core::invert(uint8_t lo, uint8_t hi){
    if(hi < lo){
        tmp_byte1 = lo;
        lo = hi;
        hi = tmp_byte1;
    }//if
    if(lo < 0 || hi >= C.number_of_bits()){
        throw(BAD_INV_LIMITS);
    }//if

    for(uint8_t i = (C.number_of_bits() - 1 - hi);
        i < (C.number_of_bits() - lo); i++){
        tmp_byte1 = i / 4;
        C.replace_nibble(tmp_byte1,
                         dec2hex[hex2dec(C.hex()[tmp_byte1])
                                 ^ pow((C.number_of_bits() - 1 - i) % 4)]
                         );
    }//for

    if(! C.width()){
        C.adjust_to_width();
    }//if

    history_push();
    return *this;
}//invert

/*****************************************************************/

core &core::replace(char mode, const string &a){
    if(! C.perm_hilite()){
        throw(NO_HILITE);
    }//if
    switch(mode){
    case 'b':
        tmp_string = a;
        break;
    case 'd':
        tmp_dec = strtoull(a.c_str(), NULL, 10);
        if(tmp_dec > pow(C.perm_hilite_max() - C.perm_hilite_min() + 1) - 1){
            throw(BAD_VALUE_FOR_PERM_WIDTH);
        }//if
        tmp_string = "";
        sprintf(tmp_hex, "%llx", (unsigned long long)tmp_dec);
        for(char *ch = tmp_hex; *ch; ch++){
            tmp_string.append(hex2bin[hex2dec(*ch)]);
        }//for
        break;
    default: // assuming 'h'
        tmp_string = "";
        for(char ch : a){
            tmp_string.append(hex2bin[hex2dec(ch)]);
        }//for
        break;
    }//switch

    string::reverse_iterator rit = tmp_string.rbegin();
    for(uint8_t i = C.number_of_bits() - 1 - C.perm_hilite_min();
        i > C.number_of_bits() - 2 - C.perm_hilite_max(); i--){
        tmp_byte1 = i / 4;
        tmp_byte2 = (C.number_of_bits() - 1 - i) % 4;
        if(rit != tmp_string.rend()){
            tmp_hex[0] = *rit;
            rit++;
        }else{
            tmp_hex[0] = '0'; // replacement_string was too short, so padding with 0s
        }//else
        C.replace_nibble(tmp_byte1,
                         dec2hex[(hex2dec(C.hex()[tmp_byte1]) | pow(tmp_byte2))
                                 - (('1' - tmp_hex[0]) * pow(tmp_byte2))]
                         );
    }//for

    if(! C.width()){
        C.adjust_to_width();
    }//if

    history_push();
    return *this;
}//replace

/*****************************************************************/

#define hilite_line(line) line.substr(0, left_ins) << BOLD << C_HILITE_1 \
    << line.substr(left_ins, right_ins - left_ins + 1) << DEFF    \
    << line.substr(right_ins + 1)

void core::print(bool hilite_now, uint8_t lo, uint8_t hi){
    if(hilite_now){
        if(hi < lo){
            tmp_byte1 = lo;
            lo = hi;
            hi = tmp_byte1;
        }//if
        if(lo < 0 || hi >= C.number_of_bits()){
            throw(BAD_INV_LIMITS);
        }//if
    }//if
    else if(C.perm_hilite()){
        lo = C.perm_hilite_min();
        hi = C.perm_hilite_max();
    }//else

    /* basic output **************************************/

    tmp_dec = strtoull(C.hex().c_str(), NULL, 16);
    if(errno){
        errno = 0;
    }else{
        cout << C_PRINT << "decimal: " << DEFF
             << C_HILITE_3 << tmp_dec << DEFF << endl;
    }//else

    line1 = string(C_PRINT) + "    hex: " + DEFF;
    line2 = string(C_PRINT) + "    bin: " + DEFF;
    line3 = string(C_PRINT) + "         " + DEFF;
    line4 = string(C_PRINT) + "indices: " + DEFF;
    line5 = string(C_PRINT) + "         " + DEFF;

    for(uint8_t i = 0; i < C.hex().length(); i++){
        line1.append(1, SEPARATOR).append("   ").append(1, C.hex()[i]);
        tmp_byte1 = hex2dec(C.hex()[i]);
        line2.append(string(1, SEPARATOR)
                     + bool2char(tmp_byte1 & 8)
                     + bool2char(tmp_byte1 & 4)
                     + bool2char(tmp_byte1 & 2)
                     + bool2char(tmp_byte1 & 1)
                     );
        line3.append("+----");
        tmp_byte1 = C.number_of_bits() - (4 * i) - 1;
        line4.append(string(1, SEPARATOR) + to_string(tmp_byte1 / 10)
                     + "  " + to_string((tmp_byte1 - 3) / 10));
        line5.append(string(1, SEPARATOR) + to_string(tmp_byte1 % 10)
                     + "  " + to_string((tmp_byte1 - 3) % 10));
    }//for
    line3.append("+");

    if((! C.perm_hilite()) && (! hilite_now)){
        cout << line1 << endl << line2;
        if(C.show_indices){
            cout << endl << line3
                 << endl << line4
                 << endl << line5;
        }//if
        return;
    }//if

    /* highlighting **************************************/

    uint16_t left_ins  = line2.length() -1 - (hi / 4 * 5) - (hi % 4);
    uint16_t right_ins = line2.length() -1 - (lo / 4 * 5) - (lo % 4);

    cout << line1 << endl << hilite_line(line2);

    if(C.show_indices){
        cout << endl << hilite_line(line3)
             << endl << hilite_line(line4)
             << endl << hilite_line(line5);
    }//if

    /* details on highlighted part ***********************/

    hilited_hex = "";
    hilited_string = "";
    for(uint16_t i = left_ins; i <= right_ins; i++){
        if(line2[i] != SEPARATOR){
            hilited_string.push_back(line2[i]);
        }//if
    }//for

    cout << endl << "highlighted bin: " << BOLD << C_HILITE_2;
    tmp_byte1 = 0;
    for(uint16_t i = 0; i < hilited_string.length(); i++){
        cout << hilited_string[i];
        tmp_byte2 = (hilited_string.length() - 1 - i) % 4;
        tmp_byte1 += ((hilited_string[i] - '0') * pow(tmp_byte2));
        if(tmp_byte2 == 0){
            assert(tmp_byte1 < 16);
            hilited_hex.append(1, dec2hex[tmp_byte1]);
            tmp_byte1 = 0;
            if(i != (hilited_string.length() - 1)){
                cout << SEPARATOR;
            }//if
        }//if
        hilited_hex.append(1, SEPARATOR);
    }//for

    cout << DEFF
         << endl
         << "highlighted hex: " << BOLD << C_HILITE_2 << hilited_hex
         << DEFF;

    hilited_dec = strtoull(hilited_string.c_str(), NULL, 2);
    if(errno){
        errno = 0;
    }else{
        cout << endl
             << "highlighted dec: " << BOLD << C_HILITE_2 << hilited_dec
             << DEFF;
    }//else
}//print

/*****************************************************************/

void core::print_registers(reg_info *RI){

    cout << "available register definitions:";

    for(register_data entry = RI->RD().begin();
        entry != RI->RD().end(); entry++){
        cout << endl << "    " << entry->first;
    }//for

}//print_registers

/*****************************************************************/

void core::print_register(reg_info *RI, const string &regname){
    if(RI->RD().find(regname) == RI->RD().end()){
        throw(UNKNOWN_REG_DEF);
    }//if
    const field_data *F = RI->RD().at(regname);
    if(F->width != C.number_of_bits()){
        throw(INCOMP_REG_WIDTH);
    }//if
    F = F->next;

    uint8_t max_fname_length = regname.length();
    uint8_t max_bin_length = 3;
    bool multi_index = false;
    while(F){
        if(F->name.length()){ // if string not empty
            if(F->width > 1){
                multi_index = true;
            }//if
            if(F->name.length() > max_fname_length){
                max_fname_length = F->name.length();
            }//if
            if(F->width > max_bin_length){
                max_bin_length = F->width;
            }//if
        }//if
        F = F->next;
    }//while
    uint8_t max_hex_length = max(3,
                                 (max_bin_length / 4)
                                 + (max_bin_length % 4 ? 1 : 0)
                                 );
    uint8_t index_width = log(C.number_of_bits() - 1);

    uint8_t tot_sgl_idx_wd = 3 + index_width;
    uint8_t tot_idx_wd_diff = multi_index ? 2 + index_width : 0;
    uint8_t tot_mlt_idx_wd = tot_sgl_idx_wd + tot_idx_wd_diff;

    cout << string(max_fname_length - regname.length(), ' ')
         << regname
         << string(tot_mlt_idx_wd, ' ')
         << "   " << BOLD << C_HILITE_1 << "bin" << string(max_bin_length - 3, ' ') << DEFF
         << "   " << BOLD << C_HILITE_2 << "hex" << string(max_hex_length - 3, ' ') << DEFF
         << "   " << BOLD << C_HILITE_3 << "dec" << DEFF
         << endl
         << string(max_fname_length + tot_mlt_idx_wd + max_bin_length
                   + max_hex_length + 12,
                   '-');

    line1 = "";
    for(char ch : C.hex()){
        tmp_byte1 = hex2dec(ch);
        line1.append(string("")
                     + bool2char(tmp_byte1 & 8)
                     + bool2char(tmp_byte1 & 4)
                     + bool2char(tmp_byte1 & 2)
                     + bool2char(tmp_byte1 & 1)
                     );
    }//for

    F = RI->RD().at(regname)->next;

    uint16_t left_ins, right_ins;
    uint16_t i = 0;
    while(F){
        if(F->name.length()){ // if string not empty
            hilited_string = line1.substr(i, F->width);
            hilited_dec = strtoull(hilited_string.c_str(), NULL, 2);
            hilited_hex = "";
            sprintf(tmp_hex, "%llx", (unsigned long long)hilited_dec);
            hilited_hex.append(tmp_hex);
            left_ins = C.number_of_bits() - i - 1;
            right_ins = C.number_of_bits() - i - F->width;
            cout << endl
                 << string(max_fname_length - F->name.length(), ' ')
                 << F->name;
            if(left_ins == right_ins){
                cout << " [" << string(index_width - log(left_ins), ' ')
                     << left_ins << "]" << string(tot_idx_wd_diff, ' ');
            }else{
                cout << " [" << string(index_width - log(left_ins), ' ')
                     << left_ins << ".."
                     << string(index_width - log(right_ins), ' ')
                     << right_ins << "]";
            }//else
            cout << " = " << BOLD << C_HILITE_1 << hilited_string
                 << string(max_bin_length - hilited_string.length(), ' ')
                 << DEFF << "   " << BOLD << C_HILITE_2 << hilited_hex
                 << string(max_hex_length - hilited_hex.length(), ' ')
                 << DEFF << "   " << BOLD << C_HILITE_3 << hilited_dec
                 << DEFF << flush;
        }//if
        i += F->width;
        F = F->next;
    }//while
}//print_register

/* aczutro ************************************************************* end */
