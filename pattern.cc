#include <vector>
#include <algorithm>
#include <cstring>

#include "reversi.h" //--
#include "pattern.h" //--

using namespace std;

pattern* ptr_pattern = NULL;

void pattern::initial(){
	static const int limit = 1 << 16;
	#define initial_set(num,v1,v2,v3,v4,v5,v6,v7,v8) \
		for(int i = 0;i != limit;++i){ \
			if(i & 0x8000){ \
				table[num][i] += v1; \
			} \
			if(i & 0x4000){ \
				table[num][i] += v2; \
			} \
			if(i & 0x2000){ \
				table[num][i] += v3; \
			} \
			if(i & 0x1000){ \
				table[num][i] += v4; \
			} \
			if(i & 0x0800){ \
				table[num][i] += v5; \
			} \
			if(i & 0x0400){ \
				table[num][i] += v6; \
			} \
			if(i & 0x0200){ \
				table[num][i] += v7; \
			} \
			if(i & 0x0100){ \
				table[num][i] += v8; \
			} \
			if(i & 0x0080){ \
				table[num][i] -= v1; \
			} \
			if(i & 0x0040){ \
				table[num][i] -= v2; \
			} \
			if(i & 0x0020){ \
				table[num][i] -= v3; \
			} \
			if(i & 0x0010){ \
				table[num][i] -= v4; \
			} \
			if(i & 0x0008){ \
				table[num][i] -= v5; \
			} \
			if(i & 0x0004){ \
				table[num][i] -= v6; \
			} \
			if(i & 0x0002){ \
				table[num][i] -= v7; \
			} \
			if(i & 0x0001){ \
				table[num][i] -= v8; \
			} \
		}

	initial_set(0,6,1,1,1,1,1,1,6);
	initial_set(1,1,-2,-0.5,-0.5,-0.5,-0.5,-2,1);
	initial_set(2,1,-1,-0.25,-0.25,-0.25,-0.25,-1,1);
	initial_set(3,1,-1,-0.25,-0.25,-0.25,-0.25,-1,1);

	for(int i = 0;i != limit;++i){
		if((i & 0xc000) == 0x4000){
			table[0][i] -= 6;
		}
		if((i & 0xc000) == 0xc000){
			table[0][i] += 6;
		}
		if((i & 0x0003) == 0x0002){
			table[0][i] -= 6;
		}
		if((i & 0x0003) == 0x0003){
			table[0][i] += 6;
		}
	}
}

