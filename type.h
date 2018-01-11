/** @file type.h
 * @brief This file includes some basic types.
 */

#ifndef TYPE_H
#define TYPE_H

#include <limits>

using namespace std;

//#define USE_REF
#define USE_FLOAT

// check whether the complier support 64-bit AT&T inline assembly
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
	mthd_ptn = 0x40, // pattern
	mthd_mpc = 0x80, // multi-probability cut
	mthd_end = 0x100, // end game solver
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

	sts_bwin = sts_black,
	sts_wwin = sts_white,
	sts_tie = sts_null,
	sts_bturn = sts_black | sts_turn,
	sts_wturn = sts_white | sts_turn,
	sts_bagain = sts_black | sts_turn | sts_again,
	sts_wagain = sts_white | sts_turn | sts_again
};

#include <random>
#include <chrono>
extern default_random_engine engine;

#ifdef DEBUG_SEARCH
	#include <fstream>
	extern ofstream out;
#endif

class board;
struct choice;
class pattern;
class group;

#ifdef USE_REF
	typedef const board& cboard;
	typedef const choice& cchoice;
	typedef const pattern& cpattern;
	typedef const group& cgroup;
#else
	typedef board cboard;
	typedef choice cchoice;
	typedef pattern cpattern;
	typedef group cgroup;
#endif //USE_REF

#endif //TYPE_H