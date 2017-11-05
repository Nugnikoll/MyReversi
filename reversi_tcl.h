#ifndef REVERSI_TCL_H
#define REVERSI_TCL_H

#include "game.h"
#include "pattern.h"

//import
class game_gui:public game{
public:
	virtual void show();
	virtual void print_log(const string&);
	virtual coordinate play_other(cmethod mthd,cbool color,cshort depth = -1);
};

//export
void process(const string& str);

//import
void quit();
void print_term(const string& str);
void print_log(const string& str);
void print_status(const string& str);
void show_choice(const vector<choice>& choices);
void load_book(const string& path);

//export
extern game_gui mygame;
extern tree book;
extern bool flag_echo;

#endif //REVERSI_TCL_H