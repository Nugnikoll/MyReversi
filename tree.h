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

	void print()const{
		print(root);
	}

	void save(const string& path);
	void load(const string& path);

	void add_path( pos_type* path,cbool is_win){
		return add_path(root->child,path,is_win);
	}
	void add_path(node*& ptr, pos_type* path,cbool is_win){
		if(*path > 0){
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
				return add_path(ptr->child,++path,is_win);
			}
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

	static void print(const node* const& ptr){
		cout<< "(" << ptr->dat.id << "," << ptr->dat.pos << ") "
			<< (ptr->child != NULL) << " "
			<< (ptr->sibling != NULL) << endl;
		
		if(ptr->child){
			print(ptr->child);
		}
		if(ptr->sibling){
			print(ptr->sibling);
		}
	}

	static void save(ostream& out,const node* const& ptr);
};

#endif //TREE_H