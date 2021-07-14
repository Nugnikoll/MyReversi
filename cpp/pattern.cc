#include <vector>
#include <algorithm>
#include <cstring>

#include "board.h"
#include "pattern.h"

using namespace std;

constexpr ull cpow(ull n, ull val){
	return val ? cpow(n * n, val >> 1) * (val & 1 ? n : 1) : 1;
}

constexpr ull cpopcnt(ull brd){
	return brd ? cpopcnt(brd >> 1) + (brd & 1) : 0;
}

#define next_bias(id) (bias_##id + (1 << (shift_##id * 2)))
#define create_first_ptn(id, mask) \
	const ull ptn_##id = mask; \
	const ull shift_##id = cpopcnt(mask); \
	const ull bias_##id = 0;
#define create_ptn(id, mask, id_prev) \
	const ull ptn_##id = mask; \
	const ull shift_##id = cpopcnt(mask); \
	const ull bias_##id = next_bias(id_prev);

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

	if(file_ptn.size()){
		ptn.load(file_ptn);
	}
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

void pattern::load(const string& path){
	#define _READ(var) fin.read((char *)(&var), sizeof(var))

	ifstream fin(path,ios::in | ios::binary);
	ull calc_size, ptn_size, group_size;

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

	fin.close();

	#undef _READ
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
