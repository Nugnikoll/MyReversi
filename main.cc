#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "reversi.h"
#include "pattern.h"
#include "cpptcl.h"

using namespace std;
using namespace Tcl;

#ifdef DEBUG_SEARCH
	ofstream out;
#endif

interpreter* ptr_inter;
board brd;
vector<board> record;
bool flag_auto_show = true;
bool flag_auto_save = true;
string prompt = ">>";
struct message_type{
	message_type():start(false){}
	bool start;
	void clear(){
		start = false;
	};
}message;

group grp;

void quit(){
	#ifdef DEBUG_SEARCH
		out.close();
	#endif
	exit(0);
}
void show(){
	brd.print(cout);
}
void start(){
	brd.assign(0x0000000810000000,0x0000001008000000);
	message.start = true;
	if(flag_auto_show){
		show();
	}
}
void auto_show(cbool flag){
	flag_auto_show = flag;
}
void auto_save(cbool flag){
	flag_auto_save = flag;
}
void push(){
	record.push_back(brd);
}
bool do_pop(){
	if(record.empty()){
		return false;
	}else{
		brd = record.back();
		record.pop_back();
		return true;
	}
}
bool pop(){
	if(record.empty()){
		return false;
	}else{
		brd = record.back();
		record.pop_back();
		if(flag_auto_show){
			show();
		}
		return true;
	}
}
object bget(){
	object result;
	result.append(*ptr_inter,object((int)(brd.bget(true) >> 32)));
	result.append(*ptr_inter,object((int)(brd.bget(true))));
	result.append(*ptr_inter,object((int)(brd.bget(false) >> 32)));
	result.append(*ptr_inter,object((int)(brd.bget(false))));
	return result;
}
void assign(const object& obj){
	if(flag_auto_save){
		push();
	}
	brd_type temp_black, temp_white;
	temp_black = (unsigned int)obj.at(*ptr_inter,0).get<int>(*ptr_inter);
	temp_black <<= 32;
	temp_black |= (unsigned int)obj.at(*ptr_inter,1).get<int>(*ptr_inter);
	temp_white = (unsigned int)obj.at(*ptr_inter,2).get<int>(*ptr_inter);
	temp_white <<= 32;
	temp_white |= (unsigned int)obj.at(*ptr_inter,3).get<int>(*ptr_inter);
	brd.assign(temp_black,temp_white);
	if(flag_auto_show){
		show();
	}
}

int visit(cint x, cint y){
	return brd.get(pos_type(x),pos_type(y));
}
void set(cint x, cint y, cint chsman){
	if(flag_auto_save){
		push();
	}
	brd.set(pos_type(x),pos_type(y),chessman(chsman));
	if(flag_auto_show){
		show();
	}
}
void mirror(cbool is_horizontal){
	if(flag_auto_save){
		push();
	}
	brd.mirror(is_horizontal);
	if(flag_auto_show){
		show();
	}
}
void rotater(cint n90){
	if(flag_auto_save){
		push();
	}
	brd.rotater(n90);
	if(flag_auto_show){
		show();
	}
}
void rotatel(cint n90){
	if(flag_auto_show){
		show();
	}
	brd.rotatel(n90);
}
bool flip(cbool color, cint x, cint y){
	if(flag_auto_save){
		push();
	}
	bool result = brd.flip(color,pos_type(x),pos_type(y));
	if(result){
		if(flag_auto_show){
			show();
		}
	}else{
		if(flag_auto_save){
			do_pop();
		}
	}
	return result;
}
int count(cbool color){
	return brd.count(color);
}
int score(cbool color,cint stage){
	return brd.score(color,stage);
}
int search(
	cint mthd,cbool color,cint height,
	cint alpha = _inf,cint beta = _inf,cint acc = 0,
	cint stage = 0,cint gamma = 0){
	return brd.search(method(mthd),color,height,alpha,beta,stage,gamma);
}
object get_choice(cint mthd,cbool color,cint height,cint stage = 0){
	vector<choice> choices = brd.get_choice(method(mthd),color,height,stage);
	object result;
	for(choice& c:choices){
		object obj;
		obj.append(*ptr_inter,object(int(c.val)));
		obj.append(*ptr_inter,object(int(c.x)));
		obj.append(*ptr_inter,object(int(c.y)));
		result.append(*ptr_inter,obj);
	}
	return result;
}
object select_choice(object obj_choices){
	vector<choice> choices;
	choice temp;
	//object obj;
	object result;
	//cout << obj_choices.get<string>(*ptr_inter) << " | ";

