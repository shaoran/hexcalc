ACLOCAL_AMFLAGS = -I .m4

bin_PROGRAMS = hexcalc

hexcalc_SOURCES = src/command-line-reader.cc\
                  src/core.cc src/core-state.cc\
                  src/hexcalc.cc src/history-index.cc\
                  src/reg-info.cc\
                  include/colours.hh\
                  include/core-state.hh\
                  include/faces.hh\
                  include/reg-info.hh\
                  include/command-line-reader.hh\
                  include/core.hh\
                  include/exceptions.hh\
                  include/history-index.hh

if WITH_READLINE
hexcalc_SOURCES += src/readline-interface.cc include/readline-interface.hh
endif

hexcalc_CXXFLAGS = -Wall -O3 -std=c++14 -I$(srcdir)/include $(AM_CXXFLAGS)

EXTRA_DIST=specs/sample-spec README.md
