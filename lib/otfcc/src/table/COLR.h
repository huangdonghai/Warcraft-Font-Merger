#ifndef CARYLL_TABLE_COLR_H
#define CARYLL_TABLE_COLR_H

#include "otfcc/table/COLR.h"

table_COLR *otfcc_readCOLR(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpCOLR(const table_COLR *table, json_value *root, const otfcc::options_t &options);
table_COLR *otfcc_parseCOLR(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildCOLR(const table_COLR *colr, const otfcc::options_t &options);

#endif
