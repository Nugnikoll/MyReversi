#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <cstring>
#include <fstream>

#include "reversi.h" //--

using namespace std;

struct pattern{
	pattern(){
		memset(table,0,sizeof(table));
	}
	static const size_t size = 11;
	static const size_t length = 1 << 16;
	float table[size][length];

	void initial();
	void compress(float* const& ptr);
	void decompress(float* const& ptr);
};

class group{
public:
	vector<pattern> vec;
	vector<short> record;
	void assign(const size_t& size);
	void initial();
	void load(
		const string& filename,cbool is_compress = true,cint num_begin = 0,cint num = 100
		,cbool is_compatible = true);
	void save(const string& filename,const bool& is_compress = true);
	void train();
	void print_record();
};

bool compete(pattern* const& p1,pattern* const& p2);

bool is_prime(const long long& num);

extern pattern* ptr_pattern;

#endif //PATTERN_H
