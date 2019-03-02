#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>

#include "../../cpp/jsoncpp/json.h"

#include "../../cpp/board.h"
#include "../../cpp/pattern.h"

using namespace std;

int main(int argc, char *argv[], char *envp[]){
	bool color;
	board brd;
	board::config();
	pattern::config();
	ptn.load("../data/pattern.dat");
	brd.initial();

	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input,result;
	Json::FastWriter writer;
	reader.parse(str, input);

	color = input["request"]["color"].asBool();
	brd.get_brd(true) = input["request"]["board"]["black"].asUInt64();
	brd.get_brd(false) = input["request"]["board"]["white"].asUInt64();

	method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_ptn | mthd_trans);
	short depth;
	vector<choice> choices;
	mutex mtx;

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

	choice best = board::select_choice(choices, 0.2);

	result["response"]["x"] = best.pos & 7;
	result["response"]["y"] = best.pos >> 3;
	result["debug"]["depth"] = depth;
	result["debug"]["color"] = color;
	result["debug"]["val"] = best.val;
	result["debug"]["rnd_val"] = best.rnd_val;
	result["debug"]["board"]["black"] = brd.get_brd(true);
	result["debug"]["board"]["white"] = brd.get_brd(false);

	cout << writer.write(result) << flush;

	return 0;
}
