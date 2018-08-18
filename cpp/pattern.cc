#include <vector>
#include <algorithm>
#include <cstring>

#include "board.h"
#include "pattern.h"

using namespace std;

pattern ptn;

int table_map[1 << 16];
int table_map_inv[6561];
int table_reverse[256];
int table_shuffle[256];

const ull pattern::table_mask[pattern::size_n] = {
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

val_type board::score_ptn(cbool color,const pattern& ptn)const{
	ull brd_blue = bget(color);
	ull brd_green = bget(!color);
	ull brd_v_blue = brd_blue;
	ull brd_v_green = brd_green;
	mirror_v(brd_v_blue);
	mirror_v(brd_v_green);
	ull brd_h_blue = brd_blue;
	ull brd_h_green = brd_green;
	mirror_h(brd_h_blue);
	mirror_h(brd_h_green);
	ull brd_r_blue = brd_h_blue;
	ull brd_r_green = brd_h_green;
	mirror_v(brd_r_blue);
	mirror_v(brd_r_green);
	ull index;
	val_type result;

	result = 0;

	//horizontal pattern
	index = 0 << 16;
	index |= board::extract(brd_blue, 0x00000000000000ff) << 8;
	index |= board::extract(brd_green, 0x00000000000000ff);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x000000000000ff00) << 8;
	index |= board::extract(brd_green, 0x000000000000ff00);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x0000000000ff0000) << 8;
	index |= board::extract(brd_green, 0x0000000000ff0000);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x00000000ff000000) << 8;
	index |= board::extract(brd_green, 0x00000000ff000000);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x000000ff00000000) << 8;
	index |= board::extract(brd_green, 0x000000ff00000000);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x0000ff0000000000) << 8;
	index |= board::extract(brd_green, 0x0000ff0000000000);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x00ff000000000000) << 8;
	index |= board::extract(brd_green, 0x00ff000000000000);
	result += ptn.table[index];

	index = 0 << 16;
	index |= board::extract(brd_blue, 0xff00000000000000) << 8;
	index |= board::extract(brd_green, 0xff00000000000000);
	result += ptn.table[index];

	//vertical pattern

	index = 0 << 16;
	index |= board::extract(brd_blue, 0x0101010101010101) << 8;
	index |= board::extract(brd_green, 0x0101010101010101);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x0202020202020202) << 8;
	index |= board::extract(brd_green, 0x0202020202020202);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x0404040404040404) << 8;
	index |= board::extract(brd_green, 0x0404040404040404);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x0808080808080808) << 8;
	index |= board::extract(brd_green, 0x0808080808080808);
	result += ptn.table[index];

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x1010101010101010) << 8;
	index |= board::extract(brd_green, 0x1010101010101010);
	result += ptn.table[index];

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x2020202020202020) << 8;
	index |= board::extract(brd_green, 0x2020202020202020);
	result += ptn.table[index];

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x4040404040404040) << 8;
	index |= board::extract(brd_green, 0x4040404040404040);
	result += ptn.table[index];

	index = 0 << 16;
	index |= board::extract(brd_blue, 0x8080808080808080) << 8;
	index |= board::extract(brd_green, 0x8080808080808080);
	result += ptn.table[index];

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_blue, 0x8040201008040201) << 8;
	index |= board::extract(brd_green, 0x8040201008040201);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_green, 0x4020100804020180);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_green, 0x2010080402018040);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_green, 0x1008040201804020);
	result += ptn.table[index];

	index = 8 << 16;
	index |= board::extract(brd_blue, 0x0804020180402010) << 8;
	index |= board::extract(brd_green, 0x0804020180402010);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_r_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_r_green, 0x1008040201804020);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_r_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_r_green, 0x2010080402018040);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_r_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_r_green, 0x4020100804020180);
	result += ptn.table[index];

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_v_blue, 0x8040201008040201) << 8;
	index |= board::extract(brd_v_green, 0x8040201008040201);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_v_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_v_green, 0x4020100804020180);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_v_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_v_green, 0x2010080402018040);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_v_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_v_green, 0x1008040201804020);
	result += ptn.table[index];

	index = 8 << 16;
	index |= board::extract(brd_v_blue, 0x0804020180402010) << 8;
	index |= board::extract(brd_v_green, 0x0804020180402010);
	result += ptn.table[index];

	index = 7 << 16;
	index |= board::extract(brd_h_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_h_green, 0x1008040201804020);
	result += ptn.table[index];

	index = 6 << 16;
	index |= board::extract(brd_h_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_h_green, 0x2010080402018040);
	result += ptn.table[index];

	index = 5 << 16;
	index |= board::extract(brd_h_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_h_green, 0x4020100804020180);
	result += ptn.table[index];

	//corner pattern

	index = 9 << 16;
	index |= board::extract(brd_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_green, 0x0000000000030707);
	result += ptn.table[index];

	index = 9 << 16;
	index |= board::extract(brd_h_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_h_green, 0x0000000000030707);
	result += ptn.table[index];

	index = 9 << 16;
	index |= board::extract(brd_v_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_v_green, 0x0000000000030707);
	result += ptn.table[index];

	index = 9 << 16;
	index |= board::extract(brd_r_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_r_green, 0x0000000000030707);
	result += ptn.table[index];

	return result;
}

