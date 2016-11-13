#ifndef TRAIN_H
#define TRAIN_H

#include <vector>
#include <random>
#include <chrono>
#include <cstring>
#include <fstream>

#include "reversi.h" 

using namespace std;

struct pattern{
	static const int length = 1 << 8;
	unsigned int factor[4];
	float table[length];
	void change();
	void fade();
	void generate();
};

template<typename type>
class vector_ptr:public vector<type*>{
public:
	class iterator:public vector<type*>::iterator{};
	vector_ptr():vector<type*>(){};
	vector_ptr(const vector_ptr& v):vector<type*>(v){
		for(auto& t: *this){
			t = new type(*t);
		}
	}
	vector_ptr& operator=(const vector_ptr& v){
		for(auto& p:*this){
			delete p;
		}
		vector<type*>::operator=(v);
		for(auto& p: *this){
			p = new type(*p);
		}
		return *this;
	}
	~vector_ptr(){
		for(auto& p:*this){
			delete p;
		}
	}
	void push_back(type*const& p){
		vector<type*>::push_back(p);
	}
	void push_back(const type& p){
		vector<type*>::push_back(new pattern(p));
	}
	void pop_back(){
		delete this->back();
		vector<type*>::pop_back();
	}
	vector_ptr::iterator erase(
		const vector_ptr::iterator& __first, const vector_ptr::iterator& __last
	){
		assert(__first <= __last);
		while(__first != __last){
			delete *__first;
			++__first;
		}
		return vector<type*>::erase(__first,__last);
	}
};

class param:public vector_ptr<pattern>{
public:
	static const int size_max = 10;
	static const int size_min = 3;
	void generate();
	void change();
};

class group:public vector_ptr<param>{
public:
	static const int size_max = 100;
	static const int size_min = 50;
	void generate();
	param produce();
	void evolve();
	void load(const string& filename);
	void save(const string& filename);
};

calc_type eval_train(const board& brd,const bool& color);

bool compare(const param* const& p1,const param* const& p2);

bool is_prime(const long long& num);

#endif //TRAIN_H
