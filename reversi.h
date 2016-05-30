#ifndef REVERSI_H
#define REVERSI_H

#include <iostream>
#include <limits>
#include <tuple>
#include <vector>

using namespace std;

//#define DEBUG_SEARCH

#ifdef DEBUG_SEARCH
	#include <fstream>
	extern ofstream out;
#endif

extern unsigned int seed;

typedef const bool& cbool;
typedef const short& cshort;
typedef const int& cint;

typedef unsigned long long brd_type;
typedef const brd_type& cbrd_type;
typedef unsigned char line_type;
typedef const line_type& cline_type;
typedef short pos_type;
typedef const pos_type& cpos_type;
typedef short calc_type;
typedef const calc_type& ccalc_type;
enum chessman{blank,white,black,null};
typedef const chessman& cchessman;

const calc_type inf = numeric_limits<short>::max();
const calc_type _inf = - inf;

class board{
protected:
	struct conf_score{
		calc_type *a,*b,*c;
	};
	typedef const conf_score& cconf_score;
public:

	board(){};
	board(cbrd_type _brd_black,cbrd_type _brd_white)
		:brd_black(_brd_black),brd_white(_brd_white){}

	static const brd_type last;
	static const pos_type chessman_num = 4;
	static const pos_type size = 8;
	static const pos_type size2 = size * size;
	static const char chr_print[chessman_num];
	enum pos_enum{pos_angle,pos_edge,pos_inner,pos_center};
	static const pos_type pos_num = 4;
	static const pos_type layer_num = 3;
	static const pos_type stage_num = 3;
	static const pos_type enum_num = 256;
	static const calc_type max_height = 20;

	static calc_type table_param[stage_num][pos_num];

	typedef tuple<float,board,pos_type,pos_type> choice;

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

	static void config_num();
	static void config();
	static conf_score stage_config(cpos_type stage){
		conf_score conf;
		if(stage < 0 || stage >= stage_num){
			cout << "Error: out of range\n";
			conf.a = conf.b = conf.c = 0;
			return conf;
		}
		static bool flag = true;
		if(flag){
			config();
			flag = false;
		}
		conf.a = table_eval[stage][0];
		conf.b = table_eval[stage][1];
		conf.c = table_eval[stage][2];
		return conf;
	}

	board& assign(cbrd_type _brd_black,cbrd_type _brd_white){
		brd_black = _brd_black;
		brd_white = _brd_white;
		return *this;
	}
	brd_type bget(cbool color)const{
		if(color){
			return this->bget<true>();
		}else{
			return this->bget<false>();
		}
	}
	chessman get(cpos_type x, cpos_type y)const{
		if(x < 0 || y < 0 || x >= size || y >= size){
			cout << "Error: out of range\n";
			return blank;
		}
		return get(brd_type(1) << ((y << 3) + x));
	}

	board& set(cpos_type x, cpos_type y, cchessman chsman){
		if(x < 0 || y < 0 || x >= size || y >= size){
			cout << "Error: out of range\n";
			return *this;
		}
		return set(brd_type(1) << ((y << 3) + x),chsman);
	}

	board& mirror(cbool is_horizontal);
	board& rotater(pos_type n90);
	board& rotatel(pos_type n90){
		return rotater(4 - n90);
	}

	bool flip(cbool color, cpos_type x, cpos_type y){
		if(x < 0 || y < 0 || x >= size || y >= size){
			cout << "Error: out of range\n";
			return false;
		}
		if(color){
			return flip<true>(brd_type(1) << ((y << 3) + x));
		}else{
			return flip<false>(brd_type(1) << ((y << 3) + x));
		}
	}
	calc_type count(cbool color)const{
		if(color){
			return count<true>();
		}else{
			return count<false>();
		}
	}
	calc_type sum()const{
		return count<true>() + count<false>();
	} 
	calc_type score(cbool color,cpos_type stage)const{
		conf_score conf = stage_config(stage);
		if(color){
			return score<true>(conf);
		}else{
			return score<false>(conf);
		}
	}
	calc_type search(cbool color,cshort height,cpos_type stage)const{
		return search(color,height,_inf,inf,0,stage);
	}
	calc_type search(cbool color,cshort height,calc_type alpha,
		ccalc_type beta,calc_type acc,cpos_type stage)const;
	vector<choice> get_choices(cbool color,cshort height,cpos_type stage)const;
	choice select_choice(vector<choice> choices)const;
	tuple<pos_type,pos_type> play(cbool color){
		short height;
		pos_type stage;

		static bool flag = true;
		if(flag){
			config_num();
			flag = false;
		}

		short total = this->sum();
		if(total <= 33){
			height = 7; stage = 0;
		}else if(total <= size2 - 15){
			height = 7; stage = 1;
		}else{
			height = 20; stage = 2;
		}

		vector<choice> choices = get_choices(color,height,stage);

		if(choices.empty()){
			return tuple<pos_type,pos_type>(-1,-1);
		}else{
			choice best = select_choice(choices);
			flip(color,std::get<2>(best),std::get<3>(best));
			return tuple<pos_type,pos_type>(std::get<2>(best),std::get<3>(best));
		}
	}

protected:

	brd_type brd_black,brd_white;

	static const brd_type  lbound = 0xfefefefefefefefe;
	static const brd_type  rbound = 0x7f7f7f7f7f7f7f7f;
	static const brd_type  ubound = 0xffffffffffffff00;
	static const brd_type  dbound = 0x00ffffffffffffff;
	static const brd_type ulbound = 0xfefefefefefefe00;
	static const brd_type urbound = 0x7f7f7f7f7f7f7f00;
	static const brd_type dlbound = 0x00fefefefefefefe;
	static const brd_type drbound = 0x007f7f7f7f7f7f7f;

