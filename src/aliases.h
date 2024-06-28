#pragma once

#define logError(fmt, ...) options.logger.error(fmt, ##__VA_ARGS__);
#define logWarning(fmt, ...) options.logger.warn(fmt, ##__VA_ARGS__);
#define logNotice(fmt, ...) options.logger.info(fmt, ##__VA_ARGS__);
#define logProgress(fmt, ...) options.logger.trace(fmt, ##__VA_ARGS__);
