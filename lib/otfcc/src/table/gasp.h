#ifndef CARYLL_TABLE_GASP_H
#define CARYLL_TABLE_GASP_H

#include "otfcc/table/gasp.h"

table_gasp *otfcc_readGasp(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpGasp(const table_gasp *table, json_value *root, const otfcc::options_t &options);
table_gasp *otfcc_parseGasp(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildGasp(const table_gasp *table, const otfcc::options_t &options);

#endif
