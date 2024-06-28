#ifndef CARYLL_TABLE_VHEA_H
#define CARYLL_TABLE_VHEA_H

#include "otfcc/table/vhea.h"

table_vhea *otfcc_readVhea(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpVhea(const table_vhea *table, json_value *root, const otfcc::options_t &options);
table_vhea *otfcc_parseVhea(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildVhea(const table_vhea *vhea, const otfcc::options_t &options);

#endif
