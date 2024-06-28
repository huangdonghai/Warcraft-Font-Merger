#ifndef CARYLL_TABLE_VDMX_H
#define CARYLL_TABLE_VDMX_H

#include "otfcc/table/VDMX.h"

table_VDMX *otfcc_readVDMX(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpVDMX(const table_VDMX *table, json_value *root, const otfcc::options_t &options);
table_VDMX *otfcc_parseVDMX(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildVDMX(const table_VDMX *vdmx, const otfcc::options_t &options);

#endif
