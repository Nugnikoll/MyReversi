/** @file search.cc
 * @brief This file includes implementation of search function.
 */

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
const short depth_mtdf = 4;
const short depth_mpc = 3;

calc_type table_val[board::size2 + 1][board::size2];
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

short table_mtdf_depth[board::size2] = {
	0,0,0,0,0,1,2,3,
	4,5,4,5,6,5,6,5,
	6,5,6,5,6,5,6,5,
	6,5,6,5,6,5,6,5,
	6,5,6,5,6,5,6,5,
	6,5,6,5,6,5,6,5,
	6,5,6,5,6,5,6,5,
	6,5,6,5,6,5,6,5,
};

short table_mpc_depth[board::size2] = {
	0,0,0,0,0,1,2,3,
	4,4,4,5,5,5,5,5,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6
};

calc_type table_mpc_threshold[board::size2] = {
	2.50,2.57,2.65,2.73,2.81,2.90,2.99,3.08,
	3.18,3.28,3.39,3.50,3.61,3.73,3.85,3.98,
	4.11,4.24,4.38,4.52,4.66,4.81,4.96,5.12,
	5.27,5.44,5.60,5.78,5.95,6.13,6.31,6.50,
	6.69,6.88,7.08,7.28,7.48,7.69,7.90,8.12,
	8.34,8.56,8.79,9.02,9.26,9.50,9.74,9.99,
	10.24,10.49,10.75,11.01,11.28,11.55,11.82,12.10,
	12.38,12.66,12.95,13.24,13.54,13.84,14.14,14.45
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
	ccalc_type alpha,ccalc_type beta
)const{

	if(mthd & mthd_ids){
		method mthd_de_ids = method(mthd & ~mthd_ids);
		for(short i = 0;i != depth;++i){
			this->search(mthd_de_ids,color,depth,alpha,beta);
		}
	}

	if(mthd & mthd_mtdf){
		method mthd_de_mtdf = method(mthd & ~mthd_mtdf);

		if(depth >= depth_mtdf){

			method mthd_presearch = method(mthd_de_mtdf & ~mthd_end & ~mthd_trans);
			short depth_presearch = table_mtdf_depth[depth];

			calc_type gamma = this->search(mthd_presearch,color,depth_presearch);

			mtdf_info& info = table_mtdf_info[this->sum()][depth][depth_presearch];

			calc_type window_width = sqrt(info.sigma) * 2;
			calc_type window_alpha = gamma + info.bias - window_width / 2;
			calc_type window_beta = gamma + info.bias + window_width / 2;

			calc_type result = this->search(mthd_de_mtdf,color,depth, window_alpha, window_beta);

			if(result <= window_alpha && result > alpha){
				do{
					//window_width *= 2;
					window_beta = result;
					window_alpha = window_beta - window_width;
					//window_alpha = max(window_beta - window_width,alpha);
					result = this->search(mthd_de_mtdf,color,depth, window_alpha, window_beta);
				}while(result <= window_alpha && result > alpha);
			}else if(result >= window_beta && result < beta){
				do{
					//window_width *= 2;
					window_alpha = result;
					window_beta = window_alpha + window_width;
					//window_beta = min(window_alpha + window_width,beta);
					result = this->search(mthd_de_mtdf,color,depth, window_alpha, window_beta);
				}while(result >= window_beta && result < beta);
			}

			if(result > alpha && result < beta){
				info.adjust(result - gamma);
			}

			return result;
		}else{
			return this->search(mthd_de_mtdf,color,depth, alpha, beta);
		}
	}

	#define search_mthd(mthd) \
		case mthd: case mthd | mthd_ids:\
			return board::search<method(mthd)>(color,depth,alpha,beta);
	#define search_mthd_ab(mthd) \
		search_mthd(mthd | mthd_ab)
	#define search_mthd_kill(mthd) \
		search_mthd_ab(mthd) search_mthd_ab(mthd | mthd_kill)
	#define search_mthd_pvs(mthd) \
		search_mthd_kill(mthd) search_mthd_kill(mthd | mthd_pvs)
	#define search_mthd_trans(mthd) \
		search_mthd_pvs(mthd) search_mthd_pvs(mthd | mthd_trans)
	#define search_mthd_ptn(mthd) \
		search_mthd_trans(mthd) search_mthd_trans(mthd | mthd_ptn)
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
	friend bool operator<(const brd_val& b1,const brd_val& b2){
		return b1.val < b2.val;
	}
};
typedef const brd_val& cbrd_val;

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

