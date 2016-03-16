#include <vector>
#include <algorithm>

#include "reversi.h"

const brd_type board::last = 0x8000000000000000;

const char board::chr_print[board::chessman_num] = {'.','O','#','*'};
calc_type board::table_param[stage_num][board::pos_num] = {{20,1,-6,-1},{10,2,-3,0},{4,4,4,4}};
calc_type board::table_count[board::enum_num];
calc_type board::table_eval[board::stage_num][board::layer_num][board::enum_num];

board& board::mirror(cbool is_horizontal){
	brd_type mask = 1,filter;
	brd_type new_black = 0,new_white = 0;
	if(is_horizontal){
		for(int i = 0;i != size;++i){
			for(int j = 0;j != size;++j){
				filter = brd_type(1) << ((i << 3) + (size - j - 1));
				if(filter & brd_black){
					new_black |= mask;
				}
				if(filter & brd_white){
					new_white |= mask;
				}
				mask <<= 1;
			}
		}
	}else{
		for(int i = 0;i != size;++i){
			for(int j = 0;j != size;++j){
				filter = brd_type(1) << (((size - i - 1) << 3) + j);
				if(filter & brd_black){
					new_black |= mask;
				}
				if(filter & brd_white){
					new_white |= mask;
				}
				mask <<= 1;
			}
		}
	}
	brd_black = new_black;
	brd_white = new_white;
	return *this;
}

board& board::rotater(pos_type n90){
	n90 = n90 % 4;
	brd_type mask = 1,filter;
	brd_type new_black = 0,new_white = 0;
	if(n90 < 0){
		n90 += 4;
	}
	if(n90 == 0){
		return *this;
	}else if(n90 == 1){
		for(int i = 0;i != size;++i){
			for(int j = 0;j != size;++j){
				filter = brd_type(1) << (((size - j - 1) << 3) + i);
				if(filter & brd_black){
					new_black |= mask;
				}
				if(filter & brd_white){
					new_white |= mask;
				}
				mask <<= 1;
			}
		}
	}else if(n90 == 2){
		filter = brd_type(1) << (size * size - 1);
		for(int i = 0;i != size * size;++i){
			if(filter & brd_black){
				new_black |= mask;
			}
			if(filter & brd_white){
				new_white |= mask;
			}
			mask <<= 1;
			filter >>= 1;
		}
	}else if(n90 == 3){
		for(int i = 0;i != size;++i){
			for(int j = 0;j != size;++j){
				filter = brd_type(1) << ((j << 3) + (size - i - 1));
				if(filter & brd_black){
					new_black |= mask;
				}
				if(filter & brd_white){
					new_white |= mask;
				}
				mask <<= 1;
			}
		}
	}
	brd_black = new_black;
	brd_white = new_white;
	return *this;
}

void board::config(){
	calc_type value;
	pos_type mask;
	for(pos_type i = 0;i != stage_num;++i){
		for(pos_type j = 0;j != size;++j){
			for(pos_type k = 0;k != enum_num;++k){
				value = 0;
				for(pos_type l = 0;l != size;++l){
					mask = 1 << l;
					if(mask & k){
						if(j == 0 && (l == 0 || l == size - 1)){
							value += table_param[i][0];
						}else if(j == 0 || l == 0 || l == size - 1){
							value += table_param[i][1];
						}else if(j == 1 && (l == 1 || l == size - 2)){
							value += table_param[i][2];
						}else{
							value += table_param[i][3];
						}
					}
				}
				table_eval[i][j][k] = value;
			}
		}
	}
	for(pos_type k = 0;k != enum_num;++k){
		value = 0;
		for(pos_type l = 0;l != size;++l){
			mask = 1 << l;
			if(mask & k){
				++value;
			}
		}
		table_count[k] = value;
	}
}

