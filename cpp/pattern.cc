#include <vector>
#include <algorithm>
#include <cstring>

#include "board.h"
#include "pattern.h"

#ifdef _BOTZONE_ONLINE
	#include "jsoncpp/json.h"
#else
	#include "../cpp/jsoncpp/json.h"
#endif

using namespace std;

constexpr const int ptn_cnt = 11;
constexpr const char ptn_name[ptn_cnt][3] = {
	"a1", "a2", "a3", "a4",
	"b1", "b2", "b3", "b4", "b5",
	"c1", "c2",
};
constexpr const ull ptn_list[ptn_cnt] = {
	0x00000000000042ff,
	0x000000000000ff00,
	0x0000000000ff0000,
	0x00000000ff000000,
	0x0000000001020408,
	0x0000000102040810,
	0x0000010204081020,
	0x0001020408102040,
	0x0102040810204080,
	0x0000000000070707,
	0x000000010101031f,
};

vector<string> pattern::get_name_list(){
	vector<string> result;
	for(auto i: ptn_name){
		result.emplace_back(string(i));
	}
	return result;
}
vector<ull> pattern::get_ptn_list(){
	vector<ull> result;
	for(auto i: ptn_list){
		result.emplace_back(i);
	}
	return result;
}

constexpr ull cpow(ull n, ull val){
	return val ? cpow(n * n, val >> 1) * (val & 1 ? n : 1) : 1;
}

constexpr ull cpopcnt(ull brd){
	return brd ? cpopcnt(brd >> 1) + (brd & 1) : 0;
}

constexpr ull cmirror_h(ull brd){
	brd = (brd & 0xaaaaaaaaaaaaaaaa) >> 1  | (brd & 0x5555555555555555) << 1;
	brd = (brd & 0xcccccccccccccccc) >> 2  | (brd & 0x3333333333333333) << 2;
	brd = (brd & 0xf0f0f0f0f0f0f0f0) >> 4  | (brd & 0x0f0f0f0f0f0f0f0f) << 4;
	return brd;
}

constexpr ull cmirror_v(ull brd){
	brd = (brd & 0xff00ff00ff00ff00) >> 8  | (brd & 0x00ff00ff00ff00ff) << 8;
	brd = (brd & 0xffff0000ffff0000) >> 16 | (brd & 0x0000ffff0000ffff) << 16;
	brd = (brd & 0xffffffff00000000) >> 32 | (brd & 0x00000000ffffffff) << 32;
	return brd;
}

constexpr ull creflect(ull brd){
	brd = cmirror_h(brd);
	brd = cmirror_v(brd);
	return brd;
}

constexpr ull crotate_r(ull brd){
	brd = (brd & 0xf0f0f0f000000000) >> 4  | (brd & 0x0f0f0f0f00000000) >> 32
		| (brd & 0x00000000f0f0f0f0) << 32 | (brd & 0x000000000f0f0f0f) << 4;
	brd = (brd & 0xcccc0000cccc0000) >> 2  | (brd & 0x3333000033330000) >> 16
		| (brd & 0x0000cccc0000cccc) << 16 | (brd & 0x0000333300003333) << 2;
	brd = (brd & 0xaa00aa00aa00aa00) >> 1  | (brd & 0x5500550055005500) >> 8
		| (brd & 0x00aa00aa00aa00aa) << 8  | (brd & 0x0055005500550055) << 1;
	return brd;
}

#define create_first_ptn(id, mask) \
	const ull ptn_##id = mask; \
	const ull shift_##id = cpopcnt(mask); \
	const ull bias_##id = 0; \
	const ull cbias_##id = 0;
#define create_ptn(id, mask, id_prev) \
	const ull ptn_##id = mask; \
	const ull shift_##id = cpopcnt(mask); \
	const ull bias_##id = bias_##id_prev + (1 << (shift_##id_prev * 2)); \
	const ull cbias_##id = cbias_##id_prev + cpow(3, shift_##id_prev);

