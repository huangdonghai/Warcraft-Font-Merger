#include "common.h"

#include <intl.hpp>

void fontop_consolidateCoverage(otfcc_Font *font, otl_Coverage *coverage, const otfcc::options_t &options) {
	if (!coverage) return;
	for (glyphid_t j = 0; j < coverage->numGlyphs; j++) {
		glyph_handle *h = &(coverage->glyphs[j]);
		if (!GlyphOrder.consolidateHandle(font->glyph_order, h)) {
			logWarning(_("[Consolidate] Ignored missing glyph /{}."), h->name);
			Handle.dispose(h);
		}
	}
}

void fontop_consolidateClassDef(otfcc_Font *font, otl_ClassDef *cd, const otfcc::options_t &options) {
	if (!cd) return;
	for (glyphid_t j = 0; j < cd->numGlyphs; j++) {
		glyph_handle *h = &(cd->glyphs[j]);
		if (!GlyphOrder.consolidateHandle(font->glyph_order, h)) {
			logWarning(_("[Consolidate] Ignored missing glyph /{}."), h->name);
			Handle.dispose(h);
			cd->classes[j] = 0;
		}
	}
}
