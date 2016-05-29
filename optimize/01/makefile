.PHONY: all clean clean_all

all: test.png

clean_all: clean
	rm gmon.out

clean:
	rm gtest test.dot rtest.dot test.png

test.exe: ../test.exe
	cp ../test.exe ./test.exe

gmon.out: test.exe in.txt
	cat in.txt | ./test.exe

gtest: test.exe gmon.out
	gprof test.exe gmon.out > gtest

test.dot: gtest
	gprof2dot gtest > test.dot

rtest.dot: test.dot
	replace.py test.dot rtest.dot

test.png: rtest.dot
	dot -Tpng -otest.png rtest.dot