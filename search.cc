#include <algorithm>
#include <cmath>

#include "reversi.h"
#include "search.h"
#include "pattern.h"

#ifdef USE_ASM
	#define trail_zero_count(brd,result) \
		asm_tzcnt(brd,result)
#else
	#define trail_zero_count(brd,result) \
		result = count(~brd & (brd - 1))
#endif

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

const brd_type mask_adj[board::size2] = {
	0x0000000000000302,
	0x0000000000000705,
	0x0000000000000e0a,
	0x0000000000001c14,
	0x0000000000003828,
	0x0000000000007050,
	0x000000000000e0a0,
	0x000000000000c040,

	0x0000000000030203,
	0x0000000000070507,
	0x00000000000e0a0e,
	0x00000000001c141c,
	0x0000000000382838,
	0x0000000000705070,
	0x0000000000e0a0e0,
	0x0000000000c040c0,

	0x0000000003020300,
	0x0000000007050700,
	0x000000000e0a0e00,
	0x000000001c141c00,
	0x0000000038283800,
	0x0000000070507000,
	0x00000000e0a0e000,
	0x00000000c040c000,

	0x0000000302030000,
	0x0000000705070000,
	0x0000000e0a0e0000,
	0x0000001c141c0000,
	0x0000003828380000,
	0x0000007050700000,
	0x000000e0a0e00000,
	0x000000c040c00000,

	0x0000030203000000,
	0x0000070507000000,
	0x00000e0a0e000000,
	0x00001c141c000000,
	0x0000382838000000,
	0x0000705070000000,
	0x0000e0a0e0000000,
	0x0000c040c0000000,

	0x0003020300000000,
	0x0007050700000000,
	0x000e0a0e00000000,
	0x001c141c00000000,
	0x0038283800000000,
	0x0070507000000000,
	0x00e0a0e000000000,
	0x00c040c000000000,

	0x0302030000000000,
	0x0705070000000000,
	0x0e0a0e0000000000,
	0x1c141c0000000000,
	0x3828380000000000,
	0x7050700000000000,
	0xe0a0e00000000000,
	0xc040c00000000000,

	0x0203000000000000,
	0x0507000000000000,
	0x0a0e000000000000,
	0x141c000000000000,
	0x2838000000000000,
	0x5070000000000000,
	0xa0e0000000000000,
	0x40c0000000000000
};

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
		search_mthd(mthd | mthd_ab)
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
	#define search_mthd_end(mthd) \
		search_mthd_mpc(mthd) search_mthd_mpc(mthd | mthd_end)

	switch(mthd){
		case mthd_rnd: return board::search<mthd_rnd>(color,depth,alpha,beta);
		search_mthd_end(mthd_rnd);
	default:
		assert(false);
		return 0;
	}

};

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif

struct brd_val{
	pos_type pos;
	calc_type val;
};
typedef const brd_val& cbrd_val;

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

