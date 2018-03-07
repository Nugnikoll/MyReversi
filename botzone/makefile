WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres
RM = rm -f

INC = 
CFLAGS = 
CXXFLAGS = -Wall -std=c++11 -O2
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS =

SRCDIR = ../cpp/
OBJDIR = obj/
OUTDIR = ./

SRC = $(wildcard $(SRCDIR)*.cc) main.cc
OBJ = $(patsubst %.cc, %.o, $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRC)))
OUT = $(OUTDIR)reversi

.PHONY: all clean before

all: $(OUT)

clean:
	-$(RM) $(OBJDIR)*.o
	-$(RM) $(addprefix $(OUTDIR), *.o *.exe *.dll *.so *.pyd *.log *.out) $(OUT)

before:
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	test -d $(OUTDIR) || mkdir -p $(OUTDIR)

$(OUT): before $(OBJ)
	$(LD) $(LDFLAGS) $(LIBDIR) $(OBJ) -o $@

$(OBJDIR)type.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)type.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)reversi.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)reversi.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)flip.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)flip.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)search.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)search.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)pattern.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)pattern.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)tree.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)tree.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM main.cc | gawk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