/*
# # # # # # # #

. # . . . . # .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_first_ptn(a1, 0x00000000000042ff);
create_first_ptn(a1_v, 0x0103010101010301);

/*
. . . . . . . .

# # # # # # # #

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(a2, 0x000000000000ff00, a1);
create_ptn(a2_v, 0x0202020202020202, a1);

/*
. . . . . . . .

. . . . . . . .

# # # # # # # #

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(a3, 0x0000000000ff0000, a2);
create_ptn(a3_v, 0x0404040404040404, a2);

/*
. . . . . . . .

. . . . . . . .

. . . . . . . .

# # # # # # # #

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(a4, 0x00000000ff000000, a3);
create_ptn(a4_v, 0x0808080808080808, a3);

/*
. . . # . . . .

. . # . . . . .

. # . . . . . .

# . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(b1, 0x0000000001020408, a4);

/*
. . . . # . . .

. . . # . . . .

. . # . . . . .

. # . . . . . .

# . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(b2, 0x0000000102040810, b1);

/*
. . . . . # . .

. . . . # . . .

. . . # . . . .

. . # . . . . .

. # . . . . . .

# . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(b3, 0x0000010204081020, b2);

/*
. . . . . . # .

. . . . . # . .

. . . . # . . .

. . . # . . . .

. . # . . . . .

. # . . . . . .

# . . . . . . .

. . . . . . . .
*/
create_ptn(b4, 0x0001020408102040, b3);

/*
. . . . . . . #

. . . . . . # .

. . . . . # . .

. . . . # . . .

. . . # . . . .

. . # . . . . .

. # . . . . . .

# . . . . . . .
*/
create_ptn(b5, 0x0102040810204080, b4);

/*
# # # . . . . .

# # # . . . . .

# # # . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(c1, 0x0000000000070707, b5);

/*
# # # # # . . .

# # . . . . . .

# . . . . . . .

# . . . . . . .

# . . . . . . .

. . . . . . . .

. . . . . . . .

. . . . . . . .
*/
create_ptn(c2, 0x000000010101031f, c1);

//const int pattern::size = bias_c2 + (1 << (shift_c2 * 2));
const int pattern::csize = cbias_c2 + cpow(3, shift_c2);

pattern ptn;

int table_map[1 << 16];
int table_map_inv[6561];
int table_reverse_2[1 << 2];
int table_reverse_3[1 << 3];
int table_reverse_4[1 << 4];
int table_reverse_5[1 << 5];
int table_reverse_6[1 << 6];
int table_reverse_7[1 << 7];
int table_reverse_8[1 << 8];
int table_reverse_a1[1 << 10];
int* const table_reverse_a2 = table_reverse_8;
int* const table_reverse_a3 = table_reverse_8;
int* const table_reverse_a4 = table_reverse_8;
int* const table_reverse_b1 = table_reverse_4;
int* const table_reverse_b2 = table_reverse_5;
int* const table_reverse_b3 = table_reverse_6;
int* const table_reverse_b4 = table_reverse_7;
int* const table_reverse_b5 = table_reverse_8;
int table_reverse_c1[1 << 9];
int table_reverse_c2[1 << 10];
int table_rotate_a1[1 << 10];

void pattern::config(const string& file_ptn){
	ull j = 0;
	for(ull i = 0; i != size; ++i){
		if((i & (i >> 8)) == 0){
			table_map[i] = j;
			table_map_inv[j] = i;
			++j;
		}else{
			table_map[i] = -1;
		}
	}

	for(ull i = 0; i != 4; ++i){
		table_reverse_2[i] = (i & 0x1) << 1 | (i & 0x2) >> 1;
	}
	for(ull i = 0; i != 8; ++i){
		table_reverse_3[i] = table_reverse_2[i & 0x3] << 1 | (i & 0x4) >> 2;
	}
	for(ull i = 0; i != 16; ++i){
		table_reverse_4[i] = table_reverse_3[i & 0x7] << 1 | (i & 0x8) >> 3;
	}
	for(ull i = 0; i != 32; ++i){
		table_reverse_5[i] = table_reverse_4[i & 0xf] << 1 | (i & 0x10) >> 4;
	}
	for(ull i = 0; i != 64; ++i){
		table_reverse_6[i] = table_reverse_5[i & 0x1f] << 1 | (i & 0x20) >> 5;
	}
	for(ull i = 0; i != 128; ++i){
		table_reverse_7[i] = table_reverse_6[i & 0x3f] << 1 | (i & 0x40) >> 6;
	}
	for(ull i = 0; i != 256; ++i){
		table_reverse_8[i] = table_reverse_7[i & 0x7f] << 1 | (i & 0x80) >> 7;
	}
	for(ull i = 0; i != 1024; ++i){
		table_reverse_a1[i] = table_reverse_2[(i & 0x300) >> 8] << 8 | table_reverse_8[i & 0xff];
	}
	for(ull i = 0; i != 512; ++i){
		j = board::deposit(i, ptn_c1);
		board::rotate_l(j);
		board::mirror_v(j);
		table_reverse_c1[i] = board::extract(j, ptn_c1);
	}
	for(ull i = 0; i != 1024; ++i){
		j = board::deposit(i, ptn_c2);
		board::rotate_l(j);
		board::mirror_v(j);
		table_reverse_c2[i] = board::extract(j, ptn_c2);
	}
	for(ull i = 0; i != 1024; ++i){
		j = board::deposit(i, ptn_a1_v);
		board::rotate_r(j);
		table_rotate_a1[i] = board::extract(j, ptn_a1);
	}

	if(!file_ptn.size()){
		return;
	}

	ifstream fin(file_ptn, ios::in | ios::binary);
	if(!fin){
		cout << "Error: Cannot open the file: " << file_ptn << " ." << endl;
		return;
	}
	char *buffer;
	buffer = new char[5];
	fin.read(buffer, 4);
	if(strncmp(buffer, "data", 4)){
		cout << "Error: unrecognized file format" << endl;
		fin.close();
		return;
	}
	fin.read(buffer, 4);
	if(!strncmp(buffer, "json", 4)){
	}
	//else if(!strncmp(buffer, "bson", 4)){} // BSON is not supported by Botzone
	else{
		cout << "Error: unrecognized file format" << endl;
		fin.close();
		return;
	}
	delete buffer;

	int length;
	fin.read((char *)&length, sizeof(length));
	buffer = new char[length + 1];
	fin.read(buffer, length);
	buffer[length] = 0;

	Json::Reader reader;
	Json::Value input, result;
	Json::FastWriter writer;
	reader.parse(buffer, input);

	auto append_list = input["append"];
	for(const auto append_item: append_list){
		string name = append_item["name"].asString();
		string type = append_item["type"].asString();

		if(type == "pattern"){
			if(name == "pattern"){
				ptn.load(fin);
			}else{
				fin.ignore(size);
			}
		}else{
			cout << "Error: unrecognized data type" << endl;
			delete buffer;
			fin.close();
			return;
		}
	}
	delete buffer;
	fin.close();
}

