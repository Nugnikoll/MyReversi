#ifndef TREE_H
#define TREE_H

#include <string>
#include <iostream>
#include <vector>

#include "type.h"

struct data;
struct node;

struct data{
	bool color;
	pos_type pos;
	int win;
	int lose;
};

struct node{
	data dat;
	node* parent;
	node* sibling;
	node* child;
};

typedef node* pnode;
typedef const pnode& cpnode;

class tree{
public:
	node* root;

	tree():count(0){
		root = new node({{true,-1,0,0},NULL,NULL,NULL});
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

	node* descend(board& brd);

	void grow(cmethod mthd,cshort height);

private:
	int count;

	static void destroy(node*& ptr){
		if(ptr){
			destroy(ptr->sibling);
			destroy(ptr->child);
			delete ptr;
		}
	}

	static void print(const node* const& ptr){
		cout<< "((" << ptr->dat.color << "," << ptr->dat.pos << ","
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