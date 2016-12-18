//#pragma GCC diagnostic error "-std=c++11"
#pragma GCC optimize ("O3")

#include <iostream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "jsoncpp/json.h"

#include "reversi.h" //++
//#include "reversi.cc" //++
#include "pattern.h" //++
//#include "pattern.cc" //++
#include "data/data.h" //++

using namespace std;
 
int main(){
	int x,y;
	bool mycolor;

	board brd;
	brd.initial();
 
 	// input JSON
 	string str;
 	getline(cin, str);
 	Json::Reader reader;
 	Json::Value input,result;
	Json::FastWriter writer;
 	reader.parse(str, input);

 	// 分析自己收到的输入和自己过往的输出，并恢复状态
 	int turns = input["responses"].size();
 	mycolor = (input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0);

 	for (int i = 0; i < turns; i++)
 	{
 		// 根据这些输入输出逐渐恢复状态到当前回合
 		x = input["requests"][i]["x"].asInt();
 		y = input["requests"][i]["y"].asInt();
 		if (x >= 0)
 			brd.flip(!mycolor,x,y); // 模拟对方落子

 		x = input["responses"][i]["x"].asInt();
 		y = input["responses"][i]["y"].asInt();
 		if (x >= 0)
 		 	brd.flip(mycolor,x,y); // 模拟己方落子
 	}

 	// 看看自己本回合输入
 	x = input["requests"][turns]["x"].asInt();
 	y = input["requests"][turns]["y"].asInt();

 	if (x >= 0)
 		brd.flip(!mycolor,x,y); // 模拟对方落子

	ptr_pattern = new pattern;
	ptr_pattern->decompress((float*)(data_table + 3 * 8));
 	auto coord = brd.play(mthd_ptn,mycolor);

	// 决策结束，输出结果（你只需修改以上部分）

	result["response"]["x"] = coord.x;
	result["response"]["y"] = coord.y;
	cout << writer.write(result) << endl;

	return 0;
}
