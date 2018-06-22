#ifndef HASH_H
#define HASH_H

#include <cstring>
#include <algorithm>

#include "type.h"
#include "reversi.h"

struct slot;
class bucket;
typedef const slot& cslot;
typedef const bucket& cbucket;

struct slot{
	board brd;
	val_type alpha;
	val_type beta;
	short depth;
	short pos;

	#ifdef __GNUC__
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	#endif

	void save(cslot slt){
		if(slt.brd != brd || slt.depth > depth){
			*this = slt;
		}else if(slt.depth == depth){
			alpha = max(alpha, slt.alpha);
			beta = min(beta, slt.beta);
			pos = slt.pos;
		}
	}

	#ifdef __GNUC__
		#pragma GCC diagnostic pop
	#endif

};

class bucket{
public:
	static const ull size = 1 << 20;
	static const ull mask = size - 1;

protected:
	slot table[size];

public:
	void clear(){
		memset(table, 0, sizeof(table));
	}

	slot& probe(cull key){
		return table[key & mask];
	}

	void save(cslot slt, cull key){
		slot& slt_old = table[key & mask];
		slt_old.save(slt);
	}
};

#endif //HASH_H
