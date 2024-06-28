#include "stopwatch.h"

#include <chrono>
#include <fmt/core.h>

#include "intl.hpp"

using namespace std::chrono;

std::string
push_stopwatch(std::chrono::time_point<std::chrono::system_clock> &sofar) {
	auto ends = system_clock::now();
	auto diff = std::chrono::duration_cast<milliseconds>(ends - sofar);
	sofar = ends;
	return fmt::format(fmt::runtime(_("Step time = {:g} s.")),
	                   diff.count() / 1000.0);
}
