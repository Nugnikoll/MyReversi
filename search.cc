#include <algorithm>
#include <cmath>

#include "reversi.h"
#include "search.h"
#include "pattern.h"

const short depth_kill = 2;
const short depth_pvs = 2;
const short depth_hash = 3;
const short depth_mtdf = 7;

calc_type table_val[board::size2][board::size2];
trans_type table_trans[2];

const calc_type table_val_init[board::size2] = {
	0.0010,0.0003,0.0007,0.0008,0.0008,0.0007,0.0003,0.0010,
	0.0003,0.0002,0.0004,0.0005,0.0005,0.0004,0.0002,0.0003,
	0.0007,0.0004,0.0009,0.0006,0.0006,0.0009,0.0004,0.0007,
	0.0008,0.0005,0.0006,0.0001,0.0001,0.0006,0.0005,0.0008,
	0.0008,0.0005,0.0006,0.0001,0.0001,0.0006,0.0005,0.0008,
	0.0007,0.0004,0.0009,0.0006,0.0006,0.0009,0.0004,0.0007,
	0.0003,0.0002,0.0004,0.0005,0.0005,0.0004,0.0002,0.0003,
	0.0010,0.0003,0.0007,0.0008,0.0008,0.0007,0.0003,0.0010
};

struct mtdf_info{
	mtdf_info():num(1),bias(0),sigma(1){}

	int num;
	calc_type bias;
	calc_type sigma;

	void adjust(ccalc_type diff){
		bias *= num;
		sigma *= num;
		bias += diff;
		++num;
		bias /= num;
		sigma += (diff - bias) * (diff - bias);
		sigma /= num;
	}
};

mtdf_info table_mtdf_info[board::size2 + 1][board::size2][16];

void board::config_search(){}

void board::clear_search_info(){
	node_count = 0;
	table_trans[0].clear();
	table_trans[1].clear();
}

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wswitch"
#endif

calc_type board::search(
	cmethod mthd,cbool color,cshort depth,
	ccalc_type alpha,ccalc_type beta,ccalc_type gamma
)const{

	if(depth < 0){
		short total = this->sum();
		if(total <= 7){
			depth = 9;
		}else if(total <= 10){
			depth = 8;
		}else if(total <= size2 - 22){
			depth = 7;
		}else if(total <= size2 - 15){
			depth = 8;
		}else{
			depth = 20;
		}
	}

	#define search_mthd(mthd) \
		case mthd: return board::search<method(mthd)>(color,depth,alpha,beta);
	#define search_mthd_ab(mthd) \
		search_mthd(mthd) search_mthd(mthd | mthd_ab)
	#define search_mthd_kill(mthd) \
		search_mthd_ab(mthd) search_mthd_ab(mthd | mthd_kill)
	#define search_mthd_pvs(mthd) \
		search_mthd_kill(mthd) search_mthd_kill(mthd | mthd_pvs)
	#define search_mthd_trans(mthd) \
		search_mthd_pvs(mthd) search_mthd_pvs(mthd | mthd_trans)
	#define search_mthd_mtdf(mthd) \
		search_mthd_trans(mthd) search_mthd_trans(mthd | mthd_mtdf)
	#define search_mthd_ptn(mthd) \
		search_mthd_mtdf(mthd) search_mthd_mtdf(mthd | mthd_ptn)
	#define search_mthd_mpc(mthd) \
		search_mthd_ptn(mthd) search_mthd_ptn(mthd | mthd_mpc)

	switch(mthd){
		search_mthd_mpc(mthd_rnd);
	default:
		assert(false);
		return 0;
	}

};

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