template float board::score_ptn<true>()const;
template float board::score_ptn<false>()const;
template<bool color>
float board::score_ptn()const{

	short blue_move = this->count_move<true>();
	short green_move = this->count_move<false>();

	if((blue_move | green_move) == 0){
		short num_diff = count<color>() - count<!color>();
		num_diff <<= 1;
		if(num_diff){
			if(num_diff > 0){
				return num_diff + 1000;
			}else if(num_diff < 0){
				return num_diff - 1000;
			}else{
				return num_diff;
			}
		}else{
			return 0;
		}
	}

	unsigned short index;
	const brd_type& brd_blue = this->bget<color>();
	const brd_type& brd_green = this->bget<!color>();
	float result = 0;

	#define extract(mask,shift1,shift2,num) \
		index = (brd_blue & mask) shift1; \
		index |= (brd_green & mask) shift2; \
		result += ptr_pattern->table[num][index];

	//horizontal pattern
	extract(0xff,<<8,,0);
	extract(0xff00,,>>8,1);
	extract(0xff0000,>>8,>>16,2);
	extract(0xff000000,>>16,>>24,3);
	extract(0xff00000000,>>24,>>32,3);
	extract(0xff0000000000,>>32,>>40,2);
	extract(0xff000000000000,>>40,>>48,1);
	extract(0xff00000000000000,>>48,>>56,0);

	#undef extract

	#define extract(mask,factor,num) \
		index = (((brd_blue & mask) * factor) & 0xff00000000000000) >> 48; \
		index |= (((brd_green & mask) * factor) & 0xff00000000000000) >> 56; \
		result += ptr_pattern->table[num][index];

	//vertical pattern
	extract(0x0101010101010101,0x8040201008040201,0);
	extract(0x0202020202020202,0x0081020408102040,1);
	extract(0x0404040404040404,0x0040810204081020,2);
	extract(0x0808080808080808,0x0020408102040810,3);
	extract(0x1010101010101010,0x0010204081020408,3);
	extract(0x2020202020202020,0x0008102040810204,2);
	extract(0x4040404040404040,0x0004081020408102,1);
	extract(0x8080808080808080,0x0002040810204081,0);

	//diagnal pattern
	extract(0x8040201008040201,0x0101010101010101,4);
	extract(0x4020100804020180,0x0202020202020202,5);
	extract(0x2010080402018040,0x0404040404040404,6);
	extract(0x1008040201804020,0x0808080808080808,7);
	extract(0x0804020180402010,0x0101010101010101,8);
	extract(0x0402018040201008,0x0101010101010101,7);
	extract(0x0201804020100804,0x0101010101010101,6);
	extract(0x0180402010080402,0x0101010101010101,5);

	extract(0x0102040810204080,0x0101010101010101,4);
	extract(0x0204081020408001,0x0808080808080808,5);
	extract(0x0408102040800102,0x1010101010101010,6);
	extract(0x0810204080010204,0x2020202020202020,7);
	extract(0x1020408001020408,0x4040404040404040,8);
	extract(0x2040800102040810,0x0101010101010101,7);
	extract(0x4080010204081020,0x0101010101010101,6);
	extract(0x8001020408102040,0x0101010101010101,5);

	//corner pattern
	extract(0xe0e0c00000000000,0x0000000000002009,9);
	extract(0x0707030000000000,0x0000000000010420,9);
	extract(0x0000000000c0e0e0,0x0100082000000000,10);
	extract(0x0000000000030707,0x2004010000000000,10);

	#undef extract

	return result + blue_move;
}

float fdecay = 1 - 0.01;

template void board::adjust_ptn<true>(float diff)const;
template void board::adjust_ptn<false>(float diff)const;
template<bool color>
void board::adjust_ptn(float diff)const{

	unsigned short index;
	const brd_type& brd_blue = this->bget<color>();
	const brd_type& brd_green = this->bget<!color>();

	diff *= (0.01 / 36);

	#define diffuse(mask,shift1,shift2,num) \
		index = (brd_blue & mask) shift1; \
		index |= (brd_green & mask) shift2; \
		(ptr_pattern->table[num][index] += diff) *= fdecay;

	//horizontal pattern
	diffuse(0xff,<<8,,0);
	diffuse(0xff00,,>>8,1);
	diffuse(0xff0000,>>8,>>16,2);
	diffuse(0xff000000,>>16,>>24,3);
	diffuse(0xff00000000,>>24,>>32,3);
	diffuse(0xff0000000000,>>32,>>40,2);
	diffuse(0xff000000000000,>>40,>>48,1);
	diffuse(0xff00000000000000,>>48,>>56,0);

	#undef diffuse

	#define diffuse(mask,factor,num) \
		index = (((brd_blue & mask) * factor) & 0xff00000000000000) >> 48; \
		index |= (((brd_green & mask) * factor) & 0xff00000000000000) >> 56; \
		(ptr_pattern->table[num][index] += diff) *= fdecay;

	//vertical pattern
	diffuse(0x0101010101010101,0x8040201008040201,0);
	diffuse(0x0202020202020202,0x0081020408102040,1);
	diffuse(0x0404040404040404,0x0040810204081020,2);
	diffuse(0x0808080808080808,0x0020408102040810,3);
	diffuse(0x1010101010101010,0x0010204081020408,3);
	diffuse(0x2020202020202020,0x0008102040810204,2);
	diffuse(0x4040404040404040,0x0004081020408102,1);
	diffuse(0x8080808080808080,0x0002040810204081,0);

	//diagnal pattern
	diffuse(0x8040201008040201,0x0101010101010101,4);
	diffuse(0x4020100804020180,0x0202020202020202,5);
	diffuse(0x2010080402018040,0x0404040404040404,6);
	diffuse(0x1008040201804020,0x0808080808080808,7);
	diffuse(0x0804020180402010,0x0101010101010101,8);
	diffuse(0x0402018040201008,0x0101010101010101,7);
	diffuse(0x0201804020100804,0x0101010101010101,6);
	diffuse(0x0180402010080402,0x0101010101010101,5);

	diffuse(0x0102040810204080,0x0101010101010101,4);
	diffuse(0x0204081020408001,0x0808080808080808,5);
	diffuse(0x0408102040800102,0x1010101010101010,6);
	diffuse(0x0810204080010204,0x2020202020202020,7);
	diffuse(0x1020408001020408,0x4040404040404040,8);
	diffuse(0x2040800102040810,0x0101010101010101,7);
	diffuse(0x4080010204081020,0x0101010101010101,6);
	diffuse(0x8001020408102040,0x0101010101010101,5);

	//corner pattern
	diffuse(0xe0e0c00000000000,0x0000000000002009,9);
	diffuse(0x0707030000000000,0x0000000000010420,9);
	diffuse(0x0000000000c0e0e0,0x0100082000000000,10);
	diffuse(0x0000000000030707,0x2004010000000000,10);

	#undef diffuse
}


