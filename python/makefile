pylib_path = /lib/python2.7/config/
pyhead_path = /usr/include/python2.7/
src_path = ../

src = $(addprefix $(src_path),type.cc reversi.cc flip.cc search.cc pattern.cc tree.cc)
head = $(addprefix $(src_path),type.h reversi.h pattern.h game.h tree.h)
obj = reversi_wrap.o type.o reversi.o flip.o search.o pattern.o tree.o
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

type.o: $(src_path)type.cc
	$(cxx) $(compile_option) $< -o $@

reversi.o: $(src_path)reversi.cc
	$(cxx) $(compile_option) $< -o $@

flip.o: $(src_path)flip.cc
	$(cxx) $(compile_option) $< -o $@

search.o: $(src_path)search.cc
	$(cxx) $(compile_option) $< -o $@

pattern.o: $(src_path)pattern.cc
	$(cxx) $(compile_option) $< -o $@

tree.o: $(src_path)tree.cc
	$(cxx) $(compile_option) $< -o $@

reversi_wrap.o: reversi_wrap.cxx
	$(cxx) $(compile_option) -I$(pyhead_path) $< -o $@

reversi_wrap.cxx: reversi.i $(head)
	swig -python -c++ -threads $<