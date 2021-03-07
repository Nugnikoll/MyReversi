#include "board.h"

#ifdef USE_ASM_BMI2

const pos_type pos_diag1[board::size2] = {
	0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,0,
	2,2,2,2,2,2,1,0,
	3,3,3,3,3,2,1,0,
	4,4,4,4,3,2,1,0,
	5,5,5,4,3,2,1,0,
	6,6,5,4,3,2,1,0,
	7,6,5,4,3,2,1,0
};

const pos_type pos_diag2[board::size2] = {
	0,0,0,0,0,0,0,0,
	0,1,1,1,1,1,1,1,
	0,1,2,2,2,2,2,2,
	0,1,2,3,3,3,3,3,
	0,1,2,3,4,4,4,4,
	0,1,2,3,4,5,5,5,
	0,1,2,3,4,5,6,6,
	0,1,2,3,4,5,6,7
};

const ull mask_h[board::size] = {
	0x00000000000000ff,
	0x000000000000ff00,
	0x0000000000ff0000,
	0x00000000ff000000,
	0x000000ff00000000,
	0x0000ff0000000000,
	0x00ff000000000000,
	0xff00000000000000
};

const ull mask_v[board::size] = {
	0x0101010101010101,
	0x0202020202020202,
	0x0404040404040404,
	0x0808080808080808,
	0x1010101010101010,
	0x2020202020202020,
	0x4040404040404040,
	0x8080808080808080
};

const ull mask_d1[board::size * 2 - 1] = {
	0x0000000000000001,
	0x0000000000000102,
	0x0000000000010204,
	0x0000000001020408,
	0x0000000102040810,
	0x0000010204081020,
	0x0001020408102040,
	0x0102040810204080,
	0x0204081020408000,
	0x0408102040800000,
	0x0810204080000000,
	0x1020408000000000,
	0x2040800000000000,
	0x4080000000000000,
	0x8000000000000000
};

const ull mask_d2[board::size * 2 - 1] = {
	0x0000000000000080,
	0x0000000000008040,
	0x0000000000804020,
	0x0000000080402010,
	0x0000008040201008,
	0x0000804020100804,
	0x0080402010080402,
	0x8040201008040201,
	0x4020100804020100,
	0x2010080402010000,
	0x1008040201000000,
	0x0804020100000000,
	0x0402010000000000,
	0x0201000000000000,
	0x0100000000000000
};

unsigned short table_flip[1 << 19];

struct flip_info{
	ull index_h, mask_h;
	ull index_v, mask_v;
	ull index_d1, mask_d1;
	ull index_d2, mask_d2;
	ull mask_hvd;
};

flip_info table_flip_info[board::size2];

void board::flip(cbool color,cpos_type pos){

	ull& brd_blue = this->get_brd(color);
	ull& brd_green = this->get_brd(!color);

	ull piece, temp, mask, brd_result;
	ull sum_blue = 0, sum_green = 0;
	const flip_info& info = table_flip_info[pos];

	//horizontal
	mask = info.mask_h;
	piece = info.index_h;
	asm_pext(brd_blue,mask,temp);
	piece |= temp << 8;
	asm_pext(brd_green,mask,temp);
	piece |= temp;
	piece = table_flip[piece];
	asm_pdep(piece,mask,brd_result);
	sum_green |= brd_result;
	piece >>= 8;
	asm_pdep(piece,mask,brd_result);
	sum_blue |= brd_result;

	//vertical
	mask = info.mask_v;
	piece = info.index_v;
	asm_pext(brd_blue,mask,temp);
	piece |= temp << 8;
	asm_pext(brd_green,mask,temp);
	piece |= temp;
	piece = table_flip[piece];
	asm_pdep(piece,mask,brd_result);
	sum_green |= brd_result;
	piece >>= 8;
	asm_pdep(piece,mask,brd_result);
	sum_blue |= brd_result;

	//diagonal
	mask = info.mask_d1;
	piece = info.index_d1;
	asm_pext(brd_blue,mask,temp);
	piece |= temp << 8;
	asm_pext(brd_green,mask,temp);
	piece |= temp;
	piece = table_flip[piece];
	asm_pdep(piece,mask,brd_result);
	sum_green |= brd_result;
	piece >>= 8;
	asm_pdep(piece,mask,brd_result);
	sum_blue |= brd_result;

	//diagonal
	mask = info.mask_d2;
	piece = info.index_d2;
	asm_pext(brd_blue,mask,temp);
	piece |= temp << 8;
	asm_pext(brd_green,mask,temp);
	piece |= temp;
	piece = table_flip[piece];
	asm_pdep(piece,mask,brd_result);
	sum_green |= brd_result;
	piece >>= 8;
	asm_pdep(piece,mask,brd_result);
	sum_blue |= brd_result;

	mask = info.mask_hvd;
	brd_blue = (brd_blue & ~mask) | (sum_blue & mask);
	brd_green = (brd_green & ~mask) | (sum_green & mask);
}

