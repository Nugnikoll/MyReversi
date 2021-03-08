/** @mainpage Reversi Documentation
 *
 * @section sec_rv Introduction of Reversi
 * Reversi (also called Othello) is a kind of strategy board game
 * which involves play by two parties on an eight-by-eight square grid.
 *
 * @section sec_pro Introduction of the Program
 *
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
 * @brief It's the board of reversi.
 *
 * It has two data member which are brd_black and brd_white.
 * They represent the board of black stones and board of white stones respectively.
 * Both brd_black and brd_white are 64-bit integer.
 * Where the bit is set to 1, there is a stone.
 * We can assume that brd_black | brd_white is always zero
 * because we can never place a black stone and a white stone at the same cell.
 * This kind of data structure saves a lot of memory and is easy to calculate.
 *
 */

class board{
protected:
	ull brd_white, brd_black;
	static val_type table_param[65][8];

public:

	/** @fn board()
	 * @brief The default constructor of class board
	 * This function do nothing at all.
	 * The value of the object board is not defined
	 * if it's constructed by this function.
	 * To initial the object, please use the function initial() .
	 */
	board() = default;

	/** @fn board(const board& brd)
	 * @brief The default copy constructor of class board
	 * @param brd
	 */
	board(const board& brd) = default;

	/** @fn board(cull _brd_black, cull _brd_white)
	 * @brief A constructor of class board
	 * @param _brd_black the value of the 64-bit board of black stones
	 * @param _brd_white the value of the 64-bit board of white stones
	 */
	board(cull _brd_black, cull _brd_white)
		: brd_white(_brd_white), brd_black(_brd_black){}

	/** @fn board(ARRAY_1D_IN_I(unsigned long long))
	 * @brief A constructor of class board
	 * @param ptri a 1D NumPy array of shape = (2) and dtype = uint64
	 * This function is expected to be invoked in python.
	 */
	board(ARRAY_1D_IN_I(unsigned long long)){
		assert(i1 == 2);
		brd_white = ptri[0];
		brd_black = ptri[1];
	}

	/** @fn void board(ARRAY_2D_IN_I(bool))
	 * @brief A constructor of class board
	 * @param ptri a 2D NumPy array of shape = (2, 64) and dtype = bool
	 * This function is expected to be invoked in python.
	 * The input array is the one-hot representation of the board.
	 */
	board(ARRAY_2D_IN_I(bool)): brd_white(0), brd_black(0){
		assert(i1 == 2);
		assert(i2 == size2);
		for(ull i = 0; i != size2; ++i){
			brd_white |= ull(ptri[i]) << i;
		}
		for(ull i = 0; i != size2; ++i){
			brd_black |= ull(ptri[i + size2]) << i;
		}
	}

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

	/** @fn string to_string()const
	 * @brief convert the board to string
	 */
	string to_string()const;

	/** @fn void const board& print(ostream& out = cout)const
	 * @brief It's a function used to show the board on a terminal.
	 * @param out the output stream
	 */
	void print(ostream& out = cout)const{
		out << to_string();
	}

	/** @fn void view(ARRAY_1D_OUT_O(ULL))
	 * @brief get a view of the board
	 * @return a 1D NumPy array of shape = (2) and dtype = uint64
	 * This function does not create a NumPy array.
	 * The return value shares memory with the board.
	 * To create an array from board, use the function board::numpy() instead.
	 */
	void view(ARRAY_1D_OUT_O(ULL)){
		*o1 = 2;
		*ptro = &brd_white;
	}

	/** @fn void numpy(ARRAY_1D_OUT_M(ULL))
	 * @brief convert the board to a NumPy array
	 * @return a 1D NumPy array of shape = (2) and dtype = uint64
	 * This function create a NumPy array.
	 * The return value does not share memory with the board.
	 */
	void numpy(ARRAY_1D_OUT_M(ULL))const{
		*m1 = 2;
		*ptrm = new ull[2];
		(*ptrm)[0] = brd_white;
		(*ptrm)[1] = brd_black;
	}

