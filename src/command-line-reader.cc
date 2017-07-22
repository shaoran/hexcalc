/* aczutro -*- c-basic-offset:4 -*-
 * vim: set sw=4 ts=4 sts=4 expandtab :
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
#include <limits>

#include <cstring>
#include <cstdlib>

#include <exceptions.hh>
#include <command-line-reader.hh>
#include <readline-interface.hh>

#include <readline/readline.h>
#include <readline/history.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>

using namespace std;


/*** class command_line_reader functions *************************************/

command_line_reader::command_line_reader(uint16_t max_command_line_length,
                                         uint8_t max_number_of_args, std::string propmt, std::string history){
    command_line_length = max_command_line_length;
    token_size = command_line_length + 1;
    capacity = max_number_of_args;
    token = new char*[capacity + 1];
    for(__i = 0; __i <= capacity; __i++){
        token[__i] = new char[token_size];
    }//for
    noa = 0;

    this->prompt = propmt;

    if(history == "")
    {
        const char *homedir = NULL;
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        if(homedir) {
            this->history = homedir;
            this->history += "/.hexcalc_history";
        }
    } else
        this->history = history;

    FILE *fp = fopen(this->history.c_str(), "a");
    if(fp)
        fclose(fp);
    else {
        std::cout << "History file " << this->history << " cannot be accessed. No history will be saved" << std::endl;
        this->history = "";
    }



    // initializing readline

    // Allow conditional parsing of the ~/.inputrc file
    rl_readline_name = "hexcalc";

    if(this->history != "")
        read_history(this->history.c_str());

    stifle_history(100); // allow max 100 entries in history file

    rl_iface::setup_readline_interface();
    rl_attempted_completion_function = rl_iface::hexcalc_complete;

    rl_set_cli(this);

}//command_line_reader

/*****************************************************************/

command_line_reader::~command_line_reader(){
    for(__i = 0; __i <= capacity; __i++){
        delete[] token[__i];
    }//for
    delete[] token;

    if(this->history != "")
        write_history(this->history.c_str());
}//~command_line_reader

/*****************************************************************/

static sigjmp_buf __env;
static volatile sig_atomic_t __jump = 0;

void sigint_hdl(int signal)
{
    if(signal == SIGINT && __jump)
    {
        __jump = 0;
        siglongjmp(__env, 1);
    }
}

void command_line_reader::operator>>(char &command){
    for(__i = 0; __i <= capacity; __i++){
        memset(token[__i], 0, token_size);
    }//for
    __i = 0;
    __j = 0;

    __jump = 0;

    if(signal(SIGINT, sigint_hdl) != SIG_ERR)
    {
        if(sigsetjmp(__env, 1) != 0)
        {

            rl_cleanup_after_signal();
            rl_free_line_state();
            std::cout << std::endl;

            if(*rl_line_buffer)
                throw(exceptions::EMPTY_COMMAND);

            // quit program after ctrl+c, write history and do clean up
            command = 'q';
            return;
        }
        __jump = 1;
    }

    char *buffer = readline(this->prompt.c_str());

    signal(SIGINT, SIG_IGN);


    if(buffer == NULL)
        throw(exceptions::EOF_COMMAND);

    int blen = strlen(buffer);

    char tmp[blen + 1];
    strcpy(tmp, buffer);
    rl_iface::trim(tmp);

    // ignore empty lines
    if(*tmp)
        add_history(buffer);

    for(int idx = 0; idx < blen ; ++idx)
    {
        ch = buffer[idx];

        switch(ch){
        case ' ':
        case '\t':
            if(__j){
                __i++;
                __j = 0;
            }//if
            break;
        default:
            // only write into token if there is space; otherwise discard ch
            if(__i <= capacity && __j < command_line_length){
                token[__i][__j] = ch;
                __j++;
            }//if
        }//switch
    }//for

    free(buffer);

    if(__j)
        __i++;

    if(token[0][0] == 0){
        throw(exceptions::EMPTY_COMMAND);
    }//if

    if(token[0][1] || isxdigit(token[0][0])){ /* self-insert command */
        command = 0;
        offset = 0;
        noa = 1;
    }else{
        command = token[0][0];
        offset = 1;
        noa = __i - 1;
    }//else
}//operator>>

/*****************************************************************/

bool command_line_reader::is_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! (isdigit(*c) || *c == '-')){
            return false;
        }//if
    }//for
    return true;
}//is_num

/*****************************************************************/

bool command_line_reader::is_not_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! (isdigit(*c) || *c == '-')){
            return true;
        }//if
    }//for
    return false;
}//is_not_num

/*****************************************************************/

bool command_line_reader::is_pos_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! isdigit(*c)){
            return false;
        }//if
    }//for
    return true;
}//is_pos_num

/*****************************************************************/

bool command_line_reader::is_not_pos_dec(uint8_t i){
    for(char *c = token[i + offset]; *c; c++){
        if(! isdigit(*c)){
            return true;
        }//if
    }//for
    return false;
}//is_not_pos_num

const int command_line_reader::get_width() const {
    return this->cmd_w_arg;
}

void command_line_reader::set_width(int width)
{
    this->cmd_w_arg = width;
}

const std::vector<std::string> &command_line_reader::get_registers() const {
    return this->cmd_s_regs;
}

void command_line_reader::set_registers(reg_info &registers)
{
    this->cmd_s_regs.clear();

    for(auto i = registers.RD().begin(); i != registers.RD().end(); ++i)
    {
        this->cmd_s_regs.push_back(std::string(i->first));
    }
}

/* aczutro ************************************************************* end */
