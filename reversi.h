/** @mainpage Reversi Documentation

 * @section sec_rv Introduction of Reversi
	Reversi (also called Othello) is a kind of strategy board game
	which involves play by two parties on an eight-by-eight square grid.
	
 * @section sec_pro Introduction of the Program

 */

/** @file reversi.h
 * @brief This head file is an essential part of the program
 * where the most important class board is defined.
 * Many basic functions are declared here.

 */

#ifndef REVERSI_H
#define REVERSI_H

#include <iostream>
#include <limits>
#include <cassert>
#include <utility>
#include <tuple>
#include <vector>
#include <unordered_map>

using namespace std;

//#define DEBUG_SEARCH
#define USE_FLOAT
#define USE_RANDOM

#include "type.h"

struct coordinate{
	coordinate():x(-1),y(-1){}
	coordinate(cpos_type _x,cpos_type _y):x(_x),y(_y){}
	coordinate(cpos_type pos):x(pos & 7),y(pos >> 3){}
	pos_type x;
	pos_type y;
	void print()const{
		cout << '(' << x << ',' << y << ')';
	}
};
typedef const coordinate& ccoordinate;

namespace std{
	template <>
	struct hash<board>;
}

/** @class board 
 *
 * @brief It represent the board of reversi.
 *
	It has two data member which are brd_black and brd_white.
	They represent the board of black stones and board of white stones respectively.
	Both brd_black and brd_white are 64-bit integer.
	Where the bit is set to 1, there is a stone.
	We can assume that brd_black | brd_white is always zero
	because we can never place a black stone and a white stone at the same cell.
	This kind of data structure saves a lot of memory and is easy to calculate.
 *
 */

class board{
	friend struct hash<board>;
public:

	/** @fn board()
	* @brief The default constructor of class board
		This function do nothing at all.
		The value of the object board is not defined
		if it's constructed by this function.
		To initial the object, please use the function initial() .
	*/
	board(){};
	board(cbrd_type _brd_black,cbrd_type _brd_white)
		:brd_black(_brd_black),brd_white(_brd_white){}

	friend bool operator==(const board& b1,const board& b2){
		return (b1.brd_black == b2.brd_black) && (b1.brd_white == b2.brd_white);
	}

	static const brd_type last;
	static const pos_type chessman_num = 4;
	static const pos_type size = 8;
	static const pos_type size2 = size * size;
	static const char chr_print[chessman_num];
	enum pos_enum{pos_angle,pos_edge,pos_inner,pos_center};
	static const pos_type pos_num = 4;
	static const pos_type stage_num = 3;
	static const short max_height = 20;

	typedef pair<calc_type,calc_type> interval;

	static calc_type table_param[stage_num][pos_num];

	friend ostream& operator<<(ostream& out,const board& brd){
		brd.do_print(out);
		return out;
	}

	/** @fn const board& print(ostream& out = cout)const
	 *	@brief It's a function used to show the board on a terminal.
	 *	@param out the output stream
	*/
	const board& print(ostream& out = cout)const{
		return do_print(out);
	}

	/** @fn board& print(ostream& out = cout)
	 *	@brief It's a function used to show the board on a terminal.
	 *	@param out the output stream
	*/
	board& print(ostream& out = cout){
		do_print(out);
		return *this;
	}

	/** @fn board& assign(cbrd_type _brd_black,cbrd_type _brd_white)
	 *	@brief Assign the board to some specific value.
	 *	@param _brd_black the value of the 64-bit board of black stones
	 *	@param _brd_black the value of the 64-bit board of white stones
	*/
	board& assign(cbrd_type _brd_black,cbrd_type _brd_white){
		brd_black = _brd_black;
		brd_white = _brd_white;
		return *this;
	}

	/** @fn board& initial()
	 *	@brief initialize the board
	*/
	board& initial(){
		return this->assign(0x0000000810000000,0x0000001008000000);
	}

