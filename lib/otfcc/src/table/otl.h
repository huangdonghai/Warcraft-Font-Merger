#ifndef CARYLL_TABLE_OTL_H
#define CARYLL_TABLE_OTL_H

#include "otfcc/table/otl.h"

otl_Subtable *otfcc_readOtl_subtable(uint8_t *data, uint32_t tableLength, uint32_t subtableOffset,
                                     otl_LookupType lookupType, const glyphid_t maxGlyphs,
                                     const otfcc::options_t &options);

table_OTL *otfcc_readOtl(const otfcc_Packet packet, const otfcc::options_t &options,
                         const uint32_t tag, const glyphid_t maxGlyphs);
void otfcc_dumpOtl(const table_OTL *table, json_value *root, const otfcc::options_t &options,
                   const char *tag);
table_OTL *otfcc_parseOtl(const json_value *root, const otfcc::options_t &options, const char *tag);
caryll_Buffer *otfcc_buildOtl(const table_OTL *table, const otfcc::options_t &options,
                              const char *tag);

#endif
