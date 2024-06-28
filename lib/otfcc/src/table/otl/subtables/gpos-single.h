#ifndef CARYLL_TABLE_OTL_GPOS_SINGLE_H
#define CARYLL_TABLE_OTL_GPOS_SINGLE_H

#include "common.h"

otl_Subtable *otl_read_gpos_single(const font_file_pointer data, uint32_t tableLength,
                                   uint32_t subtableOffset, const glyphid_t maxGlyphs,
                                   const otfcc::options_t &options);
json_value *otl_gpos_dump_single(const otl_Subtable *_subtable);
otl_Subtable *otl_gpos_parse_single(const json_value *_subtable, const otfcc::options_t &options);
caryll_Buffer *otfcc_build_gpos_single(const otl_Subtable *_subtable, otl_BuildHeuristics heuristics);

#endif
