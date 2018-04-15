WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres
RM = rm -f

INC = -I/usr/include/python3.5/
CFLAGS =
CXXFLAGS = -Wall -g -Og -std=c++11 -fPIC
RESINC =
LIBDIR = -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/
LIB = 
LDFLAGS = -shared -lpython3.5

SRCDIR = ../cpp/
OBJDIR = obj/
OUTDIR = ./

SRC = $(wildcard $(SRCDIR)*.cc)
HEAD = $(wildcard $(SRCDIR)*.h)
OBJ = $(OBJDIR)reversi_wrap.o $(patsubst $(SRCDIR)%.cc, $(OBJDIR)%.o, $(SRC))
OUT = $(OUTDIR)_reversi.so

.PHONY: all clean before

all: before $(OUT)

before:
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	test -d $(OUTDIR) || mkdir -p $(OUTDIR)

clean:
	-$(RM) $(OBJDIR)*.o
	-$(RM) $(addprefix $(OUTDIR), *wrap* reversi.py *.exe *.dll *.so *.pyd)

$(OUT): $(OBJ)
	$(LD) $(LDFLAGS) $(LIBDIR) $(OBJ) -o $@

$(OBJDIR)reversi.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)reversi.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)flip.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)flip.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)search.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)search.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)pattern.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)pattern.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUTDIR)reversi_wrap.cxx: $(OUTDIR)reversi.i $(HEAD)
	swig -python -c++ -threads -o $@ $<

$(OBJDIR)reversi_wrap.o: $(OUTDIR)reversi_wrap.cxx
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@
