#ifndef CARYLL_TABLE_BASE_H
#define CARYLL_TABLE_BASE_H

#include "otfcc/table/BASE.h"

table_BASE *otfcc_readBASE(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpBASE(const table_BASE *base, json_value *root, const otfcc::options_t &options);
table_BASE *otfcc_parseBASE(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildBASE(const table_BASE *base, const otfcc::options_t &options);

#endif
