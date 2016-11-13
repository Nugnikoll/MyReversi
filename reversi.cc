#include <vector>
#include <algorithm>

#include "reversi.h" //--

const brd_type board::last = 0x8000000000000000;

#ifdef USE_FLOAT
	const calc_type board::mark_max = 10000;
#endif
const char board::chr_print[board::chessman_num] = {'.','O','#','*'};
calc_type board::table_param[stage_num][board::pos_num] = {{20,1,-6,-1},{10,1,-3,0},{20,20,29,29}};
calc_type board::table_eval[board::stage_num][board::size][board::enum_num];
calc_type board::table_temp[2][board::max_height + 1][board::size2];

unordered_map<board,board::interval> trans_black;
unordered_map<board,board::interval> trans_white;

calc_type (*board::fun)(const board& brd,cbool color);

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

	calc_type* ptr_begin = **table_temp;
	calc_type* ptr_end = ptr_begin + (2 * max_height * size2);
	for(calc_type* ptr = ptr_begin;ptr != ptr_end;++ptr){
		*ptr = _inf;
	}

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
}

calc_type board::search(cmethod mthd,cbool color,cshort height,ccalc_type alpha,
	ccalc_type beta,ccalc_type acc,cpos_type stage,ccalc_type gamma)const{
	#define search_mthd(_mthd) \
		if(mthd == _mthd){ \
			if(color){ \
				return search<true,_mthd>(height,alpha,beta,acc,conf); \
			}else{ \
				return search<false,_mthd>(height,alpha,beta,acc,conf); \
			} \
		}
	conf_score conf;
	if(mthd != mthd_ptn && mthd != mthd_train){
		conf = stage_config(stage);
		search_mthd(mthd_ab);
		search_mthd(mthd_pvs);
		search_mthd(mthd_trans);
		search_mthd(mthd_mtdf);
		//search_mthd(mthd_ids);
		search_mthd(mthd_train);
	}
	search_mthd(mthd_ptn);
	assert(false);
	return 0;
}

