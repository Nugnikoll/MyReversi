#include "type.h"

#ifdef USE_RANDOM
	default_random_engine engine(
		chrono::system_clock::now().time_since_epoch().count()
	);
#endif //USE_RANDOM