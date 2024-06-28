#ifndef CARYLL_TABLE_GDEF_H
#define CARYLL_TABLE_GDEF_H

#include "otfcc/table/GDEF.h"

table_GDEF *otfcc_readGDEF(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpGDEF(const table_GDEF *gdef, json_value *root, const otfcc::options_t &options);
table_GDEF *otfcc_parseGDEF(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildGDEF(const table_GDEF *gdef, const otfcc::options_t &options);

#endif
