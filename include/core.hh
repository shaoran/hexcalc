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

#ifndef core_hh
#define core_hh core_hh

#include <history-index.hh>
#include <core-state.hh>
#include <reg-info.hh>


/*** class declaration *******************************************************/

class core{

private:
    core_state    C; // current state (accumulator)
    core_state    *H; // history

    history_index bottom;
    history_index top;
    history_index last_push;

    bool          no_redo;

    /* temporal variables ********************************************/

    char        *tmp_hex;
    uintmax_t   tmp_dec;
    uint8_t     tmp_byte1, tmp_byte2;
    std::string tmp_string;

    std::string line1;
    std::string line2;
    std::string line3;
    std::string line4;
    std::string line5;

    std::string hilited_hex;
    std::string hilited_string;
    uintmax_t   hilited_dec;

    /* history *******************************************************/

    void init_history();

    void history_push();

    void history_pop();

    void history_unpop();

    void __print_history();

public:
    void print_history();

    /* constructor ***************************************************/

    core();

    ~core();

    /* toggles and consistency ***************************************/

    inline void toggle_indices(){
        C.show_indices = ! C.show_indices;
        history_push();
    }//toggle_indices

    inline void turn_off_perm_hilite(bool push=true){
        C.reset_perm_hilite();
        if(push){
            history_push();
        }//if
    }//turn_off_perm_hilite

    void turn_on_perm_hilite(uint16_t a, uint16_t b, bool push=true);

    /* = modifying accumulator contents ******************************/

    core &set_to(char mode, const std::string &a);

    /* undo **********************************************************/

    void resize_history(uint8_t a);

    inline uint8_t get_history_size(){
        return history_index::history_size() - 2;
    }//get_history_size

    inline core &undo(){
        history_pop();
        return *this;
    }//undo

    inline core &redo(){
        history_unpop();
        return *this;
    }//redo

    /* width *********************************************************/

    inline uint8_t get_width(){
        return(C.width());
    }//get_width

    core &set_width(uint8_t a);

    /* bit flipping **************************************************/

    core &invert(uint8_t lo=0, uint8_t hi=0);

    inline core &invert_all(){
        if(C.perm_hilite()){
            return invert(C.perm_hilite_min(), C.perm_hilite_max());
        }else{
            return invert(0, C.number_of_bits() - 1);
        }//else
    }//invert_all

    /* bit replacement ***********************************************/

    core &replace(char mode, const std::string &a);

    /* pretty print **************************************************/

    void print(bool hilite_now=false, uint8_t lo=0, uint8_t hi=0);

    /* register print ************************************************/

    void print_registers(reg_info *RI);

    void print_register(reg_info *RI, const std::string &regname);
};

#endif

/* aczutro ************************************************************* end */
