#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <initializer_list>
#include <cassert>

#include "type.h"

template<typename T>
class matrix{
public:
    matrix(): height(0), width(0), table(NULL){}

    matrix(const int& h,const int& w): height(h), width(w){
		table = new T[height * width];
    }

    matrix(int h,int w, T* ptr): height(h), width(w){
		table = new T[height * width];
		memcpy(table,ptr,sizeof(T) * height * width);
    }

    matrix(std::initializer_list<std::initializer_list<T>> m)
		: height(m.size()), width(m.begin()->size()){
        table = new T[height * width];
        T* ptr = table;
		for(auto& i:m){
			for(const T& j:i){
				*ptr = j;
				++ptr;
			}
		}
    }

    matrix(const matrix& m): height(m.height), width(m.width){
		int size = height * width;
        table = new T[size];
        memcpy(table,m.table,sizeof(T) * size);
    }

    matrix(matrix&& m): height(m.height), width(m.width){
        table = m.table;
		m.table = NULL;
    }

	template<typename Ty>
    matrix(const matrix<Ty>& m): height(m.geth()), width(m.getw()){
		int size = height * width;
        table = new T[size];
		for(int i = 0;i != size;++i){
			table[i] = m.at(i);
		}
    }

    ~matrix(){
		delete[] table;
    }

	void numpy(int* h, int* w, T** ptr)const{
		*h = height;
		*w = width;
		*ptr = table;
	}

    matrix& operator=(const matrix& m){
		delete[] table;

        height = m.height;
        width = m.width;
        table = new T[height * width];
		memcpy(table,m.table,sizeof(T) * height * width);
        return *this;
    }

    matrix& operator=(matrix&& m){
		delete[] table;

        height = m.height;
        width = m.width;
        table = m.table;
		m.table = NULL;
		return *this;
    }

	void load(const std::string& filename){
		ifstream fin(filename);
		fin.read((char *)(&height),sizeof(height));
		fin.read((char *)(&width),sizeof(width));
		delete[] table;
		table = new T[height * width];
		fin.read((char *)(table),sizeof(T) * height * width);
		fin.close();
	}

	void save(const std::string& filename)const{
		ofstream fout(filename);
		fout.write((const char *)(&height),sizeof(height));
		fout.write((const char *)(&width),sizeof(width));
		fout.write((const char *)(table),sizeof(T) * height * width);
		fout.close();
	}

    const T* operator[](const int& h)const{
        return table + h * width;
    }
    T* operator[](const int& h){
        return table + h * width;
    }

    int geth()const{return this->height;}
    int getw()const{return this->width;}

	T& at(const int& h,const int& w){
		return table[h * width + w];
	}

	const T& at(const int& h,const int& w)const{
		return table[h * width + w];
	}

	T& at(const int& n){
		return table[n];
	}

	const T& at(const int& n)const{
		return table[n];
	}

	T __getitem__(const int& n)const{
		assert(n < height * width);
		return table[n];
	}

	void __setitem__(const int& n, const T& t){
		assert(n < height * width);
		table[n] = t;
	}

private:
    int height;
    int width;
    T* table;
};

#endif // MATRIX_H
