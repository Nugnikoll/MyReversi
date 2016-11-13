#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cstring>

#include "reversi.h" 
#include "train.h"

using namespace std;

uniform_int_distribution<unsigned char> coin(0,1);
normal_distribution<float> spread(0,1);
uniform_int_distribution<unsigned int> rand_int(0,0xffffffff);

const param* ptr_param;

calc_type eval_train(const board& brd,const bool& color){
	unsigned int index;
	const brd_type& brd_blue = brd.bget(color);
	const brd_type& brd_green = brd.bget(!color);
	float result = 0;
	for(const auto& ptn:*ptr_param){
		index = 0;
		index = unsigned(brd_blue) * ptn->factor[0]
			+ unsigned(brd_blue >> 32) * ptn->factor[1]
			+ unsigned(brd_green) * ptn->factor[2]
			+ unsigned(brd_green >> 32) * ptn->factor[3];
		result += ptn->table[index];
	}
	return result;
}

bool compare(const param* const& p1,const param* const& p2){
	board brd;
	coordinate pos1,pos2;
	brd.fun = eval_train;
	brd.assign(0x0000000810000000,0x0000001008000000);
	do{
		ptr_param = p1;
		pos1 = brd.play(true,0);
		ptr_param = p2;
		pos2 = brd.play(false,0);
	}while(pos1.x >= 0 || pos2.x >= 0);
	return brd.count(true) > brd.count(false);
};

void pattern::change(){
	for(auto& var:this->table){
		var += spread(engine);
	}
};

void pattern::fade(){
	for(auto& var:this->table){
		var *= 1 / (1 + 0.08 * abs(spread(engine)));
	}
};

void pattern::generate(){
	for(auto& f:this->factor){
		do{
			f = engine();
		}while(!is_prime(f));
	}
	memset(this->table,0,sizeof(this->table));
}

void param::generate(){
	reserve(size_max);
	for(int i = 0;i != size_max;++i){
		push_back(new pattern);
		back()->generate();
	}
}

void param::change(){
	unsigned rnd = engine() % 100;
	if(rnd < 5){
		if(this->size() < size_max){
			push_back(new pattern);
			this->back()->generate();
		}
	}else if(rnd < 10){
		this->operator[](engine() % this->size())->generate();
	}else if(rnd < 15){
		if(this->size() > size_min){
			auto& ptr = this->operator[](engine() % this->size());
			delete ptr;
			ptr = this->back();
			this->vector<pattern*>::pop_back();
		}
	}else if(rnd < 25){
		for(auto& ptn:*this){
			if(coin(engine)){
				ptn->fade();
			}
		}
	}else{
		for(auto& ptn:*this){
			if(coin(engine)){
				ptn->change();
			}
		}
	}
//	if(this->size() <= 0){
//		cout << "rnd : " << rnd << endl;
//		assert(this->size() > 0);
//	}
}

param cross(const param& p1,const param& p2){
	param result;
	result.reserve(param::size_max);
	do{
		for(auto& ptn:p1){
			if(coin(engine)){
				result.push_back(*ptn);
			}
		}
		for(auto& ptn:p2){
			if(result.size() >= param::size_max){
				break;
			}
			if(coin(engine)){
				result.push_back(*ptn);
			}
		}
	}while(result.size() < param::size_min);
	return result;
}

void group::generate(){
	reserve(size_max);
	for(int i = 0;i != size_min;++i){
		push_back(new param);
		back()->generate();
	}
}

param group::produce(){
	param result;
	if(1){
		result = *this->operator[](engine() % this->size());
		//result.change();
	}else{
//		result = cross(
//			*this->operator[](engine() % this->size()),
//			*this->operator[](engine() % this->size())
//		);
	}
	return result;
}

void group::evolve(){
	while(this->size() < size_max){
		this->push_back(new param(this->produce()));
	}
//	for(size_t r = 0;r != this->size();++r){
//		cout << "(" << r << "," << this->operator[](r)->size() << ")";
//	}cout << endl;
	random_shuffle(this->begin(),this->end());
	sort(this->begin(),this->end(),compare);
	while(this->size() > size_min){
		this->pop_back();
	}
}

#define READ(var) fin.read((char *)(&var),sizeof(var))
#define WRITE(var) fout.write((char *)(&var),sizeof(var))

void group::load(const string& filename){
	ifstream fin(filename,ios::binary);
	size_type group_size,param_size;
	READ(group_size);
	this->reserve(group_size);
	for(size_type i = 0;i != group_size;++i){
		READ(param_size);
		this->push_back(new param);
		param*& p = this->back();
		p->reserve(param_size);
		for(size_type j = 0;j != param_size;++j){
			p->push_back(new pattern);
			pattern*& ptn = p->back();
			READ(ptn->factor);
			READ(ptn->table);
		}
	}
	fin.close();
}

void group::save(const string& filename){
	ofstream fout(filename,ios::binary);
	size_type group_size = this->size(),param_size;
	WRITE(group_size);
	for(auto& p:*this){
		param_size = p->size();
		WRITE(param_size);
		for(auto& ptn:*p){
			WRITE(ptn->factor);
			WRITE(ptn->table);
		}
	}
	fout.close();
}

bool is_prime(const long long& num){
	if(num <= 1)
		return false;
	int factor = 2;
	int bound = sqrt(num);
	for (;factor != 11 && factor <= bound;++factor){
		if (num % factor == 0)
			return false;
	}
	factor = 11;
	while(factor <= bound){
		if (num % factor == 0)
			return false;
		factor += 2;
		if (num % factor == 0)
			return false;
		factor += 4;
		if (num % factor == 0)
			return false;
		factor += 2;
		if (num % factor == 0)
			return false;
		factor += 4;
		if (num % factor == 0)
			return false;
		factor += 6;
		if (num % factor == 0)
			return false;
		factor += 2;
		if (num % factor == 0)
			return false;
		factor += 6;
		if (num % factor == 0)
			return false;
		factor += 4;
	}
	return true;
}
