#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

#include "matrix.h"
#include "type.h"

using namespace std;

class pattern;
class group;

extern group grp;

extern int table_map[65536];
extern int table_map_inv[6561];
extern int table_reverse[256];
extern int table_shuffle[256];

class pattern{
public:
	pattern() = default;
	pattern(const pattern&) = default;
	pattern(pattern&&) = default;
	pattern& operator=(const pattern&) = default;
	pattern& operator=(pattern&&) = default;

	static const size_t size = 10;
	static const size_t size_n = 36;
	static const size_t length = 1 << 16;
	static const size_t length_compress = 6561;
	static const short table_num[size_n];
	static const short table_num_size[size];
	static const short table_num_convert[size][4];
	static const ull table_mask[size_n];

	val_type table[size * length];

	void initial(){
		memset(table,0,sizeof(table));
	}
	static void config(){
		ull j = 0;
		for(ull i = 0;i != length;++i){
			if((i & (i >> 8)) == 0){
				table_map[i] = j;
				table_map_inv[j] = i;
				++j;
			}else{
				table_map[i] = -1;
			}
		}

		for(ull i = 0;i != 256;++i){
			j = board::deposit(i,0x0101010101010101);
			board::mirror_v(j);
			j = board::extract(j,0x0101010101010101);
			table_reverse[i] = j;

			j = board::deposit(i,0x2004801002400801);
			board::rotate_r(j);
			j = board::extract(j,0x0420010840021080);
			table_shuffle[i] = j;
		}
	}

	val_type& at(cint n,cint pos){
		return table[(n << 16) + pos];
	};
	cval_type at(cint n,cint pos)const{
		return table[(n << 16) + pos];
	};
	val_type& at(cint n){
		return table[n];
	};
	cval_type at(cint n)const{
		return table[n];
	};

	void load(istream& fin);
	void save(ostream& fout)const;

	void balance();
};

class group{
public:
	vector<pattern> vec;
	void initial(const int& size){
		vec.clear();
		while(vec.size() < (unsigned int)(size)){
			vec.emplace_back();
		}
		for(auto& ptn:vec){
			ptn.initial();
		}
	}
	void load(istream& fin);
	void load(const string& path);
	void save(const string& path)const;
	const pattern& at(const int& pos)const{
		return vec[pos];
	}
	pattern& at(const int& pos){
		return vec[pos];
	}

	static void config(const string& str){
		grp.load(str);
	}
};

void get_index(cbool color, cboard brd, int* const& ind);
matrix<float> mat_i2f(const matrix<int>& m);
float mat_2f(const matrix<float>& m);
//matrix<board> sample_2mat(const unordered_set<board>& brds);
//unordered_set<board> sample_gen(cint n);
//matrix<int> sample_process(const unordered_set<board>& brds);
matrix<int> correlate(const matrix<int>& index1, const matrix<int>& index2);
//matrix<val_type> evaluate(const unordered_set<board>& brds,cmethod mthd,cshort height);
matrix<val_type> evaluate(const matrix<board>& brds,cmethod mthd,cshort height);
matrix<val_type> evaluate(const pattern& ptn, const matrix<int>& index);
matrix<val_type> evaluate(const pattern& ptn, const matrix<board>& brds);
void adjust(pattern& ptn, const matrix<int>& index, const matrix<val_type>& delta);
void adjust(pattern& ptn, const matrix<board>& brds, const matrix<val_type>& delta);

#endif //PATTERN_H