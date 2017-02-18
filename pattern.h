#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

#include "reversi.h" //--

using namespace std;

typedef float element;

struct pattern{
	static const size_t stage = 4;
	static const size_t size1 = 11,size2 = 30 * 30;
	static const size_t length = 1 << 16;

	int count;
	element table1[stage][size1][length];
	element table2[stage][size2];

	void initial();
	element get1(cint stage,cint num,cint pos){
		return table1[stage][num][pos];
	};
	element get2(cint stage,cint num){
		return table2[stage][num];
	};
	void save(ostream& out);
	void load(istream& in);
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
	void load(const string& path,cint num_begin = 0,cint num = 100);
	void save(const string& path);
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
extern bool check_ptn();

#endif //PATTERN_H
