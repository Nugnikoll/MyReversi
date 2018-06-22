#include "type.h"

default_random_engine engine(
	chrono::system_clock::now().time_since_epoch().count()
);

#ifdef DEBUG_SEARCH
	ofstream out;
#endif