template float board::search_ptn<true>(cshort height,float alpha,float beta)const;
template float board::search_ptn<false>(cshort height,float alpha,float beta)const;
template<bool color>
float board::search_ptn(cshort height,float alpha,float beta)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->score_ptn<color>();
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip<color>(brd_type(1) << i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip<!color>(brd_type(1) << i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			short num_diff = count<color>() - count<!color>();
			num_diff <<= 1;
			if(num_diff){
				if(num_diff > 0){
					return num_diff + 1000;
				}else if(num_diff < 0){
					return num_diff - 1000;
				}else{
					return num_diff;
				}
			}else{
				return 0;
			}
		}else{
			sort(vec,ptr);

			for(auto p = vec;p != ptr;++p){
				temp = p->brd.search_ptn<color>(height - 1,alpha,beta);
				table_ref[p->pos] = temp;
				if(temp <= alpha)
					return alpha;
				if(temp < beta)
					beta = temp;
			}
			return beta;
		}
	}else{
		sort(vec,ptr,greater<brd_val>());

		for(auto p = vec;p != ptr;++p){
			temp = - p->brd.search_ptn<!color>(height - 1,-beta,-alpha);
			table_ref[p->pos] = temp;
			if(temp >= beta)
				return beta;
			if(temp > alpha)
				alpha = temp;
		}
	}
    return alpha;

	#ifdef DEBUG_SEARCH
	};
	out << "<div color=" << color
		<<" height=" << height
		<< " alpha=" << alpha
		<< " beta=" << beta
		<< ">\n";
	do_print(out);
	calc_type result = fun();
	out << "result = " << result <<"\n"
		<< "</div>\n";
	return result;
	#endif
}

void pattern::compress(float* const& ptr){
	size_t j = 0;
	for(size_t num = 0;num != this->size;++num){
		for(size_t i = 0;i != this->length;++i){
			if(i & (i >> 8)){
				continue;
			}
			ptr[j] = this->table[num][i];
			++j;
		}
	}
}

void pattern::decompress(float* const& ptr){
	size_t j = 0;
	for(size_t num = 0;num != this->size;++num){
		for(size_t i = 0;i != this->length;++i){
			if(i & (i >> 8)){
				continue;
			}
			this->table[num][i] = ptr[j];
			++j;
		}
	}
}

bool compete(pattern* const& p1,pattern* const& p2){
	board brd;
	coordinate pos1,pos2;
	brd.assign(0x0000000810000000,0x0000001008000000);
	do{
		ptr_pattern = p1;
		pos1 = brd.play(mthd_ptn,true,0);
		ptr_pattern = p2;
		pos2 = brd.play(mthd_ptn,false,0);
	}while(pos1.x >= 0 || pos2.x >= 0);
	return brd.count(true) > brd.count(false);
};

