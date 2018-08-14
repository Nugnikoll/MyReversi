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
extern int table_reverse[256];
extern int table_shuffle[256];

class pattern{
public:
	pattern() = default;
	pattern(const pattern&) = default;
	pattern(pattern&&) = default;
	pattern(int h, int w, val_type* ptr){
		assert(h == size);
		assert(w == length);
		memcpy(table, ptr, sizeof(table));
	}

	pattern& operator=(const pattern&) = default;
	pattern& operator=(pattern&&) = default;

	static const size_t size = 10;
	static const size_t size_n = 36;
	static const size_t length = 1 << 16;
	static const size_t length_compress = 6561;
	static const ull table_mask[size_n];

	val_type table[size * length];

	void initial(){
		memset(table, 0, sizeof(table));
	}

	void numpy(int* h, int*w, val_type** ptr){
		*h = size;
		*w = length;
		*ptr = table;
	}

	static void config(){
		ull j = 0;
		for(ull i = 0; i != length; ++i){
			if((i & (i >> 8)) == 0){
				table_map[i] = j;
				table_map_inv[j] = i;
				++j;
			}else{
				table_map[i] = -1;
			}
		}

		for(ull i = 0; i != 256; ++i){
			j = board::deposit(i, 0x0101010101010101);
			board::mirror_v(j);
			j = board::extract(j, 0x0101010101010101);
			table_reverse[i] = j;

			j = board::deposit(i, 0x2004801002400801);
			board::rotate_r(j);
			j = board::extract(j, 0x0420010840021080);
			table_shuffle[i] = j;
		}
	}

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
	void load(istream& fin);
	void save(const string& path)const;
	void save(ostream& fout)const;

	void balance();
};

matrix<board> sample_gen(cint n);
matrix<val_type> evaluate(const matrix<board>& brds,cmethod mthd,cshort height);
matrix<val_type> evaluate(const pattern& ptn, const matrix<board>& brds);
void adjust(pattern& ptn, const matrix<board>& brds, const matrix<val_type>& delta);

#endif //PATTERN_H
