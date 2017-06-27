#include <vector>
#include <cmath>
#include <algorithm>

#include "reversi.h"
#include "search.h"

const pos_type board::chessman_num;
const pos_type board::size;
const pos_type board::size2;
const pos_type board::pos_num;
const pos_type board::stage_num;
const short board::max_height;
bool board::flag_unicode = true;

#ifdef USE_FLOAT
	const calc_type board::mark_max = 100;
#endif
const char board::chr_print[board::chessman_num] = {'.','O','#','*'};
calc_type board::table_param[stage_num][board::pos_num] = {{20,1,-6,-1},{10,1,-3,0},{5,2,1,1}};

unordered_map<board,board::interval> trans_black;
unordered_map<board,board::interval> trans_white;

void board::print(ostream& out)const{
	if(flag_unicode){
		string s =
			"╔═╤═╤═╤═╤═╤═╤═╤═╗\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╟─┼─┼─┼─┼─┼─┼─┼─╢\n"
			"║ │ │ │ │ │ │ │ ║\n"
			"╚═╧═╧═╧═╧═╧═╧═╧═╝\n"
		;

		pos_type pos = 0;
		for(char& chr:s){
			if(chr == ' '){
				switch(get(pos)){
				case blank:
					out << " ";
					break;
				case black:
					out << "●";
					break;
				case white:
					out << "○";
					break;
				case null:
					out << "╳";
					break;
				default:
					out << "╳";
					break;
				}
				++pos;
			}else{
				out << chr;
			}
		}
	}else{
		for(pos_type i = 0;i != size;++i){
			for(pos_type j = 0;j != size;++j){
				out << chr_print[get((i << 3) | j)];
			}
			out << '\n';
		}
	}
}

vector<choice> board::get_choice(
	cmethod mthd,cbool color,cshort height,ccalc_type gamma
)const{

    vector<choice> choices;
	calc_type result;
    choice temp;
	calc_type alpha = _inf;

	if(mthd & mthd_trans){
		table_trans.clear();
	}

	choices.reserve(30);

    board brd = *this;
	for(pos_type i = 0;i != size2;++i){
		if(brd.flip(color,i)){
			result = - brd.search(mthd,!color,height,_inf,-alpha,gamma);
			if(result - 5 > alpha){
				alpha = result - 5;
			}
			temp.val = result;
			temp.brd = brd;
			temp.pos = i;
			choices.push_back(temp);
			brd = *this;
		}
	}
    return choices;
}

#define USE_RANDOM

#ifdef USE_RANDOM
	#include <random>
	#include <chrono>
#else
	#include <cstdlib>
	#include <ctime>
#endif //USE_RANDOM

choice board::select_choice(vector<choice> choices,const float& variation){

	if(choices.empty()){
		throw runtime_error("There is no choice!");
	}

	#ifdef USE_RANDOM
		normal_distribution<float> scatter(0,variation);
	#else
		//cout << "time : " << time(NULL) << endl;
		srand(time(NULL));
		float f;
	#endif //USE_RANDOM

	for(choice& c:choices){
		#ifdef USE_RANDOM
			c.rnd_val = c.val + scatter(engine);
		#else
			f = (float(rand()) / RAND_MAX - 0.5);
			f *= f * f * 6;
			c.rnd_val = c.val + f;
		#endif //USE_RANDOM
	}

	return *max_element(
		choices.begin(),choices.end(),
		[](const choice& c1,const choice& c2) -> bool{
			return c1.rnd_val < c2.rnd_val;
		}
	);
}

coordinate board::play(cmethod mthd,cbool color,short height){

	if(height < 0){
		short total = this->sum();
		if(total <= 7){
			height = 9;
		}else if(total <= 10){
			height = 8;
		}else if(total <= size2 - 22){
			height = 7;
		}else if(total <= size2 - 15){
			height = 8;
		}else{
			height = 20;
		}
	}

	vector<choice> choices = get_choice(mthd,color,height);
	if(choices.empty()){
		return coordinate(-1,-1);
	}else{
		float variation;
		if(mthd == mthd_ptn){
			variation = 0.003;
		}else{
			variation = 0.75;
		}
		choice best = select_choice(choices,variation);
		flip(color,best.pos);
		return coordinate(best.pos);
	}
}