template<method mthd>
calc_type board::search(cbool color,cshort depth,calc_type alpha,calc_type beta,cbool flag_pass)const{

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

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(mthd == mthd_rnd){

		return 0;

	}else if((mthd & mthd_mtdf) && (depth >= depth_mtdf)){

		const method mthd_de_mtdf = method(mthd & ~mthd_mtdf);
		const method mthd_presearch = method(mthd_de_mtdf & ~mthd_end);
		short depth_presearch = (depth & 1) ? 5 : 4;
		calc_type gamma = this->template search<mthd_presearch>(color,depth_presearch,alpha,beta);

		if(mthd & mthd_trans){
			clear_search_info();
		}

		mtdf_info& info = table_mtdf_info[this->sum()][depth][depth_presearch];

		calc_type window_width = sqrt(info.sigma) * 2;
		calc_type window_alpha = gamma + info.bias - window_width / 2;
		calc_type window_beta = gamma + info.bias + window_width / 2;

		calc_type result = this->template search<mthd_de_mtdf>(color,depth, window_alpha, window_beta);
		if(result <= window_alpha){
			do{
				window_width *= 2;
				window_beta = window_alpha;
				window_alpha = window_beta - window_width;
				result = this->template search<mthd_de_mtdf>(color,depth, window_alpha, window_beta);
			}while(result <= window_alpha && result > alpha);
		}else if(result >= window_beta){
			do{
				window_width *= 2;
				window_alpha = window_beta;
				window_beta = window_alpha + window_width;
				result = this->template search<mthd_de_mtdf>(color,depth, window_alpha, window_beta);
			}while(result >= window_beta && result < beta);
		}

		if(result > alpha && result < beta){
			info.adjust(result - gamma);
		}

		return result;

	}else if(mthd & mthd_end && (depth == 5)){
		return this->template search_end_five<mthd>(color,alpha,beta,flag_pass);
	}else{

		++node_count;

		if(depth == 0){
			if(mthd & mthd_end)
				return this->score_end(color);
			else if(mthd & mthd_ptn)
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

		}else if(!flag_pass){

			return - this->template search<mthd>(!color,depth,-beta,-alpha,true);

		}else{

			result = score_end(color);
			trans_save(result);
			return result;

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

calc_type board::search_end_two(
	cbool color,cpos_type pos1,cpos_type pos2,calc_type alpha,calc_type beta,cbool flag_pass
)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	++node_count;

	board brd;
	brd_type brd_blue = bget(color);
	brd_type brd_green = bget(!color);
	brd_type brd_save = brd_black;
	calc_type result = _inf;

	if(brd_green | mask_adj[pos1]){
		brd = *this;
		brd.flip(color,pos1);
		if(brd.brd_black != brd_save){
			if(brd_blue | mask_adj[pos2]){
				brd_save = brd.brd_black;
				brd.flip(!color,pos2);
				if(brd_save == brd.brd_black && (brd_green | mask_adj[pos2])){
					brd.flip(color,pos2);
				}
			}
			result = brd.score_end(color);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(brd_green | mask_adj[pos2]){
		brd = *this;
		brd.flip(color,pos2);
		if(brd.brd_black != brd_save){
			if(brd_blue | mask_adj[pos1]){
				brd_save = brd.brd_black;
				brd.flip(!color,pos1);
				if(brd_save == brd.brd_black && (brd_green | mask_adj[pos1])){
					brd.flip(color,pos1);
				}
			}
			result = brd.score_end(color);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(result != _inf){
		return alpha;
	}else{
		if(!flag_pass){
			return - this->search_end_two(!color,pos1,pos2,-beta,-alpha,true);
		}else{
			return score_end(color);
		}
	}

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<< " depth=" << 2
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

calc_type board::search_end_three(
	cbool color,cpos_type pos1,cpos_type pos2,cpos_type pos3,calc_type alpha,calc_type beta,cbool flag_pass
)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	++node_count;

	board brd;
	brd_type brd_green = bget(!color);
	brd_type brd_save = brd_black;
	calc_type result = _inf;

	if(brd_green | mask_adj[pos1]){
		brd = *this;
		brd.flip(color,pos1);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_two(!color,pos2,pos3,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(brd_green | mask_adj[pos2]){
		brd = *this;
		brd.flip(color,pos2);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_two(!color,pos1,pos3,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(brd_green | mask_adj[pos3]){
		brd = *this;
		brd.flip(color,pos3);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_two(!color,pos1,pos2,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(result != _inf){
		return alpha;
	}else{
		if(!flag_pass){
			return - this->search_end_three(!color,pos1,pos2,pos3,-beta,-alpha,true);
		}else{
			return score_end(color);
		}
	}

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<< " depth=" << 3
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

calc_type board::search_end_four(
	cbool color,cpos_type pos1,cpos_type pos2,cpos_type pos3,cpos_type pos4,calc_type alpha,calc_type beta,cbool flag_pass
)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	++node_count;

	board brd;
	brd_type brd_green = bget(!color);
	brd_type brd_save = brd_black;
	calc_type result = _inf;

	if(brd_green | mask_adj[pos1]){
		brd = *this;
		brd.flip(color,pos1);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_three(!color,pos2,pos3,pos4,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(brd_green | mask_adj[pos2]){
		brd = *this;
		brd.flip(color,pos2);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_three(!color,pos1,pos3,pos4,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(brd_green | mask_adj[pos3]){
		brd = *this;
		brd.flip(color,pos3);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_three(!color,pos1,pos2,pos4,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(brd_green | mask_adj[pos4]){
		brd = *this;
		brd.flip(color,pos4);
		if(brd.brd_black != brd_save){
			result = - brd.search_end_three(!color,pos1,pos2,pos3,-beta,-alpha,false);
			if(result >= beta){
				return beta;
			}
			if(result > alpha){
				alpha = result;
			}
		}
	}

	if(result != _inf){
		return alpha;
	}else{
		if(!flag_pass){
			return - this->search_end_four(!color,pos1,pos2,pos3,pos4,-beta,-alpha,true);
		}else{
			return score_end(color);
		}
	}

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<< " depth=" << 4
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

template<method mthd>
calc_type board::search_end_five(
	cbool color,calc_type alpha,calc_type beta,cbool flag_pass
)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	const bool flag_kill = (mthd & mthd_kill);
//	bool flag_pvs = (mthd & mthd_pvs) && depth >= depth_pvs;
//	bool flag_hash = (mthd & mthd_trans) && depth >= depth_hash;

	brd_type brd_blank = ~(brd_black | brd_white);

	brd_val vec[32];
	brd_val* ptr = vec;
	board brd;
	calc_type result = _inf;
	calc_type* ptr_val = table_val[this->sum()];
	brd_type brd_green = bget(false);
	brd_type brd_save = this->brd_black;
	brd_type pos;

	trail_zero_count(brd_blank,pos);
	while(brd_blank){
		ptr->pos = pos;
		if(mthd & mthd_kill){
			ptr->val = ptr_val[pos];
		}
		++ptr;
		brd_blank &= brd_blank - 1;
		trail_zero_count(brd_blank,pos);
	}

	sort(vec,ptr,
		[](cbrd_val b1,cbrd_val b2){
			return b1.val > b2.val;
		}
	);

	#define repeat_search_end_five(a,b,c,d,e) \
		if(brd_green | mask_adj[vec[a].pos]){ \
			brd = *this; \
			brd.flip(color,vec[a].pos); \
			if(brd.brd_black != brd_save){ \
				result = - brd.search_end_four(!color,vec[b].pos,vec[c].pos,vec[d].pos,vec[e].pos,-beta,-alpha,false); \
				if(flag_kill){ \
					ptr_val[vec[a].pos] = result; \
				} \
				if(result >= beta){ \
					return beta; \
				} \
				if(result > alpha){ \
					alpha = result; \
				} \
			} \
		}

	repeat_search_end_five(0,1,2,3,4);
	repeat_search_end_five(1,0,2,3,4);
	repeat_search_end_five(2,0,1,3,4);
	repeat_search_end_five(3,0,1,2,4);
	repeat_search_end_five(4,0,1,2,3);

	if(result != _inf){
		return alpha;
	}else{
		if(!flag_pass){
			return - this->template search_end_five<mthd>(!color,-beta,-alpha,true);
		}else{
			return score_end(color);
		}
	}

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<< " depth=" << 5
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