	cbrd_type bget(cbool color)const{
		if(color){
			return brd_black;
		}else{
			return brd_white;
		}
	}
	brd_type& bget(cbool color){
		if(color){
			return brd_black;
		}else{
			return brd_white;
		}
	}

	chessman get(cpos_type pos)const{
		brd_type mask = brd_type(1) << pos;
		if(mask & brd_black){
			if(mask & brd_white){
				return null;
			}else{
				return black;
			}
		}else{
			if(mask & brd_white){
				return white;
			}else{
				return blank;
			}
		}
	}

	board& set(cbool color,cpos_type pos,cbool flag){
		brd_type mask = brd_type(1) << pos;
		if(color){
			if(flag){
				brd_black |= mask;
			}else{
				brd_white &= ~mask;
			}
		}else{
			if(flag){
				brd_black |= mask;
			}else{
				brd_white &= ~mask;
			}
		}
		return *this;
	}
	board& set(cpos_type pos, cchessman chsm){
		brd_type mask = brd_type(1) << pos;
		if(chsm & white){
			brd_white |= mask;
		}else{
			brd_white &= ~mask;
		}
		if(chsm & black){
			brd_black |= mask;
		}else{
			brd_black &= ~mask;
		}
		return *this;
	}

	board& mirror(cbool is_horizontal);
	board& rotate_r(pos_type n90);
	board& rotate_l(pos_type n90){
		return rotate_r(4 - n90);
	}

	short count(cbool color)const{
		return count(bget(color));
	}
	pos_type sum()const{
		return count(brd_black ^ brd_white);
	}
	brd_type get_move(cbool color)const{
		const brd_type& brd_blue = bget(color);
		const brd_type& brd_green = bget(!color);
		brd_type moves;
		brd_type brd_green_inner;
		brd_type brd_flip;
		brd_type brd_green_adj;

		brd_green_inner = brd_green & 0x7E7E7E7E7E7E7E7Eu;

		brd_flip = (brd_blue >> 1) & brd_green_inner;
		brd_flip |= (brd_flip >> 1) & brd_green_inner;

		brd_green_adj = brd_green_inner & (brd_green_inner >> 1);
		brd_flip |= (brd_flip >> 2) & brd_green_adj;
		brd_flip |= (brd_flip >> 2) & brd_green_adj;

		moves = brd_flip >> 1;

		brd_flip = (brd_blue << 1) & brd_green_inner;
		brd_flip |= (brd_flip << 1) & brd_green_inner;

		brd_green_adj = brd_green_inner & (brd_green_inner << 1);
		brd_flip |= (brd_flip << 2) & brd_green_adj;
		brd_flip |= (brd_flip << 2) & brd_green_adj;

		moves |= brd_flip << 1;

		brd_flip = (brd_blue >> 8) & brd_green;
		brd_flip |= (brd_flip >> 8) & brd_green;

		brd_green_adj = brd_green & (brd_green >> 8);
		brd_flip |= (brd_flip >> 16) & brd_green_adj;
		brd_flip |= (brd_flip >> 16) & brd_green_adj;

		moves |= brd_flip >> 8;

		brd_flip = (brd_blue << 8) & brd_green;
		brd_flip |= (brd_flip << 8) & brd_green;

		brd_green_adj = brd_green & (brd_green << 8);
		brd_flip |= (brd_flip << 16) & brd_green_adj;
		brd_flip |= (brd_flip << 16) & brd_green_adj;

		moves |= brd_flip << 8;

		brd_flip = (brd_blue >> 7) & brd_green_inner;
		brd_flip |= (brd_flip >> 7) & brd_green_inner;
		
		brd_green_adj = brd_green_inner & (brd_green_inner >> 7);
		brd_flip |= (brd_flip >> 14) & brd_green_adj;
		brd_flip |= (brd_flip >> 14) & brd_green_adj;
		
		moves |= brd_flip >> 7;

		brd_flip = (brd_blue << 7) & brd_green_inner;
		brd_flip |= (brd_flip << 7) & brd_green_inner;

		brd_green_adj = brd_green_inner & (brd_green_inner << 7);
		brd_flip |= (brd_flip << 14) & brd_green_adj;
		brd_flip |= (brd_flip << 14) & brd_green_adj;

		moves |= brd_flip << 7;

		brd_flip = (brd_blue >> 9) & brd_green_inner;
		brd_flip |= (brd_flip >> 9) & brd_green_inner;
		
		brd_green_adj = brd_green_inner & (brd_green_inner >> 9);
		brd_flip |= (brd_flip >> 18) & brd_green_adj;
		brd_flip |= (brd_flip >> 18) & brd_green_adj;
		
		moves |= brd_flip >> 9;
		
		brd_flip = (brd_blue << 9) & brd_green_inner;
		brd_flip |= (brd_flip << 9) & brd_green_inner;

		brd_green_adj = brd_green_inner & (brd_green_inner << 9);
		brd_flip |= (brd_flip << 18) & brd_green_adj;
		brd_flip |= (brd_flip << 18) & brd_green_adj;

		moves |= brd_flip << 9;

		moves &= ~(brd_blue | brd_green);
		return moves;
	}
	short count_move(cbool color)const{
		return count(get_move(color));
	}

