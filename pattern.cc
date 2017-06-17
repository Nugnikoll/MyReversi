#include <vector>
#include <algorithm>
#include <cstring>

#include "reversi.h" //--
#include "pattern.h" //--

using namespace std;

pattern* ptr_pattern = NULL;

void set_ptn(pattern* ptr){
	ptr_pattern = ptr;
}

extern bool check_ptn(){
	return (ptr_pattern != NULL);
}

void pattern::initial(){
	count = 1000;
	memset(table1,0,sizeof(table1));
	memset(table2,0,sizeof(table2));
}

const brd_type ptn_mask[] = {
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

const short ptn_num[] = {
	0, 1, 2, 3, 3, 2, 1, 0,
	0, 1, 2, 3, 3, 2, 1, 0,
	4, 5, 6, 7, 8, 7, 6, 5,
	4, 5, 6, 7, 8, 7, 6, 5,
	9, 10, 9, 10
};

float& board::extract_ptn(cbool color, float* const& ptr, cbrd_type mask, cshort num)const{
	brd_type brd_blue = this->bget(color);
	brd_type brd_green = this->bget(!color);
	brd_type index;
	index = extract(brd_blue,mask) << 8;
	index |= extract(brd_green,mask);
	//cout << hex << brd_blue << " " << brd_green << endl;
	//cout << "index: " << hex << index << " mask: " << hex << mask << " num: " << dec << num << endl;
	//assert((index & ~0xffffull) == 0);
	return ptr[(brd_type(num) << 16) + index];
}

float board::score_ptn(cbool color)const{

	short blue_move = this->count_move(color);
	short green_move = this->count_move(!color);

	if((blue_move | green_move) == 0){
		short num_diff = count(color) - count(!color);
		num_diff <<= 1;
		if(num_diff > 0){
			return num_diff + 1000;
		}else if(num_diff < 0){
			return num_diff - 1000;
		}else{
			return num_diff;
		}
	}

	brd_type brd_blue = this->bget(color);
	brd_type brd_green = this->bget(!color);
	brd_type index;

	float result = 0;
	short stage = (this->sum() - 1) >> 4;
	auto table1 = ptr_pattern->table1[stage];

	result += ptr_pattern->table2[stage][blue_move * 30 + green_move];

	for(int i = 0;i != 36;++i){
		index = extract(brd_blue,ptn_mask[i]) << 8;
		index |= extract(brd_green,ptn_mask[i]);
		result +=  table1[(brd_type(ptn_num[i]) << 16) + index];
	}

	return result;
}

vector<float> board::eval_ptn(cbool color)const{

	vector<float> result;

	short blue_move = this->count_move(color);
	short green_move = this->count_move(!color);

	brd_type brd_blue = this->bget(color);
	brd_type brd_green = this->bget(!color);
	brd_type index;

	short stage = (this->sum() - 1) >> 4;
	auto table1 = ptr_pattern->table1[stage];

	result.push_back(ptr_pattern->table2[stage][blue_move * 30 + green_move]);

	for(int i = 0;i != 36;++i){
		index = extract(brd_blue,ptn_mask[i]) << 8;
		index |= extract(brd_green,ptn_mask[i]);
		result.push_back(i);
		result.push_back(index);
		result.push_back(table1[(brd_type(ptn_num[i]) << 16) + index]);
	}

	return result;
}

//float fdecay = 1 - 0.01;

void board::adjust_ptn(cbool color,ccalc_type diff)const{

	short stage = (this->sum() - 1) >> 4;
	auto table1 = ptr_pattern->table1[stage];

	++ptr_pattern->count;

	ptr_pattern->table2[stage][count_move(color) * 30 + count_move(!color)]
		+= diff / ptr_pattern->count;

	for(int i = 0;i != 36;++i){
		extract_ptn(color,table1,ptn_mask[i],ptn_num[i]) += diff / ptr_pattern->count;
	}

}

float board::search_ptn(cbool color,cshort height,float alpha,float beta)const{

	#ifdef DEBUG_SEARCH
	auto fun = [&]()->calc_type{
	#endif

	if(height == 0){
		return this->score_ptn(color);
	}

	calc_type (&table_ref)[size2] = table_temp[color][height];
	brd_val vec[35];
	brd_val* ptr = vec;
	calc_type temp;
	
	ptr->brd = *this;
	for(register pos_type i = 0;i != size2;++i){
		if(ptr->brd.flip(color,i)){
			ptr->pos = i;
			ptr->val = table_ref[i];
			(++ptr)->brd = *this;
		}
	}
	
	if(ptr == vec){
		calc_type (&table_ref)[size2] = table_temp[!color][height];
		//ptr->brd = *this;
		for(register pos_type i = 0;i != size2;++i){
			if(ptr->brd.flip(!color,i)){
				ptr->pos = i;
				ptr->val = table_ref[i];
				(++ptr)->brd = *this;
			}
		}

		if(ptr == vec){
			short num_diff = count(color) - count(!color);
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
				temp = p->brd.search_ptn(color,height - 1,alpha,beta);
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
			temp = - p->brd.search_ptn(!color,height - 1,-beta,-alpha);
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

void pattern::save(ostream& out){
	#define WRITE(var) out.write((char *)(&var),sizeof(var))

	WRITE(this->count);
	for(auto& i:table1){
		for(int j = 0;j != size1;++j){
			for(size_t k = 0;k != this->length;++k){
				if(k & (k >> 8)){
					continue;
				}
				WRITE(i[(j << 16) + k]);
			}
		}
	}
	WRITE(table2);

	#undef WRITE
}

void pattern::load(istream& in){
	#define _READ(var) in.read((char *)(&var),sizeof(var))

	_READ(this->count);
	for(auto& i:table1){
		for(int j = 0;j != size1;++j){
			for(size_t k = 0;k != this->length;++k){
				if(k & (k >> 8)){
					continue;
				}
				_READ(i[(j << 16) + k]);
			}
		}
	}
	_READ(table2);

	#undef _READ
}

bool dflag = false;

bool compete(pattern* const& p1,pattern* const& p2,cmethod mthd,cshort depth){
	board brd;
	board vec[64];
	board* ptr = vec;
	bool flag[64];
	bool* color = flag;
	coordinate pos1,pos2;

	brd.initial();
	do{
		//cout << "board" << endl;
		//brd.print();
		*ptr++ = brd;
		ptr_pattern = p1;
		pos1 = brd.play(mthd,true,depth);
		if(pos1.x < 0){
			--ptr;
		}else{
			*color = true;
			++color;
		}

		//cout << "x1 : " << pos1.x << " y1 : " << pos1.y << endl;

		//brd.print();
		//*ptr++ = board(brd.bget(false),brd.bget(true));
		*ptr++ = brd;
		ptr_pattern = p2;
		pos2 = brd.play(mthd,false,depth);
		if(pos2.x < 0){
			--ptr;
		}else{
			*color = false;
			++color;
		}

		//cout << "x2 : " << pos2.x << " y2 : " << pos2.y << endl;
	}while(pos1.x >= 0 || pos2.x >= 0);

	calc_type result = brd.count(true) - brd.count(false);

	ptr_pattern = p1;
	calc_type diff;
	board* p;
	bool* c;
	if(result == 0){
		return false;
	}
	(result > 0) ? result = 100 : result = -100;

	if(dflag){
		cout << "result : " << result << endl;
		for(p = vec,c = flag;p != ptr;++p,++c){
			p->print();
			cout << "flag : " << *c << " ";
			cout << "score : " << p->score_ptn(true) << " ";
			cout << "diff : " << (result - p->score_ptn(true)) / 38 << endl;
		}
	}

	for(p = vec,c = flag;p != ptr;++p,++c){
		diff = (result - p->score_ptn(true)) / 38;
		*c ? p->adjust_ptn(true,diff) : p->adjust_ptn(false,-diff);
	}
	return true;
};

void group::assign(const int& size){
	vec.clear();
	while(vec.size() < (unsigned int)(size)){
		vec.emplace_back();
		record.emplace_back(0);
	}
}

void group::load(const string& path,cint num_begin,cint num){
	#define _READ(var) fin.read((char *)(&var),sizeof(var))

	ifstream fin(path,ios::in | ios::binary);
	size_t ele_size, ptn_size, group_size;

	if(!fin){
		fin.close();
		cout << "Error: Cannot open the file: " << path << " ." << endl;
		return;
	}

	_READ(ele_size);
	_READ(ptn_size);
	_READ(group_size);

	if(ptn_size != sizeof(pattern)){
		fin.close();
		cout << "Error: The size of pattern does not match." << endl;
		return;
	}
	if(ele_size != sizeof(element)){
		fin.close();
		cout << "Error: The size of element does not match." << endl;
		return;
	}
	
	this->vec.reserve(group_size);

	for(size_t i = 0;i != group_size && i < size_t(num);++i){
		this->vec.emplace_back();
		this->record.emplace_back(0);
		this->vec.back().load(fin);
	}

	fin.close();
	#undef _READ
}

void group::save(const string& path){
	#define WRITE(var) fout.write((char *)(&var),sizeof(var))
	ofstream fout(path,ios::out | ios::binary);

	size_t ele_size = sizeof(element);
	size_t ptn_size = sizeof(pattern);
	size_t group_size = vec.size();

	WRITE(ele_size);
	WRITE(ptn_size);
	WRITE(group_size);

	for(auto& ptn:this->vec){
		ptn.save(fout);
	}

	fout.close();
	#undef WRITE
}

void group::train(cmethod mthd, cshort depth){
	if(this->vec.empty()){
		return;
	}
	short temp;
	for(size_t i = 1;i != this->vec.size();++i){
		for(auto j = 0;j + i != this->vec.size();++j){
			temp = compete(&vec[j],&vec[j + i],mthd,depth);
			record[j] += temp;
			record[j + i] -= temp;
		}
		for(auto j = 0;j + i != this->vec.size();++j){
			temp = compete(&vec[j + i],&vec[j],mthd,depth);
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
