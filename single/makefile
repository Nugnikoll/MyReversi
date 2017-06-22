#gccdir = D:\\Program\\mingw\\x86_64-4.8.2-posix-seh-rt_v3-rev4\\mingw64\\bin\\
cc = $(gccdir)gcc
cxx = $(gccdir)g++

.PHONY: all clean

all: test.exe

clean:
	del smain.cc
	del test.exe

smain.cc: merge.py jmain.cc reversi.h reversi.cc pattern.h pattern.cc data\\data.h data\\data.cc
	py -3 merge.py jmain.cc smain.cc
	type data\data.cc >> smain.cc

test.exe: smain.cc
	$(cxx) -O2 -std=c++11 -x c++ smain.cc -o test