template<method mthd>
calc_type board::search(cbool color,cshort depth,calc_type alpha,calc_type beta)const{

	struct brd_val{
		pos_type pos;
		calc_type val;
	};
	typedef const brd_val& cbrd_val;

	#define trans_save(data) \
		if(flag_hash){ \
			auto& trans_interval = trans_ptr->second; \
			if(data < beta_save){ \
				trans_interval.second = data; \
			} \
			if(data > alpha_save){ \
				trans_interval.first = data; \
			} \
		}


	#ifdef USE_ASM
		#define trail_zero_count(brd,result) \
			asm_tzcnt(brd,result)
	#else
		#define trail_zero_count(brd,result) \
			result = count(~brd & (brd - 1))
	#endif

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(mthd == mthd_rnd){

		return 0;

	}else if((mthd & mthd_mtdf) && (depth >= depth_mtdf)){

		const method mthd_de_mtdf = method(mthd & ~mthd_mtdf);
		short depth_presearch = (depth & 1) ? 5 : 4;
		calc_type gamma = search<mthd_de_mtdf>(color,depth_presearch,alpha,beta);

		if(mthd & mthd_trans){
			clear_search_info();
		}

		mtdf_info& info = table_mtdf_info[this->sum()][depth][depth_presearch];

		calc_type window_width = sqrt(info.sigma) * 2;
		calc_type window_alpha = gamma + info.bias - window_width / 2;
		calc_type window_beta = gamma + info.bias + window_width / 2;

		calc_type result = search<mthd_de_mtdf>(color,depth, window_alpha, window_beta);
		if(result <= window_alpha){
			do{
				window_width *= 2;
				window_beta = window_alpha;
				window_alpha = window_beta - window_width;
				result = search<mthd_de_mtdf>(color,depth, window_alpha, window_beta);
			}while(result <= window_alpha && result > alpha);
		}else if(result >= window_beta){
			do{
				window_width *= 2;
				window_alpha = window_beta;
				window_beta = window_alpha + window_width;
				result = search<mthd_de_mtdf>(color,depth, window_alpha, window_beta);
			}while(result >= window_beta && result < beta);
		}

		if(result > alpha && result < beta){
			info.adjust(result - gamma);
		}

		return result;

	}else{

		++node_count;

		if(depth == 0){
			if(mthd & mthd_ptn)
				return this->score_ptn(color);
			else
				return this->score(color);
		}

		bool flag_kill = (mthd & mthd_kill) && depth >= depth_kill;
		bool flag_pvs = (mthd & mthd_pvs) && depth >= depth_pvs;
		bool flag_hash = (mthd & mthd_trans) && depth >= depth_hash;

		calc_type alpha_save,beta_save;
		trans_type::iterator trans_ptr;

		if(flag_hash){
			trans_ptr = table_trans[color].find(*this);
			if(trans_ptr != table_trans[color].end()){
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
				assert(alpha <= beta);
			}else{
				trans_ptr = table_trans[color].insert(
					std::make_pair(*this,interval(_inf,inf))
				).first;
			}
			alpha_save = alpha;
			beta_save = beta;
		}

		brd_val vec[32];
		brd_val* ptr = vec;
		board brd;
		calc_type result;
		calc_type* ptr_val = table_val[this->sum()];
		const method mthd_de_pvs = method(mthd & ~mthd_pvs);
		brd_type brd_move = this->get_move(color);
		brd_type pos;

		trail_zero_count(brd_move,pos);
		while(brd_move){
			ptr->pos = pos;
			if(mthd & mthd_kill){
				ptr->val = ptr_val[pos];
			}
			++ptr;
			brd_move &= brd_move - 1;
			trail_zero_count(brd_move,pos);
		}

		if(ptr != vec){

			if(flag_kill){
				make_heap(vec,ptr,
					[](cbrd_val b1,cbrd_val b2){
						return b1.val < b2.val;
					}
				);
			}

			for(brd_val* p = ptr;p != vec;){

				if(flag_kill){
					pop_heap(vec,p,
						[](cbrd_val b1,cbrd_val b2){
							return b1.val < b2.val;
						}
					);
				}

				--p;
				brd = *this;
				brd.flip(color,p->pos);

				if(flag_pvs){
					if(p + 1 != ptr){
						result = - brd.template search<mthd_de_pvs>(!color,depth - 1,-alpha - 1,-alpha);
						if(result > alpha && result < beta)
							result = - brd.template search<mthd>(!color,depth - 1,-beta,-alpha);
					}else{
						result = - brd.template search<mthd>(!color,depth - 1,-beta,-alpha);
					}
				}else{
					result = - brd.template search<mthd>(!color,depth - 1,-beta,-alpha);
				}
				if(flag_kill){
					ptr_val[p->pos] = result;
				}
				if(result >= beta){
					trans_save(result);
					return beta;
				}
				if(result > alpha){
					alpha = result;
				}
			}

			trans_save(alpha);

			return alpha;

		}else{

			brd_move = this->get_move(!color);
			trail_zero_count(brd_move,pos);
			while(brd_move){
				ptr->pos = pos;
				if(flag_kill){
					ptr->val = ptr_val[pos];
				}
				++ptr;
				brd_move &= brd_move - 1;
				trail_zero_count(brd_move,pos);
			}

			if(ptr != vec){

				if(flag_kill){
					make_heap(vec,ptr,
						[](cbrd_val b1,cbrd_val b2){
							return b1.val > b2.val;
						}
					);
				}

				for(brd_val* p = ptr;p != vec;){

					if(flag_kill){
						pop_heap(vec,p,
							[](cbrd_val b1,cbrd_val b2){
								return b1.val > b2.val;
							}
						);
					}

					--p;
					brd = *this;
					brd.flip(!color,p->pos);

					if(flag_pvs){
						if(p + 1 != ptr){
							result = brd.template search<mthd_de_pvs>(color,depth - 1,beta - 1,beta);
							if(result > alpha && result < beta)
								result = brd.template search<mthd>(color,depth - 1,alpha,beta);
						}else{
							result = brd.template search<mthd>(color,depth - 1,alpha,beta);
						}
					}else{
						result = brd.template search<mthd>(color,depth - 1,alpha,beta);
					}
					if(flag_kill){
						ptr_val[p->pos] = result;
					}
					if(result <= alpha){
						trans_save(result);
						return alpha;
					}
					if(result < beta){
						beta = result;
					}
				}

				trans_save(beta)

				return beta;

			}else{

				calc_type num_diff = count(color) - count(!color);
				if(num_diff > 0){
					result =  num_diff + mark_max;
				}else if(num_diff < 0){
					result = num_diff - mark_max;
				}else{
					result = 0;
				}
				trans_save(result);
				return result;

			}
		}

	}

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<< " depth=" << depth
		<< " alpha=" << alpha
		<< " beta=" << beta
		<< ">\n";
	this->print(out);
	calc_type result = fun();
	out << "result = " << result <<"\n"
		<< "</div>\n";
	return result;
	#endif
}

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif

vector<choice> board::get_choice(
	cmethod mthd,cbool color,cshort height,ccalc_type gamma
)const{

    vector<choice> choices;
	calc_type result;
    choice temp;
	calc_type alpha = _inf;

	clear_search_info();

//	if(mthd & mthd_kill){
//		for(int i = this->sum();i != size2;++i){
//			for(int j = 0;j != size2;++j){
//				if(table_val[i][j] == 0){
//					table_val[i][j] = table_val_init[j];
//				}
//			}
//		}
//	}

	choices.reserve(30);

	brd_type brd_move = this->get_move(color);
	brd_type pos;

    board brd = *this;
	trail_zero_count(brd_move,pos);
	while(brd_move){
		brd.flip(color,pos);
		result = - brd.search(mthd,!color,height,_inf,-alpha,gamma);
//		if(result - 5 > alpha){
//			alpha = result - 5;
//		}
		temp.val = result;
		temp.brd = brd;
		temp.pos = pos;
		choices.push_back(temp);
		brd = *this;
		brd_move &= brd_move - 1;
		trail_zero_count(brd_move,pos);
	}

    return choices;
}
