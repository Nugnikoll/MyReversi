#ifndef TYPE_H
#define TYPE_H

typedef const bool& cbool;
typedef const short& cshort;
typedef const int& cint;
typedef const float& cfloat;

typedef unsigned long long brd_type;
typedef const brd_type& cbrd_type;
typedef unsigned char line_type;
typedef const line_type& cline_type;
typedef short pos_type;
typedef const pos_type& cpos_type;
#ifdef USE_FLOAT
	typedef float calc_type;
#else
	typedef short calc_type;
#endif
typedef const calc_type& ccalc_type;
enum chessman{blank,white,black,null};
//typedef const chessman& cchessman;
typedef chessman cchessman;

const calc_type inf = numeric_limits<short>::max();
const calc_type _inf = - inf;

enum method{
	mthd_default = 0x1,

	mthd_ab = 0x1, // alpha beta pruning
	mthd_pvs = 0x2, // principal variation search
	mthd_trans = 0x4, // transition table
	mthd_kill = 0x8, // killer heuristic
	mthd_mtdf = 0x10, // memory-enhanced test driver with node n and value f
	mthd_ids = 0x20, // iterative deepening search
	mthd_train = 0x40,
	mthd_ptn = 0x80
};
//typedef const method& cmethod;
typedef method cmethod;

enum sts_type{
	sts_null = 0x0,
	sts_black = 0x1,
	sts_white = 0x2,
	sts_turn = 0x4,
	sts_again = 0x8,
	sts_end = 0x10,

	sts_bwin = sts_black | sts_end,
	sts_wwin = sts_white | sts_end,
	sts_tie = sts_end,
	sts_bturn = sts_black | sts_turn,
	sts_wturn = sts_white | sts_turn,
	sts_bagain = sts_black | sts_again,
	sts_wagain = sts_white | sts_again
};

#endif //TYPE_H