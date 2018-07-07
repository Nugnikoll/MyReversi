#include <iostream>

using namespace std;

#if defined(__GNUC__) || defined(__clang__)
#endif

int main(){
	int num;
	char vendor[13];
	char* ptr = vendor;
	int mask;

	vendor[12] = '\0';

	asm volatile(
		"mov $0, %%eax\n"
		"cpuid\n"
		"mov %%ebx, (%1)\n"
		"mov %%edx, 4(%1)\n"
		"mov %%ecx, 8(%1)\n"
		:"=&a"(num)
		:"r"(ptr)
		:"ebx", "ecx", "edx", "memory"
	);

	cout << "num: " << num << " vendor: " << vendor << endl;

	if(num >= 7){
		asm volatile(
			"mov $7, %%eax\n"
			"mov $0, %%ecx\n"
			"cpuid\n"
			:"=b"(mask)
			:
			:"eax", "ecx"
		);

		cout << hex << mask << endl;
	}

}
