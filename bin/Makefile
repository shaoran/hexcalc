# aczutro
#
# hexcalc - a handy hex calculator and register contents visualiser
#           for assembly programmers
#
# Copyright 2014 - 2017 Alexander Czutro
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public Licence as published by
# the Free Software Foundation, either version 3 of the Licence, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public Licence for more details.
#
# You should have received a copy of the GNU General Public Licence
# along with this program.  If not, see <http://www.gnu.org/licences/>.
#
################################################################### aczutro ###

.SUFFIXES:	.cc .o

### some variables ############################################################

# paths and targets

BASE = ..
INCLUDE = $(BASE)/include
LIB = $(BASE)/lib
SRC = $(BASE)/src

MAIN = hexcalc

TAGS = $(BASE)/.TAGS

# compiler

CCC = g++
DEFINITIONS =
CFLAGS = -c -Wall -O3 -std=c++14 $(DEFINITIONS) -I$(INCLUDE)
LFLAGS = -L$(LIB)

### rules #####################################################################

all:			$(LIB) $(MAIN) $(TAGS)

$(LIB):
			mkdir $@

$(MAIN):			$(LIB)/$(MAIN).o \
				$(LIB)/command-line-reader.o \
				$(LIB)/history-index.o \
				$(LIB)/core-state.o \
				$(LIB)/core.o \
				$(LIB)/reg-info.o
			$(CCC) -o $@ $^ $(LFLAGS)
			strip $@

$(LIB)/$(MAIN).o:		$(SRC)/$(MAIN).cc \
				$(INCLUDE)/faces.hh \
				$(INCLUDE)/colours.hh \
				$(INCLUDE)/exceptions.hh \
				$(INCLUDE)/command-line-reader.hh \
				$(INCLUDE)/core.hh \
				$(INCLUDE)/core-state.hh \
				$(INCLUDE)/history-index.hh \
				$(INCLUDE)/reg-info.hh
			$(CCC) -o $@ $< $(CFLAGS)

$(LIB)/core.o:			$(SRC)/core.cc $(INCLUDE)/core.hh \
				$(INCLUDE)/core-state.hh \
				$(INCLUDE)/history-index.hh \
				$(INCLUDE)/reg-info.hh \
				$(INCLUDE)/faces.hh \
				$(INCLUDE)/colours.hh \
				$(INCLUDE)/exceptions.hh
			$(CCC) -o $@ $< $(CFLAGS)

$(LIB)/command-line-reader.o:	$(SRC)/command-line-reader.cc \
				$(INCLUDE)/command-line-reader.hh \
				$(INCLUDE)/exceptions.hh
			$(CCC) -o $@ $< $(CFLAGS)

$(LIB)/%.o:		$(SRC)/%.cc $(INCLUDE)/%.hh
			$(CCC) -o $@ $< $(CFLAGS)

$(TAGS):		$(INCLUDE)/* $(SRC)/*
			etags --output=$@ $^

# cleaning

.PHONY:	clean

clean:
	@rm -rf $(MAIN) $(LIB)

### aczutro ########################################################### end ###
