#ifndef CARYLL_TABLE_TSI5_H
#define CARYLL_TABLE_TSI5_H

#include "otfcc/table/TSI5.h"

table_TSI5 *otfcc_readTSI5(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpTSI5(const table_TSI5 *table, json_value *root, const otfcc::options_t &options);
table_TSI5 *otfcc_parseTSI5(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildTSI5(const table_TSI5 *TSI, const otfcc::options_t &options, glyphid_t numGlyphs);

#endif
