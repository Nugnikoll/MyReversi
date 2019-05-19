#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

#include "type.h"

using namespace std;

class pattern;
class group;

extern pattern ptn;

extern int table_map[65536];
extern int table_map_inv[6561];

class pattern{
public:
	pattern() = default;
	pattern(const pattern&) = default;
	pattern(pattern&&) = default;
	pattern(ARRAY_1D_IN_I(VAL_TYPE)){
		assert(i1 == size);
		memcpy(table, ptri, sizeof(table));
	}

	pattern& operator=(const pattern&) = default;
	pattern& operator=(pattern&&) = default;

	static const int size = 2643200;

	val_type table[size];

	void initial(){
		memset(table, 0, sizeof(table));
	}

	void view(ARRAY_1D_OUT_O(VAL_TYPE)){
		*o1 = size;
		*ptro = table;
	}
	void numpy(ARRAY_1D_OUT_M(VAL_TYPE)){
		*m1 = size;
		*ptrm = new val_type[size];
		memcpy(*ptrm, table, sizeof(val_type) * size);
	}

	static void config(const string& file_ptn = "");

	val_type& at(cint n, cint pos){
		return table[(n << 16) + pos];
	};
	cval_type at(cint n, cint pos)const{
		return table[(n << 16) + pos];
	};
	val_type& at(cint n){
		return table[n];
	};
	cval_type at(cint n)const{
		return table[n];
	};

	val_type __getitem__(cint n)const{
		return table[n];
	}
	void __setitem__(cint n, cval_type val){
		table[n] = val;
	}

	void load(const string& path);
	void save(const string& path)const;

	void balance();
};

void sample_gen(ARRAY_2D_OUT_M(ULL), cint n);
void evaluate(ARRAY_1D_OUT_M(VAL_TYPE), ARRAY_2D_IN_I(ULL), cmethod mthd, cshort height);
void evaluate(ARRAY_1D_OUT_M(VAL_TYPE), const pattern& ptn, ARRAY_2D_IN_I(ULL));
void adjust(pattern& ptn, ARRAY_2D_IN_I(ULL), ARRAY_1D_IN_J(VAL_TYPE));

#endif //PATTERN_H
