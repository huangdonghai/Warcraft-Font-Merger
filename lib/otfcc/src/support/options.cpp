#include "otfcc/options.h"

namespace otfcc {

options_t::options_t() : options_t(logger_t{}) {}

options_t::options_t(std::string &&prog) : logger(std::move(prog)) {
	optimize_to(1);
}

options_t::options_t(logger_t &&logger) : logger(std::move(logger)) {
	optimize_to(1);
}

void options_t::optimize_to(uint8_t level) {
	cff_rollCharString = false;
	short_post = false;
	ignore_glyph_order = false;
	cff_short_vmtx = false;
	merge_features = false;
	force_cid = false;
	cff_doSubroutinize = false;

	if (level >= 1) {
		cff_rollCharString = true;
		cff_short_vmtx = true;
	}
	if (level >= 2) {
		short_post = true;
		cff_doSubroutinize = true;
		merge_features = true;
	}
	if (level >= 3) {
		ignore_glyph_order = true;
		force_cid = true;
	}
}

} // namespace otfcc
