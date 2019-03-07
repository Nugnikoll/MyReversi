/** @mainpage Reversi Documentation

 * @section sec_rv Introduction of Reversi
	Reversi (also called Othello) is a kind of strategy board game
	which involves play by two parties on an eight-by-eight square grid.
	
 * @section sec_pro Introduction of the Program

 */

/** @file board.h
 * @brief This head file is an essential part of reversi
 * where the most important class board is defined.
 * Many basic functions are declared here.
 */

#ifndef REVERSI_H
#define REVERSI_H

#include <iostream>
#include <string>
#include <limits>
#include <cassert>
#include <utility>
#include <tuple>
#include <vector>
#include <bitset>

using namespace std;

#include "type.h"
#include "asm.h"

struct coordinate{
	coordinate(): x(-1), y(-1){}
	coordinate(cpos_type _x, cpos_type _y): x(_x),y(_y){}
	coordinate(cpos_type pos): x(pos & 7), y(pos >> 3){}

	pos_type x;
	pos_type y;

	pos_type to_pos()const{
		return (y << 3) | x;
	}
	bool check()const{
		return x >= 0 && x < 8 && y >= 0 && y < 8;
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
public:

	/** @fn board()
	* @brief The default constructor of class board
		This function do nothing at all.
		The value of the object board is not defined
		if it's constructed by this function.
		To initial the object, please use the function initial() .
	*/
	board() = default;
	board(const board& brd) = default;
	board(cull _brd_black, cull _brd_white)
		: brd_white(_brd_white), brd_black(_brd_black){}

	friend bool operator==(const board& b1, const board& b2){
		return b1.brd_black == b2.brd_black && b1.brd_white == b2.brd_white;
	}
	friend bool operator!=(const board& b1, const board& b2){
		return b1.brd_black != b2.brd_black || b1.brd_white != b2.brd_white;
	}

	static const pos_type size = 8;
	static const pos_type size2 = size * size;

	friend ostream& operator<<(ostream& out, const board& brd){
		brd.print(out);
		return out;
	}

	/** @fn const board& print(ostream& out = cout)const
	 *	@brief It's a function used to show the board on a terminal.
	 *	@param out the output stream
	*/
	void print(ostream& out = cout)const;

	matrix<int> to_mat()const;

	/** @fn board& assign(cull _brd_black,cull _brd_white)
	 *	@brief Assign the board to some specific value.
	 *	@param _brd_black the value of the 64-bit board of black stones
	 *	@param _brd_black the value of the 64-bit board of white stones
	*/
	void assign(cull _brd_black, cull _brd_white){
		brd_black = _brd_black;
		brd_white = _brd_white;
	}

	/** @fn board& initial()
	 *	@brief initialize the board
	*/
	void initial(){
		return this->assign(0x0000000810000000,0x0000001008000000);
	}

	cull get_brd(cbool color)const{
		return *(&brd_white + color);
	}
	ull& get_brd(cbool color){
		return *(&brd_white + color);
	}

	chessman get(cpos_type pos)const{
		#ifdef USE_ASM
			chessman result,temp;
			asm volatile(
				"mov $0, %0\n"
				"mov %5, %1\n"
				"bt %4, %2\n"
				"cmovc %1, %0\n"
				"mov %0, %1\n"
				"or %6, %1\n"
				"bt %4, %3\n"
				"cmovc %1, %0\n"
				:"=&r"(result), "=&r"(temp)
				:"r"(brd_black), "r"(brd_white), "r"(ull(pos)),
					"g"(black), "g"(white)
			);
			return result;
		#else
			ull mask = ull(1) << pos;
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
			ull temp;
			asm volatile(
				"mov %0, %1\n"
				"btr %2, %0\n"
				"bts %2, %1\n"
				"test %4, %3\n"
				"cmovnz %1, %0\n"
				:"+r"(brd_black), "=&r"(temp)
				:"r"(ull(pos)), "r"(chsm), "g"(black)
			);
			asm volatile(
				"mov %0, %1\n"
				"btr %2, %0\n"
				"bts %2, %1\n"
				"test %4, %3\n"
				"cmovnz %1, %0\n"
				:"+r"(brd_white), "=&r"(temp)
				:"r"(ull(pos)), "r"(chsm), "g"(white)
			);
		#else
			ull mask = ull(1) << pos;
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

	static ull extract(cull brd, cull mask){
		ull result;
		fun_pext(brd,mask,result);
		return result;
	}

	static ull deposit(cull brd, cull mask){
		ull result;
		fun_pdep(brd, mask, result);
		return result;
	}

	void reverse(){
		swap(brd_black, brd_white);
	}

	/** @fn static void mirror_h(ull& brd)
	 *	@brief It's a function used to mirror a 64-bit board horizontally.
	 *	@param brd the 64-bit board
	*/
	static void mirror_h(ull& brd){
		brd = (brd & 0xaaaaaaaaaaaaaaaa) >> 1  | (brd & 0x5555555555555555) << 1;
		brd = (brd & 0xcccccccccccccccc) >> 2  | (brd & 0x3333333333333333) << 2;
		brd = (brd & 0xf0f0f0f0f0f0f0f0) >> 4  | (brd & 0x0f0f0f0f0f0f0f0f) << 4;
	}

	/** @fn static void mirror_v(ull& brd)
	 *	@brief It's a function used to mirror a 64-bit board vertically.
	 *	@param brd the 64-bit board
	*/
	static void mirror_v(ull& brd){
		#ifdef USE_ASM
			asm_bswap(brd);
		#else
			brd = (brd & 0xff00ff00ff00ff00) >> 8  | (brd & 0x00ff00ff00ff00ff) << 8;
			brd = (brd & 0xffff0000ffff0000) >> 16 | (brd & 0x0000ffff0000ffff) << 16;
			brd = (brd & 0xffffffff00000000) >> 32 | (brd & 0x00000000ffffffff) << 32;
		#endif
	}

	/** @fn static void reflect(ull& brd)
	 *	@brief It's a function used to reflect a 64-bit board.
	 *	@param brd the 64-bit board
	*/
	static void reflect(ull& brd){
		mirror_h(brd);
		mirror_v(brd);
	}

	/** @fn static void rotate_r(ull& brd)
	 *	@brief It's a function used to rotate a 64-bit board clockwise.
	 *	@param brd the 64-bit board
	*/
	static void rotate_r(ull& brd){
		brd = (brd & 0xf0f0f0f000000000) >> 4  | (brd & 0x0f0f0f0f00000000) >> 32
			| (brd & 0x00000000f0f0f0f0) << 32 | (brd & 0x000000000f0f0f0f) << 4;
		brd = (brd & 0xcccc0000cccc0000) >> 2  | (brd & 0x3333000033330000) >> 16
			| (brd & 0x0000cccc0000cccc) << 16 | (brd & 0x0000333300003333) << 2;
		brd = (brd & 0xaa00aa00aa00aa00) >> 1  | (brd & 0x5500550055005500) >> 8
			| (brd & 0x00aa00aa00aa00aa) << 8  | (brd & 0x0055005500550055) << 1;
	}

	/** @fn static void rotate_l(ull& brd)
	 *	@brief It's a function used to rotate a 64-bit board counter-clockwise.
	 *	@param brd the 64-bit board
	*/
	static void rotate_l(ull& brd){
		brd = (brd & 0xf0f0f0f000000000) >> 32 | (brd & 0x0f0f0f0f00000000) << 4
			| (brd & 0x00000000f0f0f0f0) >> 4  | (brd & 0x000000000f0f0f0f) << 32;
		brd = (brd & 0xcccc0000cccc0000) >> 16 | (brd & 0x3333000033330000) << 2
			| (brd & 0x0000cccc0000cccc) >> 2  | (brd & 0x0000333300003333) << 16;
		brd = (brd & 0xaa00aa00aa00aa00) >> 8  | (brd & 0x5500550055005500) << 1
			| (brd & 0x00aa00aa00aa00aa) >> 1  | (brd & 0x0055005500550055) << 8;
	}

	/** @fn static void count(ull& brd)
	 *	@brief It's a function used to count the number of bit
	 *	which are set in a 64-bit board.
	 *	@param brd the 64-bit board
	*/
	static pos_type count(cull brd){
		ull result;

		#ifdef USE_ASM
			asm_popcnt(brd,result);
		#else
			result = brd - ((brd >> 1) & 0x5555555555555555);
			result = (result & 0x3333333333333333)
				+ ((result >> 2) & 0x3333333333333333);
			result = (result + (result >> 4)) & 0x0F0F0F0F0F0F0F0F;
			return (result * 0x0101010101010101) >> 56;
		#endif

		return result;
	}

	/** @fn static ull get_edge_stable(cull brd)
	 *	@brief It's a function used to estimate which stones are stable.
	 *	@param brd the 64-bit board
	*/
	static ull get_edge_stable(cull brd){
		ull brd_ul,brd_ur,brd_dl,brd_dr;

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

	/** @fn static ull get_edge_stable(cull brd)
	 *	@brief It's a function used to estimate which stones are stable.
	 *	@param brd the 64-bit board
	*/
	static ull get_stable(cull brd){
		ull brd_l,brd_r,brd_u,brd_d,brd_ul,brd_ur,brd_dl,brd_dr;

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

	/** @fn static ull get_front(cull brd)
	 *	@brief It's a function used to calculate the frontier.
	 *	@param brd the 64-bit board
	*/
	static ull get_front(cull brd){
		ull brd_reverse, brd_front;

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

	/** @fn short count(cbool color)const
	 *	@brief Count the number of black stones or white stones.
	 *	@param color Whether the color is black.
	*/
	short count(cbool color)const{
		return count(get_brd(color));
	}

	/** @fn pos_type sum()const
	 *	@brief Count the number of stones.
	*/
	pos_type sum()const{
		return count(brd_black | brd_white);
	}

	ull get_key(cbool color)const{
		ull result = (brd_black * 0xe2abbb5e6688fdcf) ^ (brd_white * 0x34df417f070da53d);
		fun_rol(result, 22);
		result += color;
		return result;
	}

	static ull get_count();
	static void clear_count();
	static void clear_hash();

	/** @fn ull get_move(cbool color)const
	 *	@brief Calculate possible moves.
	 *	@param color Whether it is black's turn.
	*/
	ull get_move(cbool color)const{
		// This part of code is brought from Zebra.
		// I rewrite it in 64-bit style.

		const ull& brd_blue = get_brd(color);
		const ull& brd_green = get_brd(!color);
		ull brd_green_inner;
		ull moves;

		#ifdef USE_ASM_AVX2

			ull table_brd_blue[4] __attribute__((aligned(32)));
			ull table_brd_green[4] __attribute__((aligned(32)));
			static ull table_shift[4] __attribute__((aligned(32))) = {1, 7, 8, 9};
			ull table_move[4] __attribute__((aligned(32)));

			table_brd_blue[0] = brd_blue;
			table_brd_blue[1] = brd_blue;
			table_brd_blue[2] = brd_blue;
			table_brd_blue[3] = brd_blue;

			brd_green_inner = brd_green & 0x7E7E7E7E7E7E7E7Eu;
			table_brd_green[0] = brd_green_inner;
			table_brd_green[1] = brd_green_inner;
			table_brd_green[2] = brd_green;
			table_brd_green[3] = brd_green_inner;

			asm volatile(
				"vmovapd %1, %%ymm0;"
				"vmovapd %2, %%ymm1;"
				"vmovapd %3, %%ymm2;"
				"vpsllq $1, %%ymm2, %%ymm3;"

				"vpsrlvq %%ymm2, %%ymm0, %%ymm4;"
				"vpand %%ymm1, %%ymm4, %%ymm4;"
				"vpsrlvq %%ymm2, %%ymm4, %%ymm5;"
				"vpand %%ymm1, %%ymm5, %%ymm5;"
				"vpor %%ymm5, %%ymm4, %%ymm4;"
				"vpsrlvq %%ymm2, %%ymm1, %%ymm5;"
				"vpand %%ymm1, %%ymm5, %%ymm5;"
				"vpsrlvq %%ymm3, %%ymm4, %%ymm6;"
				"vpand %%ymm5, %%ymm6, %%ymm6;"
				"vpor %%ymm6, %%ymm4, %%ymm4;"
				"vpsrlvq %%ymm3, %%ymm4, %%ymm6;"
				"vpand %%ymm5, %%ymm6, %%ymm6;"
				"vpor %%ymm6, %%ymm4, %%ymm4;"
				"vpsrlvq %%ymm2, %%ymm4, %%ymm7;"

				"vpsllvq %%ymm2, %%ymm0, %%ymm4;"
				"vpand %%ymm1, %%ymm4, %%ymm4;"
				"vpsllvq %%ymm2, %%ymm4, %%ymm5;"
				"vpand %%ymm1, %%ymm5, %%ymm5;"
				"vpor %%ymm5, %%ymm4, %%ymm4;"
				"vpsllvq %%ymm2, %%ymm1, %%ymm5;"
				"vpand %%ymm1, %%ymm5, %%ymm5;"
				"vpsllvq %%ymm3, %%ymm4, %%ymm6;"
				"vpand %%ymm5, %%ymm6, %%ymm6;"
				"vpor %%ymm6, %%ymm4, %%ymm4;"
				"vpsllvq %%ymm3, %%ymm4, %%ymm6;"
				"vpand %%ymm5, %%ymm6, %%ymm6;"
				"vpor %%ymm6, %%ymm4, %%ymm4;"
				"vpsllvq %%ymm2, %%ymm4, %%ymm4;"
				"vpor %%ymm4, %%ymm7, %%ymm7;"

				"vmovapd %%ymm7, %0;"
				:"=m"(table_move)
				:"m"(table_brd_blue), "m"(table_brd_green), "m"(table_shift)
				:"ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7"
			);

			moves = table_move[0] | table_move[1] | table_move[2] | table_move[3];
			moves &= ~(brd_blue | brd_green);

		#else

			ull brd_flip;
			ull brd_green_adj;

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

		#endif //USE_ASM_AVX2
		
		return moves;
	}

	/** @fn short count_move(cbool color)const
	 *	@brief Count possible moves.
	 *	@param color Whether it is black's turn.
	*/
	short count_move(cbool color)const{
		return count(get_move(color));
	}

	static void config(){
		config_flip();
		config_search();
	}
	static void postprocess();

	pair<method, short> process_method(cmethod mthd, cshort depth){
		pair<method, short> result = {mthd, depth};
		short total = this->sum();

		if(result.first == mthd_rnd){
			return result;
		}

		if(result.second == -1){
			if(total <= 7){
				result.second = 10;
			}else if(total <= 10){
				result.second = 9;
			}else if(total <= size2 - 22){
				result.second = 8;
			}else if(total <= size2 - 15){
				result.second = 9;
			}else{
				result.second = 20;
			}
		}else if(result.second == -2){
			if(total <= 7){
				result.second = 10;
			}else if(total <= 10){
				result.second = 10;
			}else if(total <= size2 - 24){
				result.second = 9;
			}else if(total <= size2 - 16){
				result.second = 10;
			}else{
				result.second = 20;
			}
		}else if(result.second <= -3){
			if(total <= 7){
				result.second = 11;
			}else if(total <= 10){
				result.second = 11;
			}else if(total <= size2 - 22){
				result.second = 10;
			}else if(total <= size2 - 16){
				result.second = 11;
			}else{
				result.second = 20;
			}
		}
		if(result.second >= size2 - total){
				result.first = method(mthd | mthd_end);
			result.second = size2 - total;	
		}
		return result;
	}

	void flip(cbool color, cpos_type pos);

	val_type score_end(cbool color)const{
		val_type num_diff = count(color) - count(!color);

		if(num_diff > 0){
			return num_diff + 2;
		}else if(num_diff < 0){
			return num_diff - 2;
		}else{
			return 0;
		}
	}

	val_type score(cbool color)const{
		ull brd_blue = get_brd(color);
		ull brd_green = get_brd(!color);
		ull brd_mix = brd_blue | brd_green;
		ull brd_temp;
		const val_type table_param[3][4] = {
			{12,0.5,-6,-0.2},
			{10,0.5,-5,0.2},
			{3,1,0,0}
		};

		short stage;
		short total = count(brd_mix);
		if(total <= 40){
			stage = 0;
		}else if(total <= size2 - 7){
			stage = 1;
		}else{
			stage = 2;
		}

		val_type result = 0;
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
	val_type search(
		cbool color, cshort depth,
		val_type alpha = _inf, val_type beta = inf, cbool flag_pass = false
	)const;
	val_type search(
		cmethod mthd, cbool color, cshort depth,
		cval_type alpha = _inf, cval_type beta = inf
	)const;
	val_type search_end_two(
		cbool color, cpos_type pos1, cpos_type pos2,
		val_type alpha, val_type beta, cbool flag_pass
	)const;
	val_type search_end_three(
		cbool color, cpos_type pos1, cpos_type pos2, cpos_type pos3,
		val_type alpha, val_type beta, cbool flag_pass
	)const;
	val_type search_end_four(
		cbool color, cpos_type pos1, cpos_type pos2, cpos_type pos3, cpos_type pos4,
		val_type alpha, val_type beta, cbool flag_pass
	)const;
	template<method mthd>
	val_type search_end_five(
		cbool color, val_type alpha, val_type beta, cbool flag_pass
	)const;

	vector<choice> get_choice(
		cmethod mthd, cbool color, cshort depth,
		val_type alpha = _inf, val_type beta = inf, val_type gamma = inf
	)const;
	static choice select_choice(vector<choice> choices, const float& variation = 0.75);
	coordinate play(cmethod mthd, cbool color, cshort depth = -1);

	sts_type get_status(cbool color){
		const sts_type table_status[32] = {
			sts_wturn, sts_wturn, sts_wturn, sts_wturn,
			sts_bagain, sts_bagain, sts_bagain, sts_bagain,
			sts_wturn, sts_wturn, sts_wturn, sts_wturn,
			sts_tie, sts_tie, sts_wwin, sts_bwin,
			sts_bturn, sts_bturn, sts_bturn, sts_bturn,
			sts_bturn, sts_bturn, sts_bturn, sts_bturn,
			sts_wagain, sts_wagain, sts_wagain, sts_wagain,
			sts_tie, sts_tie, sts_wwin, sts_bwin
		};

		int num_diff = count(true) - count(false);
		short index =
			(color << 4)
			| ((count_move(true) == 0) << 3)
			| ((count_move(false) == 0) << 2)
			| ((num_diff != 0) << 1)
			| ((num_diff > 0) << 0)
		;

		return table_status[index];
	}

	val_type score_ptn(cbool color, const pattern& ptn)const;
	void adjust_ptn(cbool color, pattern& ptn, cval_type value)const;

	#ifdef DEBUG_SEARCH
		static void enable_log();
		static void disable_log();
		static void clear_log();
		static void save_log(const string& filename);
	#endif //DEBUG_SEARCH

	vector<int> get_pv(bool color)const;

protected:

	ull brd_white, brd_black;

	static void config_flip();
	static void config_search();
};

namespace std{
	template <>
	struct hash<board>: public unary_function<board, size_t>{
		size_t operator()(cboard brd)const{
			return hash<bitset<board::size2 * 2>>()(*(const bitset<board::size2 * 2>*)&brd);
		}
	};
}

struct choice{
	board brd;
	float val;
	float rnd_val;
	pos_type pos;
};

#endif // REVERSI_H