	int num = obj_choices.length(*ptr_inter);
	//cout << num << endl;
	for(int i = 0;i != num / 3;++i){
		//obj = obj_choices.at(*ptr_inter,i).get<object>(*ptr_inter);
		temp.val = obj_choices.at(*ptr_inter,i * 3 + 0).get<int>(*ptr_inter);
		temp.x = obj_choices.at(*ptr_inter,i * 3 + 1).get<int>(*ptr_inter);
		temp.y = obj_choices.at(*ptr_inter,i * 3 + 2).get<int>(*ptr_inter);
		choices.push_back(temp);
	}
	temp = brd.select_choice(choices);
	result.append(*ptr_inter,object(int(temp.val)));
	result.append(*ptr_inter,object(int(temp.x)));
	result.append(*ptr_inter,object(int(temp.y)));
	return result;
}
object play(cint mthd,cbool color,cint height = -1,cint stage = -1){
	if(flag_auto_save){
		push();
	}
	auto pos = brd.play(method(mthd),color,height,stage);
	object result;
	result.append(*ptr_inter,object(int(pos.x)));
	result.append(*ptr_inter,object(int(pos.y)));
	if(pos.x >= 0){
		if(flag_auto_show){
			show();
		}
	}else{
		if(flag_auto_save){
			do_pop();
		}
	}
	return result;
}

void generate(cint num){
	grp.assign(num);
}

void train(cint num){
	for(int i = 0;i != num;++i){
		grp.train();
	}
}

void load(const string& filename,cbool is_compress = true,cint num_start = 0,
	cint num = 100){
	grp.load(filename,is_compress,num_start,num);
}

void save(const string& filename){
	grp.save(filename);
}

int main(){

	#ifdef DEBUG_SEARCH
		out.open("out.html");
	#endif

	interpreter inter;
	ptr_inter = &inter;

	#ifdef NDEBUG
		inter.make_safe();
	#endif
	inter.def("quit",quit);
	inter.def("exit",quit);
	inter.def("start",start);
	inter.def("auto_show",auto_show);
	inter.def("assign",generate);
	inter.def("train",train);
	inter.def("load",load);
	inter.def("save",save);

	string str,s;
	string::size_type pos;
	while(cout << prompt , getline(cin,str)){
		pos = str.find_last_not_of(" \n\t");
		if(pos != str.npos){
			if(str[pos] == '\\'){
				while(getline(cin,s)){
					pos = s.find_last_not_of(" \n\t");
					if(pos == s.npos){
						break;
					}
					if(s[pos] != '\\'){
						break;
					}
					str += '\n';
					str += s;
				}
				if(!cin){
					break;
				}
				str += '\n';
				str += s;
			}
		}
		try{
			inter.eval(str);
			if(message.start){
				inter.def("show",show);
				inter.def("assign",assign);
				inter.def("bget",bget);
				inter.def("visit",visit);
				inter.def("count",count);
				inter.def("place",set);
				inter.def("flip",flip);
				inter.def("score",score);
				inter.def("mirror",mirror);
				inter.def("rotater",rotater);
				inter.def("rotatel",rotatel);
				inter.def("search",search);
				inter.def("get_choice",get_choice);
				inter.def("do_select_choice",select_choice);
				inter.def("push",push);
				inter.def("save",push);
				inter.def("pop",pop);
				inter.def("undo",pop);
				inter.def("auto_save",auto_save);
				inter.def("play",play);
				inter.eval(
					"proc select_choice choices { "
						"return [do_select_choice [join $choices]]"
					"}"
				);
			}
			message.clear();
		}catch(const tcl_error& err){
			cout << err.what() << endl;
		}
	}

	#ifdef DEBUG_SEARCH
		out.close();
	#endif

//	 long long a, b;
//	 cin >> hex >> a >> hex >> b;
//	 asm volatile(
//		 "packsswb %2,%0;"
//		 :"=y"(a)
//		 :"0"(a),"y"(b)
//		 :
//	 );
//	 cout << hex << a << " " << hex << b;
	return 0;
}