	/** @fn void expand(ARRAY_2D_OUT_M(bool))
	 * @brief convert the board to a NumPy array
	 * @return a 2D NumPy array of shape = (2, 64) and dtype = bool
	 * This function convert the board to NumPy array using one-hot representation.
	 */
	void expand(ARRAY_2D_OUT_M(bool))const{
		*m1 = 2;
		*m2 = size2;
		*ptrm = new bool[2 * size2];
		for(ull i = 0; i != size2; ++i){
			(*ptrm)[i] = bool(brd_white & (1ull << i));
		}
		for(ull i = 0; i != size2; ++i){
			(*ptrm)[i + size2] = bool(brd_black & (1ull << i));
		}
	}

	/** @fn void board& assign(cull _brd_black,cull _brd_white)
	 * @brief assign the board to some specific value
	 * @param _brd_black the value of the 64-bit board of black stones
	 * @param _brd_black the value of the 64-bit board of white stones
	 */
	void assign(cull _brd_black, cull _brd_white){
		brd_black = _brd_black;
		brd_white = _brd_white;
	}

	/** @fn board& initial()
	 * @brief initialize the board
	 */
	void initial(){
		return this->assign(0x0000000810000000,0x0000001008000000);
	}

	/** @fn cull get_brd(cbool color)const
	 * @brief get the member of the board
	 * @param color whether the color is black
	 */
	cull get_brd(cbool color)const{
		return *(&brd_white + color);
	}
	ull& get_brd(cbool color){
		return *(&brd_white + color);
	}

	/** @fn chessman get(cpos_type pos)const
	 * @brief get a chessman on the board
	 * @param pos the position of the chessman
	 */
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

