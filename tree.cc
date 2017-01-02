#include <fstream>
#include <cassert>
#include <stack>
#include <tuple>

#include "tree.h"
#include "reversi.h"

void tree::load(const string& path){
	#define _READ(var) fin.read((char *)(&var),sizeof(var))

	ifstream fin(path,ios::in | ios::binary);
	int data_size;

	if(!fin){
		fin.close();
		cout << "Error: Cannot open the file: " << path << " ." << endl;
		return;
	}

	_READ(data_size);
	_READ(this->count);

	if(data_size != sizeof(data)){
		fin.close();
		cout << "Error: The size of data does not match." << endl;
		return;
	}

	this->destroy(root);

	data dat;
	bool flag_child,flag_sibling;
	stack<node*> storage;
	node* ptr;

	_READ(dat);
	_READ(flag_child);
	_READ(flag_sibling);
	root = new node({dat,NULL,NULL,NULL});
	ptr = root;

	while(true){
		while(flag_child){
			if(flag_sibling){
				storage.push(ptr);
			}
			_READ(dat);
			_READ(flag_child);
			_READ(flag_sibling);
			ptr->child = new node({dat,ptr,NULL,NULL});
			ptr = ptr->child;
		}
		if(flag_sibling){
			storage.push(ptr);
		}
		if(storage.empty()){
			break;
		}
		ptr = storage.top();
		storage.pop();
		_READ(dat);
		_READ(flag_child);
		_READ(flag_sibling);
		ptr->sibling = new node({dat,ptr->parent,NULL,NULL});
		ptr = ptr->sibling;
	}

	fin.close();
	#undef _READ
}

void tree::save(const string& path){
	#define WRITE(var) fout.write((char *)(&var),sizeof(var))
	ofstream fout(path,ios::out | ios::binary);

	int data_size = sizeof(data);

	WRITE(data_size);
	WRITE(this->count);

	save(fout,root);

	fout.close();
	#undef WRITE
}

void tree::save(ostream& out,const node* const& ptr){
	#define WRITE(var) out.write((char *)(&var),sizeof(var))

	if(ptr){
		bool flag_child = (ptr->child != NULL);
		bool flag_sibling = (ptr->sibling != NULL);

		WRITE(ptr->dat);
		WRITE(flag_child);
		WRITE(flag_sibling);

		save(out,ptr->child);
		save(out,ptr->sibling);
	}

	#undef WRITE
}

//void tree::practice(method mthd,cshort height){
//	board brd;
//	vector<choice> choices;
//	coordinate pos1,pos2;
//	node* ptr = root;
//
//	brd.initial();
//	do{
//		if(ptr->dat.win + ptr->dat.lose > threshold){
//			choices = brd.get_choice(mthd,true,height,-1);
//			ptr = ptr->child;
//			for(auto& c:choices){
//				c.val = (c.val * 10 + ptr->dat.win - ptr->dat.lose) / (ptr->dat.win + ptr->dat.lose + 10);
//				ptr = ptr->sibling;
//			}
//		}else if(ptr->date.win + ptr->dat.lose == threshold){
//		}else{}
//			pos1 = brd.play(mthd,true,height);
//			if(pos1.x >= 0){
//				++ptr;
//				ptr->tra = trace({false,pos1.x + (pos1.y << 3)});
//			}
//
//		pos2 = brd.play(mthd,false,height);
//		if(pos2.x >= 0){
//			ptr->pos = pos2.x + (pos2.y << 3);
//			ptr->color = true;
//			++ptr;
//		}
//	}while(pos1.x >= 0 || pos2.x >= 0);
//
//	ptr->pos = -1;
//	calc_type result = brd.count(true) - brd.count(false);
//	
//	return tuple<trace*,bool,bool>(path,result != 0,result > 0);
//}

tuple<trace*,bool,bool> do_practice(method mthd,cshort height){
	board brd;
	coordinate pos1,pos2;
	trace* path = new trace[board::size2];
	trace* ptr = path;

	brd.initial();
	do{
		pos1 = brd.play(mthd,true,height);
		if(pos1.x >= 0){
			ptr->pos = pos1.x + (pos1.y << 3);
			ptr->color = false;
			++ptr;
		}

		pos2 = brd.play(mthd,false,height);
		if(pos2.x >= 0){
			ptr->pos = pos2.x + (pos2.y << 3);
			ptr->color = true;
			++ptr;
		}
	}while(pos1.x >= 0 || pos2.x >= 0);

	ptr->pos = -1;
	calc_type result = brd.count(true) - brd.count(false);
	
	return tuple<trace*,bool,bool>(path,result != 0,result > 0);
}

void tree::practice(method mthd,cshort height){
	auto record = do_practice(mthd,height);

	// auto ptr = std::get<0>(record);
	// for(int i = 0;i != 64;++i){
		// cout << "(" << ptr->pos << "," << ptr->color << ")" << endl;
		// ++ptr;
	// }

	if(std::get<1>(record)){
		this->add_path(std::get<0>(record),std::get<2>(record));
	}
	delete std::get<0>(record);
}
