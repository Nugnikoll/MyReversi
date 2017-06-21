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
	tree book;

	string show_string;
	bool flag_show;
	bool flag_auto_show = true;
	bool flag_auto_save = true;
	bool is_lock;
	string prompt = ">>";
	string log_string;
	bool flag_log;

	virtual void show(){
		ostringstream out;
		brd.print(out);
		show_string = out.str();
		flag_show = true;
	}
	void set_color(cbool _color){
		if(flag_auto_save){
			push();
		}
		color = _color;
		if(flag_auto_show){
			show();
		}
	}
	void set_pos(cint x,cint y){
		if(flag_auto_save){
			push();
		}
		pos.x = x;
		pos.y = y;
		if(flag_auto_show){
			show();
		}
	}
	void start(){
		color = true;
		pos.x = pos.y = -1;
		record.clear();
		brd.initial();
		log_string = "start a new game\n";
		flag_log = true;
		is_lock = false;
		if(flag_auto_show){
			show();
		}
	}

	virtual bool undo(){
		bool result = pop();
		if(result){
			log_string = "undo\n";
			flag_log = true;
			is_lock = false;
			if(flag_auto_show){
				show();
			}
		}
		return result;
	}
	virtual bool redo(){
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
			is_lock = false;

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
		log_string = "assign new value to the board\n";
		flag_log = true;
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
	void mirror_h(){
		if(flag_auto_save){
			push();
		}
		brd.mirror_h();
		pos.x = board::size - pos.x - 1;
		log_string = "mirror horizontally\n";
		flag_log = true;
		if(flag_auto_show){
			show();
		}
	}
	void mirror_v(){
		if(flag_auto_save){
			push();
		}
		brd.mirror_v();
		pos.y = board::size - pos.y - 1;
		log_string = "mirror vertically\n";
		flag_log = true;
		if(flag_auto_show){
			show();
		}
	}
	void rotate_l(){
		if(flag_auto_save){
			push();
		}
		brd.rotate_l();
		swap(pos.x,pos.y);
		pos.y = 7 - pos.y;
		log_string = "rotate clockwise\n";
		flag_log = true;
		if(flag_auto_show){
			show();
		}
	}
	void rotate_r(){
		if(flag_auto_save){
			push();
		}
		brd.rotate_r();
		swap(pos.x,pos.y);
		pos.x = 7 - pos.x;
		log_string = "rotate counterclockwise\n";
		flag_log = true;
		if(flag_auto_show){
			show();
		}
	}
	void reflect(){
		if(flag_auto_save){
			push();
		}
		brd.reflect();
		pos.x = board::size - pos.x - 1;
		pos.y = board::size - pos.y - 1;
		log_string = "reflect\n";
		flag_log = true;
		if(flag_auto_show){
			show();
		}
	}
	void config(){
		return brd.config();
	}
	virtual bool flip(cbool color, cpos_type x, cpos_type y){
		if(flag_auto_save){
			push();
		}
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
			if(flag_auto_show){
				show();
			}
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

	virtual coordinate play(cmethod mthd,cbool color,cint height = -1){
		if(flag_auto_save){
			push();
		}
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
			if(flag_auto_show){
				show();
			}
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
				if(flag_auto_show){
					show();
				}
				break;
			}
		}

		flag_auto_save = flag_auto;

		if(status & sts_end){
			is_lock = true;
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

	void load_book(const string& path){
		book.load(path);
	}

protected:
	void push(){
		pack temp = {brd,color,pos};
		record.push_back(temp);
		storage.clear();
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
