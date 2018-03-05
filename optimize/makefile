.PHONY: all clean clean_all

all: test.png

clean_all: clean
	rm gmon.out

clean:
	rm prof.log test.dot rtest.dot test.png

test.exe: ../bin/Debug/reversi_json.exe
	cp ../bin/Debug/reversi_json.exe ./test.exe

gmon.out: test.exe in.txt
	cat in.txt | ./test.exe

prof.log: test.exe gmon.out
	gprof test.exe gmon.out > prof.log

test.dot: prof.log
	gprof2dot prof.log > test.dot

rtest.dot: test.dot
	python replace.py test.dot rtest.dot

test.png: rtest.dot
	dot -Tpng -otest.png rtest.dot