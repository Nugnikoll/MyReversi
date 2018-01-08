#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <sstream>

#include "reversi.h"
#include "tree.h"

using namespace std;

enum player_type{
	ply_human,ply_ai,ply_other
};
struct player{
	player_type p_type;
	string path;
};

class game{
public:
	game(): brd{0,0}, color(true), pos{-1,-1},
		mthd(method(mthd_ab | mthd_kill | mthd_pvs | mthd_trans | mthd_mtdf | mthd_ptn)),
		depth(-1), flag_auto_save(true), flag_lock(true),
		ply_white{ply_ai,"Irius"},ply_black{ply_human,"Irius"} {}
	virtual ~game(){}

	board brd;
	bool color;
	coordinate pos;
	method mthd;
	short depth;

	bool flag_auto_save;
	bool flag_lock;

	player ply_white, ply_black;

	player& get_ply(bool color){
		return *(&ply_white + color);
	}
	const player& get_ply(bool color)const{
		return *(&ply_white + color);
	}

	struct pack{
		board brd;
		bool color;
		coordinate pos;
	};
	vector<pack> record;
	vector<pack> storage;

	virtual void show(){
		brd.print(cout);
	}
	virtual void print_log(const string& str){
		cout << str;
	}

	void set_color(cbool _color){
		push();
		color = _color;
		show();
	}
	void set_pos(cpos_type x,cpos_type y){
		push();
		pos = {x,y};
		show();
	}
	void start(){
		color = true;
		pos = {-1,-1};
		record.clear();
		brd.initial();
		print_log("start a new game\n");
		flag_lock = false;
		show();
		play();
	}

