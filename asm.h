#ifndef ASM_H
#define ASM_H

#define asm_bswap(brd) \
	asm volatile( \
		"bswap %0;" \
		: "+r"(brd) \
	)

#define asm_popcnt(brd,result) \
	asm volatile( \
		"popcnt %1, %0;" \
		: "=r"(result) \
		: "r"(brd) \
	)

#define asm_tzcnt(brd,result) \
	asm volatile( \
		"tzcnt %1, %0;" \
		:"=r"(result) \
		:"r"(brd) \
	)

#define asm_pext(brd, mask, result) \
	asm volatile( \
		"pext %1, %2, %0;" \
		: "=r"(result) \
		: "r"(mask), "r"(brd) \
	)

#define asm_pdep(brd, mask, result) \
	asm volatile( \
		"pdep %1, %2, %0;" \
		: "=r"(result) \
		: "r"(mask), "r"(brd) \
	)

#endif //ASM_H