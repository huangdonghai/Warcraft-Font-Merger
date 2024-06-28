#ifndef CARYLL_TABLE_CMAP_H
#define CARYLL_TABLE_CMAP_H

#include "otfcc/table/cmap.h"

table_cmap *otfcc_readCmap(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpCmap(const table_cmap *cmap, json_value *root, const otfcc::options_t &options);
table_cmap *otfcc_parseCmap(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildCmap(const table_cmap *cmap, const otfcc::options_t &options);

#endif