unsigned short flip_line(cull data){

	unsigned short brd_blue = (data >> 8) & 0xff;
	unsigned short brd_blue_save = brd_blue;
	unsigned short brd_green = data & 0xff;
	unsigned short mask = 1 << (data >> 16);
	unsigned short result = 0;
	ull pos = mask;

	if((brd_blue | brd_green) & mask){
		goto label_end;
	}

	while(pos & 0xfe){
		pos >>= 1;
		if(brd_green & pos)
			continue;
		if(brd_blue & pos){
			while(pos <<= 1, pos != mask){
				brd_blue |= pos;
				brd_green &= ~pos;
			}
		}
		break;
	}
	pos = mask;

	while(pos & 0x7f){
		pos <<= 1;
		if(brd_green & pos)
			continue;
		if(brd_blue & pos){
			while(pos >>= 1, pos != mask){
				brd_blue |= pos;
				brd_green &= ~pos;
			}
		}
		break;
	}
	pos = mask;

	if(brd_blue != brd_blue_save){
		brd_blue |= mask;
		brd_green &= ~mask;
	}

	label_end:

	result = (brd_blue << 8) | brd_green;
	return result;
}

void board::config_flip(){
	pos_type pos_x,pos_y;

	for(pos_type i = 0;i != size2;++i){
		flip_info& info = table_flip_info[i];
		pos_x = i & 0x7;
		pos_y = i >> 3;
		info.index_h = pos_x << 16;
		info.mask_h = mask_h[pos_y];
		info.index_v = pos_y << 16;
		info.mask_v = mask_v[pos_x];
		info.index_d1 = pos_diag1[i] << 16;
		info.mask_d1 = mask_d1[pos_x + pos_y];
		info.index_d2 = pos_diag2[i] << 16;
		info.mask_d2 = mask_d2[pos_y + 7 - pos_x];
		info.mask_hvd = info.mask_h | info.mask_v | info.mask_d1 | info.mask_d2;
	}

	for(ull i = 0;i != (1 << 19);++i){
		if((i >> 8) & i & 0xff){}else{
			table_flip[i] = flip_line(i);
		}
	}
}

#else

void board::flip(cbool color,cpos_type pos){
	ull& brd_blue = this->get_brd(color);
	ull& brd_green = this->get_brd(!color);
	ull brd_flip;
	ull brd_green_inner = brd_green & 0x7E7E7E7E7E7E7E7Eu;
	ull brd_green_adj;
	ull moves = 0;
	ull brd_pos = 0;

	fun_bts(brd_pos, ull(pos));

	#define flip_part(shift, val, brd_mask) \
		brd_flip = (brd_pos shift val) & brd_mask; \
		brd_flip |= (brd_flip shift val) & brd_mask; \
		\
		brd_green_adj = brd_mask & (brd_mask shift val); \
		brd_flip |= (brd_flip shift (val << 1)) & brd_green_adj; \
		brd_flip |= (brd_flip shift (val << 1)) & brd_green_adj; \
		\
		if((brd_flip shift val) & brd_blue){ \
			moves |= brd_flip; \
		}

	flip_part(<<, 1, brd_green_inner);
	flip_part(>>, 1, brd_green_inner);
	flip_part(<<, 8, brd_green);
	flip_part(>>, 8, brd_green);
	flip_part(<<, 7, brd_green_inner);
	flip_part(>>, 7, brd_green_inner);
	flip_part(<<, 9, brd_green_inner);
	flip_part(>>, 9, brd_green_inner);

	brd_blue ^= moves;
	brd_green ^= moves;
	fun_bts(brd_blue, ull(pos));
}

void board::config_flip(){}

#endif //USE_ASM
