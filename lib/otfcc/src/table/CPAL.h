#ifndef CARYLL_TABLE_CPAL_H
#define CARYLL_TABLE_CPAL_H

#include "otfcc/table/CPAL.h"

table_CPAL *otfcc_readCPAL(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpCPAL(const table_CPAL *table, json_value *root, const otfcc::options_t &options);
table_CPAL *otfcc_parseCPAL(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildCPAL(const table_CPAL *cpal, const otfcc::options_t &options);

#endif
