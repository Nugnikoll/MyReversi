#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "reversi_tcl.h"

using namespace std;

#ifdef DEBUG_SEARCH
	ofstream out;
#endif

bool flag_quit = false;

void quit(){
	#ifdef DEBUG_SEARCH
		out.close();
	#endif
	flag_quit = true;
}

void game_gui::show(){
	brd.print();
}

void term_print(const string& str){
	cout << str << endl;
}
void log_print(const string& str){}
void show_choice(const vector<choice>& choices){}
void load_book(const string& path){}

int main(){

	#ifdef DEBUG_SEARCH
		out.open("out.html");
	#endif

	board::flag_unicode = false;

	string str,s;
	string::size_type pos;
	while(cout << ">>", getline(cin,str)){
		pos = str.find_last_not_of(" \n\t");
		if(pos != str.npos){
			if(str[pos] == '\\'){
				while(getline(cin,s)){
					pos = s.find_last_not_of(" \n\t");
					if(pos == s.npos){
						break;
					}
					if(s[pos] != '\\'){
						break;
					}
					str += '\n';
					str += s;
				}
				if(!cin){
					break;
				}
				str += '\n';
				str += s;
			}
		}
		process(str);
		if(flag_quit)
			break;
	}

	#ifdef DEBUG_SEARCH
		out.close();
	#endif

	return 0;
}
