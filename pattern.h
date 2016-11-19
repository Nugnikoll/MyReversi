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
	int win,lose;
	float get_val()const{
		return val;
	}
	int get_win()const{
		return win;
	}
	int get_lose()const{
		return lose;
	}
	string to_str()const{
		ostringstream out;
		out << '(' << val << ',' << win << ',' << lose << ')';
		return out.str();
	}
};

struct pattern{
	pattern(){
		memset(table,0,sizeof(table));
	}
	static const size_t size = 11;
	static const size_t length = 1 << 16;
	element table[size][length];

	void initial();
	element* get(const int& num,const int& pos){
		return &table[num][pos];
	};
	void compress(element* const& ptr);
	void decompress(element* const& ptr);
	void refresh(){
	for(auto& i:table){
		for(auto& j:i){
			j.val = float(j.win << 1) / float(j.win + j. lose) - 1;
		}
	}
}
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
		const string& filename,cbool is_compress = true,cint num_begin = 0,cint num = 100
		,cbool is_compatible = true);
	void save(const string& filename,const bool& is_compress = true);
	pattern* get(const int& pos){
		return &vec.at(pos);
	}
	void refresh(){
		for(auto& ptn:vec){
			ptn.refresh();
		}
	}
	void train();
	void print_record();
};

bool compete(pattern* const& p1,pattern* const& p2);

bool is_prime(const long long& num);

extern pattern* ptr_pattern;

extern void set_ptn(pattern* ptr);

#endif //PATTERN_H
