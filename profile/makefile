WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres
RM = rm -f

INC = 
CFLAGS = 
CXXFLAGS = -Wall -pg -std=c++11 -fno-omit-frame-pointer -O2 -DUSE_ASM -DUSE_ASM_BMI2 -DUSE_ASM_AVX2
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -pg

SRCDIR = ../cpp/
OBJDIR = obj/
OUTDIR = ./

SRC = $(wildcard $(SRCDIR)*.cc) main.cc
OBJ = $(patsubst %.cc, %.o, $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SRC)))
OUT = $(OUTDIR)reversi

.PHONY: all clean before

all: test.png

clean:
	-$(RM) $(OBJDIR)*.o
	-$(RM) $(addprefix $(OUTDIR), *.o *.exe *.dll *.so *.pyd *.log *.dot *.png *.out) $(OUT)

test.png: test_replace.dot
	dot -Tpng -o$@ $<

test_replace.dot: test.dot
	python replace.py $< $@

test.dot: prof.log
	gprof2dot $< > $@

prof.log: $(OUT) gmon.out
	gprof $(OUT) gmon.out > $@

gmon.out: $(OUT) in.txt
	cat in.txt | $(OUT)

before:
	test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	test -d $(OUTDIR) || mkdir -p $(OUTDIR)

$(OUT): before $(OBJ)
	$(LD) $(LDFLAGS) $(LIBDIR) $(OBJ) -o $@

$(OBJDIR)board.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)board.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)flip.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)flip.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)search.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)search.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)pattern.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM $(SRCDIR)pattern.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: $(wordlist 2,100,$(shell g++ -std=c++11 -MM main.cc | awk -f parse.awk))
	$(CXX) $(CXXFLAGS) -c $< -o $@

