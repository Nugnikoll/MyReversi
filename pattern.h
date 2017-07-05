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

extern group grp;

extern int table_map[65536];
extern int table_map_inv[6561];

class pattern{
public:
	pattern() = default;
	pattern(const pattern&) = default;
	pattern(pattern&&) = default;
	pattern& operator=(const pattern&) = default;
	pattern& operator=(pattern&&) = default;

	static const size_t size = 11;
	static const size_t size_n = 36;
	static const size_t length = 1 << 16;
	static const size_t length_compress = 6561;
	static const short table_num[size_n];
	static const short table_num_size[size];
	static const short table_num_convert[size][4];
	static const brd_type table_mask[size_n];

	calc_type table[size * length];

	void initial(){
		memset(table,0,sizeof(table));
	}
	static void config(){
		int j = 0;
		for(int i = 0;i != length;++i){
			if((i & (i >> 8)) == 0){
				table_map[i] = j;
				table_map_inv[j] = i;
				++j;
			}else{
				table_map[i] = -1;
			}
		}
	}

	calc_type& at(cint n,cint pos){
		return table[(n << 16) + pos];
	};
	ccalc_type at(cint n,cint pos)const{
		return table[(n << 16) + pos];
	};
	calc_type& at(cint n){
		return table[n];
	};
	ccalc_type at(cint n)const{
		return table[n];
	};
	void load(istream& fin);
	void save(ostream& fout)const;
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
		return vec.at(pos);
	}
	pattern& at(const int& pos){
		return vec.at(pos);
	}
//	void train(cmethod mthd,cshort depth);
};

//bool compete(pattern* const& p1,pattern* const& p2,cmethod mthd,cshort depth);
void get_index(cbool color, cboard brd, int* const& ind);
matrix<float> mat_i2f(const matrix<int>& m);
float mat_2f(const matrix<float>& m);
unordered_set<board> sample_gen(cint n);
matrix<int> sample_process(const unordered_set<board>& brds);
matrix<int> correlate(const matrix<int>& index1, const matrix<int>& index2);
matrix<calc_type> evaluate(const unordered_set<board>& brds,cmethod mthd,cshort height);
matrix<calc_type> evaluate(const pattern& ptn, const matrix<int>& index);
void adjust(pattern& ptn, const matrix<int>& index, const matrix<calc_type>& delta);
void optimize(pattern& ptn, const matrix<int>& index, const matrix<float>& target, cint step);

bool is_prime(const long long& num);

#endif //PATTERN_H
