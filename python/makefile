pylib_path = /lib/python2.7/config/
pyhead_path = /usr/include/python2.7/
src_path = ../

src = $(addprefix $(src_path),type.cc reversi.cc flip.cc search.cc pattern.cc tree.cc)
head = $(addprefix $(src_path),type.h reversi.h pattern.h game.h tree.h)
obj = reversi_wrap.o type.o reversi.o flip.o search.o pattern.o tree.o
target = _reversi.cp35-win_amd64.pyd

cc = gcc
cxx = g++
compile_option = -Wall -std=c++11 -O2 -c

.PHONY: all clean

all: $(target)

clean:
	-del *.o *.cxx *.pyd *.pyc *.exe *.dll

$(target): reversi_wrap.cxx setup.py
	python setup.py build_ext --inplace --compiler=mingw32

reversi_wrap.cxx: reversi.i $(head)
	swig -python -c++ -threads $<