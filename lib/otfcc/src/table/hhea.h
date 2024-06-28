#ifndef CARYLL_TABLE_HHEA_H
#define CARYLL_TABLE_HHEA_H

#include "otfcc/table/hhea.h"

table_hhea *otfcc_newHhea();
table_hhea *otfcc_readHhea(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpHhea(const table_hhea *table, json_value *root, const otfcc::options_t &options);
table_hhea *otfcc_parseHhea(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildHhea(const table_hhea *hhea, const otfcc::options_t &options);

#endif