	static void config(){
		config_flip();
		config_search();
	}

	bool flip(cbool color,cpos_type pos);

	calc_type score(cbool color,cpos_type stage)const{
		calc_type result = 0;

		brd_type brd_blue = bget(color);
		brd_type brd_green = bget(!color);

		result += (count(brd_blue & 0x8100000000000081) - count(brd_green & 0x8100000000000081))
			* table_param[stage][0];
		result += (count(brd_blue & 0x7e8181818181817e) - count(brd_green & 0x7e8181818181817e))
			* table_param[stage][1];
		result += (count(brd_blue & 0x0042000000004200) - count(brd_green & 0x0042000000004200))
			* table_param[stage][2];
		result += (count(brd_blue & 0x003c7e7e7e7e3c00) - count(brd_green & 0x003c7e7e7e7e3c00))
			* table_param[stage][3];

		return result;
	}

	calc_type search(
		cmethod mthd,cbool color,
		cshort height,ccalc_type alpha,ccalc_type beta,
		ccalc_type acc,cshort stage,ccalc_type gamma = 0
	)const{
		if(mthd == mthd_rnd){
			return 0;
		}else if(mthd & mthd_ptn){
			return search_ptn(color,height,alpha,beta);
		}else if(mthd & mthd_mtdf){
			return search_mtd(color,height,alpha,beta,acc,stage,gamma);
		}else if(mthd & mthd_trans){
			return search_trans(color,height,alpha,beta,acc,stage);
		}else if(mthd & mthd_pvs){
			return search_pvs(color,height,alpha,beta,acc,stage);
		}else if(mthd & mthd_ab){
			return search_ab(color,height,alpha,beta,acc,stage);
		}else{
			assert(false);
			return 0;
		}
	};

	calc_type search_ab(cbool color,cshort height,calc_type alpha,calc_type beta,calc_type acc,cshort stage)const;
	calc_type search_pvs(cbool color,cshort height,calc_type alpha,calc_type beta,calc_type acc,cshort stage)const;
	calc_type search_trans(cbool color,cshort height,calc_type alpha,calc_type beta,calc_type acc,cshort stage)const;
	calc_type search_mtd(cbool color,cshort height,calc_type alpha,calc_type beta,ccalc_type acc,cshort stage,calc_type gamma)const;
	float search_ptn(cbool color,cshort height,float alpha,float beta)const;

	vector<choice> get_choice(cmethod mthd,cbool color,cshort height,cshort stage = -1,ccalc_type gamma = 0)const;

	static choice select_choice(vector<choice> choices,const float& variation = 0.75);

