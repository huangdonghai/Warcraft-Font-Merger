#ifndef CARYLL_TABLE_VORG_H
#define CARYLL_TABLE_VORG_H

#include "otfcc/table/VORG.h"

table_VORG *otfcc_readVORG(const otfcc_Packet packet, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildVORG(const table_VORG *table, const otfcc::options_t &options);

#endif