	static calc_type table_count[enum_num];
	static calc_type table_eval[stage_num][size][enum_num];
	static calc_type table_temp[2][20][size2];

	static void up(brd_type& mask){mask >>= 8;}
	static void down(brd_type& mask){mask <<= 8;}
	static void left(brd_type& mask){mask >>= 1;}
	static void right(brd_type& mask){mask <<= 1;}
	static void uleft(brd_type& mask){mask >>= 9;}
	static void uright(brd_type& mask){mask >>= 7;}
	static void dleft(brd_type& mask){mask <<= 7;}
	static void dright(brd_type& mask){mask <<= 9;}

	const board& do_print(ostream& out = cout)const{
//		brd_type mask = 1;
//		for(pos_type i = 0;i != size;++i){
//			for(pos_type j = 0;j != size;++j){
//				out << chr_print[get(mask)];
//				mask <<= 1;
//			}
//			out << '\n';
//		}
		string s =
			"¨X¨T¨h¨T¨h¨T¨h¨T¨h¨T¨h¨T¨h¨T¨h¨T¨[\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨c©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤©à©¤¨f\n"
			"¨U.©¦.©¦.©¦.©¦.©¦.©¦.©¦.¨U\n"
			"¨^¨T¨k¨T¨k¨T¨k¨T¨k¨T¨k¨T¨k¨T¨k¨T¨a\n"
		;
		brd_type mask = 1;
		string::size_type pos;
		while((pos = s.find(".")) != s.npos){
			s.replace(pos,1,"  ");
		}
		for(pos_type i = 0;i != size;++i){
			for(pos_type j = 0;j != size;++j){
				s[i * 4 + j * 70 + 37] = chr_print[get(mask)];
				mask <<= 1;
			}
		}
		while((pos = s.find(".")) != s.npos){
			s.replace(pos,1," ");
		}
		while((pos = s.find("#")) != s.npos){
			s.replace(pos,2,"¡ñ");
		}
		while((pos = s.find("O")) != s.npos){
			s.replace(pos,2,"¡ð");
		}
		out << s;
		return *this;
	}

	template<bool color> inline
	brd_type& bget(){
		if(color){
			return brd_black;
		}else{
			return brd_white;
		}
	}
	template<bool color> inline
	const brd_type& bget()const{
		if(color){
			return brd_black;
		}else{
			return brd_white;
		}
	}

	inline chessman get(cbrd_type mask)const{
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
	template<bool color> inline
	board& set(cbrd_type mask, cbool flag){
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
	inline board& set(cbrd_type mask, cchessman chsm){
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

	template<bool color>
	bool flip(cbrd_type mask){
		if((brd_black | brd_white) & mask){
			return false;
		}
		bool everflip = false;
		brd_type pos = mask;
		brd_type& blue = bget<color>();
		brd_type& green = bget<!color>();
		while(pos & lbound){
			left(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(right(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & rbound){
			right(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(left(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & ubound){
			up(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(down(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & dbound){
			down(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(up(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & ulbound){
			uleft(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(dright(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & urbound){
			uright(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(dleft(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & dlbound){
			dleft(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(uright(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		pos = mask;
		while(pos & drbound){
			dright(pos);
			if(green & pos)
				continue;
			if(blue & pos){
				while(uleft(pos), pos != mask){
					blue |= pos;
					green &= ~pos;
					everflip = true;
				}
			}
			break;
		}
		if(everflip){
			blue |= mask;
			green &= ~mask;
		}
		return everflip;
	}

	template<bool color>
	calc_type count()const{

		brd_type blue = bget<color>();
		calc_type result;

		result = table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue]; blue >>= size;
		result += table_count[(unsigned char&)blue];

		//cout << result << endl;

		return result;
	}
	template<bool color> inline
	calc_type score(cconf_score conf)const{

		brd_type blue = bget<color>();
		brd_type green = bget<!color>();
		calc_type result;

		result = conf.a[(unsigned char&)blue]; blue >>= size;
		result += conf.b[(unsigned char&)blue]; blue >>= size;
		result += conf.c[(unsigned char&)blue]; blue >>= size;
		result += conf.c[(unsigned char&)blue]; blue >>= size;
		result += conf.c[(unsigned char&)blue]; blue >>= size;
		result += conf.c[(unsigned char&)blue]; blue >>= size;
		result += conf.b[(unsigned char&)blue]; blue >>= size;
		result += conf.a[(unsigned char&)blue];

		result -= conf.a[(unsigned char&)green]; green >>= size;
		result -= conf.b[(unsigned char&)green]; green >>= size;
		result -= conf.c[(unsigned char&)green]; green >>= size;
		result -= conf.c[(unsigned char&)green]; green >>= size;
		result -= conf.c[(unsigned char&)green]; green >>= size;
		result -= conf.c[(unsigned char&)green]; green >>= size;
		result -= conf.b[(unsigned char&)green]; green >>= size;
		result -= conf.a[(unsigned char&)green];

		return result;
	}

	static const calc_type mark_max = 10000;

	template<bool color>
	calc_type search(cshort height,cconf_score conf)const{
		return search<color>(height,_inf,inf,0,conf);
	}
	template<bool color>
	calc_type search(cshort height,calc_type alpha,calc_type beta,calc_type acc,cconf_score conf)const;
	template<bool color>
	vector<choice> get_choices(cshort height,cconf_score conf)const;
};

#endif // REVERSI_H
