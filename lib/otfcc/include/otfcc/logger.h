#pragma once

#include <memory>
#include <type_traits>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/core.h>

namespace otfcc {

struct logger_t {

	std::shared_ptr<spdlog::logger> _spd_logger;

	std::vector<std::string> _indents;
	uint16_t _last_level;

	logger_t(std::string &&prog);
	logger_t(std::shared_ptr<spdlog::logger> spd_logger = spdlog::stderr_color_st("console"));
	logger_t(const logger_t &) = delete;
	logger_t(logger_t &&) = default;
	logger_t &operator=(const logger_t &) = delete;
	logger_t &operator=(logger_t &&) = default;
	~logger_t() = default;

	void indent(std::string segment);
	void start(std::string segment);
	void log(spdlog::level::level_enum level, const std::string &msg);
	void dedent();
	void finish();

	void set_level(spdlog::level::level_enum level);

	template <typename T> void trace(const T &msg) {
		log(spdlog::level::trace, msg);
	}
	template <typename T> void info(const T &msg) {
		log(spdlog::level::info, msg);
	}
	template <typename T> void warn(const T &msg) {
		log(spdlog::level::warn, msg);
	}
	template <typename T> void error(const T &msg) {
		log(spdlog::level::err, msg);
	}

	template <typename... Args>
	void log(spdlog::level::level_enum level, const char *fmt, Args &&...args) {
		log(level, fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
	}
	template <typename... Args> void trace(const char *fmt, Args &&...args) {
		log(spdlog::level::trace, fmt, std::forward<Args>(args)...);
	}
	template <typename... Args> void info(const char *fmt, Args &&...args) {
		log(spdlog::level::info, fmt, std::forward<Args>(args)...);
	}
	template <typename... Args> void warn(const char *fmt, Args &&...args) {
		log(spdlog::level::warn, fmt, std::forward<Args>(args)...);
	}
	template <typename... Args> void error(const char *fmt, Args &&...args) {
		log(spdlog::level::err, fmt, std::forward<Args>(args)...);
	}
};

struct logged_step_t {
	logger_t &_logger;

	logged_step_t(logger_t &logger, std::string segment);
	~logged_step_t();
};

} // namespace otfcc

#define loggedStep(segment) if (otfcc::logged_step_t logged_step(options.logger, segment); true)
