WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres
RM = rm -f

INC = 
CFLAGS = 
CXXFLAGS = -Wall -std=c++17 -O2 -DUSE_ASM -DUSE_ASM_BMI2 -DUSE_ASM_AVX2 -DUSE_TERMINATE
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -pthread

SRCDIR = ../cpp/
OBJDIR = obj/
OUTDIR = ./

name = reversi
SRC = $(wildcard $(SRCDIR)*.cc) main.cc
OBJ = $(patsubst %.cc, %.o, $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRC)))
OUT = $(OUTDIR)$(name)

get_depend = $(wordlist 2,100,$(shell g++ -std=c++17 -MM $(1) | awk -f parse.awk))

.PHONY: all online offline clean before

all: before $(OUT)

online: all

offline: all

clean:
	-$(RM) $(OBJDIR)*.o *_merge.cc
	-$(RM) $(addprefix $(OUTDIR), *.o *.exe *.dll *.so *.pyd *.log *.out) $(OUT)

before:
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	test -d $(OUTDIR) || mkdir -p $(OUTDIR)

ifeq ($(filter online, $(MAKECMDGOALS)), online)

$(OUT): $(OBJDIR)jsoncpp.o $(OBJDIR)main_merge.o
	$(LD) $(LDFLAGS) $^ -o $@

else

$(OUT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

endif

$(OBJDIR)jsoncpp.o: $(call get_depend, $(SRCDIR)jsoncpp.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)board.o: $(call get_depend, $(SRCDIR)board.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)flip.o: $(call get_depend, $(SRCDIR)flip.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)search.o: $(call get_depend, $(SRCDIR)search.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)sample.o: $(call get_depend, $(SRCDIR)sample.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)pattern.o: $(call get_depend, $(SRCDIR)pattern.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: $(call get_depend, main.cc)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)main_merge.o: main_merge.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

main_merge.cc: $(SRC) merge.py
	python3 merge.py
