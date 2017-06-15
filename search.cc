#include <algorithm>

#include "reversi.h"
#include "tree.h"

calc_type board::search_ab(cbool color,cshort height,calc_type alpha,calc_type beta,calc_type acc,cshort stage)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->score(color,stage) + acc;
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if((ptr->brd.*table_flip[i])(color,i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];

		for(register pos_type i = 0;i != size2;++i){
			if((ptr->brd.*table_flip[i])(!color,i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count(color) - count(!color);
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
				temp = p->brd.search_ab(color,height - 1,alpha,beta,acc,stage);
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
			acc = (ptr - vec) + (acc / 2);
		#else
			acc = (ptr - vec) + (acc >> 1);
		#endif
		for(auto p = vec;p != ptr;++p){
			temp = - p->brd.search_ab(!color,height - 1,-beta,-alpha,-acc,stage);
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

calc_type board::search_pvs(cbool color,cshort height,calc_type alpha,calc_type beta,calc_type acc,cshort stage)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->score(color,stage) + acc;
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip(color,i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip(!color,i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count(color) - count(!color);
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

			temp = vec->brd.search_pvs(color,height - 1,alpha,beta,acc,stage);
			table_ref[vec->pos] = temp;
			if(temp <= alpha)
				return alpha;
			if(temp < beta)
				beta = temp;

			for(auto p = vec + 1;p != ptr;++p){
				temp = p->brd.search_ab(color,height - 1,beta - 1,beta,acc,stage);
				if(temp > alpha && temp < beta)
					temp = p->brd.search_pvs(color,height - 1,alpha,beta - 1,acc,stage);
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
			acc = (ptr - vec) + (acc / 2);
		#else
			acc = (ptr - vec) + (acc >> 1);
		#endif

		temp = - vec->brd.search_pvs(!color,height - 1,-beta,-alpha,-acc,stage);
		table_ref[vec->pos] = temp;
		if(temp >= beta)
			return beta;
		if(temp > alpha)
			alpha = temp;

		for(auto p = vec + 1;p != ptr;++p){
			temp = - p->brd.search_ab(!color,height - 1,-alpha - 1,-alpha,-acc,stage);
			if(temp > alpha && temp < beta)
				temp = - p->brd.search_pvs(!color,height - 1,-beta,-alpha,-acc,stage);
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

calc_type board::search_mtd(
	cbool color,
	cshort height,calc_type alpha,calc_type beta,
	ccalc_type acc,cshort stage,calc_type gamma
)const{
	calc_type result = search_trans(color,height, gamma, gamma + 1, acc, stage);
	if(result == gamma){
		do{
			--gamma;
			result = search_trans(color,height, gamma, gamma + 1, acc, stage);
		}while(result == gamma && result > alpha);
	}else{
		assert(result == gamma + 1);
		do{
			++gamma;
			result = search_trans(color,height, gamma, gamma + 1, acc, stage);
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

unordered_map<board,board::interval>& get_trans(cbool color){
	if(color){
		return trans_black;
	}else{
		return trans_white;
	}
}

calc_type board::search_trans(cbool color,cshort height,calc_type alpha,calc_type beta,calc_type acc,cshort stage)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	assert(alpha < beta);

	if(height == 0){
		return this->score(color,stage) + acc;
	}

	auto& trans_table = get_trans(color);
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
		if(ptr->brd.flip(color,i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip(color,i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			calc_type num_diff = count(color) - count(!color);
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
				temp = p->brd.search_trans(color,height - 1,alpha,_beta,acc,stage);
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
			acc = (ptr - vec) + (acc / 2);
		#else
			acc = (ptr - vec) + (acc >> 1);
		#endif

		for(auto p = vec;p != ptr;++p){
			temp = - p->brd.search_trans(color,height - 1,-beta,-_alpha,-acc,stage);
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
