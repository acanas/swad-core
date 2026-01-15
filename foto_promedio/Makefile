#
#  SWAD. Calculo de la foto promedio y la foto mediana
#
#  Copyright (C) 2010-2012  Daniel J. Calandria Hernández &
#                      	    Antonio Cañas Vargas
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#CXX = g++
#CC=g++
CXX = g++-9
CC=g++-9
CFLAGS=-Wall -O3 $(shell pkg-config --cflags opencv)
#CXXFLAGS = -Wall -O3 $(shell pkg-config --cflags opencv)
CXXFLAGS = -Wall -O3 \
  $(shell pkg-config --cflags opencv) \
  -I/usr/local/opencv-3.3.0/include \
  -Wno-class-memaccess



LDFLAGS =$(shell pkg-config --libs opencv)
TARGETS=foto_promedio foto_mediana


all: foto_promedio foto_mediana
foto_promedio: foto_promedio.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
foto_mediana:  foto_mediana.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
clean:
	rm -f foto_promedio foto_mediana *.o


