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
	9, 9, 9, 9
};

const short pattern::table_num_size[pattern::size] = {
	4,4,4,4,2,4,4,4,2,4
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
	{32, 33, 34, 35}
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
	brd_type brd_v_blue = brd_blue;
	brd_type brd_v_green = brd_green;
	board::mirror_v(brd_v_blue);
	board::mirror_v(brd_v_green);
	brd_type brd_h_blue = brd_blue;
	brd_type brd_h_green = brd_green;
	board::mirror_h(brd_h_blue);
	board::mirror_h(brd_h_green);
	brd_type brd_r_blue = brd_h_blue;
	brd_type brd_r_green = brd_h_green;
	board::mirror_v(brd_r_blue);
	board::mirror_v(brd_r_green);
	brd_type index;

	//horizontal pattern
	index = 0 << 16;
	index |= board::extract(brd_blue,0x00000000000000ff) << 8;
	index |= board::extract(brd_green,0x00000000000000ff);
	ind[0] = index;

	index = 1 << 16;
	index |= board::extract(brd_blue,0x000000000000ff00) << 8;
	index |= board::extract(brd_green,0x000000000000ff00);
	ind[1] = index;

	index = 2 << 16;
	index |= board::extract(brd_blue,0x0000000000ff0000) << 8;
	index |= board::extract(brd_green,0x0000000000ff0000);
	ind[2] = index;

	index = 3 << 16;
	index |= board::extract(brd_blue,0x00000000ff000000) << 8;
	index |= board::extract(brd_green,0x00000000ff000000);
	ind[3] = index;

	index = 3 << 16;
	index |= board::extract(brd_blue,0x000000ff00000000) << 8;
	index |= board::extract(brd_green,0x000000ff00000000);
	ind[4] = index;

	index = 2 << 16;
	index |= board::extract(brd_blue,0x0000ff0000000000) << 8;
	index |= board::extract(brd_green,0x0000ff0000000000);
	ind[5] = index;

	index = 1 << 16;
	index |= board::extract(brd_blue,0x00ff000000000000) << 8;
	index |= board::extract(brd_green,0x00ff000000000000);
	ind[6] = index;

	index = 0 << 16;
	index |= board::extract(brd_blue,0xff00000000000000) << 8;
	index |= board::extract(brd_green,0xff00000000000000);
	ind[7] = index;

	//vertical pattern

	index = 0 << 16;
	index |= board::extract(brd_blue,0x0101010101010101) << 8;
	index |= board::extract(brd_green,0x0101010101010101);
	ind[8] = index;

	index = 1 << 16;
	index |= board::extract(brd_blue,0x0202020202020202) << 8;
	index |= board::extract(brd_green,0x0202020202020202);
	ind[9] = index;

	index = 2 << 16;
	index |= board::extract(brd_blue,0x0404040404040404) << 8;
	index |= board::extract(brd_green,0x0404040404040404);
	ind[10] = index;

	index = 3 << 16;
	index |= board::extract(brd_blue,0x0808080808080808) << 8;
	index |= board::extract(brd_green,0x0808080808080808);
	ind[11] = index;

	index = 3 << 16;
	index |= board::extract(brd_blue,0x1010101010101010) << 8;
	index |= board::extract(brd_green,0x1010101010101010);
	ind[12] = index;

	index = 2 << 16;
	index |= board::extract(brd_blue,0x2020202020202020) << 8;
	index |= board::extract(brd_green,0x2020202020202020);
	ind[13] = index;

	index = 1 << 16;
	index |= board::extract(brd_blue,0x4040404040404040) << 8;
	index |= board::extract(brd_green,0x4040404040404040);
	ind[14] = index;

	index = 0 << 16;
	index |= board::extract(brd_blue,0x8080808080808080) << 8;
	index |= board::extract(brd_green,0x8080808080808080);
	ind[15] = index;

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_blue,0x8040201008040201) << 8;
	index |= board::extract(brd_green,0x8040201008040201);
	ind[16] = index;

	index = 5 << 16;
	index |= board::extract(brd_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_green,0x4020100804020180);
	ind[17] = index;

	index = 6 << 16;
	index |= board::extract(brd_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_green,0x2010080402018040);
	ind[18] = index;

	index = 7 << 16;
	index |= board::extract(brd_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_green,0x1008040201804020);
	ind[19] = index;

	index = 8 << 16;
	index |= board::extract(brd_blue,0x0804020180402010) << 8;
	index |= board::extract(brd_green,0x0804020180402010);
	ind[20] = index;

	index = 7 << 16;
	index |= board::extract(brd_r_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_r_green,0x1008040201804020);
	ind[21] = index;

	index = 6 << 16;
	index |= board::extract(brd_r_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_r_green,0x2010080402018040);
	ind[22] = index;

	index = 5 << 16;
	index |= board::extract(brd_r_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_r_green,0x4020100804020180);
	ind[23] = index;

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_v_blue,0x8040201008040201) << 8;
	index |= board::extract(brd_v_green,0x8040201008040201);
	ind[24] = index;

	index = 5 << 16;
	index |= board::extract(brd_v_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_v_green,0x4020100804020180);
	ind[25] = index;

	index = 6 << 16;
	index |= board::extract(brd_v_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_v_green,0x2010080402018040);
	ind[26] = index;

	index = 7 << 16;
	index |= board::extract(brd_v_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_v_green,0x1008040201804020);
	ind[27] = index;

	index = 8 << 16;
	index |= board::extract(brd_v_blue,0x0804020180402010) << 8;
	index |= board::extract(brd_v_green,0x0804020180402010);
	ind[28] = index;

	index = 7 << 16;
	index |= board::extract(brd_h_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_h_green,0x1008040201804020);
	ind[29] = index;

	index = 6 << 16;
	index |= board::extract(brd_h_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_h_green,0x2010080402018040);
	ind[30] = index;

	index = 5 << 16;
	index |= board::extract(brd_h_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_h_green,0x4020100804020180);
	ind[31] = index;

	//corner pattern

	index = 9 << 16;
	index |= board::extract(brd_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_green,0x0000000000030707);
	ind[32] = index;

	index = 9 << 16;
	index |= board::extract(brd_h_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_h_green,0x0000000000030707);
	ind[33] = index;

	index = 9 << 16;
	index |= board::extract(brd_v_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_v_green,0x0000000000030707);
	ind[34] = index;

	index = 9 << 16;
	index |= board::extract(brd_r_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_r_green,0x0000000000030707);
	ind[35] = index;
}

