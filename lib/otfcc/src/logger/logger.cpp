#include "otfcc/logger.h"

#include "intl.hpp"

namespace otfcc {

logger_t::logger_t(std::string &&prog) : logger_t() {
	indent(std::move(prog));
}

logger_t::logger_t(std::shared_ptr<spdlog::logger> spd_logger) : _spd_logger(spd_logger) {}

void logger_t::indent(std::string segment) {
	_indents.push_back(std::move(segment));
}

void logger_t::start(std::string segment) {
	indent(std::move(segment));
	trace(_("Begin"));
}

void logger_t::log(spdlog::level::level_enum level, const std::string &msg) {
	if (level < _spd_logger->level()) return;
	std::string prefix;
	for (uint16_t level = 0; level < _indents.size(); level++) {
		if (level < _last_level - 1) {
			for (auto c : _indents[level])
				prefix.push_back(' ');
			if (level < _last_level - 2)
				prefix.append(" | ");
			else
				prefix.append(" |-");
		} else
			prefix.append(_indents[level] + " : ");
	}
	_spd_logger->log(level, prefix + msg);
}

void logger_t::dedent() {
	if (_indents.empty()) return;
	_indents.pop_back();
	if (_indents.size() < _last_level) _last_level = _indents.size();
}

void logger_t::finish() {
	trace(_("Finish"));
	dedent();
}

void logger_t::set_level(spdlog::level::level_enum level) {
	_spd_logger->set_level(level);
}

logged_step_t::logged_step_t(logger_t &logger, std::string segment) : _logger(logger) {
	_logger.start(segment);
}

logged_step_t::~logged_step_t() {
	_logger.finish();
}

} // namespace otfcc
