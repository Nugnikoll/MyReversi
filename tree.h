#ifndef TREE_H
#define TREE_H

#include <string>
#include <iostream>

#include "type.h"

struct data{
	int id;
	bool color;

	pos_type pos;

	int win;
	int lose;
};

struct node{
	data dat;
	node* sibling;
	node* child;
};

typedef node* pnode;
typedef const pnode& cpnode;

class tree{
public:
	tree():count(0){
		root = new node({{0,true,-1,0,0},NULL,NULL});
	}
	~tree(){
		destroy(root);
	}

	void save(const string& path);
	void load(const string& path);

	void add_path(const pos_type* path,cbool is_win){
		return add_path(root->child,path,is_win);
	}
	void add_path(node* ptr,const pos_type* path,cbool is_win){
		if(path > 0){
			if(ptr){
				if(ptr->dat.pos == *path){
					if(ptr->dat.color ^ is_win){
						++ptr->dat.win;
					}else{
						++ptr->dat.lose;
					}
					return add_path(ptr->child,++path,is_win);
				}
				return add_path(ptr->sibling,path,is_win);
			}else{
				ptr = new node({{++count,true,*path,0,0},NULL,NULL});
			}
		}else{
			ptr = new node({{++count,true,*path,0,0},NULL,NULL});
		}
	}

private:
	node* root;
	int count;

	static void destroy(node*& ptr){
		if(ptr){
			destroy(ptr->sibling);
			destroy(ptr->child);
			delete ptr;
		}
	}

	static void save(ostream& out,const node* const& ptr);
};

#endif //TREE_H