float board::score_ptn(cbool color,cpattern ptn)const{
	brd_type brd_blue = bget(color);
	brd_type brd_green = bget(!color);
	brd_type brd_v_blue = brd_blue;
	brd_type brd_v_green = brd_green;
	mirror_v(brd_v_blue);
	mirror_v(brd_v_green);
	brd_type brd_h_blue = brd_blue;
	brd_type brd_h_green = brd_green;
	mirror_h(brd_h_blue);
	mirror_h(brd_h_green);
	brd_type brd_r_blue = brd_h_blue;
	brd_type brd_r_green = brd_h_green;
	mirror_v(brd_r_blue);
	mirror_v(brd_r_green);
	brd_type index;
	float result;

	result = 0;

	//horizontal pattern
	index = 0 << 16;
	index |= board::extract(brd_blue,0x00000000000000ff) << 8;
	index |= board::extract(brd_green,0x00000000000000ff);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue,0x000000000000ff00) << 8;
	index |= board::extract(brd_green,0x000000000000ff00);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue,0x0000000000ff0000) << 8;
	index |= board::extract(brd_green,0x0000000000ff0000);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue,0x00000000ff000000) << 8;
	index |= board::extract(brd_green,0x00000000ff000000);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue,0x000000ff00000000) << 8;
	index |= board::extract(brd_green,0x000000ff00000000);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue,0x0000ff0000000000) << 8;
	index |= board::extract(brd_green,0x0000ff0000000000);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue,0x00ff000000000000) << 8;
	index |= board::extract(brd_green,0x00ff000000000000);
	result += ptn.table[index];

	index = 0 << 16;
	index |= board::extract(brd_blue,0xff00000000000000) << 8;
	index |= board::extract(brd_green,0xff00000000000000);
	result += ptn.table[index];

	//vertical pattern

	index = 0 << 16;
	index |= board::extract(brd_blue,0x0101010101010101) << 8;
	index |= board::extract(brd_green,0x0101010101010101);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue,0x0202020202020202) << 8;
	index |= board::extract(brd_green,0x0202020202020202);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue,0x0404040404040404) << 8;
	index |= board::extract(brd_green,0x0404040404040404);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue,0x0808080808080808) << 8;
	index |= board::extract(brd_green,0x0808080808080808);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue,0x1010101010101010) << 8;
	index |= board::extract(brd_green,0x1010101010101010);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue,0x2020202020202020) << 8;
	index |= board::extract(brd_green,0x2020202020202020);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue,0x4040404040404040) << 8;
	index |= board::extract(brd_green,0x4040404040404040);
	result += ptn.table[index];

	index = 0 << 16;
	index |= board::extract(brd_blue,0x8080808080808080) << 8;
	index |= board::extract(brd_green,0x8080808080808080);
	result += ptn.table[index];

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_blue,0x8040201008040201) << 8;
	index |= board::extract(brd_green,0x8040201008040201);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_green,0x4020100804020180);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_green,0x2010080402018040);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_green,0x1008040201804020);
	result += ptn.table[index];

	index = 8 << 16;
	index |= board::extract(brd_blue,0x0804020180402010) << 8;
	index |= board::extract(brd_green,0x0804020180402010);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_r_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_r_green,0x1008040201804020);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_r_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_r_green,0x2010080402018040);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_r_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_r_green,0x4020100804020180);
	result += ptn.table[index];

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_v_blue,0x8040201008040201) << 8;
	index |= board::extract(brd_v_green,0x8040201008040201);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_v_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_v_green,0x4020100804020180);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_v_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_v_green,0x2010080402018040);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_v_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_v_green,0x1008040201804020);
	result += ptn.table[index];

	index = 8 << 16;
	index |= board::extract(brd_v_blue,0x0804020180402010) << 8;
	index |= board::extract(brd_v_green,0x0804020180402010);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_h_blue,0x1008040201804020) << 8;
	index |= board::extract(brd_h_green,0x1008040201804020);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_h_blue,0x2010080402018040) << 8;
	index |= board::extract(brd_h_green,0x2010080402018040);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_h_blue,0x4020100804020180) << 8;
	index |= board::extract(brd_h_green,0x4020100804020180);
	result += ptn.table[index];

	//corner pattern

	index = 9 << 16;
	index |= board::extract(brd_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_green,0x0000000000030707);
	result += ptn.table[index];

	index = 9 << 16;
	index |= board::extract(brd_h_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_h_green,0x0000000000030707);
	result += ptn.table[index];

	index = 9 << 16;
	index |= board::extract(brd_v_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_v_green,0x0000000000030707);
	result += ptn.table[index];

	index = 9 << 16;
	index |= board::extract(brd_r_blue,0x0000000000030707) << 8;
	index |= board::extract(brd_r_green,0x0000000000030707);
	result += ptn.table[index];

	return result;
}

