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

node* tree::descend(board& brd){
	node* ptr = this->root;
	vector<node*> vec;
	vector<float> param;

	while(ptr->child){
		if(ptr->dat.color){
			for(ptr = ptr->child;ptr;ptr = ptr->sibling){
				vec.push_back(ptr);
				param.push_back(float(ptr->dat.win + 5) / (ptr->dat.win + ptr->dat.lose + 10));
			}
		}else{
			for(ptr = ptr->child;ptr;ptr = ptr->sibling){
				vec.push_back(ptr);
				param.push_back(float(ptr->dat.lose + 5) / (ptr->dat.win + ptr->dat.lose + 10));
			}
		}
		discrete_distribution<int> selector(param.begin(),param.end());
		ptr = vec[selector(engine)];
		brd.flip(ptr->parent->dat.color,ptr->dat.pos);
		// cout << "color: " << ptr->parent->dat.color << endl;
		// cout << "x: " << (ptr->dat.pos & 7) << " y: " << (ptr->dat.pos >> 3) << endl;
		// brd.print();
		vec.clear();
		param.clear();
	}
	return ptr;
}

void tree::grow(cmethod mthd,cshort height){
	board brd;
	coordinate pos1,pos2;

	brd.initial();
	node* ptr = descend(brd);

	bool flag = false, flag_next = false, color = ptr->dat.color;
	board brd_save = brd;

	do{
		flag_next = flag;
		flag = brd.play(mthd,color,height).x >= 0;
		color = !color;
	}while(flag || flag_next);

	calc_type diff = brd.count(true) - brd.count(false);

	if(ptr->dat.win + ptr->dat.lose >= threshold - 2){
		node* ptr_next = ptr;
		brd = brd_save;
		pos_type i;
		for(i = 0;i != board::size2;++i){
			if(brd.flip(ptr->dat.color,i)){
				flag = (brd.count_move(!ptr->dat.color) > 0) ^ ptr->dat.color;
				ptr_next->child = new node({{flag,i,0,0},ptr,NULL,NULL});
				ptr_next = ptr_next->child;
				brd = brd_save;
				++i;
				break;
			}
		}
		for(;i != board::size2;++i){
			if(brd.flip(ptr->dat.color,i)){
				flag = (brd.count_move(!ptr->dat.color) > 0) ^ ptr->dat.color;
				ptr_next->sibling = new node({{flag,i,0,0},ptr,NULL,NULL});
				ptr_next = ptr_next->sibling;
				brd = brd_save;
			}
		}
	}

	// if(diff == 0){
		// while(ptr){
			// ++ptr->dat.draw;
			// ptr = ptr->parent;
		// }
	// }else 
	if(diff > 0){
		while(ptr){
			++ptr->dat.win;
			ptr = ptr->parent;
		}
	}else if(diff < 0){
		while(ptr){
			++ptr->dat.lose;
			ptr = ptr->parent;
		}
	}
}
