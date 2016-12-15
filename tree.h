#ifndef TREE_H
#define TREE_H

#include "type.h"

struct node{
	int id;
	//bool color;

	node* sibling;
	node* child;

	pos_type pos;

	int win;
	int lose;
};

typedef node* pnode;
typedef const pnode& cpnode;

class tree{
public:
	tree():count(0){
		root = new node({0,NULL,NULL,-1,0,0});
	}
	~tree(){
		destroy(root);
	}

	void add_path(const pos_type* path,cbool is_win){
		return add_path(root->child,path,is_win);
	}
	void add_path(node* ptr,const pos_type* path,cbool is_win){
		if(path > 0){
			if(ptr){
				if(ptr->pos == *path){
					if(is_win){
						++ptr->win;
					}else{
						++ptr->lose;
					}
					return add_path(ptr->child,++path,is_win);
				}
				return add_path(ptr->sibling,path,is_win);
			}else{
				ptr = new node({++count,NULL,NULL,*path});
			}
		}else{
			ptr = new node({++count,NULL,NULL,*path});
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
};

#endif //TREE_H