void board::adjust_ptn(cbool color, pattern& ptn, cval_type value)const{
	ull brd_blue = bget(color);
	ull brd_green = bget(!color);
	ull brd_v_blue = brd_blue;
	ull brd_v_green = brd_green;
	mirror_v(brd_v_blue);
	mirror_v(brd_v_green);
	ull brd_h_blue = brd_blue;
	ull brd_h_green = brd_green;
	mirror_h(brd_h_blue);
	mirror_h(brd_h_green);
	ull brd_r_blue = brd_h_blue;
	ull brd_r_green = brd_h_green;
	mirror_v(brd_r_blue);
	mirror_v(brd_r_green);
	ull index;

	//horizontal pattern
	index = 0 << 16;
	index |= board::extract(brd_blue, 0x00000000000000ff) << 8;
	index |= board::extract(brd_green, 0x00000000000000ff);
	ptn.table[index] += value;

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x000000000000ff00) << 8;
	index |= board::extract(brd_green, 0x000000000000ff00);
	ptn.table[index] += value;

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x0000000000ff0000) << 8;
	index |= board::extract(brd_green, 0x0000000000ff0000);
	ptn.table[index] += value;

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x00000000ff000000) << 8;
	index |= board::extract(brd_green, 0x00000000ff000000);
	ptn.table[index] += value;

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x000000ff00000000) << 8;
	index |= board::extract(brd_green, 0x000000ff00000000);
	ptn.table[index] += value;

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x0000ff0000000000) << 8;
	index |= board::extract(brd_green, 0x0000ff0000000000);
	ptn.table[index] += value;

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x00ff000000000000) << 8;
	index |= board::extract(brd_green, 0x00ff000000000000);
	ptn.table[index] += value;

	index = 0 << 16;
	index |= board::extract(brd_blue, 0xff00000000000000) << 8;
	index |= board::extract(brd_green, 0xff00000000000000);
	ptn.table[index] += value;

	//vertical pattern

	index = 0 << 16;
	index |= board::extract(brd_blue, 0x0101010101010101) << 8;
	index |= board::extract(brd_green, 0x0101010101010101);
	ptn.table[index] += value;

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x0202020202020202) << 8;
	index |= board::extract(brd_green, 0x0202020202020202);
	ptn.table[index] += value;

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x0404040404040404) << 8;
	index |= board::extract(brd_green, 0x0404040404040404);
	ptn.table[index] += value;

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x0808080808080808) << 8;
	index |= board::extract(brd_green, 0x0808080808080808);
	ptn.table[index] += value;

	index = 3 << 16;
	index |= board::extract(brd_blue, 0x1010101010101010) << 8;
	index |= board::extract(brd_green, 0x1010101010101010);
	ptn.table[index] += value;

	index = 2 << 16;
	index |= board::extract(brd_blue, 0x2020202020202020) << 8;
	index |= board::extract(brd_green, 0x2020202020202020);
	ptn.table[index] += value;

	index = 1 << 16;
	index |= board::extract(brd_blue, 0x4040404040404040) << 8;
	index |= board::extract(brd_green, 0x4040404040404040);
	ptn.table[index] += value;

	index = 0 << 16;
	index |= board::extract(brd_blue, 0x8080808080808080) << 8;
	index |= board::extract(brd_green, 0x8080808080808080);
	ptn.table[index] += value;

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_blue, 0x8040201008040201) << 8;
	index |= board::extract(brd_green, 0x8040201008040201);
	ptn.table[index] += value;

	index = 5 << 16;
	index |= board::extract(brd_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_green, 0x4020100804020180);
	ptn.table[index] += value;

	index = 6 << 16;
	index |= board::extract(brd_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_green, 0x2010080402018040);
	ptn.table[index] += value;

	index = 7 << 16;
	index |= board::extract(brd_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_green, 0x1008040201804020);
	ptn.table[index] += value;

	index = 8 << 16;
	index |= board::extract(brd_blue, 0x0804020180402010) << 8;
	index |= board::extract(brd_green, 0x0804020180402010);
	ptn.table[index] += value;

	index = 7 << 16;
	index |= board::extract(brd_r_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_r_green, 0x1008040201804020);
	ptn.table[index] += value;

	index = 6 << 16;
	index |= board::extract(brd_r_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_r_green, 0x2010080402018040);
	ptn.table[index] += value;

	index = 5 << 16;
	index |= board::extract(brd_r_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_r_green, 0x4020100804020180);
	ptn.table[index] += value;

	//diagnal pattern

	index = 4 << 16;
	index |= board::extract(brd_v_blue, 0x8040201008040201) << 8;
	index |= board::extract(brd_v_green, 0x8040201008040201);
	ptn.table[index] += value;

	index = 5 << 16;
	index |= board::extract(brd_v_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_v_green, 0x4020100804020180);
	ptn.table[index] += value;

	index = 6 << 16;
	index |= board::extract(brd_v_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_v_green, 0x2010080402018040);
	ptn.table[index] += value;

	index = 7 << 16;
	index |= board::extract(brd_v_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_v_green, 0x1008040201804020);
	ptn.table[index] += value;

	index = 8 << 16;
	index |= board::extract(brd_v_blue, 0x0804020180402010) << 8;
	index |= board::extract(brd_v_green, 0x0804020180402010);
	ptn.table[index] += value;

	index = 7 << 16;
	index |= board::extract(brd_h_blue, 0x1008040201804020) << 8;
	index |= board::extract(brd_h_green, 0x1008040201804020);
	ptn.table[index] += value;

	index = 6 << 16;
	index |= board::extract(brd_h_blue, 0x2010080402018040) << 8;
	index |= board::extract(brd_h_green, 0x2010080402018040);
	ptn.table[index] += value;

	index = 5 << 16;
	index |= board::extract(brd_h_blue, 0x4020100804020180) << 8;
	index |= board::extract(brd_h_green, 0x4020100804020180);
	ptn.table[index] += value;

	//corner pattern

	index = 9 << 16;
	index |= board::extract(brd_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_green, 0x0000000000030707);
	ptn.table[index] += value;

	index = 9 << 16;
	index |= board::extract(brd_h_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_h_green, 0x0000000000030707);
	ptn.table[index] += value;

	index = 9 << 16;
	index |= board::extract(brd_v_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_v_green, 0x0000000000030707);
	ptn.table[index] += value;

	index = 9 << 16;
	index |= board::extract(brd_r_blue, 0x0000000000030707) << 8;
	index |= board::extract(brd_r_green, 0x0000000000030707);
	ptn.table[index] += value;
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

