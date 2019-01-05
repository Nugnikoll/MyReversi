#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

#include "board.h"
#include "matrix.h"
#include "search.h"

default_random_engine engine(
	chrono::system_clock::now().time_since_epoch().count()
);

const pos_type board::size;
const pos_type board::size2;

void board::print(ostream& out)const{
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
}

matrix<int> board::to_mat()const{
	matrix<int> mat(2, size2);
	for(int i = 0; i != size2; ++i){
		mat.at(i) = bool(brd_white & 1 << i);
		mat.at(i + size2) = bool(brd_black & 1 << i);
	}
	return mat;
}

choice board::select_choice(vector<choice> choices,const float& variation){

	normal_distribution<float> scatter(0,variation);

	for(choice& c:choices){
		c.rnd_val = c.val + scatter(engine);
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
