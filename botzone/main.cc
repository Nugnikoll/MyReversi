#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>

#ifdef _BOTZONE_ONLINE
	#include "jsoncpp/json.h"
#else
	#include "../cpp/jsoncpp/json.h"
#endif

#include "../cpp/board.h"
#include "../cpp/pattern.h"

using namespace std;
 
int main(int argc, char *argv[], char *envp[]){
	int x,y;
	bool color;

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
 	color = (input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0);

 	for (int i = 0; i < turns; i++)
 	{
 		// 根据这些输入输出逐渐恢复状态到当前回合
 		x = input["requests"][i]["x"].asInt();
 		y = input["requests"][i]["y"].asInt();
 		if (x >= 0)
 			brd.flip(!color,x + (y << 3)); // 模拟对方落子

 		x = input["responses"][i]["x"].asInt();
 		y = input["responses"][i]["y"].asInt();
 		if (x >= 0)
 		 	brd.flip(color,x + (y << 3)); // 模拟己方落子
 	}

 	// 看看自己本回合输入
 	x = input["requests"][turns]["x"].asInt();
 	y = input["requests"][turns]["y"].asInt();
	if (x >= 0)
		brd.flip(!color,x + (y << 3)); // 模拟对方落子

	method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_ptn | mthd_trans);
	short depth;
	vector<choice> choices;
	mutex mtx;

	if(brd.sum() >= 64 - 16){
		depth = 64 - brd.sum();
		choices = brd.get_choice(method(mthd | mthd_end | mthd_mtdf), color, depth);
	}else{
		auto fun = [&](){
			for(short i = 2; i != 100; ++i){
				auto p_mthd = brd.process_method(mthd, i);
				auto temp = brd.get_choice(p_mthd.first, color, p_mthd.second);
				mtx.lock();
				depth = i;
				choices = temp;
				mtx.unlock();
			}
		};

		thread thrd(fun);
		thrd.detach();

		this_thread::sleep_for(chrono::milliseconds(900));
		mtx.lock();
	}

	choice best{brd, 0, 0, -1};
	if(!choices.empty()){
		best = board::select_choice(choices, 0.2);
	}

	// 决策结束，输出结果（你只需修改以上部分）

	if(best.pos >= 0){
		result["response"]["x"] = best.pos & 7;
		result["response"]["y"] = best.pos >> 3;
	}else{
		result["response"]["x"] = -1;
		result["response"]["y"] = -1;
	}
	result["debug"]["depth"] = depth;
	result["debug"]["color"] = color;
	result["debug"]["val"] = best.val;
	result["debug"]["rnd_val"] = best.rnd_val;
	#ifdef _BOTZONE_ONLINE
		result["debug"]["board"]["black"] = to_string(brd.bget(true));
		result["debug"]["board"]["white"] = to_string(brd.bget(false));
	#else
		result["debug"]["board"]["black"] = brd.bget(true);
		result["debug"]["board"]["white"] = brd.bget(false);
	#endif
	for(unsigned int i = 0; i != choices.size(); ++i){
		result["debug"]["choice"][i]["pos"] = choices[i].pos;
		result["debug"]["choice"][i]["val"] = choices[i].val;
	}

	cout << writer.write(result) << endl;

	return 0;
}
