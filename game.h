#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <sstream>

#include "reversi.h"
#include "tree.h"

using namespace std;

class game{
public:
	game(){
		brd.assign(0,0);
		color = true;
		pos.x = pos.y = -1;
		flag_show = false;
		flag_term = false;
		flag_log = false;
	}
	virtual ~game(){}

	board brd;
	bool color;
	coordinate pos;
	struct pack{
		board brd;
		bool color;
		coordinate pos;
	};
	vector<pack> record;
	vector<pack> storage;

	bool flag_print_term = true;
	bool flag_auto_save = true;
	bool flag_show;
	string term_string;
	bool flag_term;
	string log_string;
	bool flag_log;
	bool flag_lock;

	virtual void show(){
		if(flag_print_term){
			ostringstream out;
			brd.print(out);
			term_string = out.str();
			flag_term = true;
		}
		flag_show = true;
	}
	void set_color(cbool _color){
		push();
		color = _color;
		show();
	}
	void set_pos(cint x,cint y){
		push();
		pos.x = x;
		pos.y = y;
		show();
	}
	void start(){
		color = true;
		pos.x = pos.y = -1;
		record.clear();
		brd.initial();
		log_string = "start a new game\n";
		flag_log = true;
		flag_lock = false;
		show();
	}

	bool undo(){
		bool result = pop();
		if(result){
			log_string = "undo\n";
			flag_log = true;
			flag_lock = false;
			show();
		}
		return result;
	}
	bool redo(){
		if(storage.empty()){
			return false;
		}else{
			pack temp = {brd,color,pos};
			record.push_back(temp);

			temp = storage.back();
			storage.pop_back();
			brd = temp.brd;
			color = temp.color;
			pos = temp.pos;

			log_string = "redo\n";
			flag_log = true;
			flag_lock = false;

			show();
			return true;
		}
	}
	brd_type bget(cbool color){
		return brd.bget(color);
	}
	void assign(cbrd_type _brd_black,cbrd_type _brd_white){
		push();
		brd.assign(_brd_black,_brd_white);
		show();
		log_string = "assign new value to the board\n";
		flag_log = true;
	}

	chessman get(cpos_type x,cpos_type y){
		return brd.get(x + (y << 3));
	}
	void set(cpos_type x, cpos_type y, cchessman chsman){
		push();
		brd.set(x,y,chsman);
		show();
	}
	void mirror_h(){
		push();
		brd.mirror_h();
		pos.x = board::size - pos.x - 1;
		log_string = "mirror horizontally\n";
		flag_log = true;
		show();
	}
	void mirror_v(){
		push();
		brd.mirror_v();
		pos.y = board::size - pos.y - 1;
		log_string = "mirror vertically\n";
		flag_log = true;
		show();
	}
	void rotate_l(){
		push();
		brd.rotate_l();
		swap(pos.x,pos.y);
		pos.y = 7 - pos.y;
		log_string = "rotate clockwise\n";
		flag_log = true;
		show();
	}
	void rotate_r(){
		push();
		brd.rotate_r();
		swap(pos.x,pos.y);
		pos.x = 7 - pos.x;
		log_string = "rotate counterclockwise\n";
		flag_log = true;
		show();
	}
	void reflect(){
		push();
		brd.reflect();
		pos.x = board::size - pos.x - 1;
		pos.y = board::size - pos.y - 1;
		log_string = "reflect\n";
		flag_log = true;
		show();
	}
	void config(){
		return brd.config();
	}
	bool flip(cbool color, cpos_type x, cpos_type y){
		push();
		bool result = brd.flip(color,x + (y << 3));
		ostringstream out;
		if(result){
			out << 	(color? "black" : "white")
				<< " place a stone at ("
				<< x
				<< ","
				<< y
				<< ")\n";
			log_string = out.str();
			flag_log = true;
			if(brd.get_status(!color) & sts_again){
			}else{
				this->color = !color;
			}
			this->pos = coordinate(x,y);
			show();
		}else{
			if(flag_auto_save){
				pop();
			}
			out << (color? "black" : "white")
				<< " cannot place a stone here\n";
			log_string = out.str();
			flag_log = true;
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
	float score(cbool color,cint stage){
		return brd.score(color,stage);
	}
	float score_ptn(cbool color){
		return brd.score_ptn(color);
	}
	vector<float> eval_ptn(cbool color){
		return brd.eval_ptn(color);
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

	coordinate play(cmethod mthd,cbool color,cint height = -1){
		push();
		auto pos = brd.play(mthd,color,height);
		if(pos.x >= 0){
			ostringstream out;
			out << (color? "black" : "white")
				<< " place a stone at ("
				<< pos.x
				<< ","
				<< pos.y
				<< ")\n";
			log_string = out.str();
			flag_log = true;
			if(brd.get_status(!color) & sts_again){
			}else{
				this->color = !color;
			}
			this->pos = pos;
			show();
		}else{
			log_string = 
				string(color? ("black") : ("white"))
				+ " is unable to move.\n";
			flag_log = true;
			if(flag_auto_save){
				pop();
			}
		}
		return pos;
	}
	coordinate play(ccoordinate _pos,cmethod mthd){
		bool color_save = color;
		bool flag = flip(color,_pos.x,_pos.y);
		if(!flag){
			return coordinate(-1,-1);
		}
		sts_type status = sts_again;
		coordinate pos;

		bool flag_auto = flag_auto_save;
		flag_auto_save = false;

		while(status & sts_again){
			pos = play(mthd,!color_save);
			status = brd.get_status(color_save);
			if(pos.x < 0){
				show();
				break;
			}
		}

		flag_auto_save = flag_auto;

		if(status & sts_end){
			flag_lock = true;
			ostringstream out;
			out << brd.count(true)
				<< " black stones and "
				<< brd.count(false)
				<< " white stones remain\n";
			if(status == sts_bwin){
				out << "black wins\n";
			}else if(status == sts_wwin){
				out << "white wins\n";
			}else{
				out << "a tie\n";
			}
			log_string = out.str();	
			flag_log = true;
		}
		return pos;
	}

protected:
	void push(){
		if(flag_auto_save){
			pack temp = {brd,color,pos};
			record.push_back(temp);
			storage.clear();
		}
	}

	bool pop(){
		if(record.empty()){
			return false;
		}else{
			pack temp = {brd,color,pos};
			storage.push_back(temp);

			temp = record.back();
			record.pop_back();
			brd = temp.brd;
			color = temp.color;
			pos = temp.pos;

			return true;
		}
	}
};
