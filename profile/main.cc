#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

#include "../cpp/jsoncpp/json.h"

#include "../cpp/board.h"
#include "../cpp/pattern.h"

using namespace std;
 
int main(int argc, char *argv[], char *envp[]){
	int x,y;
	bool mycolor;

	board brd;
	board::config();
	pattern::config();
	ptn.load("../data/pattern.dat");
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
 			brd.flip(!mycolor,x + (y << 3)); // 模拟对方落子

 		x = input["responses"][i]["x"].asInt();
 		y = input["responses"][i]["y"].asInt();
 		if (x >= 0)
 		 	brd.flip(mycolor,x + (y << 3)); // 模拟己方落子
 	}

 	// 看看自己本回合输入
 	x = input["requests"][turns]["x"].asInt();
 	y = input["requests"][turns]["y"].asInt();

 	if (x >= 0)
 		brd.flip(!mycolor,x + (y << 3)); // 模拟对方落子

	method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_mtdf | mthd_ptn | mthd_trans);
	short depth = 14;
	auto p_mthd = brd.process_method(mthd, depth);
	mthd = p_mthd.first;
	depth = p_mthd.second;
	auto best = brd.play(mthd, mycolor, depth);

	// 决策结束，输出结果（你只需修改以上部分）

	result["response"]["x"] = best.pos & 7;
	result["response"]["y"] = best.pos >> 3;
	cout << writer.write(result) << endl;

	return 0;
}
