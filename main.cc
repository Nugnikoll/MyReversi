#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "cpptcl.h"
#include "reversi.h"

using namespace std;
using namespace Tcl;

void quit(){
	exit(0);
}

int main(){

	interpreter inter;
	#ifdef NDEBUG
		inter.make_safe();
	#endif
	inter.def("quit",quit);
	inter.def("exit",quit);
//	#ifdef DEBUG
//		inter.class_<board>("board",init<const brd_type&,const brd_type&>())
//			.def("print",&board::print)
//			//.def("get",&board::visit)
//			;
//	#endif

	board brd(123,2314);
	cout << brd.get<true>() << " " << brd.get<false>() << endl;


	string str;
	while(getline(cin,str)){
		try{
			inter.eval(str);
		}catch(const tcl_error& err){
			cout << err.what() << endl;
		}
	}


//	 long long a, b;
//	 cin >> hex >> a >> hex >> b;
//	 asm volatile(
//		 "packsswb %2,%0;"
//		 :"=y"(a)
//		 :"0"(a),"y"(b)
//		 :
//	 );
//	 cout << hex << a << " " << hex << b;
	return 0;
}
