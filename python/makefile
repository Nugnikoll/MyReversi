pylib_path = /lib/python2.7/config/
pyhead_path = /usr/include/python2.7/
src_path = ../

src = $(addprefix $(src_path),reversi.cc pattern.cc)
head = $(addprefix $(src_path),reversi.h pattern.h game.h)
obj = reversi_wrap.o reversi.o pattern.o
target = _reversi.dll

cc = gcc
cxx = g++
compile_option = -Wall -std=c++11 -O2 -c

.PHONY: all clean

all: $(target)

clean:
	-rm -f *.o *.cxx *.pyd *.exe *.dll

$(target): $(obj)
	$(cxx) -shared -L$(pylib_path) $(obj) -lpython2.7 -o $@

$(obj): $(head)

reversi.o: $(src_path)reversi.cc
	$(cxx) $(compile_option) $< -o $@

pattern.o: $(src_path)pattern.cc
	$(cxx) $(compile_option) $< -o $@

reversi_wrap.o: reversi_wrap.cxx
	$(cxx) $(compile_option) -I$(pyhead_path) $< -o $@

reversi_wrap.cxx: reversi.i $(head)
	swig -python -c++ -threads $<