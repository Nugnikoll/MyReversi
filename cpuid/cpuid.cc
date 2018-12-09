#include <iostream>
#include <bitset>
#include <string>

using namespace std;

int main(){
	int num;
	char vendor[13];
	char* ptr = vendor;
	unsigned int a, b, c, d;
	vendor[12] = '\0';
	bitset<32> bits;

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

	cout << "vendor: " << vendor << endl;
	cout << "highest parameter: " << num << endl;

	if(num >= 1){
		char table_d[32][8] = {
			"fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
			"cx8", "apic", "", "sep", "mtrr", "pge", "mca", "cmov",
			"pat", "pse-36", "psn", "clfsh", "", "ds", "acpi", "mmx",
			"fxsr", "sse", "sse2", "ss", "htt", "tm", "ia64", "pbe"
		};

		char table_c[32][16] = {
			"sse3", "pclmulqdq", "dtes64", "monitor", "ds-cpi", "vmx", "smx", "est",
			"tm2", "ssse3", "cnxt-id", "sdbg", "fma", "cx16", "xtpr", "pdcm",
			"", "pcid", "dca", "sse4.1", "sse4.2", "x2apic", "movbe", "popcnt",
			"tsc-deadline", "aes", "xsave", "osxsave", "avx", "f16c", "rdrnd", "hypervisor"
		};

		asm volatile(
			"mov $1, %%eax\n"
			"cpuid\n"
			:"=d"(d), "=c"(c)
			:
			:"eax", "ebx"
		);

		cout << "eax = 1" << endl;
		bits = d;
		cout << bits << endl;
		for(int i = 0; i != 32; ++i){
			if(d & (1 << i)){
				cout << table_d[i] << " ";
			}
		}
		cout << endl;
		bits = c;
		cout << bits << endl;
		for(int i = 0; i != 32; ++i){
			if(c & (1 << i)){
				cout << table_c[i] << " ";
			}
		}
		cout << endl;
	}

	if(num >= 7){
		char table_b[32][16] = {
			"fsgsbase", "", "sgx", "bmi1", "hle", "avx2", "", "smep",
			"bmi2", "erms", "invpcid", "rtm", "pqm", "", "mpx", "pqe",
			"avx512f", "avx512dq", "rdseed", "adx", "smap", "avx512ifma", "pcommit", "clflushopt",
			"clwb", "intel_pt", "avx512pf", "avx512er", "avx512cd", "sha", "avx512bw", "abx512vl"
		};
		char table_c[32][16] = {
			"prefetchwt1", "avx512vbmi", "umip", "pku", "ospke", "", "avx512vbmi2", "",
			"gfni", "vaes", "vpclmulqdq", "avx512vnni", "avx512bitalg", "avx512vpopcntdq", "",
			"", "mawau", "mawau", "mawau", "mawau", "mawau", "rdpid", "",
			"", "", "", "", "", "", "sgx_lc", ""
		};

		asm volatile(
			"mov $7, %%eax\n"
			"mov $0, %%ecx\n"
			"cpuid\n"
			:"=b"(b), "=c"(c)
			:
			:"eax", "edx"
		);

		cout << "eax = 7" << endl;
		bits = b;
		cout << bits << endl;
		for(int i = 0; i != 32; ++i){
			if(b & (1 << i)){
				cout << table_b[i] << " ";
			}
		}
		cout << endl;
		bits = c;
		cout << bits << endl;
		for(int i = 0; i != 32; ++i){
			if(c & (1 << i)){
				cout << table_c[i] << " ";
			}
		}
		cout << endl;
	}

	return 0;
}