void pattern::load(const string& path){
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

	if(calc_size != sizeof(val_type)){
		fin.close();
		cout << "Error: The size of element does not match." << endl;
		return;
	}
	if(ptn_size != sizeof(pattern)){
		fin.close();
		cout << "Error: The size of pattern does not match." << endl;
		return;
	}
	
	this->load(fin);

	fin.close();
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

void pattern::save(const string& path)const{
	#define WRITE(var) fout.write((const char *)(&var),sizeof(var))
	ofstream fout(path,ios::out | ios::binary);

	size_t calc_size = sizeof(val_type);
	size_t ptn_size = sizeof(pattern);
	size_t group_size = 1;

	WRITE(calc_size);
	WRITE(ptn_size);
	WRITE(group_size);

	this->save(fout);
	
	fout.close();
	#undef WRITE
}

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wchar-subscripts"
#endif

void pattern::balance(){
	unsigned char index_l, index_h;
	int index;
	val_type value;

	#define balance_part(num) \
		value = (table[(num << 16) | i] + table[(num << 16) | index]) / 2; \
		table[(num << 16) | i] = value; \
		table[(num << 16) | index] = value; \

	for(int i = 0; i != length; ++i){
		if(!(i & (i >> 8))){
			index_l = i;
			index_l = table_reverse[index_l];
			index_h = i >> 8;
			index_h = table_reverse[index_h];
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(0);
			balance_part(1);
			balance_part(2);
			balance_part(3);
			balance_part(4);

			index_l = i;
			fun_ror(index_l,1);
			index_l = table_reverse[index_l];
			index_h = i >> 8;
			fun_ror(index_h,1);
			index_h = table_reverse[index_h];
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(5);

			index_l = i;
			fun_ror(index_l,2);
			index_l = table_reverse[index_l];
			index_h = i >> 8;
			fun_ror(index_h,2);
			index_h = table_reverse[index_h];
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(6);

			index_l = i;
			fun_ror(index_l,3);
			index_l = table_reverse[index_l];
			index_h = i >> 8;
			fun_ror(index_h,3);
			index_h = table_reverse[index_h];
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(7);

			index_l = i;
			fun_ror(index_l,4);
			index_l = table_reverse[index_l];
			index_h = i >> 8;
			fun_ror(index_h,4);
			index_h = table_reverse[index_h];
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(8);

			index_l = i;
			fun_ror(index_l,4);
			index_h = i >> 8;
			fun_ror(index_h,4);
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(8);

			index_l = i;
			index_l = table_shuffle[index_l];
			index_h = i >> 8;
			index_h = table_shuffle[index_h];
			index = index_h;
			index <<= 8;
			index |= index_l;

			balance_part(9);
		}
	}
}

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif

matrix<board> sample_gen(cint n){
	unordered_set<board> brds;
	board brd, brd_save;
	coordinate pos1, pos2;

	for(int i = 0;i != n;++i){
		brd.initial();
		do{
			brd_save = brd;
			pos1 = brd.play(mthd_rnd, true, 0);
			if(pos1.x >= 0){
				brds.insert(brd_save);
			}

			brd_save = brd;
			pos2 = brd.play(mthd_rnd, false, 0);
			if(pos2.x >= 0){
				brd_save.reverse();
				brds.insert(brd_save);
			}
		}while(pos1.x >= 0 || pos2.x >= 0);
	}

	int i = 0;
	matrix<board> result(brds.size(), 1);
	for(cboard brd:brds){
		result.at(i) = brd;
		++i;
	}
	return result;
};

matrix<board> sample_gen(cint n, matrix<int>& occurrence){
	unordered_map<board, int> brds;
	board brd, brd_save;
	coordinate pos1, pos2;

	for(int i = 0;i != n;++i){
		brd.initial();
		do{
			brd_save = brd;
			pos1 = brd.play(mthd_rnd, true, 0);
			if(pos1.x >= 0){
				++brds[brd_save];
			}

			brd_save = brd;
			pos2 = brd.play(mthd_rnd, false, 0);
			if(pos2.x >= 0){
				brd_save.reverse();
				++brds[brd_save];
			}
		}while(pos1.x >= 0 || pos2.x >= 0);
	}

	int i = 0;
	matrix<board> result(brds.size(), 1);
	occurrence.resize(brds.size(), 1);

	for(const auto& p: brds){
		result.at(i) = p.first;
		occurrence.at(i) = p.second;
		++i;
	}

	return result;
};

matrix<val_type> evaluate(const matrix<board>& brds, cmethod mthd, cshort height){
	matrix<val_type> result(brds.geth(), 1);
	for(int i = 0; i != brds.geth(); ++i){
		result.at(i) = brds.at(i).search(mthd, true, height);
	}
	return result;
}

matrix<val_type> evaluate(const pattern& ptn, const matrix<board>& brds){
	matrix<val_type> result(brds.geth(), 1);
	
	for(int i = 0; i != brds.geth(); ++i){
		result.at(i) = brds.at(i).score_ptn(true, ptn);
	}
	return result;
}

void adjust(pattern& ptn, const matrix<board>& brds, const matrix<val_type>& delta){
	for(int i = 0; i != brds.geth(); ++i){
		brds.at(i).adjust_ptn(true, ptn, delta.at(i));
	}
}