calc_type board::search(cbool color,cshort height,calc_type alpha,
	ccalc_type beta,calc_type acc,cpos_type stage)const{
	conf_score conf = stage_config(stage);
	if(color){
		return search<true>(height,alpha,beta,acc,conf);
	}else{
		return search<false>(height,alpha,beta,acc,conf);
	}
}
vector<board::choice> board::get_choices(cbool color,cshort height,cpos_type stage)const{
	conf_score conf = stage_config(stage);
	if(color){
		return get_choices<true>(height,conf);
	}else{
		return get_choices<false>(height,conf);
	}
}

template<bool color>
calc_type board::search(cshort height,calc_type alpha,calc_type beta,calc_type acc,cconf_score conf)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->score<color>(conf) + acc;
	}

	vector<board> vec;
	calc_type temp;
	
	vec.reserve(30);
	vec.push_back(*this);
	for(register brd_type i = 1;i != last;i <<= 1){
		if(vec.back().flip<color>(i)){
			vec.push_back(*this);
		}
	}
	if(!vec.back().flip<color>(last)){
		vec.pop_back();
	}
	
	if(vec.empty()){
		vec.push_back(*this);
		for(register brd_type i = 1;i != last;i <<= 1){
			if(vec.back().flip<!color>(i)){
				vec.push_back(*this);
			}
		}
		if(!vec.back().flip<!color>(last)){
			vec.pop_back();
		}

		if(vec.empty()){
			calc_type num_diff = count<color>() - count<!color>();
			num_diff <<= 4;
			if(num_diff){
				if(num_diff > 0){
					return num_diff + mark_max;
				}else if(num_diff < 0){
					return num_diff - mark_max;
				}else{
					return num_diff;
				}
			}else{
				return 0;
			}
		}else{
			acc = (acc >> 1) - vec.size();
			for(const auto& brd:vec){
				temp = brd.search<color>(height - 1,alpha,beta,acc,conf);
				if(temp <= alpha)
					return alpha;
				if(temp < beta)
					beta = temp;
			}
			return beta;
		}
	}else{
		acc = vec.size() + (acc >> 1);
		for(const auto& brd:vec){
			temp = - brd.search<!color>(height - 1,-beta,-alpha,-acc,conf);
			if(temp >= beta)
				return beta;
			if(temp > alpha)
				alpha = temp;
		}
	}
    return alpha;

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<<" height=" << height
		<< " alpha=" << alpha
		<< " beta=" << beta
		<< " acc=" << acc
		<< ">\n";
	do_print(out);
	calc_type result = fun();
	out << "result = " << result <<"\n"
		<< "</div>\n";
	return result;
	#endif
}

template<bool color>
vector<board::choice> board::get_choices(cshort height,board::cconf_score conf)const{

    vector<choice> choices;
	calc_type result;
    choice temp;
	calc_type alpha = _inf;

    if(height < 0)
        return choices;

    board brd = *this;
	for(pos_type i = 0;i != size;++i){
		for(pos_type j = 0;j != size;++j){
			if(brd.flip<color>(brd_type(1) << ((j << 3) + i))){
				result = - brd.search<!color>(height,_inf,-alpha,0,conf);
				if(result - 5 > alpha){
					alpha = result - 5;
				}
				std::get<0>(temp) = result;
				std::get<1>(temp) = brd;
				std::get<2>(temp) = i;
				std::get<3>(temp) = j;
				choices.push_back(temp);
				brd = *this;
			}
		}
	}
    return choices;
}

#include <random>
#include <chrono>
default_random_engine engine(
	chrono::system_clock::now().time_since_epoch().count()
);

board::choice board::select_choice(vector<board::choice> choices)const{

	if(choices.empty()){
		throw runtime_error("There's no choice!");
	}

	normal_distribution<float> scatter(0,0.8);

	for(choice& c:choices){
		std::get<0>(c) += scatter(engine);
	}

	return *max_element(
		choices.begin(),choices.end(),
		[](const choice& c1,const choice& c2) -> bool{
			return std::get<0>(c1) < std::get<0>(c2);
		}
	);
}
