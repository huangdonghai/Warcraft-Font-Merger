#include "gsub-ligature.h"

#include <intl.hpp>

bool consolidate_gsub_ligature(otfcc_Font *font, table_OTL *table, otl_Subtable *_subtable,
                               const otfcc::options_t &options) {
	subtable_gsub_ligature *subtable = &(_subtable->gsub_ligature);
	subtable_gsub_ligature nt;
	iSubtable_gsub_ligature.init(&nt);
	for (glyphid_t k = 0; k < subtable->length; k++) {
		if (!GlyphOrder.consolidateHandle(font->glyph_order, &subtable->items[k].to)) {
			logWarning(_("[Consolidate] Ignored missing glyph /{}."), subtable->items[k].to.name);
			continue;
		}
		fontop_consolidateCoverage(font, subtable->items[k].from, options);
		Coverage.shrink(subtable->items[k].from, false);
		if (!subtable->items[k].from->numGlyphs) {
			logWarning(_("[Consolidate] Ignoring empty ligature substitution to glyph /{}."),
			           subtable->items[k].to.name);
			continue;
		}
		iSubtable_gsub_ligature.push(
		    &nt, ((otl_GsubLigatureEntry){
		             .from = subtable->items[k].from, .to = Handle.dup(subtable->items[k].to),
		         }));
		subtable->items[k].from = NULL;
	}
	iSubtable_gsub_ligature.replace(subtable, nt);
	return (subtable->length == 0);
}
