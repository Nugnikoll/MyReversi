#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>

#include "../../cpp/jsoncpp/json.h"

#include "../../cpp/board.h"
#include "../../cpp/pattern.h"

using namespace std;

int main(int argc, char *argv[], char *envp[]){

	bool flag_time = true;
	bool flag_continue = false;
	bool flag_pattern = true;
	int arg_time = 900;
	int arg_depth = 8;
	string data_path = "../data/pattern.dat";

	for(int i = 1; i != argc; ++i){
		if(!strcmp(argv[i], "-c")){
			flag_continue = true;
		}else if(!strcmp(argv[i], "-t")){
			++i;
			if(i == argc){
				cout << "missing parameter" << endl;
				return 1;
			}
			flag_time = true;
			arg_time = atoi(argv[i]);
		}else if(!strcmp(argv[i], "-d")){
			++i;
			if(i == argc){
				cout << "missing parameter" << endl;
				return 1;
			}
			flag_time = false;
			arg_depth = atoi(argv[i]);
		}else if(!strcmp(argv[i], "-p")){
			++i;
			if(i == argc){
				cout << "missing parameter" << endl;
				return 1;
			}
			flag_pattern = true;
			data_path = argv[i];
		}else if(!strcmp(argv[i], "-w")){
			++i;
			if(i == argc){
				cout << "missing parameter" << endl;
				return 1;
			}
			flag_pattern = false;
			data_path = argv[i];
		}else{
			cout << "unrecognized option" << endl;
			return 1;
		}
	}

	bool color;
	board brd;
	if(flag_pattern){
		board::config();
		pattern::config();
		ptn.load(data_path);
	}else{
		board::config(data_path);
	}
	brd.initial();

	string str;
	while(getline(cin, str)){
		Json::Reader reader;
		Json::Value input,result;
		Json::FastWriter writer;
		reader.parse(str, input);

		color = input["request"]["color"].asBool();
		brd.get_brd(true) = input["request"]["board"]["black"].asUInt64();
		brd.get_brd(false) = input["request"]["board"]["white"].asUInt64();

		method mthd;
		if(flag_pattern){
			mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_ptn | mthd_trans);
		}else{
			mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_trans);
		}
		short depth;
		vector<choice> choices;
		mutex mtx;

		if(flag_time){
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

			this_thread::sleep_for(chrono::milliseconds(arg_time));
			mtx.lock();
		}else{
			auto p_mthd = brd.process_method(mthd, arg_depth);
			auto temp = brd.get_choice(p_mthd.first, color, p_mthd.second);
			depth = p_mthd.second;
			choices = temp;
		}

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

		if(!flag_continue){
			break;
		}
	}

	return 0;
}
