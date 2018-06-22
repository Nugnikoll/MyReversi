/** @file asm.h
 * @brief This file includes some macros for inline assembly.
 */

#ifndef ASM_H
#define ASM_H

#include "type.h"

#define asm_bt(brd, pos, result) \
	asm volatile( \
		"bt %2,%1;" \
		"setc %0;" \
		:"=&r"(result) \
		:"r"(brd), "r"(pos) \
	)

#define equ_bt(brd, pos, result) \
	(result = bool(brd & (1 << pos)))

#ifdef USE_ASM
	#define fun_bt(brd, mask, result) asm_bt(brd, mask, result)
#else
	#define fun_bt(brd, mask, result) equ_bt(brd, mask, result)
#endif //USE_ASM

#define asm_bts(brd, pos) \
	asm volatile( \
		"bts %1,%0;" \
		:"+r"(brd) \
		:"r"(pos) \
	)

#define equ_bts(brd, pos) \
	(brd |= 1 << pos)

#ifdef USE_ASM
	#define fun_bts(brd, pos) asm_bts(brd, pos)
#else
	#define fun_bts(brd, pos) equ_bts(brd, pos)
#endif //USE_ASM

#define asm_btr(brd, pos) \
	asm volatile( \
		"btr %1,%0;" \
		:"+r"(brd) \
		:"r"(pos) \
	)

#define equ_btr(brd, pos) \
	(brd &= ~(1 << pos))

#ifdef USE_ASM
	#define fun_btr(brd, pos) asm_btr(brd, pos)
#else
	#define fun_btr(brd, pos) equ_btr(brd, pos)
#endif //USE_ASM

#define asm_rol(brd, val) \
	asm volatile( \
		"rol %1, %0;" \
		: "+r"(brd) \
		: "c"(char(val)) \
	)

inline void equ_rol(unsigned char& brd, char val){
	brd = (brd << val) | (brd >> (8 - val));
}
inline void equ_rol(ull& brd, char val){
	brd = (brd << val) | (brd >> (64 - val));
}

#ifdef USE_ASM
	#define fun_rol(brd, val) asm_rol(brd, val)
#else
	#define fun_rol(brd, val) equ_rol(brd, val)
#endif //USE_ASM

#define asm_ror(brd, val) \
	asm volatile( \
		"ror %1, %0;" \
		: "+r"(brd) \
		: "c"(char(val)) \
	)

inline void equ_ror(unsigned char& brd, char val){
	brd = (brd >> val) | (brd << (8 - val));
}
inline void equ_ror(ull& brd, char val){
	brd = (brd >> val) | (brd << (64 - val));
}

#ifdef USE_ASM
	#define fun_ror(brd, val) asm_ror(brd, val)
#else
	#define fun_ror(brd, val) equ_ror(brd, val)
#endif //USE_ASM

#define asm_bswap(brd) \
	asm volatile( \
		"bswap %0;" \
		: "+r"(brd) \
	)

inline void equ_bswap(ull& brd){
	brd = (brd & 0xff00ff00ff00ff00) >> 8  | (brd & 0x00ff00ff00ff00ff) << 8;
	brd = (brd & 0xffff0000ffff0000) >> 16 | (brd & 0x0000ffff0000ffff) << 16;
	brd = (brd & 0xffffffff00000000) >> 32 | (brd & 0x00000000ffffffff) << 32;
}

#ifdef USE_ASM
	#define fun_bswap(brd) asm_bswap(brd);
#else
	#define fun_bswap(brd) equ_bswap(brd);
#endif //USE_ASM

#define asm_popcnt(brd, result) \
	asm volatile( \
		"popcnt %1, %0;" \
		: "=r"(result) \
		: "r"(brd) \
	)

inline void equ_popcnt(cull brd, ull& result){
	result = brd - ((brd >> 1) & 0x5555555555555555);
	result = (result & 0x3333333333333333) + ((result >> 2) & 0x3333333333333333);
	result = (result + (result >> 4)) & 0x0F0F0F0F0F0F0F0F;
	result = (result * 0x0101010101010101) >> 56;
}

#ifdef USE_ASM
	#define fun_popcnt(brd, result) asm_popcnt(brd, result)
#else
	#define fun_popcnt(brd, result) equ_popcnt(brd, result)
#endif //USE_ASM

#define asm_tzcnt(brd, result) \
	asm volatile( \
		"tzcnt %1, %0;" \
		:"=r"(result) \
		:"r"(brd) \
	)

inline void equ_tzcnt(cull brd, ull& result){
	fun_popcnt(~brd & (brd - 1), result);
}

#ifdef USE_ASM
	#define fun_tzcnt(brd, result) asm_tzcnt(brd, result)
#else
	#define fun_tzcnt(brd, result) equ_tzcnt(brd, result)
#endif //USE_ASM

#define asm_pext(brd, mask, result) \
	asm volatile( \
		"pext %1, %2, %0;" \
		: "=r"(result) \
		: "r"(mask), "r"(brd) \
	)

inline void equ_pext(cull brd, cull mask, ull& result){
	ull msk;
	result = 0;
	msk = mask;
	for(ull i = 1; msk; i <<= 1){
		if(brd & msk & -msk){
			result |= i;
		}
		msk &= msk - 1;
	}
}

#ifdef USE_ASM_BMI2
	#define fun_pext(brd, mask, result) asm_pext(brd, mask, result)
#else
	#define fun_pext(brd, mask, result) equ_pext(brd, mask, result)
#endif //USE_ASM_BMI2

#define asm_pdep(brd, mask, result) \
	asm volatile( \
		"pdep %1, %2, %0;" \
		: "=r"(result) \
		: "r"(mask), "r"(brd) \
	)

inline void equ_pdep(cull brd, cull mask, ull& result){
	ull msk;
	result = 0;
	msk = mask;
	for(ull i = 1; msk; i <<= 1){
		if(brd & i){
			result |= msk & -msk;
		}
		msk &= msk - 1;
	}
}

#ifdef USE_ASM_BMI2
	#define fun_pdep(brd, mask, result) asm_pdep(brd, mask, result)
#else
	#define fun_pdep(brd, mask, result) equ_pdep(brd, mask, result)
#endif //USE_ASM_BMI2

#endif //ASM_H
