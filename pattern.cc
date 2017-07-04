#include <vector>
#include <algorithm>
#include <cstring>

#include "reversi.h"
#include "pattern.h"

using namespace std;

group grp;

int table_map[1 << 16];
int table_map_inv[6561];

const short pattern::table_num[pattern::size_n] = {
	0, 1, 2, 3, 3, 2, 1, 0,
	0, 1, 2, 3, 3, 2, 1, 0,
	4, 5, 6, 7, 8, 7, 6, 5,
	4, 5, 6, 7, 8, 7, 6, 5,
	9, 10, 9, 10
};

const short pattern::table_num_size[pattern::size] = {
	4,4,4,4,2,4,4,4,2,2,2
};


const short pattern::table_num_convert[pattern::size][4] = {
	{0, 7, 8, 15},
	{1, 6, 9, 14},
	{2, 5, 10, 13},
	{3, 4, 11, 12},
	{16, 24},
	{17, 23, 25, 31},
	{18, 22, 26, 30},
	{19, 21, 27, 29},
	{20, 28},
	{32, 34},
	{33, 35}
};

const brd_type pattern::table_mask[pattern::size_n] = {
	// horizontal pattern
	0x00000000000000ff,
	0x000000000000ff00,
	0x0000000000ff0000,
	0x00000000ff000000,
	0x000000ff00000000,
	0x0000ff0000000000,
	0x00ff000000000000,
	0xff00000000000000,

	//vertical pattern
	0x0101010101010101,
	0x0202020202020202,
	0x0404040404040404,
	0x0808080808080808,
	0x1010101010101010,
	0x2020202020202020,
	0x4040404040404040,
	0x8080808080808080,

	//diagnal pattern
	0x8040201008040201,
	0x4020100804020180,
	0x2010080402018040,
	0x1008040201804020,
	0x0804020180402010,
	0x0402018040201008,
	0x0201804020100804,
	0x0180402010080402,

	0x0102040810204080,
	0x0204081020408001,
	0x0408102040800102,
	0x0810204080010204,
	0x1020408001020408,
	0x2040800102040810,
	0x4080010204081020,
	0x8001020408102040,

	//corner pattern
	0xe0e0c00000000000,
	0x0707030000000000,
	0x0000000000c0e0e0,
	0x0000000000030707
};

void get_index(cbool color, cboard brd, int* const& ind){
	
	brd_type brd_blue = brd.bget(color);
	brd_type brd_green = brd.bget(!color);
	brd_type result,mask;

	for(int i = 0;i != pattern::size_n;++i){
		mask = pattern::table_mask[i];
		result = pattern::table_num[i] << 16;
		result |= board::extract(brd_blue,mask) << 8;
		result |= board::extract(brd_green,mask);
		ind[i] = result;
	}
}

float board::score_ptn(cbool color)const{
	brd_type brd_blue = bget(color);
	brd_type brd_green = bget(!color);
	brd_type index,mask;
	float result;

	result = 0;
	for(int i = 0;i != pattern::size_n;++i){
		mask = pattern::table_mask[i];
		index = pattern::table_num[i] << 16;
		index |= board::extract(brd_blue,mask) << 8;
		index |= board::extract(brd_green,mask);
		result += grp.at(0).at(index);
	}
	return result;
}

void group::load(const string& path,cint num_begin,cint num){
	#define _READ(var) fin.read((char *)(&var),sizeof(var))

	ifstream fin(path,ios::in | ios::binary);
	size_t calc_size, ptn_size, group_size;

	if(!fin){
		fin.close();
		cout << "Error: Cannot open the file: " << path << " ." << endl;
		return;
	}

	_READ(calc_size);
	_READ(ptn_size);
	_READ(group_size);

	if(calc_size != sizeof(calc_type)){
		fin.close();
		cout << "Error: The size of element does not match." << endl;
		return;
	}
	if(ptn_size != sizeof(pattern)){
		fin.close();
		cout << "Error: The size of pattern does not match." << endl;
		return;
	}
	
	this->vec.reserve(group_size);

	for(size_t i = 0;i != group_size && i < size_t(num);++i){
		this->vec.emplace_back();
		_READ(this->vec.back());
	}

	fin.close();
	#undef _READ
}

