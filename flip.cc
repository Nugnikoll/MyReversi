#include "reversi.h"

#ifdef USE_ASM_BMI

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

const brd_type mask_h[board::size] = {
	0x00000000000000ff,
	0x000000000000ff00,
	0x0000000000ff0000,
	0x00000000ff000000,
	0x000000ff00000000,
	0x0000ff0000000000,
	0x00ff000000000000,
	0xff00000000000000
};

const brd_type mask_v[board::size] = {
	0x0101010101010101,
	0x0202020202020202,
	0x0404040404040404,
	0x0808080808080808,
	0x1010101010101010,
	0x2020202020202020,
	0x4040404040404040,
	0x8080808080808080
};

const brd_type mask_d1[board::size * 2 - 1] = {
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

const brd_type mask_d2[board::size * 2 - 1] = {
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

brd_type mask_hvd[board::size2];

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

unsigned short table_flip[1 << 19];

bool board::flip(cbool color,cpos_type pos){

	brd_type& brd_blue = this->bget(color);
	brd_type& brd_green = this->bget(!color);

	brd_type brd_save = brd_blue;

	brd_type piece, temp, mask, brd_result;
	brd_type sum_blue = 0, sum_green = 0;
	brd_type pos_x = pos & 0x7;
	brd_type pos_y = pos >> 3;

	//horizontal
	mask = mask_h[pos_y];
	piece = pos_x << 16;
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
	mask = mask_v[pos_x];
	piece = pos_y << 16;
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
	mask = mask_d1[pos_x + pos_y];
	piece = pos_diag1[pos] << 16;
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
	mask = mask_d2[pos_y + 7 - pos_x];
	piece = pos_diag2[pos] << 16;
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

	mask = mask_hvd[pos];
	brd_blue = (brd_blue & ~mask) | (sum_blue & mask);
	brd_green = (brd_green & ~mask) | (sum_green & mask);

	bool result = (brd_blue != brd_save);
	return result;
}

unsigned short flip_line(cbrd_type data){

	unsigned short brd_blue = (data >> 8) & 0xff;
	unsigned short brd_blue_save = brd_blue;
	unsigned short brd_green = data & 0xff;
	unsigned short mask = 1 << (data >> 16);
	unsigned short result = 0;
	brd_type pos = mask;

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
	brd_type mask;
	pos_type pos_x,pos_y;
	for(pos_type i = 0;i != size2;++i){
		pos_x = i & 0x7;
		pos_y = i >> 3;
		mask = mask_h[pos_y];
		mask |= mask_v[pos_x];
		mask |= mask_d1[pos_x + pos_y];
		mask |= mask_d2[pos_y + 7 - pos_x];
		mask_hvd[i] = mask;
	}

	for(brd_type i = 0;i != (1 << 19);++i){
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
	bool name(board* const& ptr,cbool color,cpos_type _pos){ \
		brd_type& blue = ptr->bget(color), blue_save = blue; \
		brd_type& green = ptr->bget(!color); \
		brd_type mask = brd_type(1) << _pos;\
 \
		if((blue | green) & mask){ \
			return false; \
		} \
 \
		brd_type pos = mask; \
 \
		kernel \
 \
		bool everflip = (blue != blue_save); \
		if(everflip){ \
			blue |= mask; \
			green &= ~mask; \
		} \
		return everflip; \
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

bool flip_n(board* const&,cbool color,cpos_type pos){
	return false;
}

bool (* const table_flip[board::size2]) (board* const&,cbool,cpos_type) =
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

bool board::flip(cbool color,cpos_type pos){
	return (table_flip[pos])(this,color,pos);
}
void board::config_flip(){}

#endif //USE_ASM
