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

#include <readline/readline.h>
#include <readline/history.h>

#include <readline-interface.hh>


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
