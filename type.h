#ifndef TYPE_H
#define TYPE_H

#include <limits>

using namespace std;

//#define USE_REF
#define USE_FLOAT
#define USE_RANDOM
#if defined(__GNUC__) || defined(__clang__)
	#if defined(__x86_64__) || defined(__ppc64__)
		#define USE_ASM
		#define USE_ASM_BMI
	#endif
#endif
#ifndef USE_ASM
	#undef USE_ASM_BMI
#endif
//#define DEBUG_SEARCH

#ifdef USE_REF
	typedef const bool& cbool;
	typedef const short& cshort;
	typedef const int& cint;
	typedef const float& cfloat;
#else
	typedef bool cbool;
	typedef short cshort;
	typedef int cint;
	typedef float cfloat;
#endif //USE_REF

typedef unsigned long long brd_type;
typedef unsigned char line_type;
typedef short pos_type;

#ifdef USE_FLOAT
	typedef float calc_type;
#else
	typedef short calc_type;
#endif

#ifdef USE_REF
	typedef const brd_type& cbrd_type;
	typedef const line_type& cline_type;
	typedef const pos_type& cpos_type;
	typedef const calc_type& ccalc_type;
#else
	typedef brd_type cbrd_type;
	typedef line_type cline_type;
	typedef pos_type cpos_type;
	typedef calc_type ccalc_type;
#endif //USE_REF

enum chessman{blank,white,black,null};

#ifdef USE_REF
	typedef const chessman& cchessman;
#else
	typedef chessman cchessman;
#endif //USE_REF

const calc_type inf = numeric_limits<short>::max();
const calc_type _inf = - inf;

enum method{

	mthd_rnd = 0x0, // random
	mthd_ab = 0x1, // alpha beta pruning
	mthd_kill = 0x2, // killer heuristic
	mthd_pvs = 0x4, // principal variation search
	mthd_trans = 0x8, // transition table
	mthd_mtdf = 0x10, // memory-enhanced test driver with node n and value f
	mthd_ids = 0x20, // iterative deepening search
	mthd_ptn = 0x40,

	mthd_default = mthd_kill | mthd_ab
};

#ifdef USE_REF
	typedef const method& cmethod;
#else
	typedef method cmethod;
#endif //USE_REF

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

#ifdef USE_RANDOM
	#include <random>
	#include <chrono>
	extern default_random_engine engine;
#endif //USE_RANDOM

#ifdef DEBUG_SEARCH
	#include <fstream>
	extern ofstream out;
#endif

class board;
struct choice;

#ifdef USE_REF
	typedef const board& cboard;
	typedef const choice& cchoice;
#else
	typedef board cboard;
	typedef choice cchoice;
#endif //USE_REF

#endif //TYPE_H