#include <string>
#include <cmath>
#include <random>
#include <fstream>
#include <iostream>

#include "board.h"

struct node{
	node* sibling;
	node* child;
	board brd;
	int count;
	int win_white, win_black;
	float value, bound;

	node() = default;
	node(cboard _board, cfloat _value)
		: sibling(NULL), child(NULL), brd(_board),
		count(0), win_white(0), win_black(0), value(_value), bound(0){}

	void destroy(){
		if(sibling){
			sibling->destroy();
		}
		if(child){
			child->destroy();
		}
	}

	void load(const string& str){
		ifstream fin;
		fin.open(str, ios::in | ios::binary);
		if(fin){
			load(fin);
		}
	}

	void load(ifstream& fin){
		#define _READ(var) fin.read((char *)(&var), sizeof(var))

		_READ(brd);
		_READ(count);
		_READ(win_white);
		_READ(win_black);
		_READ(value);
		_READ(bound);
		char flag;
		_READ(flag);
		if(flag & 1){
			child = new node;
			child->load(fin);
		}else{
			child = NULL;
		}
		if(flag & 2){
			sibling = new node;
			sibling->load(fin);
		}else{
			sibling = NULL;
		}

		#undef _READ
	}

	void save(const string& str)const{
		ofstream fout;
		fout.open(str, ios::out | ios::binary);
		if(fout){
			save(fout);
		}
	}

	void save(ofstream& fout)const{
		#define WRITE(var) fout.write((const char *)(&var), sizeof(var))

		WRITE(brd);
		WRITE(count);
		WRITE(win_white);
		WRITE(win_black);
		WRITE(value);
		WRITE(bound);
		char flag = 0;
		if(child){
			flag |= 1;
		}
		if(sibling){
			flag |= 2;
		}
		WRITE(flag);

		if(child){
			child->save(fout);
		}
		if(sibling){
			sibling->save(fout);
		}

		#undef WRITE
	}

	string info()const{
		return "count: " + to_string(count)
			+ " black: " + to_string(win_black)
			+ " white: " + to_string(win_white)
			+ " value: " + to_string(value);
	}

	int& get_win(cbool color){
		return *(&win_white + color);
	}
	int get_win(cbool color)const{
		return *(&win_white + color);
	}

	void expand(cbool color, cshort depth_search){
		ull brd_move = brd.get_move(color);
		node *ptr = this;
		board brd;
		ull pos;
		const method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_trans | mthd_mtdf | mthd_ptn);
		bound = inf;

		while(brd_move){
			fun_tzcnt(brd_move, pos);
			brd = this->brd;
			brd.flip(color, pos);
			if(ptr == this){
				ptr->child = new node(brd, brd.search(mthd, !color, depth_search));
				ptr = ptr->child;
			}else{
				ptr->sibling = new node(brd, brd.search(mthd, !color, depth_search));
				ptr = ptr->sibling;
			}
			bound = min(bound, ptr->value);
			fun_blsr(brd_move);
		}
		if(ptr == this){
			ptr->child = new node(this->brd, this->brd.search(mthd, !color, depth_search));
			bound = ptr->child->value;
		}
	}

	node* descend(cbool color)const{
		node *ptr, *p;
		float best = _inf;
		float temp;

		for(ptr = child; ptr; ptr = ptr->sibling){
			temp = 1 / (exp(ptr->value * 0.07) + 1)
				+ float(ptr->get_win(color) + 1) / (ptr->count + 2) / 3 
				+ sqrt(log(count) / (ptr->count + 1));
			if(temp > best){
				best = temp;
				p = ptr;
			}
		}
		return p;
	}

	int simulate(cbool color, cshort depth_search)const{
		const method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_trans | mthd_mtdf | mthd_ptn);
		board brd;
		board brd_next = this->brd;
		bool color_next = color;
		bool flag = true, flag_next = true;
		int count_black, count_white;

		while(flag || flag_next){
			brd = brd_next;
			brd_next.play(mthd, color_next, depth_search);
			color_next = !color_next;
			flag = flag_next;
			flag_next = (brd != brd_next);
		}

		count_black = brd.get_brd(true);
		count_white = brd.get_brd(false);
		return (count_black > count_white) - (count_white > count_black);
	}

	void grow(cbool color, cshort depth_search){
		bool color_next = color;
		node *ptr;
		node *table[100];
		node **table_end = table;

		for(ptr = this; ptr->child; color_next = !color_next){
			*table_end = ptr;
			++table_end;
			ptr = ptr->descend(color_next);
		}

		if(ptr->count > 2){
			ptr->expand(color_next, depth_search);
			*table_end = ptr;
			++table_end;
			ptr = ptr->descend(color_next);
			color_next = !color_next;
		}

		int value = ptr->simulate(color_next, depth_search);
		bool flag_black = (value > 0);
		bool flag_white = (value < 0);

		++ptr->count;
		ptr->win_black += flag_black;
		ptr->win_white += flag_white;

		while(table_end != table){
			--table_end;
			ptr = *table_end;
			++ptr->count;
			ptr->win_black += flag_black;
			ptr->win_white += flag_white;
		}
	}
};
