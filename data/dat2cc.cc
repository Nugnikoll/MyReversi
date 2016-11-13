#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc,char* argv[]){
	if(argc <= 2){
		return 1;
	}
	ifstream fin(argv[1],ios::in | ios::binary);
	if(!fin){
		return 2;
		fin.close();
	}

	unsigned char* buffer;
	int size;
	fin.seekg(0,ios_base::end);
	size = fin.tellg();
	fin.seekg(0);
	buffer = new unsigned char[size];
	fin.read((char*)buffer,size);
	fin.close();

	ofstream fout(argv[2],ios::out);
	if(!fout){
		return 3;
		fout.close();
	}
	fout << "unsigned char data_table["<< size <<"] = {";
	if(size > 0){
		fout << short(*buffer);
	}
	for(int i = 1;i != size;++i){
		fout << "," << short(buffer[i]);
	}fout << "};";
	fout.close();

	return 0;
}