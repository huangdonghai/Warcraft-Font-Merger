#pragma once

#include <chrono>

#include "sds.h"

sds push_stopwatch(std::chrono::time_point<std::chrono::system_clock> &sofar);

#define logStepTime                                                                                \
	options->logger->logSDS(options->logger, log_vl_progress, log_type_progress,                   \
	                        push_stopwatch(begin));
