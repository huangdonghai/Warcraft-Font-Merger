#include "stopwatch.h"

#include <chrono>

#include "intl.hpp"

using namespace std::chrono;

sds push_stopwatch(std::chrono::time_point<std::chrono::system_clock> &sofar) {
	auto ends = system_clock::now();
	auto diff = std::chrono::duration_cast<milliseconds>(ends - sofar);
	sofar = ends;
	sds log = sdscatprintf(sdsempty(), _("Step time = %g s.\n"), diff.count() / 1000.0);
	return log;
}
