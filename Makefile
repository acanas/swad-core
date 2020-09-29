##########################################################################
#
# Makefile to compile SWAD core
#
###############################################################################
#                                                                             #
#   SWAD (Shared Workspace At a Distance),                                    #
#   is a web platform developed at the University of Granada (Spain),         #
#   and used to support university teaching.                                  #
#                                                                             #
#   This file is part of SWAD core.                                           #
#   Copyright (C) 1999-2020 Antonio Cañas Vargas                              #
#                                                                             #
#   This program is free software: you can redistribute it and/or modify      #
#   it under the terms of the GNU Affero General Public License as            #
#   published by the Free Software Foundation, either version 3 of the        #
#   License, or (at your option) any later version.                           #
#                                                                             #
#   This program is distributed in the hope that it will be useful,           #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of            #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
#   GNU Affero General Public License for more details.                       #
#                                                                             #
#   You should have received a copy of the GNU Affero General Public License  #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.     #
#                                                                             #
###############################################################################

OBJS = $(shell find . -maxdepth 1 -type f -iname '*.c' | sed 's/\.c/\.o/g')
LANGSRC = swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
SOAPOBJS = soap/soapC.o soap/soapServer.o
SHAOBJS = sha2/sha2.o
CC = gcc

LANGS = swad_ca swad_de swad_en swad_es swad_fr swad_gn swad_it swad_pl swad_pt

# LIBS when using MySQL:
#LIBS = -lmysqlclient -lz -L/usr/lib64/mysql -lm -lgsoap

# LIBS when using MariaDB (also valid with MySQL):
LIBS = -lssl -lcrypto -lpthread -lrt -lmysqlclient -lz -L/usr/lib64/mysql -lm -lgsoap

CFLAGS = -Wall -Wextra -mtune=native -O2 -s

.PHONY: all clean
all: $(LANGS)

_pos = $(if $(findstring $1,$2),$(call _pos,$1,\
       $(wordlist 2,$(words $2),$2),x $3),$3)
pos = $(words $(call _pos,$1,$2))
$(LANGS): $(OBJS) $(SOAPOBJS) $(SHAOBJS)
	$(CC) $(CFLAGS) -c -D L=$(call pos,$@,$(LANGS)) $(LANGSRC)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

clean:
	rm -f swad $(LANGS) $(OBJS) $(SOAPOBJS) $(SHAOBJS)
