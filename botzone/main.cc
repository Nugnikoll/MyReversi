#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>

#ifdef _BOTZONE_ONLINE
	#include "jsoncpp/json.h"
#else
	#include "../cpp/jsoncpp/json.h"
#endif

#ifndef USE_TERMINATE
	#define USE_TERMINATE
#endif
#include "../cpp/board.h"
#include "../cpp/pattern.h"

using namespace std;

int main(int argc, char *argv[], char *envp[]){
	int x,y;
	bool color;

	board brd;
	board::config();
	#ifdef _BOTZONE_ONLINE
		pattern::config("../data/pattern_test.dat");
	#else
		pattern::config("../data/pattern.dat");
	#endif
	brd.initial();

	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input,result;
	Json::FastWriter writer;
	reader.parse(str, input);

	int turns = input["responses"].size();
	color = (input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0);

	for (int i = 0; i < turns; i++)
	{
		x = input["requests"][i]["x"].asInt();
		y = input["requests"][i]["y"].asInt();
		if (x >= 0)
			brd.flip(!color,x + (y << 3));

		x = input["responses"][i]["x"].asInt();
		y = input["responses"][i]["y"].asInt();
		if (x >= 0)
			brd.flip(color,x + (y << 3));
	}

	x = input["requests"][turns]["x"].asInt();
	y = input["requests"][turns]["y"].asInt();
	if (x >= 0)
		brd.flip(!color,x + (y << 3));

	method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_ptn | mthd_trans | mthd_mtdf);
	short depth;
	choice best{brd, _inf, inf, -1};
	int depth_limit = 64 - brd.sum();
	val_type principle_value[64];

	auto fun = [&](){
		try{
			for(short i = 1; i <= depth_limit; ++i){
				auto p_mthd = brd.process_method(mthd, i);
				val_type gamma;
				if(p_mthd.second >= 4){
					gamma = principle_value[i - 2];
				}else{
					gamma = inf;
				}
				auto choices = brd.get_choice(p_mthd.first, color, p_mthd.second, _inf, inf, gamma);
				if(choices.empty()){
					break;
				}
				depth = i;
				principle_value[i] = max_element(
					choices.begin(), choices.end(),
					[](const choice& c1, const choice& c2) -> bool{
						return c1.beta < c2.beta;
					}
				)->beta;
				best = board::select_choice(choices);
			}
		}catch(timeout_exception){}
	};

	flag_timeout = false;
	thread thrd(fun);

	this_thread::sleep_for(900ms);
	
	flag_timeout = true;
	thrd.join();
	flag_timeout = false;

	if(best.pos >= 0){
		result["response"]["x"] = best.pos & 7;
		result["response"]["y"] = best.pos >> 3;
	}else{
		result["response"]["x"] = -1;
		result["response"]["y"] = -1;
	}
	result["debug"]["depth"] = depth;
	result["debug"]["color"] = color;
	result["debug"]["alpha"] = best.alpha;
	result["debug"]["beta"] = best.beta;
	#ifdef _BOTZONE_ONLINE
		result["debug"]["board"]["black"] = to_string(brd.get_brd(true));
		result["debug"]["board"]["white"] = to_string(brd.get_brd(false));
	#else
		result["debug"]["board"]["black"] = brd.get_brd(true);
		result["debug"]["board"]["white"] = brd.get_brd(false);
	#endif

	cout << writer.write(result) << endl;

	return 0;
}
