#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <cstring>
#include <initializer_list>
#include <cassert>

template<typename T>
class matrix{
public:
    matrix():height(0),width(0),table(NULL){}
    matrix(const int& h,const int& w):height(h),width(w){
		table = new T[height * width];
    }
    matrix(const int& h,const int& w,const T* const& ptr):height(h),width(w){
		table = new T[height];
		memcpy(table,ptr,sizeof(T) * height * width);
    }
    matrix(std::initializer_list<std::initializer_list<T>> m)
		:height(m.size()),width(m.begin()->size()){
        table = new T[height * width];
        T* ptr = table;
		for(auto& i:m){
			for(const T& j:i){
				*ptr = j;
				++ptr;
			}
		}
    }
    matrix(const matrix& m):height(m.height),width(m.width){
		int size = height * width;
        table = new T[size];
        memcpy(table,m.table,sizeof(T) * size);
    }
    matrix(matrix&& m):height(m.height),width(m.width){
        table = m.table;
		m.table = NULL;
    }
	template<typename Ty>
    matrix(const matrix<Ty>& m):height(m.geth()),width(m.getw()){
		int size = height * width;
        table = new T[size];
		for(int i = 0;i != size;++i){
			table[i] = m.at(i);
		}
    }

    ~matrix(){
        if(table){
            delete[] table;
        }
    }

    matrix& operator=(const matrix& m){
        if(table){
            delete[] table;
        }

        height = m.height;
        width = m.width;
        table = new T[height * width];
		memcpy(table,m.table,sizeof(T) * height * width);
        return *this;
    }
    matrix& operator=(matrix&& m){
        if(table){
            delete[] table;
        }

        height = m.height;
        width = m.width;
        table = m.table;
		m.table = NULL;
		return *this;
    }
    matrix operator+()const{
        return *this;
    }
    matrix operator-()const{
		matrix result(height,width);
		int size = height * width;
		for(int i = 0;i != size;++i){
			result.table[i] = -table[i];
		}
        return result;
    }
    matrix& operator+=(const matrix& m){
        assert(height == m.height && width == m.width);
		int size = height * width;
        for(int i = 0;i != size;++i){
			table[i] += m.table[i];
		}
        return *this;
    }
    matrix& operator+=(const T& m){
		int size = height * width;
        for(int i = 0;i != size;++i){
			table[i] += m;
		}
        return *this;
    }
    matrix& operator-=(const matrix& m){
        assert(height == m.height && width == m.width);
		int size = height * width;
        for(int i = 0;i != size;++i){
			table[i] -= m.table[i];
		}
        return *this;
    }
    matrix& operator-=(const T& m){
		int size = height * width;
        for(int i = 0;i != size;++i){
			table[i] -= m;
		}
        return *this;
    }
    matrix& operator*=(const matrix& m){
        return *this = *this * m;
    }
    matrix& operator*=(const T& m){
		int size = height * width;
        for(int i = 0;i != size;++i){
			table[i] *= m;
		}
        return *this;
    }
    matrix& operator/=(const T& m){
		int size = height * width;
        for(int i = 0;i != size;++i){
			table[i] /= m;
		}
        return *this;
    }
//    matrix& operator%=(const T& m){
//		int size = height * width;
//        for(int i = 0;i != size;++i){
//			table[i] %= m;
//		}
//        return *this;
//    }

    matrix& input(std::istream& in = std::cin){
		int size = height * width;
        for(int i = 0;i != size;++i){
			in >> table[i];
		}
        return *this;
    }
    const matrix& output(std::ostream& out = std::cout)const{
        return this->do_output(out);
    }
    matrix& output(std::ostream& out = std::cout){
        this->do_output(out);
        return *this;
    }

    bool operator==(const matrix& m)const{
        assert(height == m.height && width == m.width);
        matrix result(height,width);
		int size = height * width;
        for(int i = 0;i != size;++i){
			if(table[i] != m.table[i])
				return false;
        }
        return true;
    }
    bool operator!=(const matrix& m)const{
        assert(height == m.height && width == m.width);
        matrix result(height,width);
		int size = height * width;
        for(int i = 0;i != size;++i){
			if(table[i] != m.table[i])
				return true;
        }
        return false;
    }

