#include "reversi.h"

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

#define table_flip table_flip1
#define flip_u board::flip_u<true>
#define flip_d board::flip_d<true>
#define flip_l board::flip_l<true>
#define flip_r board::flip_r<true>
#define flip_ul board::flip_ul<true>
#define flip_ur board::flip_ur<true>
#define flip_dl board::flip_dl<true>
#define flip_dr board::flip_dr<true>
#define flip_o board::flip_o<true>
#define flip_n board::flip_n<true>

bool (board::* const board::table_flip[board::size2]) (cbrd_type) =
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

#undef table_flip
#undef flip_u
#undef flip_d
#undef flip_l
#undef flip_r
#undef flip_ul
#undef flip_ur
#undef flip_dl
#undef flip_dr
#undef flip_o
#undef flip_n

#define table_flip table_flip2
#define flip_u board::flip_u<false>
#define flip_d board::flip_d<false>
#define flip_l board::flip_l<false>
#define flip_r board::flip_r<false>
#define flip_ul board::flip_ul<false>
#define flip_ur board::flip_ur<false>
#define flip_dl board::flip_dl<false>
#define flip_dr board::flip_dr<false>
#define flip_o board::flip_o<false>
#define flip_n board::flip_n<false>

bool (board::* const board::table_flip[board::size2]) (cbrd_type) =
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

#undef table_flip
#undef flip_u
#undef flip_d
#undef flip_l
#undef flip_r
#undef flip_ul
#undef flip_ur
#undef flip_dl
#undef flip_dr
#undef flip_o
#undef flip_n

#define flip_part(bound,dir1,dir2) \
	while(pos & bound){ \
		dir1(pos); \
		if(green & pos) \
			continue; \
		if(blue & pos){ \
			while(dir2(pos), pos != mask){ \
				blue |= pos; \
				green &= ~pos; \
				everflip = true; \
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
	template bool board::name<true>(cbrd_type mask); \
	template bool board::name<false>(cbrd_type mask); \
 \
	template<bool color> \
	bool board::name(cbrd_type mask){ \
 \
		if((brd_black | brd_white) & mask){ \
			return false; \
		} \
 \
		bool everflip = false; \
		brd_type pos = mask; \
		brd_type& blue = bget<color>(); \
		brd_type& green = bget<!color>(); \
 \
		kernel \
 \
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

template bool board::flip_n<true>(cbrd_type mask);
template bool board::flip_n<false>(cbrd_type mask);

template<bool color>
bool board::flip_n(cbrd_type mask){
	return false;
}
