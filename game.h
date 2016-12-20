#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "reversi.h"
#include "tree.h"

using namespace std;

class game{
public:
	virtual ~game(){}

	board brd;
	vector<board> record;
	tree book;

	bool flag_auto_show = true;
	bool flag_auto_save = true;
	string prompt = ">>";

	virtual void show(){
		brd.print(cout);
	}
	void start(){
		brd.initial();
		if(flag_auto_show){
			show();
		}
	}
	virtual void push(){
		record.push_back(brd);
	}
	virtual bool pop(){
		if(record.empty()){
			return false;
		}else{
			brd = record.back();
			record.pop_back();
			if(flag_auto_show){
				show();
			}
			return true;
		}
	}
	brd_type bget(cbool color){
		return brd.bget(color);
	}
	void assign(cbrd_type _brd_black,cbrd_type _brd_white){
		if(flag_auto_save){
			push();
		}
		brd.assign(_brd_black,_brd_white);
		if(flag_auto_show){
			show();
		}
	}

	chessman get(cpos_type x,cpos_type y){
		return brd.get(x + (y << 3));
	}
	void set(cpos_type x, cpos_type y, cchessman chsman){
		if(flag_auto_save){
			push();
		}
		brd.set(x,y,chsman);
		if(flag_auto_show){
			show();
		}
	}
	void mirror(cbool is_horizontal){
		if(flag_auto_save){
			push();
		}
		brd.mirror(is_horizontal);
		if(flag_auto_show){
			show();
		}
	}
	void rotate_r(cint n90){
		if(flag_auto_save){
			push();
		}
		brd.rotate_r(n90);
		if(flag_auto_show){
			show();
		}
	}
	void rotate_l(cint n90){
		if(flag_auto_save){
			push();
		}
		brd.rotate_l(n90);
		if(flag_auto_show){
			show();
		}
	}
	virtual bool flip(cbool color, cpos_type x, cpos_type y){
		if(flag_auto_save){
			push();
		}
		bool result = brd.flip(color,x + (y << 3));
		if(result){
			if(flag_auto_show){
				show();
			}
		}else{
			if(flag_auto_save){
				do_pop();
			}
		}
		return result;
	}
	int count(cbool color){
		return brd.count(color);
	}
	brd_type get_move(cbool color){
		return brd.get_move(color);
	}
	int count_move(cbool color){
		return brd.count_move(color);
	}
	int score(cbool color,cint stage){
		return brd.score(color,stage);
	}
	int search(cmethod mthd,cbool color,cshort height,
		ccalc_type alpha = _inf,ccalc_type beta = inf,cpos_type stage = 0,ccalc_type gamma = 0){
		return brd.search(mthd,color,height,alpha,beta,stage,gamma);
	}
	vector<choice> get_choice(cmethod mthd,cbool color,cint height,cint stage){
		return brd.get_choice(mthd,color,height,stage);
	}
	choice select_choice(const vector<choice>& choices){
		return brd.select_choice(choices);
	}

	virtual coordinate play(cmethod mthd,cbool color,cint height = -1){
		if(flag_auto_save){
			push();
		}
		auto pos = brd.play(mthd,color,height);
		if(pos.x >= 0){
			if(flag_auto_show){
				show();
			}
		}else{
			if(flag_auto_save){
				do_pop();
			}
		}
		return pos;
	}

	void load_book(const string& path){
		book.load(path);
	}
protected:
	virtual bool do_pop(){
		if(record.empty()){
			return false;
		}else{
			brd = record.back();
			record.pop_back();
			return true;
		}
	}
};
