#ifndef REVERSI_TCL_H
#define REVERSI_TCL_H

#include "game.h"
#include "pattern.h"

//import
class game_gui:public game{
public:
	virtual void show();
	virtual void log_print(const string&);
};

//export
void process(const string& str);

//import
void quit();
void term_print(const string& str);
void log_print(const string& str);
void status_print(const string& str);
void show_choice(const vector<choice>& choices);
void load_book(const string& path);

//export
extern game_gui mygame;
extern group grp;
extern tree book;
extern bool flag_text_term;

#endif //REVERSI_TCL_H