	coordinate play(cmethod mthd,cbool color,short height = -1,cshort stage = -1);

	sts_type get_status(cbool color){
		bool flag_black = (count_move(true) == 0);
		bool flag_white = (count_move(false) == 0);
		int num_diff = count(true) - count(false);
		if(color){
			if(flag_black){
				if(flag_white){
					if(num_diff){
						if(num_diff > 0){
							return sts_bwin;
						}else{
							return sts_wwin;
						}
					}else{
						return sts_tie;
					}
				}else{
					return sts_wagain;
				}
			}else{
				return sts_bturn;
			}
		}else{
			if(flag_white){
				if(flag_black){
					if(num_diff){
						if(num_diff > 0){
							return sts_bwin;
						}else{
							return sts_wwin;
						}
					}else{
						return sts_tie;
					}
				}else{
					return sts_bagain;
				}
			}else{
				return sts_wturn;
			}
		}
	}

	float& extract_ptn(cbool color, float* const& ptr, cbrd_type mask, cshort num)const;
	float score_ptn(cbool color)const;
	vector<float> eval_ptn(cbool color)const;
	void adjust_ptn(cbool,ccalc_type diff)const;

protected:

	brd_type brd_black,brd_white;

	static float table_temp[2][board::max_height + 1][board::size2];

	static pos_type table_pos[board::size2][board::size2];
	static pos_type table_check[board::size2][board::size2];

	static void config_flip();
	static void config_search();

	inline static brd_type extract(cbrd_type brd,cbrd_type mask){
		brd_type result;
		asm volatile(
			"pext %1, %2, %0;"
			: "=&r"(result)
			: "r"(mask), "r"(brd)
			:
		);
		return result;
	}

	static brd_type deposit(cbrd_type brd,cbrd_type mask){
		brd_type result;
		asm volatile(
			"pdep %1, %2, %0;"
			: "=&r"(result)
			: "r"(mask), "r"(brd)
			:
		);
		return result;
	}

	/** @fn static void mirror_h(brd_type& brd)
	 *	@brief It's a function used to mirror a 64-bit board horizontally.
	 *	@param brd the 64-bit board
	*/
	static void mirror_h(brd_type& brd){
		brd = (brd & 0xaaaaaaaaaaaaaaaa) >> 1  | (brd & 0x5555555555555555) << 1;
		brd = (brd & 0xcccccccccccccccc) >> 2  | (brd & 0x3333333333333333) << 2;
		brd = (brd & 0xf0f0f0f0f0f0f0f0) >> 4  | (brd & 0x0f0f0f0f0f0f0f0f) << 4;
	}

	/** @fn static void mirror_v(brd_type& brd)
	 *	@brief It's a function used to mirror a 64-bit board vertically.
	 *	@param brd the 64-bit board
	*/
	static void mirror_v(brd_type& brd){
		asm volatile(
			"bswap %0;"
			: "=r"(brd)
			: "0"(brd)
			:
		);
	}

	/** @fn static void reflect(brd_type& brd)
	 *	@brief It's a function used to reflect a 64-bit board.
	 *	@param brd the 64-bit board
	*/
	static void reflect(brd_type& brd){
		mirror_h(brd);
		mirror_v(brd);
	}

	/** @fn static void rotate_r(brd_type& brd)
	 *	@brief It's a function used to rotate a 64-bit board clockwise.
	 *	@param brd the 64-bit board
	*/
	static void rotate_r(brd_type& brd){
		brd = (brd & 0xf0f0f0f000000000) >> 4  | (brd & 0x0f0f0f0f00000000) >> 32
			| (brd & 0x00000000f0f0f0f0) << 32 | (brd & 0x000000000f0f0f0f) << 4;
		brd = (brd & 0xcccc0000cccc0000) >> 2  | (brd & 0x3333000033330000) >> 16
			| (brd & 0x0000cccc0000cccc) << 16 | (brd & 0x0000333300003333) << 2;
		brd = (brd & 0xaa00aa00aa00aa00) >> 1  | (brd & 0x5500550055005500) >> 8
			| (brd & 0x00aa00aa00aa00aa) << 8  | (brd & 0x0055005500550055) << 1;
	}

