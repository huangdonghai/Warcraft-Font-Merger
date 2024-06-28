#ifndef CARYLL_TABLE_CFF_H
#define CARYLL_TABLE_CFF_H

#include "otfcc/table/CFF.h"
#include "glyf.h"

table_CFFAndGlyf otfcc_readCFFAndGlyfTables(const otfcc_Packet packet, const otfcc::options_t &options,
                                            const table_head *head);
void otfcc_dumpCFF(const table_CFF *table, MODIFY json_value *root, const otfcc::options_t &options);
table_CFF *otfcc_parseCFF(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildCFF(const table_CFFAndGlyf cffAndGlyf, const otfcc::options_t &options);

#endif