val_type board::score_ptn(cbool color, const pattern& ptn)const{
	ull brd_blue = get_brd(color);
	ull brd_green = get_brd(!color);
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

	#define extract_pattern(id, brd) \
		index = bias_##id; \
		index += board::extract(brd##_blue, ptn_##id) << shift_##id; \
		index += board::extract(brd##_green, ptn_##id); \
		result += ptn.table[index];

	//horizontal pattern
	extract_pattern(a1, brd);
	extract_pattern(a2, brd);
	extract_pattern(a3, brd);
	extract_pattern(a4, brd);
	extract_pattern(a1, brd_v);
	extract_pattern(a2, brd_v);
	extract_pattern(a3, brd_v);
	extract_pattern(a4, brd_v);

	//vertical pattern
	index = bias_a1_v;
	index += table_rotate_a1[board::extract(brd_blue, ptn_a1_v)] << 10;
	index += table_rotate_a1[board::extract(brd_green, ptn_a1_v)];
	result += ptn.table[index];
	extract_pattern(a2_v, brd);
	extract_pattern(a3_v, brd);
	extract_pattern(a4_v, brd);
	index = bias_a1_v;
	index += table_rotate_a1[board::extract(brd_h_blue, ptn_a1_v)] << 10;
	index += table_rotate_a1[board::extract(brd_h_green, ptn_a1_v)];
	result += ptn.table[index];
	extract_pattern(a2_v, brd_h);
	extract_pattern(a3_v, brd_h);
	extract_pattern(a4_v, brd_h);

	//diagnal pattern
	extract_pattern(b1, brd);
	extract_pattern(b2, brd);
	extract_pattern(b3, brd);
	extract_pattern(b4, brd);
	extract_pattern(b5, brd);
	extract_pattern(b1, brd_r);
	extract_pattern(b2, brd_r);
	extract_pattern(b3, brd_r);
	extract_pattern(b4, brd_r);

	//diagnal pattern
	extract_pattern(b1, brd_v);
	extract_pattern(b2, brd_v);
	extract_pattern(b3, brd_v);
	extract_pattern(b4, brd_v);
	extract_pattern(b5, brd_v);
	extract_pattern(b1, brd_h);
	extract_pattern(b2, brd_h);
	extract_pattern(b3, brd_h);
	extract_pattern(b4, brd_h);

	//corner pattern
	extract_pattern(c1, brd);
	extract_pattern(c1, brd_v);
	extract_pattern(c1, brd_h);
	extract_pattern(c1, brd_r);

	extract_pattern(c2, brd);
	extract_pattern(c2, brd_v);
	extract_pattern(c2, brd_h);
	extract_pattern(c2, brd_r);

	#undef extract_pattern

	return result;
}

void board::adjust_ptn(cbool color, pattern& ptn, cval_type value)const{
	ull brd_blue = get_brd(color);
	ull brd_green = get_brd(!color);
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

	#define extract_pattern(id, brd) \
		index = bias_##id; \
		index += board::extract(brd##_blue, ptn_##id) << shift_##id; \
		index += board::extract(brd##_green, ptn_##id); \
		ptn.table[index] += value;

	//horizontal pattern
	extract_pattern(a1, brd);
	extract_pattern(a2, brd);
	extract_pattern(a3, brd);
	extract_pattern(a4, brd);
	extract_pattern(a1, brd_v);
	extract_pattern(a2, brd_v);
	extract_pattern(a3, brd_v);
	extract_pattern(a4, brd_v);

	//vertical pattern
	index = bias_a1_v;
	index += table_rotate_a1[board::extract(brd_blue, ptn_a1_v)] << 10;
	index += table_rotate_a1[board::extract(brd_green, ptn_a1_v)];
	ptn.table[index] += value;
	extract_pattern(a2_v, brd);
	extract_pattern(a3_v, brd);
	extract_pattern(a4_v, brd);
	index = bias_a1_v;
	index += table_rotate_a1[board::extract(brd_h_blue, ptn_a1_v)] << 10;
	index += table_rotate_a1[board::extract(brd_h_green, ptn_a1_v)];
	ptn.table[index] += value;
	extract_pattern(a2_v, brd_h);
	extract_pattern(a3_v, brd_h);
	extract_pattern(a4_v, brd_h);

	//diagnal pattern
	extract_pattern(b1, brd);
	extract_pattern(b2, brd);
	extract_pattern(b3, brd);
	extract_pattern(b4, brd);
	extract_pattern(b5, brd);
	extract_pattern(b1, brd_r);
	extract_pattern(b2, brd_r);
	extract_pattern(b3, brd_r);
	extract_pattern(b4, brd_r);

	//diagnal pattern
	extract_pattern(b1, brd_v);
	extract_pattern(b2, brd_v);
	extract_pattern(b3, brd_v);
	extract_pattern(b4, brd_v);
	extract_pattern(b5, brd_v);
	extract_pattern(b1, brd_h);
	extract_pattern(b2, brd_h);
	extract_pattern(b3, brd_h);
	extract_pattern(b4, brd_h);

	//corner pattern
	extract_pattern(c1, brd);
	extract_pattern(c1, brd_v);
	extract_pattern(c1, brd_h);
	extract_pattern(c1, brd_r);

	extract_pattern(c2, brd);
	extract_pattern(c2, brd_v);
	extract_pattern(c2, brd_h);
	extract_pattern(c2, brd_r);

	#undef extract_pattern
}

void pattern::from_compact(ARRAY_1D_IN_I(VAL_TYPE)){
	assert(i1 == csize);
	
	#define extract_pattern(id) \
		for(int i = 0; i != 1 << (shift_##id << 1); ++i){ \
			if(!(i >> shift_##id & i)){ \
				 table[i + bias_##id] = ptri[j]; \
				++j; \
			} \
		}
	#define debug_print(id) cout << #id << " " << j << endl;
	
	int j = 0;
	extract_pattern(a1);
	extract_pattern(a2);
	extract_pattern(a3);
	extract_pattern(a4);
	extract_pattern(b1);
	extract_pattern(b2);
	extract_pattern(b3);
	extract_pattern(b4);
	extract_pattern(b5);
	extract_pattern(c1);
	extract_pattern(c2);

	#undef extract_pattern
}

void pattern::compact(ARRAY_1D_OUT_M(VAL_TYPE))const{
	*m1 = csize;
	*ptrm = new val_type[csize];
	
	#define extract_pattern(id) \
		for(int i = 0; i != 1 << (shift_##id << 1); ++i){ \
			if(!(i >> shift_##id & i)){ \
				(*ptrm)[j] = table[i + bias_##id]; \
				++j; \
			} \
		}
	
	int j = 0;
	extract_pattern(a1);
	extract_pattern(a2);
	extract_pattern(a3);
	extract_pattern(a4);
	extract_pattern(b1);
	extract_pattern(b2);
	extract_pattern(b3);
	extract_pattern(b4);
	extract_pattern(b5);
	extract_pattern(c1);
	extract_pattern(c2);

	#undef extract_pattern
}

void pattern::load(istream& fin){
	#define _READ(var) fin.read((char *)(&var), sizeof(var))
	#define read_pattern(id) \
		for(int i = 0; i != 1 << (shift_##id << 1); ++i){ \
			if(!(i >> shift_##id & i)){ \
				_READ(table[i + bias_##id]); \
			} \
		}

	read_pattern(a1);
	read_pattern(a2);
	read_pattern(a3);
	read_pattern(a4);
	read_pattern(b1);
	read_pattern(b2);
	read_pattern(b3);
	read_pattern(b4);
	read_pattern(b5);
	read_pattern(c1);
	read_pattern(c2);

	#undef _READ
}

void pattern::load(const string& path){
	
	ull calc_size, ptn_size, group_size;

	ifstream fin(path,ios::in | ios::binary);

	#define _READ(var) fin.read((char *)(&var), sizeof(var))
	_READ(calc_size);
	_READ(ptn_size);
	_READ(group_size);
	#undef _READ

	if(calc_size != sizeof(val_type)){
		cout << "Error: The size of element does not match." << endl;
		return;
	}
	if(ptn_size != sizeof(pattern)){
		cout << "Error: The size of pattern does not match." << endl;
		return;
	}
	
	if(!fin){
		fin.close();
		cout << "Error: Cannot open the file: " << path << " ." << endl;
		return;
	}

	this->load(fin);

	fin.close();
}

void pattern::save(const string& path)const{
	#define WRITE(var) fout.write((const char *)(&var), sizeof(var))

	ofstream fout(path,ios::out | ios::binary);

	ull calc_size = sizeof(val_type);
	ull ptn_size = sizeof(pattern);
	ull group_size = 1;

	WRITE(calc_size);
	WRITE(ptn_size);
	WRITE(group_size);

	#define write_pattern(id) \
		for(int i = 0; i != 1 << (shift_##id << 1); ++i){ \
			if(!(i >> shift_##id & i)){ \
				WRITE(table[i + bias_##id]); \
			} \
		}

	write_pattern(a1);
	write_pattern(a2);
	write_pattern(a3);
	write_pattern(a4);
	write_pattern(b1);
	write_pattern(b2);
	write_pattern(b3);
	write_pattern(b4);
	write_pattern(b5);
	write_pattern(c1);
	write_pattern(c2);

	fout.close();

	#undef WRITE
}

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wchar-subscripts"
#endif

void pattern::balance(){
	val_type value;
	int index;

	#define balance_pattern(id) \
		for(int i = 0; i != 1 << (shift_##id << 1); ++i){ \
			index = (table_reverse_##id[i >> shift_##id] << shift_##id) | table_reverse_##id[i & ((1 << shift_##id) - 1)]; \
			value = (table[i + bias_##id] + table[index + bias_##id]) / 2; \
			table[i + bias_##id] = value; \
			table[index + bias_##id] = value; \
		}

	balance_pattern(a1);
	balance_pattern(a2);
	balance_pattern(a3);
	balance_pattern(a4);
	balance_pattern(b1);
	balance_pattern(b2);
	balance_pattern(b3);
	balance_pattern(b4);
	balance_pattern(b5);
	balance_pattern(c1);
	balance_pattern(c2);
}

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif

void evaluate(ARRAY_1D_OUT_M(VAL_TYPE), ARRAY_2D_IN_I(ULL), cmethod mthd, cshort depth, cbool flag_verbose){
	*m1 = i1;
	*ptrm = new val_type[i1];
	for(int i = 0; i != i1; ++i){
		board* ptr_brd = (board*)(ptri + i * 2);
		auto p_mthd = ptr_brd->process_method(mthd, depth);
		//ptr_brd->print();
		//cout << ptr_brd->sum() << " " << int(p_mthd.first) << " " << int(p_mthd.second) << endl;
		(*ptrm)[i] = ptr_brd->search(p_mthd.first, true, p_mthd.second);
		if(flag_verbose && i % 100 == 0){
			cout << "\rfinish " << i << "/" << i1;
		}
	}
	if(flag_verbose){
		cout << endl;
	}
}

void evaluate(ARRAY_1D_OUT_M(VAL_TYPE), const pattern& ptn, ARRAY_2D_IN_I(ULL)){
	*m1 = i1;
	*ptrm = new val_type[i1];
	for(int i = 0; i != i1; ++i){
		(*ptrm)[i] = ((board*)(ptri + i * 2))->score_ptn(true, ptn);
	}
}

void adjust(pattern& ptn, ARRAY_2D_IN_I(ULL), ARRAY_1D_IN_J(VAL_TYPE)){
	assert(j1 == pattern::size);
	for(int i = 0; i != i1; ++i){
		((board*)(ptri + i * 2))->adjust_ptn(true, ptn, ptrj[i]);
	}
}