	/** @fn static void rotate_l(brd_type& brd)
	 *	@brief It's a function used to rotate a 64-bit board counter-clockwise.
	 *	@param brd the 64-bit board
	*/
	static void rotate_l(brd_type& brd){
		brd = (brd & 0xf0f0f0f000000000) >> 32 | (brd & 0x0f0f0f0f00000000) << 4
			| (brd & 0x00000000f0f0f0f0) >> 4  | (brd & 0x000000000f0f0f0f) << 32;
		brd = (brd & 0xcccc0000cccc0000) >> 16 | (brd & 0x3333000033330000) << 2
			| (brd & 0x0000cccc0000cccc) >> 2  | (brd & 0x0000333300003333) << 16;
		brd = (brd & 0xaa00aa00aa00aa00) >> 8  | (brd & 0x5500550055005500) << 1
			| (brd & 0x00aa00aa00aa00aa) >> 1  | (brd & 0x0055005500550055) << 8;
	}
	static void transform(brd_type& brd){
		brd_type brd_high = brd & 0x0103070f1f3f7fff;
		brd_high = (brd_high & 0xffffffff00000000) << 4 | (brd_high & 0x00000000ffffffff);
		brd_high = (brd_high & 0xffff0000ffff0000) << 2 | (brd_high & 0x0000ffff0000ffff);
		brd_high = (brd_high & 0xff00ff00ff00ff00) << 1 | (brd_high & 0x00ff00ff00ff00ff);
		brd_type brd_low = (brd & 0xfefcf8f0e0c08000) >> 1;
		brd_low = (brd_low & 0xffffffff00000000) | (brd_low & 0x00000000ffffffff) >> 4;
		brd_low = (brd_low & 0xffff0000ffff0000) | (brd_low & 0x0000ffff0000ffff) >> 2;
		brd_low = (brd_low & 0xff00ff00ff00ff00) | (brd_low & 0x00ff00ff00ff00ff) >> 1;
		brd = brd_high | brd_low;
	}

	/** @fn static void count(brd_type& brd)
	 *	@brief It's a function used to count the number of bit
	 *	which are set in a 64-bit board.
	 *	@param brd the 64-bit board
	*/
	static pos_type count(cbrd_type brd){
		brd_type result;
		asm volatile(
			"popcnt %1, %0;"
			: "=&r"(result)
			: "r"(brd)
			:
		);
		return result;
	}

	const board& do_print(ostream& out)const;

	#ifdef USE_FLOAT
		static const calc_type mark_max;
	#else
		static const calc_type mark_max = 10000;
	#endif
};

template <>
struct hash<board> : public unary_function<board, size_t>{
	size_t operator()(const board& brd) const{
		return (
			size_t(brd.brd_black)
			+ size_t(brd.brd_black >> 32) * 1867970917
			+ size_t(brd.brd_white) * 1009562269
			+ size_t(brd.brd_white >> 32) * 739351663
		);
	}
};

struct brd_val{
	board brd;
	pos_type pos;
	calc_type val;
	friend inline bool operator<(const brd_val& b1,const brd_val& b2){
		return b1.val < b2.val;
	}
	friend inline bool operator>(const brd_val& b1,const brd_val& b2){
		return b1.val > b2.val;
	}
};

struct choice{
	float val;
	board brd;
	pos_type pos;
	float rnd_val;
	void print()const{
		cout << '(' << val << ',' << (pos & 7) << ',' << (pos >> 3) << ')';
	}
};

extern unordered_map<board,board::interval> trans_black;
extern unordered_map<board,board::interval> trans_white;

#endif // REVERSI_H
