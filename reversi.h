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

#include "type.h"

struct coordinate{
	coordinate():x(-1),y(-1){}
	coordinate(cpos_type _x,cpos_type _y):x(_x),y(_y){}
	coordinate(cpos_type pos):x(pos & 7),y(pos >> 3){}
	pos_type x;
	pos_type y;
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
	board() = default;
	board(cbrd_type _brd_black,cbrd_type _brd_white)
		:brd_black(_brd_black),brd_white(_brd_white){}

	friend bool operator==(const board& b1,const board& b2){
		return (b1.brd_black == b2.brd_black) && (b1.brd_white == b2.brd_white);
	}

	static const pos_type chessman_num = 4;
	static const pos_type size = 8;
	static const pos_type size2 = size * size;
	static const char chr_print[chessman_num];
	enum pos_enum{pos_angle,pos_edge,pos_inner,pos_center};
	static const pos_type pos_num = 4;
	static const pos_type stage_num = 3;
	static const short max_height = 20;

	static bool flag_unicode;

	typedef pair<calc_type,calc_type> interval;

	static calc_type table_param[stage_num][pos_num];

	friend ostream& operator<<(ostream& out,const board& brd){
		brd.print(out);
		return out;
	}

	/** @fn const board& print(ostream& out = cout)const
	 *	@brief It's a function used to show the board on a terminal.
	 *	@param out the output stream
	*/
	void print(ostream& out = cout)const;

	/** @fn board& assign(cbrd_type _brd_black,cbrd_type _brd_white)
	 *	@brief Assign the board to some specific value.
	 *	@param _brd_black the value of the 64-bit board of black stones
	 *	@param _brd_black the value of the 64-bit board of white stones
	*/
	void assign(cbrd_type _brd_black,cbrd_type _brd_white){
		brd_black = _brd_black;
		brd_white = _brd_white;
	}

	/** @fn board& initial()
	 *	@brief initialize the board
	*/
	void initial(){
		return this->assign(0x0000000810000000,0x0000001008000000);
	}

	cbrd_type bget(cbool color)const{
		#ifdef USE_ASM
			const brd_type* ptr;
			asm volatile(
				"test %3, %3;"
				"cmovnz %1, %0;"
				"cmovz %2, %0"
				:"=r"(ptr)
				:"r"(&brd_black), "r"(&brd_white), "r"(color)
			);
			return *ptr;
		#else
			if(color){
				return brd_black;
			}else{
				return brd_white;
			}
		#endif
	}
	brd_type& bget(cbool color){
		#ifdef USE_ASM
			brd_type* ptr;
			asm volatile(
				"test %3, %3;"
				"cmovnz %1, %0;"
				"cmovz %2, %0"
				:"=r"(ptr)
				:"r"(&brd_black), "r"(&brd_white), "r"(color)
			);
			return *ptr;
		#else
			if(color){
				return brd_black;
			}else{
				return brd_white;
			}
		#endif
	}

	chessman get(cpos_type pos)const{
		#ifdef USE_ASM
			chessman result,temp;
			asm volatile(
				"mov $0, %0;"
				"mov %5, %1;"
				"bt %4, %2;"
				"cmovc %1, %0;"
				"mov %0, %1;"
				"or %6, %1;"
				"bt %4, %3;"
				"cmovc %1, %0;"
				:"=&r"(result), "=&r"(temp)
				:"r"(brd_black), "r"(brd_white), "r"(brd_type(pos)),
					"g"(black), "g"(white)
			);
			return result;
		#else
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
		#endif
	}

	void set(cpos_type pos, cchessman chsm){
		#ifdef USE_ASM
			brd_type temp;
			asm volatile(
				"mov %0, %1;"
				"btr %2, %0;"
				"bts %2, %1;"
				"test %4, %3;"
				"cmovnz %1, %0;"
				:"+r"(brd_black), "=&r"(temp)
				:"r"(brd_type(pos)), "r"(chsm), "g"(black)
			);
			asm volatile(
				"mov %0, %1;"
				"btr %2, %0;"
				"bts %2, %1;"
				"test %4, %3;"
				"cmovnz %1, %0;"
				:"+r"(brd_white), "=&r"(temp)
				:"r"(brd_type(pos)), "r"(chsm), "g"(white)
			);
		#else
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
		#endif
	}

	static brd_type extract(cbrd_type brd,cbrd_type mask){
		brd_type result;

		#ifdef USE_ASM
			asm volatile(
				"pext %1, %2, %0;"
				: "=&r"(result)
				: "r"(mask), "r"(brd)
				:
			);
		#else
			brd_type msk = mask;
			result = 0;
			for(brd_type i = 1;msk;i <<= 1){
				if(brd & msk & -msk){
					result |= i;
				}
				msk &= msk - 1;
			}
		#endif

		return result;
	}

