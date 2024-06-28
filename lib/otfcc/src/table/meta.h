#ifndef CARYLL_TABLE_META_H
#define CARYLL_TABLE_META_H

#include "otfcc/table/meta.h"

table_meta *otfcc_readMeta(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpMeta(const table_meta *table, json_value *root, const otfcc::options_t &options);
table_meta *otfcc_parseMeta(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildMeta(const table_meta *meta, const otfcc::options_t &options);

#endif
