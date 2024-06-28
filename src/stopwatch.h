#pragma once

#include <chrono>
#include <string>

std::string
push_stopwatch(std::chrono::time_point<std::chrono::system_clock> &sofar);

#define logStepTime options.logger.trace(push_stopwatch(begin));