	/** @fn set(cpos_type pos, cchessman chsm)
	 * @brief set a position on board to a specified chessman
	 * @param pos the position of the chessman
	 * @param chsm a chessman
	 */
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
		fun_pext(brd, mask, result);
		return result;
	}

	static ull deposit(cull brd, cull mask){
		ull result;
		fun_pdep(brd, mask, result);
		return result;
	}

	/** @fn void reverse()
	 * @brief reverse the color of the board
	 */
	void reverse(){
		swap(brd_black, brd_white);
	}

	/** @fn static void mirror_h(ull& brd)
	 * @brief mirror a 64-bit board horizontally
	 * @param brd the 64-bit board
	 */
	static void mirror_h(ull& brd){
		brd = (brd & 0xaaaaaaaaaaaaaaaa) >> 1  | (brd & 0x5555555555555555) << 1;
		brd = (brd & 0xcccccccccccccccc) >> 2  | (brd & 0x3333333333333333) << 2;
		brd = (brd & 0xf0f0f0f0f0f0f0f0) >> 4  | (brd & 0x0f0f0f0f0f0f0f0f) << 4;
	}

	/** @fn static void mirror_v(ull& brd)
	 * @brief mirror a 64-bit board vertically
	 * @param brd the 64-bit board
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
	 * @brief reflect a 64-bit board
	 * @param brd the 64-bit board
	 */
	static void reflect(ull& brd){
		mirror_h(brd);
		mirror_v(brd);
	}

	/** @fn static void rotate_r(ull& brd)
	 * @brief rotate a 64-bit board clockwise
	 * @param brd the 64-bit board
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
	 * @brief rotate a 64-bit board counterclockwise
	 * @param brd the 64-bit board
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
	 * @brief count the number of bit which are set in a 64-bit board
	 * @param brd the 64-bit board
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

	/** @fn static ull get_front(cull brd)
	 * @brief calculate the frontier
	 * @param brd the 64-bit board
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

	/** @fn void mirror_h()
	 * @brief mirror the board horizontally
	 */
	void mirror_h(){
		mirror_h(brd_black);
		mirror_h(brd_white);
	}

	/** @fn void mirror_v()
	 * @brief mirror the board vertically
	 */
	void mirror_v(){
		mirror_v(brd_black);
		mirror_v(brd_white);
	}

	/** @fn void rotate_l()
	 * @brief rotate the board counterclockwise
	 */
	void rotate_l(){
		rotate_l(brd_black);
		rotate_l(brd_white);
	}

	/** @fn void rotate_r()
	 * @brief rotate the board clockwise
	 */
	void rotate_r(){
		rotate_r(brd_black);
		rotate_r(brd_white);
	}

	/** @fn void reflect()
	 * @brief reflect the board
	 */
	void reflect(){
		reflect(brd_black);
		reflect(brd_white);
	}

	/** @fn short count(cbool color)const
	 * @brief count the number of black stones or white stones
	 * @param color whether the color is black
	 */
	short count(cbool color)const{
		return count(get_brd(color));
	}

	/** @fn pos_type sum()const
	 * @brief count the number of stones
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
	 * @brief calculate possible moves
	 * @param color whether it is black's turn
	 * This part of code is brought from Zebra.
	 * I rewrite it in 64-bit style.
	 * If macro USE_ASM_AVX2 is defined, AVX2 instructions will be used.
	 */
	ull get_move(cbool color)const{

		const ull& brd_blue = get_brd(color);
		const ull& brd_green = get_brd(!color);
		ull brd_green_inner;
		ull moves;

		#ifdef USE_ASM_AVX2

			ull table_brd_green[4] __attribute__((aligned(32)));
			static const ull table_shift[4] __attribute__((aligned(32))) = {1, 7, 8, 9};
			ull table_move[4] __attribute__((aligned(32)));

			brd_green_inner = brd_green & 0x7E7E7E7E7E7E7E7Eu;
			table_brd_green[0] = brd_green_inner;
			table_brd_green[1] = brd_green_inner;
			table_brd_green[2] = brd_green;
			table_brd_green[3] = brd_green_inner;

			asm volatile(
				"vpbroadcastq %1, %%ymm0;"
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
				:"m"(brd_blue), "m"(table_brd_green), "m"(table_shift)
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

	/** @fn pull get_move_flip(cbool color)const
	 * @brief calculate possible moves and flippable stones
	 * @param color whether it is black's turn
	 */
	pull get_move_flip(cbool color)const{
		const ull& brd_blue = get_brd(color);
		const ull& brd_green = get_brd(!color);
		ull brd_blank = ~(brd_blue | brd_green);
		ull brd_green_inner;
		ull moves;
		ull flips;

		#ifdef USE_ASM_AVX2

			ull table_brd_green[4] __attribute__((aligned(32)));
			static ull table_shift[4] __attribute__((aligned(32))) = {1, 7, 8, 9};
			ull table_move[4] __attribute__((aligned(32)));
			ull table_flip[4] __attribute__((aligned(32)));

			brd_green_inner = brd_green & 0x7E7E7E7E7E7E7E7Eu;
			table_brd_green[0] = brd_green_inner;
			table_brd_green[1] = brd_green_inner;
			table_brd_green[2] = brd_green;
			table_brd_green[3] = brd_green_inner;

			asm volatile(
				"vpbroadcastq %2, %%ymm0;"
				"vmovapd %3, %%ymm1;"
				"vmovapd %4, %%ymm2;"
				"vpsllq $1, %%ymm2, %%ymm3;"
				"vpbroadcastq %5, %%ymm8;"

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
				"vpsrlvq %%ymm2, %%ymm4, %%ymm4;"
				"vpand %%ymm8, %%ymm4, %%ymm4;"
				"vmovdqa %%ymm4, %%ymm7;"

				"vpsllvq %%ymm2, %%ymm4, %%ymm4;"
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
				"vpor %%ymm6, %%ymm4, %%ymm9;"

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
				"vpand %%ymm8, %%ymm4, %%ymm4;"
				"vpor %%ymm4, %%ymm7, %%ymm7;"

				"vpsrlvq %%ymm2, %%ymm4, %%ymm4;"
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
				"vpor %%ymm4, %%ymm9, %%ymm9;"

				"vmovapd %%ymm7, %0;"
				"vmovapd %%ymm9, %1;"
				:"=m"(table_move), "=m"(table_flip)
				:"m"(brd_blue), "m"(table_brd_green), "m"(table_shift) ,"m"(brd_blank)
				:"ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9"
			);

			moves = table_move[0] | table_move[1] | table_move[2] | table_move[3];
			flips = table_flip[0] | table_flip[1] | table_flip[2] | table_flip[3];

		#else

			ull brd_flip;
			ull brd_green_adj;
			ull move_part;

			brd_green_inner = brd_green & 0x7E7E7E7E7E7E7E7Eu;

			brd_flip = (brd_blue >> 1) & brd_green_inner;
			brd_flip |= (brd_flip >> 1) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner >> 1);
			brd_flip |= (brd_flip >> 2) & brd_green_adj;
			brd_flip |= (brd_flip >> 2) & brd_green_adj;

			move_part = brd_flip >> 1;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part << 1) & brd_green_inner;
			brd_flip |= (brd_flip << 1) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner << 1);
			brd_flip |= (brd_flip << 2) & brd_green_adj;
			brd_flip |= (brd_flip << 2) & brd_green_adj;

			flips = brd_flip;

			brd_flip = (brd_blue << 1) & brd_green_inner;
			brd_flip |= (brd_flip << 1) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner << 1);
			brd_flip |= (brd_flip << 2) & brd_green_adj;
			brd_flip |= (brd_flip << 2) & brd_green_adj;

			move_part = brd_flip << 1;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part >> 1) & brd_green_inner;
			brd_flip |= (brd_flip >> 1) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner >> 1);
			brd_flip |= (brd_flip >> 2) & brd_green_adj;
			brd_flip |= (brd_flip >> 2) & brd_green_adj;

			flips |= brd_flip;

			brd_flip = (brd_blue >> 8) & brd_green;
			brd_flip |= (brd_flip >> 8) & brd_green;

			brd_green_adj = brd_green & (brd_green >> 8);
			brd_flip |= (brd_flip >> 16) & brd_green_adj;
			brd_flip |= (brd_flip >> 16) & brd_green_adj;

			move_part = brd_flip >> 8;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part << 8) & brd_green;
			brd_flip |= (brd_flip << 8) & brd_green;

			brd_green_adj = brd_green & (brd_green << 8);
			brd_flip |= (brd_flip << 16) & brd_green_adj;
			brd_flip |= (brd_flip << 16) & brd_green_adj;

			flips |= brd_flip;

			brd_flip = (brd_blue << 8) & brd_green;
			brd_flip |= (brd_flip << 8) & brd_green;

			brd_green_adj = brd_green & (brd_green << 8);
			brd_flip |= (brd_flip << 16) & brd_green_adj;
			brd_flip |= (brd_flip << 16) & brd_green_adj;

			move_part = brd_flip << 8;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part >> 8) & brd_green;
			brd_flip |= (brd_flip >> 8) & brd_green;

			brd_green_adj = brd_green & (brd_green >> 16);
			brd_flip |= (brd_flip >> 16) & brd_green_adj;
			brd_flip |= (brd_flip >> 16) & brd_green_adj;

			flips |= brd_flip;

			brd_flip = (brd_blue >> 7) & brd_green_inner;
			brd_flip |= (brd_flip >> 7) & brd_green_inner;
			
			brd_green_adj = brd_green_inner & (brd_green_inner >> 7);
			brd_flip |= (brd_flip >> 14) & brd_green_adj;
			brd_flip |= (brd_flip >> 14) & brd_green_adj;
			
			move_part = brd_flip >> 7;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part << 7) & brd_green_inner;
			brd_flip |= (brd_flip << 7) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner << 7);
			brd_flip |= (brd_flip << 14) & brd_green_adj;
			brd_flip |= (brd_flip << 14) & brd_green_adj;

			flips |= brd_flip;

			brd_flip = (brd_blue << 7) & brd_green_inner;
			brd_flip |= (brd_flip << 7) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner << 7);
			brd_flip |= (brd_flip << 14) & brd_green_adj;
			brd_flip |= (brd_flip << 14) & brd_green_adj;

			move_part = brd_flip << 7;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part >> 7) & brd_green_inner;
			brd_flip |= (brd_flip >> 7) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner >> 7);
			brd_flip |= (brd_flip >> 14) & brd_green_adj;
			brd_flip |= (brd_flip >> 14) & brd_green_adj;

			flips |= brd_flip;

			brd_flip = (brd_blue >> 9) & brd_green_inner;
			brd_flip |= (brd_flip >> 9) & brd_green_inner;
			
			brd_green_adj = brd_green_inner & (brd_green_inner >> 9);
			brd_flip |= (brd_flip >> 18) & brd_green_adj;
			brd_flip |= (brd_flip >> 18) & brd_green_adj;
			
			move_part = brd_flip >> 9;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part << 9) & brd_green_inner;
			brd_flip |= (brd_flip << 9) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner << 9);
			brd_flip |= (brd_flip << 18) & brd_green_adj;
			brd_flip |= (brd_flip << 18) & brd_green_adj;

			flips |= brd_flip;
			
			brd_flip = (brd_blue << 9) & brd_green_inner;
			brd_flip |= (brd_flip << 9) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner << 9);
			brd_flip |= (brd_flip << 18) & brd_green_adj;
			brd_flip |= (brd_flip << 18) & brd_green_adj;

			move_part = brd_flip << 9;
			move_part &= brd_blank;
			moves = move_part;

			brd_flip = (move_part >> 9) & brd_green_inner;
			brd_flip |= (brd_flip >> 9) & brd_green_inner;

			brd_green_adj = brd_green_inner & (brd_green_inner >> 9);
			brd_flip |= (brd_flip >> 18) & brd_green_adj;
			brd_flip |= (brd_flip >> 18) & brd_green_adj;

			flips |= brd_flip;

		#endif //USE_ASM_AVX2
		
		return pull{moves, flips};
	}

	/** @fn short count_move(cbool color)const
	 * @brief count possible moves
	 * @param color whether it is black's turn
	 */
	short count_move(cbool color)const{
		return count(get_move(color));
	}

	/** @fn static void config(const string& file_param = "")
	 * @brief do some configurations
	 * @param file_param the path to find the file of parameters (optional)
	 * This function conducts some preprocessions.
	 * If this function is not invoked, the behavior of board::flip() and board::search() are undefined.
	 */
	static void config(const string& file_param = "");
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

	/** @fn void flip(cbool color, cpos_type pos)
	 * @brief make a move and flip some stones
	 * @param color whether it's black's turn
	 * @param pos the position to put the stone
	 */
	void flip(cbool color, cpos_type pos);

	/** @fn val_type score_end(cbool color)const
	 * @brief evaluate the board at the end of the game
	 * @param color whether it's black's turn
	 */
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

	/** @fn val_type score(cbool color)const
	 * @brief evaluate the board during the game
	 * @param color whether it's black's turn
	 */
	val_type score(cbool color)const{
		ull brd_blue = get_brd(color);
		ull brd_green = get_brd(!color);
		ull brd_mix = brd_blue | brd_green;
		pull brd_blue_move_flip;
		pull brd_green_move_flip;
		ull brd_front;

		short total = count(brd_mix);
		auto table_ptr = table_param[total];

		val_type result = 0;

		result += (count(brd_blue & 0x8100000000000081) - count(brd_green & 0x8100000000000081))
			* table_ptr[0];
		result += (count(brd_blue & 0x7e8181818181817e) - count(brd_green & 0x7e8181818181817e))
			* table_ptr[1];
		result += (count(brd_blue & 0x0042000000004200) - count(brd_green & 0x0042000000004200))
			* table_ptr[2];
		result += (count(brd_blue & 0x003c7e7e7e7e3c00) - count(brd_green & 0x003c7e7e7e7e3c00))
			* table_ptr[3];

		brd_blue_move_flip = get_move_flip(color);
		brd_green_move_flip = get_move_flip(!color);
		result += table_ptr[4] * (count(brd_blue_move_flip.first) - count(brd_green_move_flip.first));
		result += table_ptr[5] * (
			count(brd_blue & ~brd_green_move_flip.second)
			- count(brd_green & ~brd_blue_move_flip.second)
		);
		brd_front = get_front(brd_mix);
		result += table_ptr[6] * (count(brd_blue & brd_front) - count(brd_green & brd_front));

		return result;
	}

	template<method mthd>
	val_type search(
		cbool color, cshort depth,
		val_type alpha = _inf, val_type beta = inf, cbool flag_pass = false
	)const;

	/** @fn val_type search(mthd, color, depth, alpha, beta)
	 * @brief perform a search to evaluate the board
	 * @param mthd the method
	 * @param color whether it's black's turn
	 * @param depth the depth of the search tree
	 * @param alpha the lower bound (default: _inf)
	 * @param beta the upper bound (default: inf)
	 * 
	 * parameter mthd should be mthd_rnd or
	 * a combination of mthd_ab, mthd_kill, mthd_pvs, mthd_trans, mthd_mtdf,
	 * mthd_ids, mthd_ptn, mthd_mpc, mthd_end
	 * Not all kinds of combinations are available.
	 * mthd_ab should be added if it's not mthd_rnd.
	 * mthd_ids and mthd_mpc are still experimental.
	 * mthd_end can only be added if depth is equal to the number of empty cells on the board.
	 * Thus, the most commonly used mthd should be mthd_ab | mthd_kill | mthd_pvs | mthd_trans | mthd_mtdf | mthd_ptn .
	 * 
	 * Assume that the result of the search tree should be v.
	 * If v is in the interval [alpha, beta], then this function will return v.
	 * If v is below alpha, this function can return any value in the interval [v, alpha]. 
	 * If v is above beta, this function can return any value in the interval [beta, v]. 
	 */
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

	/** @fn val_type get_choice(mthd, color, depth, alpha, beta, gamma)
	 * @brief perform a search to evaluate the board and return possible choices and corresponding evaluations.
	 * @param mthd the method
	 * @param color whether it's black's turn
	 * @param depth the depth of the search tree
	 * @param alpha the lower bound (default: _inf)
	 * @param beta the upper bound (default: inf)
	 * @param gamma estimation of the result (default: inf)
	 * 
	 * parameter mthd should be mthd_rnd or
	 * a combination of mthd_ab, mthd_kill, mthd_pvs, mthd_trans, mthd_mtdf,
	 * mthd_ids, mthd_ptn, mthd_mpc, mthd_end
	 * Not all kinds of combinations are available.
	 * mthd_ab should be added if it's not mthd_rnd.
	 * mthd_ids and mthd_mpc are still experimetal.
	 * mthd_end can only be added if depth is equal to the number of empty cells on the board.
	 * Thus, the most commonly used mthd should be mthd_ab | mthd_kill | mthd_pvs | mthd_trans | mthd_mtdf | mthd_ptn .
	 * 
	 * Assume the return value is choices of type vector<choice>
	 * The position of each choice is stored in choices[i].pos .
	 * The evaluation of each choice is stored in choices[i].val .
	 * The maximum value of choices[i].val is correct,
	 * but it's not garranteed that the other value is correct.
	 * If the difference between a value of the subtree and the maximum value is large than a specific threshold,
	 * the value may not be returned correctly, as it's no point to know how bad a choice is. 
	 */
	vector<choice> get_choice(
		cmethod mthd, cbool color, cshort depth,
		val_type alpha = _inf, val_type beta = inf, val_type gamma = inf
	)const;

	/** @fn static choice select_choice(vector<choice> choices, const float& variation = 0.75)
	 * @brief select a choice
	 * @param choices vector of choices
	 * @param variation a float value greater than zero
	 * 
	 * The large the variation, the greater the randomness,
	 * and the more likely to select an inferior choice.
	 */
	static choice select_choice(vector<choice> choices, const float& variation = 0.75);

	/** @fn val_type coordinate play(cmethod mthd, cbool color, cshort depth = -1)
	 * @brief perform a search and make a move.
	 * @param mthd the method
	 * @param color whether it's black's turn
	 * @param depth the depth of the search tree
	 * 
	 * parameter mthd should be mthd_rnd or
	 * a combination of mthd_ab, mthd_kill, mthd_pvs, mthd_trans, mthd_mtdf,
	 * mthd_ids, mthd_ptn, mthd_mpc, mthd_end
	 * Not all kinds of combinations are available.
	 * mthd_ab should be added if it's not mthd_rnd.
	 * mthd_ids and mthd_mpc are still experimetal.
	 * mthd_end can only be added if depth is equal to the number of empty cells on the board.
	 * Thus, the most commonly used mthd should be mthd_ab | mthd_kill | mthd_pvs | mthd_trans | mthd_mtdf | mthd_ptn .
	 */
	coordinate play(cmethod mthd, cbool color, cshort depth = -1);

	/** @fn sts_type get_status(cbool color)
	 * @brief get the status of the board
	 * @param color whether it's black's turn
	 * 
	 * This function can be used to determine whether the game is end,
	 * whether the next player should pass, and who is the winner.
	 */
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

	/** @fn vector<int> get_pv(bool color)const
	 * @brief get the principle variation
	 * @param color whether it's black's turn
	 * @return a series of positions stored in a vector
	 * 
	 * This function read data from hash and return a series of positions.
	 * A search should be performed before invoking this function.
	 * It's not garanteed that the result is correct.
	 * The result can be corrupted by hash collision.
	 */
	vector<int> get_pv(bool color)const;

protected:

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
