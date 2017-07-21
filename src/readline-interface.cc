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
#include <iostream>
#include <cctype>
#include <cstring>
#include <map>
#include <algorithm>

#include <readline/readline.h>
#include <readline/history.h>

#include <readline-interface.hh>


static std::map<std::string, rl_compentry_func_t*> __command_map;
static std::vector<std::string> __command_list;


int rl_iface::tokenizer(std::vector<std::string> &tokens, const char *stream, const char *delim, char escape, const char *quotes)
{
	tokens.clear();

	if(stream == NULL || delim == NULL || quotes == NULL)
		return 0;


	int len = strlen(stream);
	volatile bool escape_state = false;
	volatile bool quote_state = false;
	volatile char quote_char = 0;

	char token[len+1] = { 0 };
	int token_len = 0;

	for(int i = 0; i < len; ++i)
	{
		char c = stream[i];
		if(escape_state)
		{
			token[token_len++] = c;
			escape_state = false;
			continue;
		} else {
			if(c == escape)
			{
				escape_state = true;
				continue;
			}

			const char *q_f = strchr(quotes, c);

			if(q_f)
			{
				if(quote_state)
				{
					if(c == quote_char)
					{
						quote_state = false;
						quote_char = 0;
						continue; // ignore quote char
					} // treat this a regular character
				} else {
					quote_state = true;
					quote_char = *q_f;
					continue; // ignore quote char
				}

			}

			if(strchr(delim, c) && !quote_state)
			{
				token[token_len] = 0;

				if(token_len > 0)
					tokens.push_back(token);

				token[0] = 0;
				token_len = 0;
				quote_state = false;
				quote_char = 0;
				continue;
			}

			token[token_len++] = c;
		}
	}

	if(token_len)
	{
		token[token_len] = 0;
		tokens.push_back(token);
	}

	return 1;
}

char **rl_iface::hexcalc_complete(const char *text, int start, int end)
{
    std::vector<std::string> tokens;

    rl_attempted_completion_over = 1;


    if(tokenizer(tokens, rl_line_buffer, " ", '\\', "\"'") == 0)
        return NULL;

    if(tokens.size() == 0)
        return rl_completion_matches(text, __command_map["h"]);

    char *cmd = strdup(tokens[0].c_str());

    if(cmd == NULL)
        return NULL;

    trim(cmd);

    if(__command_map.find(cmd) == __command_map.end())
    {
        free(cmd);
        return NULL;
    }

    std::string cmd_s = cmd;
    free(cmd);

    return rl_completion_matches(text, __command_map[cmd_s]);
}


bool less_no_case(const std::string &x, const std::string &y)
{
    return (strcasecmp(x.c_str(), y.c_str()) < 0);
}

void rl_iface::setup_readline_interface(void)
{
	__command_map.clear();

	__command_map["p"] = simple_command_generator;
	__command_map["I"] = simple_command_generator;
	__command_map["w"] = w_command_generator;
	__command_map["i"] = ilL_command_generator;
	__command_map["l"] = ilL_command_generator;
	__command_map["L"] = ilL_command_generator;
	__command_map["="] = equals_command_generator;
	__command_map["u"] = simple_command_generator;
	__command_map["r"] = simple_command_generator;
	__command_map["U"] = simple_command_generator;
	__command_map["s"] = s_command_generator;
	__command_map["S"] = simple_command_generator;
	__command_map["R"] = R_command_generator;
	__command_map["h"] = command_list_generator;
	__command_map["v"] = simple_command_generator;
	__command_map["q"] = simple_command_generator;
	__command_map["h"] = command_list_generator;

    __command_list.clear();

    for(auto i = __command_map.begin(); i != __command_map.end(); ++i)
        __command_list.push_back(i->first);

    std::sort(__command_list.begin(), __command_list.end(), less_no_case);
}

char *rl_iface::simple_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::ilL_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::equals_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::s_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::w_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::U_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::R_command_generator(const char *text, int state)
{
	return NULL;
}

char *rl_iface::command_list_generator(const char *text, int state)
{
    static int size, idx;

    if(state == 0)
    {
        size = __command_list.size();
        idx = 0;
    }

    if(idx >= size)
        return NULL;

    return strdup(__command_list[idx++].c_str());
}



void rl_iface::ltrim(char *text)
{
	if(text == NULL)
		return;

	int i, len = strlen(text);

	for(i = 0; i < len; ++i)
	{
		if(!isblank(text[i]))
			break;
	}

	if(i >= len)
	{
		*text = 0;
		return;
	}


	memmove(text, text + i, len - i + 1);
}

void rl_iface::rtrim(char *text)
{
	if(text == NULL)
		return;

	int i, len = strlen(text);

	for(i = len - 1; i > -1; --i)
	{
		if(!isblank(text[i]))
			break;
	}

	if(i == -1)
	{
		*text = 0;
		return;
	}

	text[i + 1] = 0;
}

void rl_iface::trim(char *text)
{
	rl_iface::ltrim(text);
	rl_iface::rtrim(text);
}
