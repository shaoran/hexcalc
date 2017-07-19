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

#include <cstring>

#include <colours.hh>
#include <exceptions.hh>
#include <command-line-reader.hh>
#include <core.hh>

#include <getopt.h>

using namespace std;
using namespace exceptions;


/*** macros ******************************************************************/

#define HEXCALC_VERSION "2.0"

#define HELP_BUFFER_LENGTH 1800

#define __error cout << BOLD << C_ERROR << "error: " << DEFF << " "

#define isbindigit(ch) ((ch == '0') || (ch == '1'))

#define QUIT          "q"
#define HELP          "h"
#define VERSION       "v"
#define PRINT         "p"
#define INDICES       "I"
#define HILITE        "l"
#define PERM_HILITE   "L"
#define WIDTH         "w"
#define INVERT        "i"
#define REPLACE       "="
#define UNDO          "u"
#define REDO          "r"
#define HISTORY_SIZE  "U"
#define PRINT_HISTORY "H"
#define SPLIT_FIELDS  "s"
#define SPLIT_REPEAT  "S"
#define LOAD_SPECS    "R"

#define CMD_QUIT          QUIT[0]
#define CMD_HELP          HELP[0]
#define CMD_VERSION       VERSION[0]
#define CMD_PRINT         PRINT[0]
#define CMD_INDICES       INDICES[0]
#define CMD_HILITE        HILITE[0]
#define CMD_PERM_HILITE   PERM_HILITE[0]
#define CMD_WIDTH         WIDTH[0]
#define CMD_INVERT        INVERT[0]
#define CMD_REPLACE       REPLACE[0]
#define CMD_UNDO          UNDO[0]
#define CMD_REDO          REDO[0]
#define CMD_HISTORY_SIZE  HISTORY_SIZE[0]
#define CMD_PRINT_HISTORY PRINT_HISTORY[0]
#define CMD_SPLIT_FIELDS  SPLIT_FIELDS[0]
#define CMD_SPLIT_REPEAT  SPLIT_REPEAT[0]
#define CMD_LOAD_SPECS    LOAD_SPECS  [0]

#define __cmd(str) BOLD C_HELP_CMD str DEFF

#define __arg(str) ULNE C_HELP_ARG str DEFF

#define __title(str) BOLD C_HELP_TITLE str DEFF

#define __print_errmsg catch(signal e){__error << errmsg[e];}

const char *__cmd_help_str =
"usage: hexcalc [options] [specs file]\n\
\n\
options:\n\
    -h,   --help            prints this help\n\
    -i,   --history FILE    loads the input history\n\
                            from FILE. By default\n\
                            ~/.hexcalc_history is used\n\
\n\
specs file\n\
    The path of the specs file conatining the register.\n\
    If this file is passed it will automatically load it.\n\
    Otherwise you'll have to use the 'R' command to load\n\
    another specs file.";

/*** main ********************************************************************/