// param cross(const param& p1,const param& p2){
	// param result;
	// result.reserve(param::size_max);
	// do{
		// for(auto& ptr_pattern:p1){
			// if(coin(engine)){
				// result.push_back(*ptr_pattern);
			// }
		// }
		// for(auto& ptr_pattern:p2){
			// if(result.size() >= param::size_max){
				// break;
			// }
			// if(coin(engine)){
				// result.push_back(*ptr_pattern);
			// }
		// }
	// }while(result.size() < param::size_min);
	// return result;
// }

void group::assign(const size_t& size){
	while(vec.size() < size){
		vec.emplace_back();
		record.emplace_back(0);
	}
}

void group::initial(){
	for(auto& ptn:vec){
		ptn.initial();
	}
}

void group::load(const string& filename,cbool is_compress,cint num_begin,cint num
	,cbool is_compatible){
	#define _READ(var) fin.read((char *)(&var),sizeof(var))

	ifstream fin(filename,ios::in | ios::binary);
	size_t group_size,ptn_size,ptn_length;

	if(!fin){
		fin.close();
		cout << "Error: Cannot open the file: " << filename << " ." << endl;
		return;
	}
	
	_READ(group_size);
	_READ(ptn_size);
	_READ(ptn_length);

	if(is_compatible){
		if(ptn_size != pattern::size){
			cout << "Warning: The pattern.size does not match." << endl;
			if(ptn_size > pattern::size){
				ptn_size = pattern::size;
			}
		}
	}else{
		if(ptn_size != pattern::size){
			fin.close();
			cout << "Error: The pattern.size does not match." << endl;
			return;
		}
	}
	if(ptn_length != pattern::length){
		fin.close();
		cout << "The pattern.length does not match." << endl;
		return;
	}
	
	this->vec.reserve(group_size);
	if(is_compress){
		size_t ptr_size = ptn_size * 6561;
		float* ptr = new float[ptr_size];
		for(size_t i = 0;i != group_size && i < size_t(num);++i){
			this->vec.emplace_back();
			this->record.emplace_back(0);
			//_READ(this->vec.back());
			//fin.read((char *)(&this->vec.back()),sizeof(float) * ptn_size * pattern::length);
			fin.read((char *)(ptr),ptr_size * sizeof(float));
			this->vec.back().decompress(ptr);
		}
		delete ptr;
	}else{
		for(size_t i = 0;i != group_size && i < size_t(num);++i){
			this->vec.emplace_back();
			this->record.emplace_back(0);
			//_READ(this->vec.back());
			fin.read((char *)(&this->vec.back()),sizeof(float) * ptn_size * pattern::length);
		}
	}
	fin.close();
	#undef _READ
}

void group::save(const string& filename,const bool& is_compress){
	#define WRITE(var) fout.write((char *)(&var),sizeof(var))
	ofstream fout(filename,ios::out | ios::binary);

	size_t group_size = vec.size();
	size_t ptn_size = pattern::size;
	size_t ptn_length = pattern::length;

	WRITE(group_size);
	WRITE(ptn_size);
	WRITE(ptn_length);

	if(is_compress){
		size_t ptr_size = ptn_size * 6561;
		float* ptr = new float[ptr_size];
		for(auto& ptn:this->vec){
			ptn.compress(ptr);
			fout.write((char *)(ptr),ptr_size * sizeof(float));
		}
		delete ptr;
	}else{
		for(auto& ptn:this->vec){
			WRITE(ptn);
		}
	}
	fout.close();
}

void group::train(){
	if(this->vec.empty()){
		return;
	}
	short temp;
	for(size_t i = 1;i != this->vec.size();++i){
		for(auto j = 0;j + i != this->vec.size();++j){
			temp = compete(&vec[j],&vec[j + i]);
			record[j] += temp;
			record[j + i] -= temp;
		}
		for(auto j = 0;j + i != this->vec.size();++j){
			temp = compete(&vec[j + i],&vec[j]);
			record[j] -= temp;
			record[j + i] += temp;
		}
	}
}

void group::print_record(){
	cout << '(';
	for(auto& i:this->record){
		cout << i << ',';
	}cout << "\b)";
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
