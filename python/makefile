src_path = ../
obj_path = ../obj/
bin_path = ../bin/
py_path = ./
pylib_path = /usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/
pyhead_path = /usr/include/python3.5/

src = $(addprefix $(src_path), type.cc reversi.cc flip.cc search.cc pattern.cc tree.cc)
head = $(addprefix $(src_path),type.h asm.h reversi.h matrix.h pattern.h game.h tree.h)
obj = $(addprefix $(obj_path), reversi_wrap.o type.o reversi.o flip.o search.o pattern.o tree.o)
target = $(py_path)_reversi.so

option = -Wall -g -Og -std=c++11 -fPIC -c

.PHONY: all clean

all: $(target)

clean:
	-$(RM) ../obj/*.o
	-$(RM) *.cxx *.exe *.dll *.so *.pyd

$(target): $(obj)
	$(CXX) -shared -L$(pylib_path) $(obj) -lpython3.5 -o $@

$(obj_path)type.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(src_path)type.cc | gawk -f parse.awk))
	$(CXX) $(option) $< -o $@

$(obj_path)reversi.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(src_path)reversi.cc | gawk -f parse.awk))
	$(CXX) $(option) $< -o $@

$(obj_path)flip.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(src_path)flip.cc | gawk -f parse.awk))
	$(CXX) $(option) $< -o $@

$(obj_path)search.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(src_path)search.cc | gawk -f parse.awk))
	$(CXX) $(option) $< -o $@

$(obj_path)pattern.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(src_path)pattern.cc | gawk -f parse.awk))
	$(CXX) $(option) $< -o $@

$(obj_path)tree.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(src_path)tree.cc | gawk -f parse.awk))
	$(CXX) $(option) $< -o $@

$(py_path)reversi_wrap.cxx: $(py_path)reversi.i $(head)
	swig -python -c++ -threads -o $@ $<

$(obj_path)reversi_wrap.o: $(py_path)reversi_wrap.cxx
	$(CXX) $(option) -I$(pyhead_path) $< -o $@