int main(int argc, char *argv[]){

    reg_info *RI = NULL;

    int option_index = 0;

    static struct option long_options[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"history", required_argument, NULL, 'i'},
        {NULL,      0,                 NULL, 0},
    };

    std::string history_file = "";
    std::string specs_file = "";

    while(1)
    {
        int c = getopt_long(argc, argv, "hi:", long_options, &option_index);

        if(c == -1)
            break;

        switch(c)
        {
            case 'h':
                std::cout << __cmd_help_str << std::endl;
                return 0;
                break;

            case 'i':
                if(optarg == NULL)
                {
                    std::cerr << "Invalid -i/--history argument" << std::endl;
                    std::cerr << __cmd_help_str << std::endl;
                    return 1;
                }

                history_file = optarg;
                break;

            default:
                std::cerr << "Invalid option" << std::endl;
                std::cerr << __cmd_help_str << std::endl;
                return 1;
        }
    };

    if(optind < argc)
        specs_file = argv[optind];

    /* set up some constants and declare main variables **************/

    /* command ids */
    const char SELF_INSERT   = 0;

    /* strings for help printing */
    string main_help;
    map<char, string> help_on;
    string version_text;
    string intro;

    {
        auto __quit          = __cmd(QUIT         );
        auto __help          = __cmd(HELP         );
        auto __version       = __cmd(VERSION      );
        auto __print         = __cmd(PRINT        );
        auto __indices       = __cmd(INDICES      );
        auto __hilite        = __cmd(HILITE       );
        auto __perm_hilite   = __cmd(PERM_HILITE  );
        auto __width         = __cmd(WIDTH        );
        auto __invert        = __cmd(INVERT       );
        auto __replace       = __cmd(REPLACE      );
        auto __undo          = __cmd(UNDO         );
        auto __redo          = __cmd(REDO         );
        auto __history_size  = __cmd(HISTORY_SIZE );
        auto __print_history = __cmd(PRINT_HISTORY);
        auto __split_fields  = __cmd(SPLIT_FIELDS );
        auto __split_repeat  = __cmd(SPLIT_REPEAT );
        auto __load_specs    = __cmd(LOAD_SPECS   );

        char help_buffer[HELP_BUFFER_LENGTH];

        sprintf(help_buffer, "%s\n\
  %s Print accumulator.                   %s Toggle indices.\n\
\n\
%s\n\
  %s %s Set accumulator's width.       %s %s   Flip bit.\n\
  %s Print current width.                 %s %s %s Flip bit range.\n\
                                         %s         Flip all bits.\n\
%s\n\
  %s %s   Highlight bit permanently.   %s %s   Highlight bit once.          \n\
  %s %s %s Highlight bit range perm.    %s %s %s Highlight bit range once.    \n\
  %s         Turn off perm. highlighting.\n\
  %s { %s | %s%s | %s%s } Set value of perm. highlighted bits.\n\
\n\
%s\n\
  %s Undo.            %s Redo.             %s Print undo history.\n\
  %s %s Set undo history capacity.      %s Print current history capacity.\n\
\n\
%s\n\
  %s %s Print register info.        %s Repeat last \"%s %s\" command.\n\
  %s          Print available registers.\n\
  %s %s     Load register specs from file %s.\n\
\n\
%s\n\
  %s Quit.                          %s         Print this text.\n\
  %s Print version info.            %s %s Print detailed help on %s.",
                __title("Output commands"),
                __print, __indices,
                __title("Modification commands"),
                __width, __arg("WIDTH"), __invert, __arg("INDEX"),
                __width, __invert, __arg("FROM"), __arg("TO"),
                __invert,
                __title("Highlighting commands"),
                __perm_hilite, __arg("INDEX"), __hilite, __arg("INDEX"),
                __perm_hilite, __arg("FROM"), __arg("TO"), __hilite, __arg("FROM"), __arg("TO"),
                __perm_hilite,
                __replace, __arg("HEX_No"), __cmd("'b"), __arg("BIN_No"), __cmd("'d"), __arg("DEC_No"),
                __title("History commands"),
                __undo, __redo, __print_history,
                __history_size, __arg("SIZE"), __history_size,
                __title("Register information commands"),
                __split_fields, __arg("REGISTER"), __split_repeat, __split_fields, __arg("REGISTER"),
                __split_fields,
                __load_specs, __arg("FILE"), __arg("FILE"),
                __title("Common commands"),
                __quit, __help,
                __version, __help, __arg("COMMAND"), __arg("COMMAND")
                );
        main_help = help_buffer;

        sprintf(help_buffer,
                "%s  Print the current value of the accumulator.",
                __print);
        help_on[CMD_PRINT] = help_buffer;

        sprintf(help_buffer, "%s  Toggle showing indices.", __indices);
        help_on[CMD_INDICES] = help_buffer;

        sprintf(help_buffer, "%s %s  Set the accumulator's width to %s hexadecimal digits.\n\
                If %s is 0, the accumulator will have variable width\n\
                (i.e. its width will change automatically depending on the\n\
                stored value).\n\
       %s        Print current accumulator width.",
                __width, __arg("WIDTH"),
                __arg("WIDTH"),
                __arg("WIDTH"),
                __width);
        help_on[CMD_WIDTH] = help_buffer;

        sprintf(help_buffer, "%s %s    Flip bit with index %s.\n\
       %s %s %s  Flip bits %s..%s (or %s..%s).\n\
       %s          Flip all bits.\n\
                  If permanent highlighting is on, flip only highlighted\n\
                  bits.",
                __invert, __arg("INDEX"),
                __arg("INDEX"),
                __invert, __arg("FROM"), __arg("TO"),
                __arg("FROM"), __arg("TO"), __arg("TO"), __arg("FROM"),
                __invert);
        help_on[CMD_INVERT] = help_buffer;

        sprintf(help_buffer,
                "%s  Undo the last operation that changed the accumulator's permanent\n\
          state.", __undo);
        help_on[CMD_UNDO] = help_buffer;

        sprintf(help_buffer,
                "%s  Undo the last undo operation (redo).", __redo);
        help_on[CMD_REDO] = help_buffer;

        sprintf(help_buffer,
                "%s  Print the current undo history.", __print_history);
        help_on[CMD_PRINT_HISTORY] = help_buffer;

        sprintf(help_buffer, "%s %s  Set the undo history capacity to %s.\n\
               (Deletes current history!)\n\
       %s       Print the current undo history capacity.",
                __history_size, __arg("SIZE"),
                __arg("SIZE"),
                __history_size);
        help_on[CMD_HISTORY_SIZE] = help_buffer;

        sprintf(help_buffer, "%s %s    Print accumulator and highlight the bit with index %s.\n\
       %s %s %s  Print accumulator and highlight bits %s..%s\n\
                  (or %s..%s).",
                __hilite, __arg("INDEX"),
                __arg("INDEX"),
                __hilite, __arg("FROM"), __arg("TO"),
                __arg("FROM"), __arg("TO"),
                __arg("TO"), __arg("FROM")
                );
        help_on[CMD_HILITE] = help_buffer;

        sprintf(help_buffer, "%s %s    Turn on permanent highlighting of bit with index %s.\n\
       %s %s %s  Turn on permanent highlighting of bits %s..%s\n\
                  (or %s..%s).\n\
       %s          Turn off permanent highlighting.",
                __perm_hilite, __arg("INDEX"),
                __arg("INDEX"),
                __perm_hilite, __arg("FROM"), __arg("TO"),
                __arg("FROM"), __arg("TO"),
                __arg("TO"), __arg("FROM"),
                __perm_hilite);
        help_on[CMD_PERM_HILITE] = help_buffer;

        sprintf(help_buffer, "%s { %s | %s%s | %s%s }\n\
              Set value of permanently highlighted bits to %s,\n\
              %s or %s.\n\
              If the new value is too large for the number of highlighted\n\
              bits, its most significant bits are cut off silently.",
                __replace, __arg("HEX_NUMBER"), __cmd("'b"), __arg("BIN_NUMBER"), __cmd("'d"), __arg("DEC_NUMBER"),
                __arg("HEX_NUMBER"),
                __arg("BIN_NUMBER"), __arg("DEC_NUMBER")
                );
        help_on[CMD_REPLACE] = help_buffer;

        sprintf(help_buffer,
                "%s  Repeat the last call of the %s command (same register).",
                __split_repeat, __split_fields);
        help_on[CMD_SPLIT_REPEAT] = help_buffer;

        sprintf(help_buffer, "%s %s  Print the value of the individual fields of register\n\
                   %s if that register had the same contents as the\n\
                   accumulator.\n\
       %s           Print list of available register definitions.",
                __split_fields, __arg("REGISTER"),
                __arg("REGISTER"),
                __split_fields);
        help_on[CMD_SPLIT_FIELDS] = help_buffer;

        sprintf(help_buffer, "%s %s  Load register specs from file %s.\n\
               %s must be a plain text file that specifies any number of\n\
               registers and the bit fields those registers are composed of.\n\
               The specification of a register is as follows:\n\
                   %s0 REGISTER_NAME\n\
                   FIELD_WIDTH_IN_BITS FIELD_NAME\n\
                   FIELD_WIDTH_IN_BITS FIELD_NAME\n\
                   ...\n\
                   FIELD_WIDTH_IN_BITS FIELD_NAME\n\
                   0%s\n\
               Fields are specified in order from left to right, one per\n\
               line.  If a line lacks a field name, the amount of bits\n\
               specified in that line are interpreted as unused bits; such\n\
               bits are not listed by the %s command.  The sum of all field\n\
               widths must equal the register's width in bits.  The total\n\
               width must be divisible by 4 and it must match the number of\n\
               bits of the accumulator when the %s command is run.\n\
               An example specification:\n\
                   %s0 cause\n\
                   11\n\
                   16 addr\n\
                   1 addr_valid\n\
                   4 errcode\n\
                   0%s\n\
               This defines a (fictitious) 32-bit register called 'cause',\n\
               whose bits 3..0 are used to store the error code\n\
               corresponding to the failure caused by the last instruction.\n\
               Bit 4 is a flag that indicates whether the failure involves\n\
               an address, and bits 20..5 are used to store that address.\n\
               Bits 31..21 are not used.",
                __load_specs, __arg("FILE"), __arg("FILE"),
                __arg("FILE"),
                C_PROMPT,
                DEFF,
                __split_fields,
                __split_fields,
                C_PROMPT,
                DEFF );
        help_on[CMD_LOAD_SPECS] = help_buffer;

        sprintf(help_buffer, "%s%shexcalc%s %sv. %s%s\n\
%sCopyright 2014 - 2017 Alexander Czutro%s\n\
%sThis program is free software: you can redistribute it and/or modify%s\n\
%sit under the terms of the GPLv3+ <http://www.gnu.org/licences>.%s\n\
%sThis program is distributed WITHOUT ANY WARRANTY.%s",
                BOLD, C_PROMPT, DEFF, C_PROMPT, HEXCALC_VERSION, DEFF,
                C_PROMPT, DEFF,
                C_PROMPT, DEFF,
                C_PROMPT, DEFF,
                C_PROMPT, DEFF);
        version_text = help_buffer;

        sprintf(help_buffer, "Think of this calculator as the direct interface to a single register\n\
called the \"accumulator\".\n\
    You can directly set the value stored in the accumulator by typing\n\
that value and hitting Enter.  The entered values can be entered as\n\
hexadecimal, binary or decimal numbers.  For example, to set the\n\
accumulator to \"47\", you can enter \"%s%s2f%s\" (hex), \"%s%s'b101111%s\" (bin) or\n\
\"%s%s'd47%s\" (dec).\n\
    To print the current value of the accumulator, type \"%s\".\n\
    Type \"%s\" to print a list of available commands.\n",
                BOLD, C_HELP_CMD, DEFF, BOLD, C_HELP_CMD, DEFF,
                BOLD, C_HELP_CMD, DEFF,
                __print,
                __help);
        intro = help_buffer;
    }//{

    /* prompt strings */
    const string prompt = string(BOLD) + C_PROMPT + "hex-calc>" + DEFF + " ";

    char command;
    core A; // the "accumulator"
    string suffix;
    string last_register;

    /* initialise command line reader and print welcome text *********/

    command_line_reader R(256, 2, prompt, history_file);
    cout << version_text << endl << endl << intro << flush;

    /* run the main loop, consisiting of
     * printing prompt, reading command, executing, for ever *********/

    if(specs_file != "")
    {
        std::cout << "\n\nAutoloading specs file " << specs_file << std::endl;
        try{
            RI = new reg_info(specs_file.c_str());
            A.print_registers(RI);
            std::cout << std::endl;
        }//try
        catch(exception &e){
            std::cout << "Failed to load specs file: " << e.what() << std::endl;
            RI = NULL;
        }//catch
    }

    while(true){

        cout << endl;
    l_read_command_line:
        cout << prompt << flush;
        try{R >> command;}
        catch(signal e){
            if(e == EMPTY_COMMAND){
                goto l_read_command_line;
            }else{ // EOF_COMMAND
                cout << endl;
                delete RI;
                return 0; // allow the dtors of R to be executed
            }
        }//catch

        switch(command){

        case SELF_INSERT:
            try{
                string token = R.get_string(0);
                if((token.length() == 2) && (token[0] == '\'')){
                    switch(token[1]){
                    case 'd': throw(EMPTY_DEC_STRING);
                    case 'b': throw(EMPTY_BIN_STRING);
                    default: throw(BAD_HEX_STRING);
                    }//switch
                }//if
                else if((token.length() > 2) && (token[0] == '\'')){
                    suffix = token.substr(2);
                    switch(token[1]){
                    case 'd':
                        for(char ch : suffix){
                            if(! isdigit(ch)){
                                throw(BAD_DEC_STRING);
                            }//if
                        }//for
                        A.set_to('d', suffix);
                        break;
                    case 'b':
                        for(char ch : suffix){
                            if(! isbindigit(ch)){
                                throw(BAD_BIN_STRING);
                            }//if
                        }//for
                        A.set_to('b', suffix);
                        break;
                    default:
                        throw(BAD_HEX_STRING);
                    }//switch
                }//else if
                else{
                    for(char &ch : token){
                        if(! isxdigit(ch)){
                            throw(BAD_HEX_STRING);
                        }//if
                        ch = tolower(ch);
                    }//for
                    A.set_to('h', token);
                }//else
                A.print();
            }//try
            __print_errmsg;
            break;

        case CMD_QUIT:
            delete RI;
            return 0; // allow the dtors of R to be executed

        case CMD_HELP:
            if(R.get_number_of_args()){
                const char *token = R.get_string(0);
                if(strlen(token) != 1){
                    __error << "help on that topic not available";
                    break;
                }//if
                if(help_on.find(token[0]) == help_on.end()){
                    __error << "help on that topic not available";
                }else{
                    cout << "       " << help_on[token[0]];
                }//else
            }else{
                cout << main_help;
            }//else
            break;

        case CMD_PRINT:
            A.print();
            break;

        case CMD_VERSION:
            cout << version_text;
            break;

        case CMD_UNDO:
            try{
                A.undo();
                A.print();
            }__print_errmsg;
            break;

        case CMD_REDO:
            try{
                A.redo();
                A.print();
            }__print_errmsg;
            break;

        case CMD_INDICES:
            A.toggle_indices();
            A.print();
            break;

        case CMD_HILITE:
            switch(R.get_number_of_args()){
            case 0:
                __error << "highlighting command expects 1 or 2 arguments";
                break;
            case 1:
                try{
                    if(R.is_not_pos_dec(0)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.print(true, R.get_num(0), R.get_num(0));
                }__print_errmsg;
                break;
            default:
                try{
                    if(R.is_not_pos_dec(0) || R.is_not_pos_dec(1)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.print(true, R.get_num(0), R.get_num(1));
                }__print_errmsg;
                break;
            }//switch
            break;

        case CMD_PERM_HILITE:
            switch(R.get_number_of_args()){
            case 0:
                A.turn_off_perm_hilite();
                A.print();
                break;
            case 1:
                try{
                    if(R.is_not_pos_dec(0)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.turn_on_perm_hilite(R.get_num(0), R.get_num(0));
                    A.print();
                }__print_errmsg;
                break;
            default:
                try{
                    if(R.is_not_pos_dec(0) || R.is_not_pos_dec(1)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.turn_on_perm_hilite(R.get_num(0), R.get_num(1));
                    A.print();
                }__print_errmsg;
                break;
            }//default
            break;

        case CMD_WIDTH:
            if(R.get_number_of_args()){
                try{
                    if(R.is_not_pos_dec(0)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.set_width(R.get_num(0));
                    A.print();
                }__print_errmsg;
            }else{
                if(A.get_width()){
                    cout << (uint)A.get_width();
                }else{
                    cout << "variable";
                }//else
            }//else
            break;

        case CMD_HISTORY_SIZE:
            if(R.get_number_of_args()){
                try{
                    if(R.is_not_pos_dec(0)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.resize_history(R.get_num(0));
                    cout << "undo history capacity set to "
                         << (uint)A.get_history_size();
                }__print_errmsg;
            }else{
                cout << (uint)A.get_history_size();
            }//else
            break;

        case CMD_PRINT_HISTORY:
            A.print_history();
            break;

        case CMD_INVERT:
            switch(R.get_number_of_args()){
            case 0:
                A.invert_all();
                A.print();
                break;
            case 1:
                try{
                    if(R.is_not_pos_dec(0)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.invert(R.get_num(0), R.get_num(0));
                    A.print();
                }__print_errmsg;
                break;
            default:
                try{
                    if(R.is_not_pos_dec(0) || R.is_not_pos_dec(1)){
                        throw(IS_NOT_POS_DEC);
                    }//if
                    A.invert(R.get_num(0), R.get_num(1));
                    A.print();
                }__print_errmsg;
                break;
            }//switch
            break;

        case CMD_REPLACE:
            if(R.get_number_of_args() == 0){
                __error << "replacement command expects an argument";
            }else{
                try{
                    string token = R.get_string(0);
                    if((token.length() == 2) && (token[0] == '\'')){
                        switch(token[1]){
                        case 'd': throw(EMPTY_DEC_STRING);
                        case 'b': throw(EMPTY_BIN_STRING);
                        default: throw(BAD_HEX_STRING);
                        }//switch
                    }//if
                    else if((token.length() > 2) && (token[0] == '\'')){
                        suffix = token.substr(2);
                        switch(token[1]){
                        case 'd':
                            for(char ch : suffix){
                                if(! isdigit(ch)){
                                    throw(BAD_DEC_STRING);
                                }//if
                            }//for
                            A.replace('d', suffix);
                            break;
                        case 'b':
                            for(char ch : suffix){
                                if(! isbindigit(ch)){
                                    throw(BAD_BIN_STRING);
                                }//if
                            }//for
                            A.replace('b', suffix);
                            break;
                        default:
                            throw(BAD_HEX_STRING);
                        }//switch
                    }//else if
                    else{
                        for(char &ch : token){
                            if(! isxdigit(ch)){
                                throw(BAD_HEX_STRING);
                            }//if
                            ch = tolower(ch);
                        }//for
                        A.replace('h', token);
                    }//else
                    A.print();
                }__print_errmsg;
            } /* end else */
            break;

        case CMD_SPLIT_FIELDS:
            if(! RI){
                __error << "need to load register specs first";
                break;
            }
            if(R.get_number_of_args()){
                try{
                    A.print_register(RI, R.get_string(0));
                    last_register = R.get_string(0);
                }__print_errmsg;
            }else{
                A.print_registers(RI);
            }//else
            break;

        case CMD_SPLIT_REPEAT:
            if(! RI){
                __error << "need to load register specs first";
                break;
            }
            try{
                A.print_register(RI, last_register);
            }__print_errmsg;
            break;

        case CMD_LOAD_SPECS:
            if(R.get_number_of_args() == 0){
                __error << "load command expects an argument";
            }else{
                delete RI;
                RI = NULL;
                try{
                    RI = new reg_info(R.get_string(0));
                    A.print_registers(RI);
                }//try
                catch(exception &e){
                    __error << e.what();
                }//catch
            }//else
            break;

        default:
            __error << errmsg[BAD_HEX_STRING];

        }//switch
    }//while (main loop)
}//main

/* aczutro ************************************************************* end */
