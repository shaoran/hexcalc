/* shaoran -*- c-basic-offset:4 -*-
 * vim: set sw=4 ts=4 sts=4 expandtab :
 *
 * hexcalc - a handy hex calculator and register contents visualiser
 *           for assembly programmers
 *
 * Copyright 2014 - 2017 Alexander Czutro, Pablo Yanez Trujillo
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
#ifndef __READLINE_INTERFACE
#define __READLINE_INTERFACE

#include <vector>
#include <string>

namespace rl_iface {

	/**
	 * this method parses the input stream as a shell command line. It supports quotes and an escape character.
	 *
	 * examples:
	 *  let escape =  '\\', quotes = "\"'", delim = " "
	 *
	 *  stream = c1 c2 "c3 c4" c5\ c6 'c7 c8 "c9 c10"'
	 *
	 *  tokens:
	 *    - c1
	 *    - c2
	 *    - c3 c4
	 *    - c5 c6
	 *    - c7 c8 "c9 c10"
	 *
	 *    Note: escaping only works for quotes, escape and delimiter characters. It will be ignored with
	 *    other characters
	 *
	 *    returns 1 on success, 0 otherwise. tokens is cleared on entry regardless of the return value
	 */
	int tokenizer(std::vector<std::string> &tokens, const char *stream, const char *delim, char escape, const char *quotes);

    /**
     * returns all previous tokens depending where the cursor is placed (inidatced by start)
     */
    int get_previous_tokens(std::vector<std::string> &tokens, const char *line, int start);

	/**
	 * setups variables needed in order to do multilevel completion
	 */
	void setup_readline_interface(void);

	/**
	 * The main interface to readline's completion framework.
	 */
	char **hexcalc_complete(const char *text, int start, int end);


	/**
	 * This is a generator for commands with no arguments, returns NULL.
	 */
	char *simple_command_generator(const char *text, int state);

	/**
	 * This is a generator for the 'i', 'l' and 'L' commands
	 */
	char *ilL_command_generator(const char *text, int state);

	/**
	 * This is a generator for the '=' commands
	 */
	char *equals_command_generator(const char *text, int state);

	/**
	 * This is a generator for the 's' commands
	 */
	char *s_command_generator(const char *text, int state);

	/**
	 * This is a generator for the 'w' commands
	 */
	char *w_command_generator(const char *text, int state);

	/**
	 * This is a generator for the 'R' commands
	 */
	char *R_command_generator(const char *text, int state);

	/**
	 * This is a generator for command list
	 */
	char *command_list_generator(const char *text, int state);


	// helpers


	void ltrim(char *text);
	void rtrim(char *text);
	void trim(char *text);
};

extern void* rl_CLI_object;

#define rl_set_cli(x) rl_CLI_object = (void*) (x);

#endif