    matrix operator+(const matrix& m)const{
        assert(height == m.height && width == m.width);
        matrix result(height,width);
		int size = height * width;
        for(int i = 0;i != size;++i){
			result.table[i] = table[i] + m.table[i];
        }
        return result;
    }
    friend matrix operator+(const matrix& m1,const T& m2){
        matrix result(m1.height,m1.width);
		int size = m1.height * m1.width;
        for(int i = 0;i != size;++i){
			result.table[i] = m1.table[i] + m2;
        }
        return result;
    }
    friend matrix operator+(const T& m1,const matrix& m2){
        matrix result(m2.height,m2.width);
		int size = m2.height * m2.width;
        for(int i = 0;i != size;++i){
			result.table[i] = m1 + m2.table[i];
        }
        return result;
    }
    matrix operator-(const matrix& m)const{
        assert(height == m.height && width == m.width);
        matrix result(height,width);
		int size = height * width;
        for(int i = 0;i != size;++i){
			result.table[i] = table[i] - m.table[i];
        }
        return result;
    }
    friend matrix operator-(const matrix& m1,const T& m2){
        matrix result(m1.height,m1.width);
		int size = m1.height * m1.width;
        for(int i = 0;i != size;++i){
			result.table[i] = m1.table[i] - m2;
        }
        return result;
    }
    friend matrix operator-(const T& m1,const matrix& m2){
        matrix result(m2.height,m2.width);
		int size = m2.height * m2.width;
        for(int i = 0;i != size;++i){
			result.table[i] = m1 - m2.table[i];
        }
        return result;
    }
    matrix operator*(const matrix& m)const{
        assert(width == m.height);
        matrix result(height,m.width);
        for(int i = 0;i != height;++i){
            for(int j = 0;j != m.width;++j){
                result.table[i * m.width + j] = 0;
                for(int k = 0;k != width;++k)
                    result.table[i * m.width + j] += table[i * width + k] * m.table[k * m.width + j];
            }
        }
        return result;
    }
    friend matrix operator*(const matrix& m1,const T& m2){
        matrix result(m1.height,m1.width);
		int size = m1.height * m1.width;
        for(int i = 0;i != size;++i){
			result.table[i] = m1.table[i] * m2;
        }
        return result;
    }
    friend matrix operator*(const T& m1,const matrix& m2){
        matrix result(m2.height,m2.width);
		int size = m2.height * m2.width;
        for(int i = 0;i != size;++i){
			result.table[i] = m1 * m2.table[i];
        }
        return result;
    }
    const T* operator[](const int& h)const{
        return table + h * width;
    }
    T* operator[](const int& h){
        return table + h * width;
    }

    int geth()const{return this->height;}
    int getw()const{return this->width;}

    const T& at(const int& h,const int& w)const{
        assert(h <= height || w <= width);
        return table[h * width + w];
    }
    T& at(const int& h,const int& w){
        assert(h <= height || w <= width);
        return table[h * width + w];
    }
    const T& at(const int& n)const{
        assert(n <= height * width);
        return table[n];
    }
    T& at(const int& n){
        assert(n <= height * width);
        return table[n];
    }

    matrix transpose()const{
		matrix result(width,height);
		for(int i = 0;i != height;++i){
			for(int j = 0;j != width;++j){
				result.table[j * height + i] = table[i * width + j];
			}
		}
		return result;
    }

	matrix& reshape(const int& h,const int& w){
		assert(height * width == h * w);
		height = h;
		width = w;
		return *this;
	}

	T modulus(){
		T result = 0;
		int size = height * width;
        for(int i = 0;i != size;++i){
			result += table[i] * table[i];
        }
		return result;
	}

private:
    int height;
    int width;
    T* table;

    const matrix& do_output(std::ostream& out = std::cout)const{
        if(table == NULL)
            return *this;
        for(int i = 0;i != height;++i){
            for(int j = 0;j != width;++j)
                out << table[i * width + j] << " ";
            out << "\n";
        }
        return *this;
    }
};

#endif // MATRIX_H