	static brd_type deposit(cbrd_type brd,cbrd_type mask){
		brd_type result;
		#ifdef USE_ASM
			asm volatile(
				"pdep %1, %2, %0;"
				: "=&r"(result)
				: "r"(mask), "r"(brd)
				:
			);
		#else
			brd_type msk = mask;
			result = 0;
			for(pos_type i = 1;msk;i <<= 1){
				if(brd & i){
					result |= msk & -msk;
				}
				msk &= msk - 1;
			}
		#endif
		return result;
	}

	void reverse(){
		swap(brd_black,brd_white);
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
		#ifdef USE_ASM
			asm volatile(
				"bswap %0;"
				: "=r"(brd)
				: "0"(brd)
				:
			);
		#else
			brd = (brd & 0xff00ff00ff00ff00) >> 8  | (brd & 0x00ff00ff00ff00ff) << 8;
			brd = (brd & 0xffff0000ffff0000) >> 16 | (brd & 0x0000ffff0000ffff) << 16;
			brd = (brd & 0xffffffff00000000) >> 32 | (brd & 0x00000000ffffffff) << 32;
		#endif
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

	/** @fn static void count(brd_type& brd)
	 *	@brief It's a function used to count the number of bit
	 *	which are set in a 64-bit board.
	 *	@param brd the 64-bit board
	*/
	static pos_type count(cbrd_type brd){
		brd_type result;

		#ifdef USE_ASM
			asm volatile(
				"popcnt %1, %0;"
				: "=&r"(result)
				: "r"(brd)
				:
			);
		#else
			result = brd - ((brd >> 1) & 0x5555555555555555);
			result = (result & 0x3333333333333333)
				+ ((result >> 2) & 0x3333333333333333);
			result = (result + (result >> 4)) & 0x0F0F0F0F0F0F0F0F;
			return (result * 0x0101010101010101) >> 56;
		#endif

		return result;
	}

	static brd_type get_edge_stable(cbrd_type brd){
		brd_type brd_ul,brd_ur,brd_dl,brd_dr;

		brd_ul = brd;
		brd_ul &= (brd_ul >>  1) | 0x8080808080808080;
		brd_ul &= (brd_ul >>  2) | 0xc0c0c0c0c0c0c0c0;
		brd_ul &= (brd_ul >>  4) | 0xf0f0f0f0f0f0f0f0;
		brd_dl = brd_ul;
		brd_ul &= (brd_ul >>  8) | 0xff00000000000000;
		brd_ul &= (brd_ul >> 16) | 0xffff000000000000;
		brd_ul &= (brd_ul >> 32) | 0xffffffff00000000;
		brd_dl &= (brd_dl <<  8) | 0x00000000000000ff;
		brd_dl &= (brd_dl << 16) | 0x000000000000ffff;
		brd_dl &= (brd_dl << 32) | 0x00000000ffffffff;

		brd_ur = brd;
		brd_ur &= (brd_ur <<  1) | 0x0101010101010101;
		brd_ur &= (brd_ur <<  2) | 0x0303030303030303;
		brd_ur &= (brd_ur <<  4) | 0x0f0f0f0f0f0f0f0f;
		brd_dr = brd_ur;
		brd_ur &= (brd_ur >>  8) | 0xff00000000000000;
		brd_ur &= (brd_ur >> 16) | 0xffff000000000000;
		brd_ur &= (brd_ur >> 32) | 0xffffffff00000000;
		brd_dr &= (brd_dr <<  8) | 0x00000000000000ff;
		brd_dr &= (brd_dr << 16) | 0x000000000000ffff;
		brd_dr &= (brd_dr << 32) | 0x00000000ffffffff;

		return brd_ul | brd_ur | brd_dl | brd_dr;
	}

	static brd_type get_stable(cbrd_type brd){
		brd_type brd_l,brd_r,brd_u,brd_d,brd_ul,brd_ur,brd_dl,brd_dr;

		brd_l  = brd;
		brd_l  &= (brd_l  >>  1) | 0x8080808080808080;
		brd_l  &= (brd_l  >>  2) | 0xc0c0c0c0c0c0c0c0;
		brd_l  &= (brd_l  >>  4) | 0xf0f0f0f0f0f0f0f0;

		brd_r  = brd;
		brd_r  &= (brd_r  <<  1) | 0x0101010101010101;
		brd_r  &= (brd_r  <<  2) | 0x0303030303030303;
		brd_r  &= (brd_r  <<  4) | 0x0f0f0f0f0f0f0f0f;

		brd_u  = brd;
		brd_u  &= (brd_u  >>  8) | 0xff00000000000000;
		brd_u  &= (brd_u  >> 16) | 0xffff000000000000;
		brd_u  &= (brd_u  >> 32) | 0xffffffff00000000;

		brd_d  = brd;
		brd_d  &= (brd_d  <<  8) | 0x00000000000000ff;
		brd_d  &= (brd_d  << 16) | 0x000000000000ffff;
		brd_d  &= (brd_d  << 32) | 0x00000000ffffffff;

		brd_ul = brd;
		brd_ul &= (brd_ul >>  9) | 0xff80808080808080;
		brd_ul &= (brd_ul >> 18) | 0xffffc0c0c0c0c0c0;
		brd_ul &= (brd_ul >> 36) | 0xfffffffff0f0f0f0;

		brd_dl = brd;
		brd_dl &= (brd_dl <<  7) | 0x80808080808080ff;
		brd_dl &= (brd_dl << 14) | 0xc0c0c0c0c0c0ffff;
		brd_dl &= (brd_dl << 28) | 0xf0f0f0f0ffffffff;

		brd_ur = brd;
		brd_ur &= (brd_ur >>  7) | 0xff01010101010101;
		brd_ur &= (brd_ur >> 14) | 0xffff030303030303;
		brd_ur &= (brd_ur >> 28) | 0xffffffff0f0f0f0f;

		brd_dr = brd;
		brd_dr &= (brd_dr <<  9) | 0x01010101010101ff;
		brd_dr &= (brd_dr << 18) | 0x030303030303ffff;
		brd_dr &= (brd_dr << 36) | 0x0f0f0f0fffffffff;

		return (brd_l | brd_r) & (brd_u | brd_d)
			& (brd_ul | brd_dr) & (brd_ur | brd_dl);
	}

	static brd_type get_front(cbrd_type brd){
		brd_type brd_reverse, brd_front;

		brd_reverse = ~brd;
		brd_front = 0;

		brd_front |= brd & (brd_reverse >> 1) & 0x7f7f7f7f7f7f7f7f;
		brd_front |= brd & (brd_reverse << 1) & 0xfefefefefefefefe;
		brd_front |= brd & (brd_reverse >> 8);
		brd_front |= brd & (brd_reverse << 8);
		brd_front |= brd & (brd_reverse >> 9) & 0x7f7f7f7f7f7f7f7f;
		brd_front |= brd & (brd_reverse >> 7) & 0xfefefefefefefefe;
		brd_front |= brd & (brd_reverse << 7) & 0x7f7f7f7f7f7f7f7f;
		brd_front |= brd & (brd_reverse << 9) & 0xfefefefefefefefe;

		return brd_front;
	}

	void mirror_h(){
		mirror_h(brd_black);
		mirror_h(brd_white);
	}
	void mirror_v(){
		mirror_v(brd_black);
		mirror_v(brd_white);
	}
	void rotate_l(){
		rotate_l(brd_black);
		rotate_l(brd_white);
	}
	void rotate_r(){
		rotate_r(brd_black);
		rotate_r(brd_white);
	}
	void reflect(){
		reflect(brd_black);
		reflect(brd_white);
	}

	short count(cbool color)const{
		return count(bget(color));
	}
	pos_type sum()const{
		return count(brd_black | brd_white);
	}
	brd_type get_move(cbool color)const{
		// This part of code is brought from Zebra.
		// I rewrite it in 64-bit style.

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

	calc_type score(cbool color)const{
		brd_type brd_blue = bget(color);
		brd_type brd_green = bget(!color);
		brd_type brd_mix = brd_blue | brd_white;
		brd_type brd_temp;

		short stage;
		short total = count(brd_mix);
		if(total <= 40){
			stage = 0;
		}else if(total <= size2 - 7){
			stage = 1;
		}else{
			stage = 2;
		}

		calc_type result = 0;
		result += count_move(color) - count_move(!color);
		brd_temp = get_stable(brd_mix);
		result += count(brd_blue & brd_temp) - count(brd_green & brd_temp);
		brd_temp = get_front(brd_mix);
		result += count(brd_green & brd_temp) - count(brd_blue & brd_temp);

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

	template<method mthd>
	calc_type search(cbool color,cshort height,calc_type alpha,calc_type beta)const;

	calc_type search(
		cmethod mthd,cbool color,cshort height,
		ccalc_type alpha = _inf,ccalc_type beta = inf,ccalc_type gamma = 0
	)const;

	vector<choice> get_choice(cmethod mthd,cbool color,cshort height,ccalc_type gamma = 0)const;

	static choice select_choice(vector<choice> choices,const float& variation = 0.75);

	coordinate play(cmethod mthd,cbool color,short height = -1);

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

	float score_ptn(cbool color)const;

protected:

	brd_type brd_black,brd_white;

	static void config_flip();
	static void config_search();

	#ifdef USE_FLOAT
		static const calc_type mark_max;
	#else
		static const calc_type mark_max = 100;
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

struct choice{
	float val;
	board brd;
	pos_type pos;
	float rnd_val;
};

#endif // REVERSI_H
