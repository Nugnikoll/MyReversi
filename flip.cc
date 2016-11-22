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

template bool board::flip<true>(cbrd_type mask);
template bool board::flip<false>(cbrd_type mask);

template<bool color>
bool board::flip(cbrd_type mask){

	if((brd_black | brd_white) & mask){
		return false;
	}

	bool everflip = false;
	brd_type pos = mask;
	brd_type& blue = bget<color>();
	brd_type& green = bget<!color>();

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

	flip_part(lbound,left,right);
	flip_part(rbound,right,left);
	flip_part(ubound,up,down);
	flip_part(dbound,down,up);
	flip_part(ulbound,uleft,dright);
	flip_part(urbound,uright,dleft);
	flip_part(dlbound,dleft,uright);
	flip_part(drbound,dright,uleft);

	if(everflip){
		blue |= mask;
		green &= ~mask;
	}
	return everflip;
}