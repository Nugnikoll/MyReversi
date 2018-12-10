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

	ull& brd_blue = this->bget(color);
	ull& brd_green = this->bget(!color);

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

#define lbound 0xfefefefefefefefe
#define rbound 0x7f7f7f7f7f7f7f7f
#define ubound 0xffffffffffffff00
#define dbound 0x00ffffffffffffff
#define ulbound 0xfefefefefefefe00
#define urbound 0x7f7f7f7f7f7f7f00
#define dlbound 0x00fefefefefefefe
#define drbound 0x007f7f7f7f7f7f7f

#define up(mask) mask >>= 8
#define down(mask) mask <<= 8
#define left(mask) mask >>= 1
#define right(mask) mask <<= 1
#define uleft(mask) mask >>= 9
#define uright(mask) mask >>= 7
#define dleft(mask) mask <<= 7
#define dright(mask) mask <<= 9

#define flip_part(bound,dir1,dir2) \
	while(pos & bound){ \
		dir1(pos); \
		if(green & pos) \
			continue; \
		if(blue & pos){ \
			while(dir2(pos), pos != mask){ \
				blue |= pos; \
				green &= ~pos; \
			} \
		} \
		break; \
	} \
	pos = mask;

#define flip_part_u flip_part(ubound,up,down)
#define flip_part_d flip_part(dbound,down,up)
#define flip_part_l flip_part(lbound,left,right)
#define flip_part_r flip_part(rbound,right,left)
#define flip_part_ul flip_part(ulbound,uleft,dright)
#define flip_part_ur flip_part(urbound,uright,dleft)
#define flip_part_dl flip_part(dlbound,dleft,uright)
#define flip_part_dr flip_part(drbound,dright,uleft)

#define flip_fun(name,kernel) \
 \
	void name(board* const& ptr,cbool color,cpos_type _pos){ \
		ull& blue = ptr->bget(color), blue_save = blue; \
		ull& green = ptr->bget(!color); \
		ull mask = ull(1) << _pos;\
 \
		ull pos = mask; \
 \
		kernel \
 \
		if(blue != blue_save){ \
			blue |= mask; \
			green &= ~mask; \
		} \
	}

flip_fun(flip,
	flip_part_u
	flip_part_d
	flip_part_l
	flip_part_r
	flip_part_ul
	flip_part_ur
	flip_part_dl
	flip_part_dr
)

flip_fun(flip_o,
	flip_part_u
	flip_part_d
	flip_part_l
	flip_part_r
	flip_part_ul
	flip_part_ur
	flip_part_dl
	flip_part_dr
)

flip_fun(flip_u,
	flip_part_u
	flip_part_ul
	flip_part_ur
	flip_part_l
	flip_part_r
)

flip_fun(flip_d,
	flip_part_d
	flip_part_dl
	flip_part_dr
	flip_part_l
	flip_part_r
)

flip_fun(flip_l,
	flip_part_l
	flip_part_ul
	flip_part_dl
	flip_part_u
	flip_part_d
)

flip_fun(flip_r,
	flip_part_r
	flip_part_ur
	flip_part_dr
	flip_part_u
	flip_part_d
)

flip_fun(flip_ul,
	flip_part_u
	flip_part_l
	flip_part_ul
)

flip_fun(flip_ur,
	flip_part_u
	flip_part_r
	flip_part_ur
)

flip_fun(flip_dl,
	flip_part_d
	flip_part_l
	flip_part_dl
)

flip_fun(flip_dr,
	flip_part_d
	flip_part_r
	flip_part_dr
)

void flip_n(board* const&,cbool color,cpos_type pos){
}

void (* const table_flip[board::size2]) (board* const&,cbool,cpos_type) =
{
	flip_dr,flip_dr,flip_d,flip_d,flip_d,flip_d,flip_dl,flip_dl,
	flip_dr,flip_dr,flip_d,flip_d,flip_d,flip_d,flip_dl,flip_dl,
	flip_r,flip_r,flip_o,flip_o,flip_o,flip_o,flip_l,flip_l,
	flip_r,flip_r,flip_o,flip_n,flip_n,flip_o,flip_l,flip_l,
	flip_r,flip_r,flip_o,flip_n,flip_n,flip_o,flip_l,flip_l,
	flip_r,flip_r,flip_o,flip_o,flip_o,flip_o,flip_l,flip_l,
	flip_ur,flip_ur,flip_u,flip_u,flip_u,flip_u,flip_ul,flip_ul,
	flip_ur,flip_ur,flip_u,flip_u,flip_u,flip_u,flip_ul,flip_ul
};

void board::flip(cbool color,cpos_type pos){
	return (table_flip[pos])(this,color,pos);
}
void board::config_flip(){}

#endif //USE_ASM
