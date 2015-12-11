#include <iostream>
#include <unordered_map>

using namespace std;

class board{
public:
	typedef unsigned long long brd_type;
	typedef unsigned char line_type;
	typedef unsigned short lline_type;
	typedef unsigned int exline_type;
	typedef short pos_type;
	typedef short calc_type;

	board(){};
	board(const brd_type& _brd_black,const brd_type& _brd_white)
		:brd_black(_brd_black),brd_white(_brd_white){}

	template<bool color> const brd_type& get()const;
	template<bool color> brd_type& get();
	const brd_type& visit(const bool& color)const{
		return color ? get<true>() : get<false>();
	}
	brd_type& visit(const bool& color){
		return color ? get<true>() : get<false>();
	}

	enum chessman{blank,white,black,null};
	static const pos_type chessman_num = 4;
	static const pos_type size = 4;
	static const brd_type begin = 0x0;
	static const brd_type end = 0x8000000000000000;
	constexpr static const char chr_print[chessman_num] = {'.','#','O','*'};
	enum pos_line{pos_edge,pos_margin,pos_center};
	static const pos_type pos_num = pos_center;
	static const pos_type stage_num = 5;
	static const pos_type enum_num = 256;

	friend ostream& operator<<(ostream& out,const board& brd){
		brd.do_print(out);
		return out;
	}
	const board& print(ostream& out = cout)const{
		return do_print(out);
	}
	board& print(ostream& out = cout){
		do_print(out);
		return *this;
	}

protected:

	brd_type brd_black,brd_white;

	static calc_type table_count[enum_num];
	static calc_type table_eval[stage_num][board::pos_num][enum_num];
	static unordered_map<lline_type,line_type> table_valid;
	static unordered_map<exline_type,lline_type> table_flip; 

	const board& do_print(ostream& out = cout)const{
		brd_type mask = 1;
		for(pos_type i = 0;i != size;++i){
			for(pos_type j = 0;j != size;++j){
				out << mask_visit(mask);
				mask <<= 1;
			}
			cout << '\n';
		}
		return *this;
	}
	chessman mask_visit(const pos_type& mask)const{
		if(mask & brd_black){
			if(mask & brd_white){
				return null;
			}else{
				return black;
			}
		}else{
			if(mask & brd_white){
				return white;
			}else{
				return blank;
			}
		}
	}
	template<bool color>
	board& mask_set(const brd_type& mask,bool flag){
		brd_type& handle = this->get<color>();
		if(flag){
			handle |= mask;
		}else{
			handle &= ~mask;
		}
		return *this;
	}
	board& mask_set(const brd_type& mask,bool color,bool flag){
		if(color){
			if(flag){
				brd_black |= mask;
			}else{
				brd_white &= ~mask;
			}
		}else{
			if(flag){
				brd_black |= mask;
			}else{
				brd_white &= ~mask;
			}
		}
		return *this;
	}
	board& mask_set(const brd_type& mask,chessman chsm){
		if(chsm & white){
			brd_white |= mask;
		}else{
			brd_white &= ~mask;
		}
		if(chsm & black){
			brd_black |= mask;
		}else{
			brd_black &= ~mask;
		}
		return *this;
	}
};

template<> inline const board::brd_type& board::get<true>()const{
	return brd_black;
}
template<> inline const board::brd_type& board::get<false>()const{
	return brd_white;
}
template<> inline board::brd_type& board::get<true>(){
	return brd_black;
}
template<> inline board::brd_type& board::get<false>(){
	return brd_white;
}