void group::save(const string& path){
	#define WRITE(var) fout.write((char *)(&var),sizeof(var))
	ofstream fout(path,ios::out | ios::binary);

	size_t calc_size = sizeof(calc_type);
	size_t ptn_size = sizeof(pattern);
	size_t group_size = vec.size();

	WRITE(calc_size);
	WRITE(ptn_size);
	WRITE(group_size);

	for(auto& ptn:this->vec){
		WRITE(ptn);
	}

	fout.close();
	#undef WRITE
}

matrix<float> mat_i2f(const matrix<int>& m){
	return m;
}
float mat_2f(const matrix<float>& m){
	return m.at(0);
}

unordered_set<board> sample_gen(cint n){
	unordered_set<board> brds;
	board brd,brd_save;
	coordinate pos1,pos2;

	for(int i = 0;i != n;++i){
		brd.initial();
		do{
			brd_save = brd;
			pos1 = brd.play(mthd_rnd,true,0);
			if(pos1.x >= 0){
				brds.insert(brd_save);
			}

			brd_save = brd;
			pos2 = brd.play(mthd_rnd,false,0);
			if(pos2.x >= 0){
				brd_save.reverse();
				brds.insert(brd_save);
			}
		}while(pos1.x >= 0 || pos2.x >= 0);
	}
	return brds;
};

matrix<int> sample_process(const unordered_set<board>& brds){
	int i = 0;
	matrix<int> result(brds.size(),pattern::size_n);
	for(cboard brd:brds){
		get_index(true,brd,result[i]);
		++i;
	}
	return result;
}

matrix<int> correlate(const matrix<int>& index1, const matrix<int>& index2){
	int h = index1.geth(), w = index2.geth();
	matrix<int> result(h,w);
	for(int i = 0;i != h;++i){
		for(int j = 0;j != w;++j){
			result[i][j] = 0;
			for(int k = 0;k != pattern::size;++k){
				for(int l = 0;l != pattern::table_num_size[k];++l){
					for(int m = 0;m != pattern::table_num_size[k];++m){
						result[i][j] += (
							index1[i][pattern::table_num_convert[k][l]]
							== index2[j][pattern::table_num_convert[k][m]]
						);
					}
				}
			}
		}
	}
	return result;
}

matrix<calc_type> evaluate(const unordered_set<board>& brds,cmethod mthd,cshort height){
	int i = 0;
	matrix<calc_type> result(brds.size(),1);
	for(cboard brd:brds){
		result.at(i) = brd.search(mthd,true,height);
		++i;
	}
	return result;
}

matrix<calc_type> evaluate(const pattern& ptn, const matrix<int>& index){
	matrix<calc_type> result(index.geth(),1);
	for(int i = 0;i != index.geth();++i){
		result.at(i) = 0;
		for(int j = 0;j != index.getw();++j){
			result.at(i) += ptn.at(index[i][j]);
		}
	}
	return result;
}

void adjust(pattern& ptn, const matrix<int>& index, const matrix<calc_type>& delta){
	for(int i = 0;i != index.geth();++i){
		for(int j = 0;j != index.getw();++j){
			ptn.at(index[i][j]) += delta.at(i);
		}
	}
}

void optimize(pattern& ptn, const matrix<int>& index, const matrix<float>& target, cint step){
	matrix<float> corr = matrix<float>(correlate(index,index));
	matrix<float> corr_2 = corr * corr;

	matrix<calc_type> value = evaluate(ptn,index);
	matrix<calc_type> epsilon = target - value;
	calc_type epsilon_2 = epsilon.modulus();
	cout << "epsilon_2 : " << epsilon_2 << endl;
	//delta_ptn = epsilon.transpose() * 2 * pattern
	calc_type alpha = (epsilon.transpose() * corr * epsilon).at(0) / (epsilon.transpose() * corr_2 * epsilon).at(0);
	adjust(ptn,index,- alpha * 2 * epsilon);

	value = evaluate(ptn,index);
	epsilon = target - value;
	epsilon_2 = epsilon.modulus();
	cout << "epsilon_2 : " << epsilon_2 << endl;
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
