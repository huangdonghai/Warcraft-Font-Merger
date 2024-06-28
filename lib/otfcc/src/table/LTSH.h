#ifndef CARYLL_TABLE_LTSH_H
#define CARYLL_TABLE_LTSH_H

#include "otfcc/table/LTSH.h"

table_LTSH *otfcc_readLTSH(const otfcc_Packet packet, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildLTSH(const table_LTSH *ltsh, const otfcc::options_t &options);

#endif
