#include <cmath>

#include "board.h"

struct node{
	node* parent;
	node* sibling;
	node* child;
	board brd;
	bool color;
	short pos;
	short count_leaf;
	float value;
	int win_white, win_black;

	node(node* const& _parent, cboard _brd, cbool _color, cshort _pos, cfloat value)
		: parent(parent), sibling(NULL), child(NULL), brd(_board), color(_color),
		pos(pos), count_leaf(0), value(_value), win_black(0), win_white(0){}

	void destroy(){
		if(this){
			sibling->destroy();
			child->destroy();
			delete this;
		}
	}

	int& get_win(cbool color){
		return *(&win_white + color);
	}
	cint get_win(cbool color)const{
		return *(&win_white + color);
	}

	void expand(cbool color){
		ull brd_move = brd.get_move(color);
		while(brd_move){
			
			brd_move &= brd_move - 1;
		}
	}

	void descend(){
	}
};
