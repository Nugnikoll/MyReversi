#include <vector>
#include <cmath>
#include <algorithm>

#include "reversi.h"
#include "search.h"

const pos_type board::size;
const pos_type board::size2;
const pos_type board::pos_num;
const pos_type board::stage_num;
const short board::max_height;
bool board::flag_unicode = true;
brd_type board::node_count;

#ifdef USE_FLOAT
	const calc_type board::mark_max = 2;
#endif

calc_type board::table_param[stage_num][board::pos_num] = {{12,0.5,-6,-0.2},{10,0.5,-5,0.2},{3,1,0,0}};

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
		const char chr_print[4] = {'.','O','#','*'};
		for(pos_type i = 0;i != size;++i){
			for(pos_type j = 0;j != size;++j){
				out << chr_print[get((i << 3) | j)];
			}
			out << '\n';
		}
	}
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

//	if(choices.empty()){
//		throw runtime_error("There is no choice!");
//	}

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

coordinate board::play(cmethod mthd,cbool color,cshort depth){

	vector<choice> choices = get_choice(mthd,color,depth);
	if(choices.empty()){
		return coordinate(-1,-1);
	}else{
		choice best;
		if(mthd == mthd_rnd){
			uniform_int_distribution<int> scatter(0,choices.size() - 1);
			best = choices[scatter(engine)];
		}else{
			float variation;
			if(mthd & mthd_ptn){
				variation = 0.2;
			}else{
				variation = 0.75;
			}
			best = select_choice(choices,variation);
		}
		flip(color,best.pos);
		return coordinate(best.pos);
	}
}
