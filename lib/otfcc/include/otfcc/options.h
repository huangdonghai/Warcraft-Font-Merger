#pragma once

#include "logger.h"

#include <string>

namespace otfcc {

struct options_t {
	bool debug_wait_on_start = false;
	bool ignore_glyph_order = false;
	bool ignore_hints = false;
	bool has_vertical_metrics = false;
	bool export_fdselect = false;
	bool keep_average_char_width = false;
	bool keep_unicode_ranges = false;
	bool short_post = false;
	bool dummy_DSIG = false;
	bool keep_modified_time = false;
	bool instr_as_bytes = false;
	bool verbose = false;
	bool quiet = false;
	bool cff_short_vmtx = false;
	bool merge_lookups = false;
	bool merge_features = false;
	bool force_cid = false;
	bool cff_rollCharString = false;
	bool cff_doSubroutinize = false;
	bool stub_cmap4 = false;
	bool decimal_cmap = true;
	bool name_glyphs_by_hash = false;
	bool name_glyphs_by_gid = false;
	std::string glyph_name_prefix;
	mutable logger_t logger;

	options_t();
	options_t(std::string &&prog);
	options_t(logger_t &&logger);
	~options_t() = default;

	void optimize_to(uint8_t level);
};

} // namespace otfcc
