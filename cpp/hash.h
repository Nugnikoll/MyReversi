#ifndef HASH_H
#define HASH_H

#include <cstring>
#include <algorithm>

#include "type.h"

struct slot;
class bucket;
typedef const slot& cslot;
typedef const bucket& cbucket;

struct slot{
	ull key;
	val_type alpha;
	val_type beta;
	short depth;
	short pos;

	#ifdef __GNUC__
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	#endif

	void save(cslot slt){
		if(slt.key != key || slt.depth > depth){
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

	void save(cslot slt){
		slot& slt_old = table[slt.key & mask];
		if(slt.key != slt_old.key || slt.depth > slt_old.depth){
			slt_old = slt;
		}else if(slt.depth == slt_old.depth){
			slt_old.alpha = max(slt_old.alpha, slt.alpha);
			slt_old.beta = min(slt_old.beta, slt.beta);
			slt_old.pos = slt.pos;
		}
	}
};

#endif //HASH_H
