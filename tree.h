#ifndef TREE_H
#define TREE_H

#include <string>
#include <iostream>

#include "type.h"

struct trace{
	bool color;
	pos_type pos;
};

struct data{
	trace tra;
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
		root = new node({{true,-1,0,0},NULL,NULL});
	}
	~tree(){
		destroy(root);
	}

	static const int threshold = 10;

	void print()const{
		print(root);
	}

	void save(const string& path);
	void load(const string& path);

	void add_path(trace* path, cbool is_win){
		node* head = root;
		bool flag;
		for(node* ptr = head->child;path->pos >= 0;++path){
			flag = true;
			while(ptr){
				flag = false;
				assert(ptr->dat.tra.color == path->color);
				if(ptr->dat.tra.pos == path->pos){
					if(ptr->dat.tra.color ^ is_win){
						++ptr->dat.win;
					}else{
						++ptr->dat.lose;
					}
					if(ptr->dat.win + ptr->dat.lose >= threshold){
						goto label;
					}else{
						return;
					}
				};
				head = ptr;
				ptr = ptr->sibling;
			}

			(flag ? head->child : head->sibling)
				= (
					path->color ^ is_win
					? new node({{*path,1,0},NULL,NULL})
					: new node({{*path,0,1},NULL,NULL})
				);
			++count;
			break;

			label:
			head = ptr;
			ptr = ptr->child;
		}
	}

	void practice(method mthd,cshort height);

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
		cout<< "(((" << ptr->dat.tra.color << "," << ptr->dat.tra.pos << "),"
			<< ptr->dat.win << "," << ptr->dat.lose << "),"
			<< (ptr->child != NULL) << ","
			<< (ptr->sibling != NULL) << ")" << endl;
		
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