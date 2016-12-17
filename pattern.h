#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

#include "reversi.h" //--

using namespace std;

struct element{
	float val;
	int cnt;
	float get_val()const{
		return val;
	}
	int get_cnt()const{
		return cnt;
	}
	string to_str()const{
		ostringstream out;
		out << '(' << val << ',' << cnt << ')';
		return out.str();
	}
};

struct pattern{
	pattern(){
		memset(table1,0,sizeof(table1));
		memset(table2,0,sizeof(table2));
	}
	static const size_t stage = 4;
	static const size_t size1 = 11,size2 = 30 * 30;
	static const size_t length = 1 << 16;
	element table1[stage][size1][length];
	element table2[stage][size2];

	void initial();
	element* get1(cint stage,cint num,cint pos){
		return &table1[stage][num][pos];
	};
	element* get2(cint stage,cint num){
		return &table2[stage][num];
	};
	void compress(element* const& ptr);
	void decompress(element* const& ptr);
};

class group{
public:
	vector<pattern> vec;
	vector<short> record;
	void assign(const int& size);
	void initial(){
		for(auto& ptn:vec){
			ptn.initial();
		}
	}
	void load(
		const string& path,cbool is_compress = true,cint num_begin = 0,cint num = 100
		,cbool is_compatible = true);
	void save(const string& path,const bool& is_compress = true);
	pattern* get(const int& pos){
		return &vec.at(pos);
	}
	void train();
	void print_record();
};

bool compete(pattern* const& p1,pattern* const& p2);

bool is_prime(const long long& num);

extern pattern* ptr_pattern;

extern void set_ptn(pattern* ptr);

#endif //PATTERN_H
