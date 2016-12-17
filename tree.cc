#include <fstream>
#include <stack>

#include "tree.h"

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
	root = new node({dat,NULL,NULL});
	ptr = root;

	while(true){
		while(flag_child){
			if(flag_sibling){
				storage.push(ptr);
			}
			_READ(dat);
			_READ(flag_child);
			_READ(flag_sibling);
			ptr->child = new node({dat,NULL,NULL});
			ptr = ptr->child;
		}
		if(storage.empty()){
			break;
		}
		ptr = storage.top();
		storage.pop();
		_READ(dat);
		_READ(flag_child);
		_READ(flag_sibling);
		ptr->sibling = new node({dat,NULL,NULL});
		ptr = ptr->sibling;
	}

	fin.close();
	#undef _READ
}

void tree::save(const string& path){
	#define WRITE(var) fout.write((char *)(&var),sizeof(var))
	ofstream fout(path,ios::out | ios::binary);

	size_t data_size = sizeof(data);

	WRITE(data_size);
	WRITE(this->count);

	save(fout,root);

	fout.close();
	#undef WRITE
}

void tree::save(ostream& out,const node* const& ptr){
	#define WRITE(var) out.write((char *)(&var),sizeof(var))

	WRITE(ptr->dat);
	WRITE(ptr->child != NULL);
	WRITE(ptr->sibling != NULL);

	if(ptr->child){
		save(out,ptr->child);
	}
	if(ptr->sibling){
		save(out,ptr->sibling);
	}

	#undef WRITE
}
