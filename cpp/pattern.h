#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include "matrix.h"
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
	pattern(int h, int w, val_type* ptr){
		assert(h == 1);
		assert(w == size);
		memcpy(table, ptr, sizeof(table));
	}

	pattern& operator=(const pattern&) = default;
	pattern& operator=(pattern&&) = default;

	static const int size = 2643200;

	val_type table[size];

	void initial(){
		memset(table, 0, sizeof(table));
	}

	void numpy(int* h, int*w, val_type** ptr){
		*h = 1;
		*w = size;
		*ptr = table;
	}

	static void config();

	static void config(const string& path){
		ptn.load(path);
	}

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

matrix<board> sample_gen(cint n);
matrix<val_type> evaluate(const matrix<board>& brds,cmethod mthd,cshort height);
matrix<val_type> evaluate(const pattern& ptn, const matrix<board>& brds);
void adjust(pattern& ptn, const matrix<board>& brds, const matrix<val_type>& delta);

#endif //PATTERN_H
