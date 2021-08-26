#include "sample.h"

board random_transform(board brd){
	static uniform_int_distribution<int> dist(0, 7);
	int value = dist(engine);
	switch(value >> 1){
	case 0:
		break;
	case 1:
		brd.rotate_l();
		break;
	case 2:
		brd.rotate_r();
		break;
	case 3:
		brd.reflect();
		break;
	default:
		assert(false);
	}

	if(value & 1){
		brd.mirror_v();
	}

	return brd;
}

void sample_gen(ARRAY_2D_OUT_M(ULL), cint n){
	unordered_set<board> brds;
	board brd, brd_save;
	pos_type pos1, pos2;

	for(int i = 0;i != n;++i){
		brd.initial();
		do{
			brd_save = brd;
			pos1 = brd.play(mthd_rnd, true, 0).pos;
			if(pos1 >= 0){
				brds.insert(brd_save);
			}

			brd_save = brd;
			pos2 = brd.play(mthd_rnd, false, 0).pos;
			if(pos2 >= 0){
				brd_save.reverse();
				brds.insert(brd_save);
			}
		}while(pos1 >= 0 || pos2 >= 0);
	}

	*m1 = brds.size();
	*m2 = 2;
	*ptrm = new ull[*m1 * 2];
	int i = 0;
	for(cboard brd:brds){
		(*ptrm)[i * 2] = brd.get_brd(false);
		(*ptrm)[i * 2 + 1] = brd.get_brd(true);
		++i;
	}
};

void sample_pos_gen(ARRAY_2D_OUT_M(ULL), cint n){
	using brd_pair = pair<board, short>;
	using map_pair = pair<board, brd_pair>;
	unordered_map<board, brd_pair> brds;
	board brd, brd_save;
	pos_type pos1, pos2;

	for(int i = 0;i != n;++i){
		brd.initial();
		do{
			brd_save = brd;
			pos1 = brd.play(mthd_rnd, true, 0).pos;
			if(pos1 >= 0){
				brds.insert(map_pair(brd_save, brd_pair(brd, pos1 | (pos1 << 3))));
			}

			brd_save = brd;
			pos2 = brd.play(mthd_rnd, false, 0).pos;
			if(pos2 >= 0){
				brd_save.reverse();
				board temp = brd;
				temp.reverse();
				brds.insert(map_pair(brd_save, brd_pair(temp, pos2 | (pos2 << 3))));
			}
		}while(pos1 >= 0 || pos2 >= 0);
	}

	*m1 = brds.size();
	*m2 = 5;
	*ptrm = new ull[*m1 * 5];
	int i = 0;
	for(auto& brd:brds){
		(*ptrm)[i * 5] = brd.first.get_brd(false);
		(*ptrm)[i * 5 + 1] = brd.first.get_brd(true);
		(*ptrm)[i * 5 + 2] = brd.second.second;
		(*ptrm)[i * 5 + 3] = brd.second.first.get_brd(false);
		(*ptrm)[i * 5 + 4] = brd.second.first.get_brd(true);
		++i;
	}
};

void sample_gen_select(ARRAY_2D_OUT_M(ULL), cint n, cshort depth, cfloat noise, cbool flag_verbose){
	unordered_set<board> brds;
	board brd;
	int m = 5;
	bool color;
	method mthd = method(mthd_ab | mthd_pvs | mthd_kill | mthd_ptn | mthd_trans);

	while((int)brds.size() < n){
		brd.initial();
		color = true;
		while(true){
			auto p_mthd = brd.process_method(mthd, depth);
			brd.play(p_mthd.first, color, p_mthd.second, noise);
			if(brd.get_move(!color)){
				color = !color;
			}
			if(brd.sum() >= m){
				if(!color){
					brd.reverse();
				}
				brds.insert(brd);
				break;
			}
			if(!brd.get_move(color)){
				break;
			}
		}
		if(flag_verbose && brds.size() % 1000 == 0){
			cout << "\rfinish " << brds.size() << "/" << n << flush;
		}
		++m;
		if(m >= 64){
			m = 5;
		}
	}
	if(flag_verbose){
		cout << endl;
	}

	*m1 = brds.size();
	*m2 = 2;
	*ptrm = new ull[*m1 * 2];
	int i = 0;
	for(cboard brd:brds){
		(*ptrm)[i * 2] = brd.get_brd(false);
		(*ptrm)[i * 2 + 1] = brd.get_brd(true);
		++i;
	}
};

void benchmark_flip(ARRAY_2D_OUT_M(ULL), ARRAY_2D_IN_I(ULL)){
	assert(i2 == 5);
	*m1 = i1;
	*m2 = 2;
	*ptrm = new ull[*m1 * *m2];

	for(int i = 0; i != i1; ++i){
		board brd(ptri[i * 5], ptri[i * 5 + 1]);
		short pos = ptri[i * 5 + 2];
		brd.flip(true, pos);
		(*ptrm)[i * 2] = brd.get_brd(false);
		(*ptrm)[i * 2 + 1] = brd.get_brd(true);
	}
	
	for(int j = 0; j != 100; ++j){
		for(int i = 0; i != i1; ++i){
			board brd(ptri[i * 5], ptri[i * 5 + 1]);
			short pos = ptri[i * 5 + 2];
			brd.flip(true, pos);
		}
	}
};

void benchmark_get_move(ARRAY_2D_OUT_M(ULL), ARRAY_2D_IN_I(ULL)){
	assert(i2 == 5);
	
	*m1 = i1;
	*m2 = 1;
	*ptrm = new ull[*m1 * *m2];

	for(int i = 0; i != i1; ++i){
		board brd(ptri[i * 5], ptri[i * 5 + 1]);
		ull brd_move = brd.get_move(true);
		(*ptrm)[i] = brd_move;
	}

	ull junk = 0;
	for(int j = 0; j != 100; ++j){
		for(int i = 0; i != i1; ++i){
			board brd(ptri[i * 5], ptri[i * 5 + 1]);
			ull brd_move = brd.get_move(true);
			junk ^= brd_move;
		}
	}

	(*ptrm)[0] = junk;
};
