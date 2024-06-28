#ifndef CARYLL_FONTOPS_OTL_COMMON_H
#define CARYLL_FONTOPS_OTL_COMMON_H

#include "otfcc/font.h"
#include "table/otl/private.h"

void fontop_consolidateCoverage(otfcc_Font *font, otl_Coverage *coverage, const otfcc::options_t &options);
void fontop_consolidateClassDef(otfcc_Font *font, otl_ClassDef *cd, const otfcc::options_t &options);

#endif