	bool undo(){
		bool result = pop();
		if(result){
			print_log("undo\n");
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

			print_log("redo\n");
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
		print_log("assign new value to the board\n");
	}
	void assign(cboard _brd){
		push();
		brd = _brd;
		show();
		print_log("assign new value to the board\n");
	}

	chessman get(cpos_type x,cpos_type y){
		return brd.get(x + (y << 3));
	}
	void set(cpos_type x, cpos_type y, cchessman chsman){
		push();
		brd.set(x + (y << 3),chsman);
		show();
	}
	void mirror_h(){
		push();
		brd.mirror_h();
		pos.x = board::size - pos.x - 1;
		print_log("mirror horizontally\n");
		show();
	}
	void mirror_v(){
		push();
		brd.mirror_v();
		pos.y = board::size - pos.y - 1;
		print_log("mirror vertically\n");
		show();
	}
	void rotate_l(){
		push();
		brd.rotate_l();
		swap(pos.x,pos.y);
		pos.y = 7 - pos.y;
		print_log("rotate clockwise\n");
		show();
	}
	void rotate_r(){
		push();
		brd.rotate_r();
		swap(pos.x,pos.y);
		pos.x = 7 - pos.x;
		print_log("rotate counterclockwise\n");
		show();
	}
	void reflect(){
		push();
		brd.reflect();
		pos.x = board::size - pos.x - 1;
		pos.y = board::size - pos.y - 1;
		print_log("reflect\n");
		show();
	}
	void reverse(){
		push();
		brd.reverse();
		color = !color;
		print_log("reverse\n");
		show();
	}
	void config(){
		return brd.config();
	}
	bool flip(cbool color, cpos_type x, cpos_type y){
		push();
		bool result;
		if(x < 0 || x >= board::size || y < 0 || y >= board::size){
			result = false;
		}else{
			board brd_save = brd;
			brd.flip(color,x + (y << 3));
			result = (brd != brd_save);
		}
		if(result){
			print_log(
				string("place a ") + (color ? "black" : "white")
				+ " stone at " + char(x + 'A') + char(y + '1') + "\n"
			);
			if(!(brd.get_status(!color) & sts_again)){
				this->color = !color;
			}
			this->pos = coordinate(x,y);
			show();
		}else{
			if(flag_auto_save){
				pop();
			}
			print_log(
				string("cannot place a ") + (color ? "black" : "white")
				+" stone here\n"
			);
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
	float score(cbool color){
		return brd.score(color);
	}
//	float score_ptn(cbool color){
//		return brd.score_ptn(color);
//	}
	pair<method,short> process_method(cmethod mthd,cshort depth){
		pair<method,short> result = {mthd,depth};
		short total = this->brd.sum();

		if(result.first == mthd_rnd){
			return result;
		}

		if(result.second == -1){
			if(total <= 7){
				result.second = 9;
			}else if(total <= 10){
				result.second = 8;
			}else if(total <= board::size2 - 22){
				result.second = 7;
			}else if(total <= board::size2 - 15){
				result.second = 8;
			}else{
				result.second = 20;
			}
		}
		if(result.second == -2){
			if(total <= 7){
				result.second = 9;
			}else if(total <= 10){
				result.second = 9;
			}else if(total <= board::size2 - 24){
				result.second = 8;
			}else if(total <= board::size2 - 16){
				result.second = 9;
			}else{
				result.second = 20;
			}
		}
		if(result.second <= -3){
			if(total <= 7){
				result.second = 11;
			}else if(total <= 10){
				result.second = 10;
			}else if(total <= board::size2 - 22){
				result.second = 10;
			}else if(total <= board::size2 - 16){
				result.second = 10;
			}else{
				result.second = 20;
			}
		}
		if(result.second >= board::size2 - total - 1){
				result.first = method(mthd | mthd_end);
			result.second = board::size2 - total - 1;	
		}
		return result;
	}
	calc_type search(cmethod mthd,cbool color,cshort depth = -1,
		ccalc_type alpha = _inf,ccalc_type beta = inf){
		pair<method,short> p_mthd = process_method(mthd,depth);
		return brd.search(p_mthd.first,color,p_mthd.second,alpha,beta);
	}
	virtual vector<choice> get_choice(cmethod mthd,cbool color,cshort depth = -1){
		pair<method,short> p_mthd = process_method(mthd,depth);
		return brd.get_choice(p_mthd.first,color,p_mthd.second);
	}
	choice select_choice(const vector<choice>& choices){
		return brd.select_choice(choices);
	}
	coordinate play_ai(cmethod mthd,cbool color,cshort depth = -1){
		push();
		pair<method,short> p_mthd = process_method(mthd,depth);
		auto pos = brd.play(p_mthd.first,color,p_mthd.second);
		if(pos.x >= 0){
			print_log(
				string("place a ") + (color ? "black" : "white")
				+ " stone at " + char(pos.x + 'A') + char(pos.y + '1') + "\n"
			);
			if(!(brd.get_status(!color) & sts_again)){
				this->color = !color;
			}
			this->pos = pos;
			show();
		}else{
			print_log(
				string(color? ("black") : ("white"))
				+ " is unable to move.\n"
			);
			if(flag_auto_save){
				pop();
			}
		}
		return pos;
	}
	virtual coordinate play_other(cmethod mthd,cbool color,cshort depth = -1){
		return coordinate(-1,-1);
	}
	coordinate play(cmethod mthd,cbool color,cshort depth = -1){
		switch(get_ply(color).p_type){
		case ply_human:
			return coordinate(-1,-1);
		case ply_ai:
			return play_ai(mthd,color,depth);
		case ply_other:
			return play_other(mthd,color,depth);
		default:
			return coordinate(-1,-1);
		}
	}
	coordinate play(){
		sts_type status;

		while((status = brd.get_status(color)) & sts_turn){
			if(get_ply(color).p_type == ply_human){
				return coordinate(-1,-1);
			}

			play(mthd,color,depth);
		}

		flag_lock = true;
		string str = to_string(brd.count(true))
			+ " black stones and "
			+ to_string(brd.count(false))
			+ " white stones remain\n";
		if(status == sts_bwin){
			str += "black wins\n";
		}else if(status == sts_wwin){
			str += "white wins\n";
		}else{
			str += "a tie\n";
		}
		print_log(str);

		return coordinate(-1,-1);
	}

	coordinate play(ccoordinate pos){
		sts_type status = brd.get_status(color);

		if(!(status & sts_turn)){
			return coordinate(-1,-1);
		}

		if(get_ply(color).p_type == ply_human){
			flip(color,pos.x,pos.y);
		}
		play(mthd,color,depth);

		play();
		return coordinate(-1,-1);
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

#endif //GAME_H
