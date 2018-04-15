#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "reversi.h"

using namespace std;

struct node{
	board brd;
	bool color;
	short depth;
	calc_type alpha, beta;
	calc_type value;
};
typedef const node& cnode;

class log_record{
protected:
	bool flag;
	vector<node> vec;

public:
	log_record():flag(false){}

	void enable(){
		flag = true;
	}
	void disable(){
		flag = false;
	}
	void clear(){
		vec.clear();
	}
	void insert(cnode n){
		if(flag){
			vec.push_back(n);
		}
	}
	void save(const string& filename)const{
		ofstream out(filename, ios::binary | ios::out);
		for(auto iter = vec.rbegin(); iter != vec.rend(); ++iter){
			out.write((const char*) &(*iter), sizeof(node));
		}
		out.close();
	}
};