template<method mthd>
calc_type board::search(cbool color,cshort depth,calc_type alpha,calc_type beta,cbool flag_pass)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(mthd == mthd_rnd){

		return 0;

	}else if(mthd & mthd_end && (depth == 5)){
		return this->template search_end_five<mthd>(color,alpha,beta,flag_pass);
	}else{

		++node_count;

		if(depth == 0){
			if(mthd & mthd_end)
				return this->score_end(color);
			else if(mthd & mthd_ptn)
				return this->score_ptn(color,grp.vec[0]);
			else
				return this->score(color);
		}

		bool flag_hash = (mthd & mthd_trans) && depth >= depth_hash;

		calc_type alpha_save;

		if(flag_hash){
			trans_type::iterator trans_ptr = table_trans[color].find(*this);
			if(trans_ptr != table_trans[color].end()){
				auto& trans_interval = trans_ptr->second;
				if(trans_interval.first >= beta){
					return trans_interval.first;
				}
				if(trans_interval.second <= alpha){
					return trans_interval.second;
				}
				if(trans_interval.first > alpha){
					alpha = trans_interval.first;
				}
				if(trans_interval.second < beta){
					beta = trans_interval.second;
				}
				if(alpha == beta){
					return alpha;
				}
				assert(alpha <= beta);
			}else{
				table_trans[color][*this] = interval(_inf,inf);
			}
			alpha_save = alpha;
		}

		bool flag_kill = (mthd & mthd_kill) && depth >= depth_kill;
		bool flag_pvs = (mthd & mthd_pvs) && depth >= depth_pvs;
		bool flag_mpc = (mthd & mthd_mpc) && depth >= depth_mpc;

		brd_val vec[32];
		brd_val* ptr = vec;
		board brd;
		calc_type temp, result = _inf;
		calc_type* ptr_val = table_val[this->sum()];
		const method mthd_de_pvs = method(mthd & ~mthd_pvs);
		brd_type brd_move = this->get_move(color);
		brd_type pos;

		trail_zero_count(brd_move,pos);
		while(brd_move){
			ptr->pos = pos;
			if(flag_mpc){
				const method mthd_presearch = method(mthd & ~mthd_end & ~mthd_trans);
				board brd = *this;
				brd.flip(color,pos);
				ptr->val = - brd.search<mthd_presearch>(!color,table_mpc_depth[depth]);
			}else if(flag_kill){
				ptr->val = ptr_val[pos];
			}
			++ptr;
			brd_move &= brd_move - 1;
			trail_zero_count(brd_move,pos);
		}

		if(ptr != vec){

			if(flag_mpc){
				calc_type best = max_element(
					vec,ptr,
					[](cbrd_val b1,cbrd_val b2) -> bool{
						return b1.val < b2.val;
					}
				)->val;
				calc_type mpc_threshold = table_mpc_threshold[this->sum()];
				ptr = remove_if(vec,ptr,
					[&mpc_threshold,&best](cbrd_val b) -> bool{
						return b.val < best - mpc_threshold;
					}
				);
			}

			if(flag_kill){
				make_heap(vec,ptr,
					[](cbrd_val b1,cbrd_val b2) -> bool{
						return b1.val < b2.val;
					}
				);
			}

			for(brd_val* p = vec;p != ptr;++p){
				brd = *this;
				brd.flip(color,p->pos);

				if(flag_pvs){
					if(p != vec){
						temp = - brd.template search<mthd_de_pvs>(!color,depth - 1,-alpha - 1,-alpha);
						if(temp > alpha && temp < beta)
							temp = - brd.template search<mthd>(!color,depth - 1,-beta,-alpha);
					}else{
						temp = - brd.template search<mthd>(!color,depth - 1,-beta,-alpha);
					}
				}else{
					temp = - brd.template search<mthd_de_pvs>(!color,depth - 1,-beta,-alpha);
				}
				if(flag_kill){
					ptr_val[p->pos] = temp;
				}
				if(temp >= beta){
					if(flag_hash){
						table_trans[color][*this].first = temp;
					}
					return temp;
				}
				result = max(result,temp);
				alpha = max(alpha,result);
			}

			if(flag_hash){
				if(result > alpha_save){
					table_trans[color][*this] = {result,result};
				}else{
					table_trans[color][*this].second = result;
				}
			}

			return result;

		}else if(!flag_pass){

			return - this->template search<mthd>(!color,depth,-beta,-alpha,true);

		}else{

			result = score_end(color);
			if(flag_hash){
				table_trans[color][*this] = {result,result};
			}
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
	out << "result = " << result << "\n"
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
	brd_type brd_save;
	calc_type result = _inf,temp;

	if(brd_green | mask_adj[pos1]){
		brd = *this;
		brd.flip(color,pos1);
		if(brd.brd_black != brd_black){
			if(brd_blue | mask_adj[pos2]){
				brd_save = brd.brd_black;
				brd.flip(!color,pos2);
				if(brd_save == brd.brd_black && (brd_green | mask_adj[pos2])){
					brd.flip(color,pos2);
				}
			}
			temp = brd.score_end(color);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(brd_green | mask_adj[pos2]){
		brd = *this;
		brd.flip(color,pos2);
		if(brd.brd_black != brd_black){
			if(brd_blue | mask_adj[pos1]){
				brd_save = brd.brd_black;
				brd.flip(!color,pos1);
				if(brd_save == brd.brd_black && (brd_green | mask_adj[pos1])){
					brd.flip(color,pos1);
				}
			}
			temp = brd.score_end(color);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(result != _inf){
		return result;
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
		<< " pos1=" << pos1
		<< " pos2=" << pos2
		<< ">\n";
	this->print(out);
	calc_type result = fun();
	out << "result = " << result << "\n"
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
	calc_type result = _inf,temp;

	if(brd_green | mask_adj[pos1]){
		brd = *this;
		brd.flip(color,pos1);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_two(!color,pos2,pos3,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(brd_green | mask_adj[pos2]){
		brd = *this;
		brd.flip(color,pos2);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_two(!color,pos1,pos3,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(brd_green | mask_adj[pos3]){
		brd = *this;
		brd.flip(color,pos3);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_two(!color,pos1,pos2,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(result != _inf){
		return result;
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
		<< " pos1=" << pos1
		<< " pos2=" << pos2
		<< " pos3=" << pos3
		<< ">\n";
	this->print(out);
	calc_type result = fun();
	out << "result = " << result << "\n"
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
	calc_type result = _inf,temp;

	if(brd_green | mask_adj[pos1]){
		brd = *this;
		brd.flip(color,pos1);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_three(!color,pos2,pos3,pos4,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(brd_green | mask_adj[pos2]){
		brd = *this;
		brd.flip(color,pos2);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_three(!color,pos1,pos3,pos4,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(brd_green | mask_adj[pos3]){
		brd = *this;
		brd.flip(color,pos3);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_three(!color,pos1,pos2,pos4,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(brd_green | mask_adj[pos4]){
		brd = *this;
		brd.flip(color,pos4);
		if(brd.brd_black != brd_black){
			temp = - brd.search_end_three(!color,pos1,pos2,pos3,-beta,-alpha,false);
			if(temp >= beta){
				return temp;
			}
			result = max(result,temp);
			alpha = max(alpha,result);
		}
	}

	if(result != _inf){
		return result;
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
		<< " pos1=" << pos1
		<< " pos2=" << pos2
		<< " pos3=" << pos3
		<< " pos4=" << pos4
		<< ">\n";
	this->print(out);
	calc_type result = fun();
	out << "result = " << result << "\n"
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
	calc_type result = _inf,temp;
	calc_type* ptr_val = table_val[this->sum()];
	brd_type brd_green = bget(false);
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
			if(brd.brd_black != brd_black){ \
				temp = - brd.search_end_four(!color,vec[b].pos,vec[c].pos,vec[d].pos,vec[e].pos,-beta,-alpha,false); \
				if(flag_kill){ \
					ptr_val[vec[a].pos] = result; \
				} \
				if(temp >= beta){ \
					return temp; \
				} \
				result = max(result,temp); \
				alpha = max(alpha,result); \
			} \
		}

	repeat_search_end_five(0,1,2,3,4);
	repeat_search_end_five(1,0,2,3,4);
	repeat_search_end_five(2,0,1,3,4);
	repeat_search_end_five(3,0,1,2,4);
	repeat_search_end_five(4,0,1,2,3);

	if(result != _inf){
		return result;
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
	out << "result = " << result << "\n"
		<< "</div>\n";
	return result;
	#endif

}

vector<choice> board::get_choice(
	cmethod mthd,cbool color,cshort depth
)const{

    vector<choice> choices;
	calc_type result,best;
    choice temp;
	calc_type alpha = _inf, beta = inf;
	calc_type threshold = (mthd & mthd_ptn) ? 3 : 5;
	calc_type* ptr_val = table_val[this->sum()];

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

	trail_zero_count(brd_move,pos);
	while(brd_move){
		temp.brd = *this;
		temp.pos = pos;
		temp.brd.flip(color,pos);
		if(mthd & mthd_kill){
			temp.rnd_val = ptr_val[pos];
		}
		choices.push_back(temp);
		brd_move &= brd_move - 1;
		trail_zero_count(brd_move,pos);
	}

	if(mthd == mthd_rnd){
		return choices;
	}

	if(mthd & mthd_kill){
		sort(choices.begin(),choices.end(),
			[](const choice& c1,const choice& c2) -> bool{
				return c1.rnd_val > c2.rnd_val;
			}
		);
	}


	/*
	if(mthd & mthd_mtdf){
		method mthd_de_mtdf = method(mthd & ~mthd_mtdf);

		if(depth + 1 >= depth_mtdf){

			method mthd_presearch = method(mthd_de_mtdf & ~mthd_end);
			short depth_presearch = table_mtdf_depth[depth + 1];

			calc_type gamma = this->search(mthd_presearch,color,depth_presearch);

			mtdf_info& info = table_mtdf_info[this->sum()][depth][depth_presearch];

			calc_type window_width = sqrt(info.sigma) * 2;
			calc_type window_alpha = gamma + info.bias - window_width / 2;
			calc_type window_beta = gamma + info.bias + window_width / 2;

			clear_search_info();

			best = _inf;
			for(choice& c:choices){
				result = - c.brd.search(mthd_de_mtdf,!color,depth,-window_beta,-window_alpha);
				c.val = result;
				best = max(best,result);
			}

			if(best <= window_alpha && best > alpha){
				do{
					//window_width *= 2;
					window_beta = best;
					window_alpha = window_beta - window_width;
					//window_alpha = max(window_beta - window_width,alpha);
					best = _inf;
					for(choice& c:choices){
						result = - c.brd.search(mthd_de_mtdf,!color,depth,-window_beta,-window_alpha);
						c.val = result;
						best = max(best,result);
					}
				}while(best <= window_alpha && best > alpha);
			}else if(best >= window_beta && best < beta){
				do{
					//window_width *= 2;
					window_alpha = best;
					window_beta = window_alpha + window_width;
					//window_beta = min(window_alpha + window_width,beta);
					best = _inf;
					for(choice& c:choices){
						result = - c.brd.search(mthd_de_mtdf,!color,depth,-window_beta,-window_alpha);
						c.val = result;
						best = max(best,result);
					}
				}while(best >= window_beta && best < beta);
			}

			if(best > alpha && best < beta){
				info.adjust(best - gamma);
			}

		}else{
			best = _inf;
			for(choice& c:choices){
				result = - c.brd.search(mthd_de_mtdf, !color, depth, -beta, -alpha);
				if(mthd & mthd_kill){
					ptr_val[c.pos] = result;
				}
				best = max(best,result);
				alpha = best - threshold;
				c.val = result;
			}
		}
		return choices;
	}
	*/

	best = _inf;
	for(choice& c:choices){
		result = - c.brd.search(mthd, !color, depth, -beta, -alpha);
		if(mthd & mthd_kill){
			ptr_val[c.pos] = result;
		}
		best = max(best,result);
		alpha = best - threshold;
		c.val = result;
	}

    return choices;
}
