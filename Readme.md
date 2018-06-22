## Reversi

### Screenshot
![](./image/reversi.png)

### Overview
Reversi (also called Othello) is a kind of strategy board game which involves play by two parties on an eight-by-eight square grid. For details of reversi please go to [https://en.wikipedia.org/wiki/Reversi](https://en.wikipedia.org/wiki/Reversi).

This reversi project is my simple work. The kernel is written in C++11 whith a bit inline assembly. The GUI is witten in python. The sources of this project can be merged in to a single file and submitted to [Botzone](https://botzone.org) which is a online platform for competition of differnt bots made by students and other programming learners.

### Build

- Prerequisites
	- A decent C++ compiler  
		C++11 must be supported by this compiler. Inline assembly will not be used when compiled by Visual Studio as MS compiler supports neither 64-bit inline assembly nor AT&T flavor syntax.
	- [Python3](https://www.python.org/) (required by the GUI written in Python and some simple scripts)
	- [swig](http://swig.org/) (required by the GUI written in Python)
	- [wxPython](https://www.wxpython.org/) (required by the GUI written in Python)
	- [CMake](https://cmake.org/)

- Build reversi with GUI
	1. Compile the source code and generate dynamic link library _reversi.so  
		`cd build`
		`cmake .` (To generate MinGW Makefiles on windows, it should be `cmake -G"MinGW Makefiles" .`)
		`make`
		`cd ..`
	2. Execute the project
		`cd python`
		`python3 main.py`

- Build reversi for [Botzone](https://botzone.org)
	1. Enter the directory botzone/
		`cd botzone`
	2. Merge sources into a single file "main_merge.cc"
		`python3 merge.py`

### Development

- File Structure  

	cpp/reversi.h: class board
	│	cpp/flip.cc board::flip()
	│	cpp/reversi.cc
	│	cpp/search.cc board::search()
	│	...
	│
	└───python/game.py: class game
	│		│
	│		└───python/main.py (python GUI project)
	│
	└───botzone/main.cc (Botzone online project)

- Profile (only available on UNIX-like systems)
	- Prerequisites
		- [graphiz](https://www.graphviz.org/)
		- [gprof2dot](https://github.com/jrfonseca/gprof2dot)

	1. Enter the directory profile/
		`cd profile`
	2. Compile the source code and generate files "prof.log" "test.png"
		`make`
