#ifndef CARYLL_TABLE_NAME_H
#define CARYLL_TABLE_NAME_H

#include "otfcc/table/name.h"

table_name *otfcc_readName(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpName(const table_name *table, json_value *root, const otfcc::options_t &options);
table_name *otfcc_parseName(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildName(const table_name *name, const otfcc::options_t &options);

#endif