void pattern::load(istream& fin){
	#define _READ(var) fin.read((char *)(&var),sizeof(var))

	for(int i = 0;i != size;++i){
		for(int j = 0;j != length_compress;++j){
			_READ(table[(i << 16) + table_map_inv[j]]);
		}
	}

	#undef _READ
}
void pattern::save(ostream& fout)const{
	#define WRITE(var) fout.write((const char *)(&var),sizeof(var))

	for(int i = 0;i != size;++i){
		for(int j = 0;j != length_compress;++j){
			WRITE(table[(i << 16) + table_map_inv[j]]);
		}
	}

	#undef WRITE
}

void group::load(istream& fin){
	#define _READ(var) fin.read((char *)(&var),sizeof(var))

	size_t calc_size, ptn_size, group_size;

	_READ(calc_size);
	_READ(ptn_size);
	_READ(group_size);

	if(calc_size != sizeof(calc_type)){
		cout << "Error: The size of element does not match." << endl;
		return;
	}
	if(ptn_size != sizeof(pattern)){
		cout << "Error: The size of pattern does not match." << endl;
		return;
	}
	
	this->vec.reserve(group_size);

	for(size_t i = 0;i != group_size;++i){
		this->vec.emplace_back();
		this->vec.back().load(fin);
	}

	#undef _READ
}

void group::load(const string& path){
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

	for(size_t i = 0;i != group_size;++i){
		this->vec.emplace_back();
		this->vec.back().load(fin);
	}

	fin.close();
	#undef _READ
}

void group::save(const string& path)const{
	#define WRITE(var) fout.write((const char *)(&var),sizeof(var))
	ofstream fout(path,ios::out | ios::binary);

	size_t calc_size = sizeof(calc_type);
	size_t ptn_size = sizeof(pattern);
	size_t group_size = vec.size();

	WRITE(calc_size);
	WRITE(ptn_size);
	WRITE(group_size);

	for(const pattern& ptn:this->vec){
		ptn.save(fout);
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

matrix<board> sample_2mat(const unordered_set<board>& brds){
	int i = 0;
	matrix<board> result(brds.size(),1);
	for(cboard brd:brds){
		result.at(i) = brd;
		++i;
	}
	return result;
}

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

matrix<calc_type> evaluate(const pattern& ptn, const matrix<board>& brds){
	matrix<calc_type> result(brds.geth(),1);
	
	for(int i = 0;i != brds.geth();++i){
		result.at(i) = brds.at(i).score_ptn(true,grp.at(0));
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
