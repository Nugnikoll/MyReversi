#include <iostream>

using namespace std;

typedef unsigned long long ull;

int main(){
	ull a = 0, b = 0, c = 0;
	asm volatile(
		"pext %2, %1, %0\n"
		:"=r"(c)
		:"r"(b), "r"(a)
	);

	return 0;
}