vector<choice> board::get_choice(cmethod mthd,cbool color,cshort height,cpos_type stage)const{
	#define get_choice_mthd(_mthd) \
		if(mthd == _mthd){ \
			if(color){ \
				return get_choice<true,_mthd>(height,conf); \
			}else{ \
				return get_choice<false,_mthd>(height,conf); \
			} \
		}

	conf_score conf;
	if(mthd != mthd_ptn && mthd != mthd_train){
		conf = stage_config(stage);
		get_choice_mthd(mthd_ab);
		get_choice_mthd(mthd_pvs);
		get_choice_mthd(mthd_trans);
		get_choice_mthd(mthd_mtdf);
		//get_choice_mthd(mthd_ids);
		get_choice_mthd(mthd_train);
	}
	get_choice_mthd(mthd_ptn);

	if(color){
		return get_choice<true,mthd_default>(height,conf);
	}else{
		return get_choice<false,mthd_default>(height,conf);
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

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip<color>(brd_type(1) << i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip<!color>(brd_type(1) << i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count<color>() - count<!color>();
			#ifdef USE_FLOAT
				num_diff *= 16;
			#else
				num_diff <<= 4;
			#endif
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
			sort(vec,ptr);

			#ifdef USE_FLOAT
				acc = (acc / 2) - (ptr - vec);
			#else
				acc = (acc >> 1) - (ptr - vec);
			#endif
			for(auto p = vec;p != ptr;++p){
				temp = p->brd.search<color>(height - 1,alpha,beta,acc,conf);
				table_ref[p->pos] = temp;
				if(temp <= alpha)
					return alpha;
				if(temp < beta)
					beta = temp;
			}
			return beta;
		}
	}else{
		sort(vec,ptr,greater<brd_val>());

		#ifdef USE_FLOAT
			acc = (ptr - vec) + (acc / 1);
		#else
			acc = (ptr - vec) + (acc >> 1);
		#endif
		for(auto p = vec;p != ptr;++p){
			temp = - p->brd.search<!color>(height - 1,-beta,-alpha,-acc,conf);
			table_ref[p->pos] = temp;
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
calc_type board::search_train(cshort height,calc_type alpha,calc_type beta,calc_type acc,cconf_score conf)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->fun(*this,color);
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip<color>(brd_type(1) << i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip<!color>(brd_type(1) << i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count<color>() - count<!color>();
			#ifdef USE_FLOAT
				num_diff *= 16;
			#else
				num_diff <<= 4;
			#endif
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
			sort(vec,ptr);

			//acc = (acc >> 1) - (ptr - vec);
			for(auto p = vec;p != ptr;++p){
				temp = p->brd.search<color>(height - 1,alpha,beta,acc,conf);
				table_ref[p->pos] = temp;
				if(temp <= alpha)
					return alpha;
				if(temp < beta)
					beta = temp;
			}
			return beta;
		}
	}else{
		sort(vec,ptr,greater<brd_val>());

		//acc = (ptr - vec) + (acc >> 1);
		for(auto p = vec;p != ptr;++p){
			temp = - p->brd.search<!color>(height - 1,-beta,-alpha,-acc,conf);
			table_ref[p->pos] = temp;
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
calc_type board::search_pvs(cshort height,calc_type alpha,calc_type beta,calc_type acc,cconf_score conf)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->score<color>(conf) + acc;
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip<color>(brd_type(1) << i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip<!color>(brd_type(1) << i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count<color>() - count<!color>();
			#ifdef USE_FLOAT
				num_diff *= 16;
			#else
				num_diff <<= 4;
			#endif
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
			sort(vec,ptr);

			#ifdef USE_FLOAT
				acc = (acc / 2) - (ptr - vec);
			#else
				acc = (acc >> 1) - (ptr - vec);
			#endif

			temp = vec->brd.search_pvs<color>(height - 1,alpha,beta,acc,conf);
			table_ref[vec->pos] = temp;
			if(temp <= alpha)
				return alpha;
			if(temp < beta)
				beta = temp;

			for(auto p = vec + 1;p != ptr;++p){
				temp = p->brd.search<color>(height - 1,beta - 1,beta,acc,conf);
				if(temp > alpha && temp < beta)
					temp = p->brd.search_pvs<color>(height - 1,alpha,beta - 1,acc,conf);
				table_ref[p->pos] = temp;
				if(temp <= alpha)
					return alpha;
				if(temp < beta)
					beta = temp;
			}

			return beta;
		}
	}else{
		sort(vec,ptr,greater<brd_val>());

		#ifdef USE_FLOAT
			acc = (ptr - vec) + (acc / 1);
		#else
			acc = (ptr - vec) + (acc >> 1);
		#endif

		temp = - vec->brd.search_pvs<!color>(height - 1,-beta,-alpha,-acc,conf);
		table_ref[vec->pos] = temp;
		if(temp >= beta)
			return beta;
		if(temp > alpha)
			alpha = temp;

		for(auto p = vec + 1;p != ptr;++p){
			temp = - p->brd.search<!color>(height - 1,-alpha - 1,-alpha,-acc,conf);
			if(temp > alpha && temp < beta)
				temp = - p->brd.search_pvs<!color>(height - 1,-beta,-alpha,-acc,conf);
			table_ref[p->pos] = temp;
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
calc_type board::search_mtd(
	cshort height,calc_type alpha,calc_type beta,
	ccalc_type acc,cconf_score conf,calc_type gamma
)const{
	calc_type result = search_trans<color>(height, gamma, gamma + 1, acc, conf);
	if(result == gamma){
		do{
			--gamma;
			result = search_trans<color>(height, gamma, gamma + 1, acc, conf);
		}while(result == gamma && result > alpha);
	}else{
		assert(result == gamma + 1);
		do{
			++gamma;
			result = search_trans<color>(height, gamma, gamma + 1, acc, conf);
		}while(result == gamma + 1 && result < beta);
	}
	return result;
}

//Iterative_deepening(root)
//1:	firstguess ¡û 0
//2:	for depth = 1 to MAX_SEARCH_DEPTH do
//3:		firstguess ¡û MTDF(root, firstguess, depth)
//4:		if times_up() then
//5:		break
//6:	return firstguess


template<bool color>
inline unordered_map<board,board::interval>& get_trans(){
	if(color){
		return trans_black;
	}else{
		return trans_white;
	}
}

template<bool color>
calc_type board::search_trans(cshort height,calc_type alpha,calc_type beta,calc_type acc,cconf_score conf)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	assert(alpha < beta);

	if(height == 0){
		return this->score<color>(conf) + acc;
	}

	auto& trans_table = get_trans<color>();
	auto trans_ptr = trans_table.find(*this);
	if(trans_ptr != trans_table.end()){
		auto& trans_interval = trans_ptr->second;
		if(trans_interval.first >= beta){
			return trans_interval.first;
			//return beta;
		}
		if(trans_interval.second <= alpha){
			return trans_interval.second;
			//return alpha;
		}
		if(trans_interval.first > alpha){
			alpha = trans_interval.first;
		}
		if(trans_interval.second < beta){
			beta = trans_interval.second;
		}
		assert(alpha < beta);
	}else{
		trans_ptr = trans_table.insert(
			std::make_pair(*this,interval(_inf,inf))
		).first;
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];

	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip<color>(brd_type(1) << i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip<!color>(brd_type(1) << i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count<color>() - count<!color>();
			#ifdef USE_FLOAT
				num_diff *= 16;
			#else
				num_diff <<= 4;
			#endif
			if(num_diff){
				if(num_diff > 0){
					temp = num_diff + mark_max;
				}else if(num_diff < 0){
					temp = num_diff - mark_max;
				}else{
					temp = num_diff;
				}
			}else{
				temp = 0;
			}
		}else{
			sort(vec,ptr);

			calc_type _beta = beta;

			#ifdef USE_FLOAT
				acc = (acc / 2) - (ptr - vec);
			#else
				acc = (acc >> 1) - (ptr - vec);
			#endif
			for(auto p = vec;p != ptr;++p){
				temp = p->brd.search_trans<color>(height - 1,alpha,_beta,acc,conf);
				table_ref[p->pos] = temp;
				if(temp <= alpha){
					temp = alpha;
					goto label_end;
				}
				if(temp < _beta)
					_beta = temp;
			}
			temp = _beta;
		}
	}else{
		sort(vec,ptr,greater<brd_val>());

		calc_type _alpha = alpha;

		#ifdef USE_FLOAT
			acc = (ptr - vec) + (acc / 1);
		#else
			acc = (ptr - vec) + (acc >> 1);
		#endif

		for(auto p = vec;p != ptr;++p){
			temp = - p->brd.search_trans<!color>(height - 1,-beta,-_alpha,-acc,conf);
			table_ref[p->pos] = temp;
			if(temp >= beta){
				temp = beta;
				goto label_end;
			}
			if(temp > _alpha)
				_alpha = temp;
		}
		temp = _alpha;
	}

	label_end:

	assert(alpha <= beta);
	auto& trans_interval = trans_ptr->second;
	if(temp < beta){
		trans_interval.second = temp;
	}
	if(temp > alpha){
		trans_interval.first = temp;
	}

    return temp;

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


template<bool color,method mthd>
vector<choice> board::get_choice(
	cshort height,board::cconf_score conf,ccalc_type gamma
)const{

    vector<choice> choices;
	calc_type result;
    choice temp;
	calc_type alpha = _inf;

    if(height < 0)
        return choices;

	if(mthd & mthd_mtdf){
		trans_black.clear();
		trans_white.clear();
	}

	choices.reserve(30);

    board brd = *this;
	for(pos_type i = 0;i != size;++i){
		for(pos_type j = 0;j != size;++j){
			if(brd.flip<color>(brd_type(1) << ((j << 3) + i))){
				result = - brd.search<!color,mthd>(height,_inf,-alpha,0,conf,gamma);
				if(result - 5 > alpha){
					alpha = result - 5;
				}
				temp.val = result;
				temp.brd = brd;
				temp.x = i;
				temp.y = j;
				choices.push_back(temp);
				brd = *this;
			}
		}
	}
    return choices;
}

#define USE_RANDOM

#ifdef USE_RANDOM
	#include <random>
	#include <chrono>
	default_random_engine engine(
		chrono::system_clock::now().time_since_epoch().count()
	);
#else
	#include <cstdlib>
	#include <ctime>
#endif //USE_RANDOM

choice board::select_choice(vector<choice> choices,const float& variation){

	if(choices.empty()){
		throw runtime_error("There's no choice!");
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

coordinate board::play(cmethod mthd,cbool color,short height,short stage){

	if(height < 0){
		short total = this->sum();
		if(total <= 7){
			height = 9;
		}else if(total <= 10){
			height = 8;
		}else if(total <= 33){
			height = 7;
		}else if(total <= size2 - 22){
			height = 7;
		}else if(total <= size2 - 15){
			height = 8;
		}else{
			height = 20;
		}
	}
	if(stage < 0){
		short total = this->sum();
		if(total <= 7){
			stage = 0;
		}else if(total <= 10){
			stage = 0;
		}else if(total <= 33){
			stage = 0;
		}else if(total <= size2 - 23){
			stage = 1;
		}else if(total <= size2 - 16){
			stage = 1;
		}else{
			stage = 2;
		}
	}
	vector<choice> choices = get_choice(mthd,color,height,stage);
	if(choices.empty()){
		return coordinate(-1,-1);
	}else{
		auto comp = [](const choice& c1,const choice& c2){
				return c1.val < c2.val;
			};
		if(mthd & mthd_ptn){
			if(color){
				this->adjust_ptn<true>(
					(
						max_element(choices.begin(),choices.end(),comp)->val
						//+ this->search_ptn<true>(1,_inf,inf)
					)// / 2
					- this->score_ptn<true>()
				);
			}else{
				this->adjust_ptn<false>(
					(
						max_element(choices.begin(),choices.end(),comp)->val
						//+ this->search_ptn<false>(1,_inf,inf)
					)// / 2
					- this->score_ptn<false>()
				);
			}
		}
		float variation;
		if(mthd == mthd_ptn){
			variation = 0;
		}else{
			variation = 0.75;
		}
		choice best = select_choice(choices,variation);
		flip(color,best.x,best.y);
		return coordinate(best.x,best.y);
	}
}

void print(const vector<choice>& choices){
	cout << '(';
	for(const auto& c: choices){
		c.print();
		cout << ',';
	}cout << "\b)";
}
