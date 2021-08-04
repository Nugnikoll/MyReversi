/** @file type.h
 * @brief This file includes some basic types.
 */

#ifndef TYPE_H
#define TYPE_H

#include <limits>

using namespace std;

//#define USE_REF
//#define DEBUG_SEARCH
//#define USE_TERMINATE

//check whether the complier support 64-bit AT&T inline assembly
#ifndef USE_CMAKE
	#if defined(__GNUC__) || defined(__clang__)
		#if defined(__x86_64__)
			#ifndef USE_ASM
				#define USE_ASM
			#endif
			//#ifndef USE_ASM_BMI2
				//#define USE_ASM_BMI2
			//#endif
			//#ifndef USE_ASM_AVX2
				//#define USE_ASM_AVX2
			//#endif
		#endif
	#endif
#endif
#ifndef USE_ASM
	#undef USE_ASM_BMI2
	#undef USE_ASM_AVX2
#endif
#ifdef _BOTZONE_ONLINE
	#define USE_ASM
	#define USE_ASM_BMI2
	#define USE_ASM_AVX2
	#define USE_TERMINATE
#endif //_BOTZONE_ONLINE

typedef const bool& cbool;
typedef const short& cshort;
typedef const int& cint;
typedef const float& cfloat;
#define LL long long
typedef long long ll;
typedef const ll& cll;
#define ULL unsigned long long
typedef unsigned long long ull;
typedef const ull& cull;

typedef unsigned char line_type;
typedef const line_type& cline_type;
typedef short pos_type;
typedef const pos_type& cpos_type;

#define VAL_TYPE float
typedef VAL_TYPE val_type;
typedef const val_type& cval_type;

struct pull{
	ull first;
	ull second;
};
typedef const pull& cpull;

enum chessman{blank, white, black, null};
#ifdef USE_REF
	typedef const chessman& cchessman;
#else
	typedef chessman cchessman;
#endif //USE_REF

const val_type inf = numeric_limits<short>::max();
const val_type _inf = - inf;
const val_type epsilon = 0.0001;

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
	mthd_end = 0x100 // end game solver
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

#ifdef USE_TERMINATE
	#include <exception>
	extern bool flag_timeout;
	class timeout_exception: public exception
	{
		virtual const char* what() const throw(){
			return "Timeout exception happened";
		}
	};
#endif

class board;
typedef const board& cboard;
struct choice;
typedef const choice& cchoice;
class pattern;
typedef const pattern& cpattern;

enum player_type{
	ply_human, ply_ai, ply_other
};
#ifdef USE_REF
	typedef const player_type& cplayer_type;
#else
	typedef player_type cplayer_type;
#endif //USE_REF

#define ARRAY_1D_IN_I(TYPE) TYPE* ptri, int i1
#define ARRAY_1D_IN_J(TYPE) TYPE* ptrj, int j1
#define ARRAY_1D_OUT_O(TYPE) TYPE** ptro, int* o1
#define ARRAY_1D_OUT_M(TYPE) TYPE** ptrm, int* m1
#define ARRAY_2D_IN_I(TYPE) TYPE* ptri, int i1, int i2
#define ARRAY_2D_IN_J(TYPE) TYPE* ptrj, int j1, int j2
#define ARRAY_2D_OUT_O(TYPE) TYPE** ptro, int* o1, int* o2
#define ARRAY_2D_OUT_M(TYPE) TYPE** ptrm, int* m1, int* m2
#define ARRAY_3D_IN_I(TYPE) TYPE* ptri, int i1, int i2, int i3
#define ARRAY_3D_IN_J(TYPE) TYPE* ptrj, int j1, int j2, int j3
#define ARRAY_3D_OUT_O(TYPE) TYPE** ptro, int* o1, int* o2, int* o3
#define ARRAY_3D_OUT_M(TYPE) TYPE** ptrm, int* m1, int* m2, int* m3

#if defined(__GNUC__)
	#define likely(x) (__builtin_expect(!!(x), 1))
	#define unlikely(x) (__builtin_expect(!!(x), 0))
#else
	#define likely(x) (x)
	#define unlikely(x) (x)
#endif

#endif //TYPE_H
