#ifndef CARYLL_TABLE_HEAD_H
#define CARYLL_TABLE_HEAD_H

#include "otfcc/table/head.h"

table_head *otfcc_readHead(const otfcc_Packet packet, const otfcc::options_t &options);
void otfcc_dumpHead(const table_head *table, json_value *root, const otfcc::options_t &options);
table_head *otfcc_parseHead(const json_value *root, const otfcc::options_t &options);
caryll_Buffer *otfcc_buildHead(const table_head *head, const otfcc::options_t &options);

#endif
