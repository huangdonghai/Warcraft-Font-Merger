#!/bin/bash

set -e

xgettext --c++ --from-code=UTF-8 --keyword=_ --no-location --omit-header --output=po/unified.pot \
	lib/otfcc/src/consolidate/consolidate.c \
	lib/otfcc/src/consolidate/otl/GDEF.c \
	lib/otfcc/src/consolidate/otl/chaining.c \
	lib/otfcc/src/consolidate/otl/common.c \
	lib/otfcc/src/consolidate/otl/gpos-cursive.c \
	lib/otfcc/src/consolidate/otl/gpos-single.c \
	lib/otfcc/src/consolidate/otl/gsub-ligature.c \
	lib/otfcc/src/consolidate/otl/gsub-multi.c \
	lib/otfcc/src/consolidate/otl/gsub-reverse.c \
	lib/otfcc/src/consolidate/otl/gsub-single.c \
	lib/otfcc/src/consolidate/otl/mark.c \
	lib/otfcc/src/font/caryll-sfnt-builder.c \
	lib/otfcc/src/json-reader/json-reader.c \
	lib/otfcc/src/libcff/cff-parser.c \
	lib/otfcc/src/libcff/subr.c \
	lib/otfcc/src/logger/logger.cpp \
	lib/otfcc/src/otf-writer/stat.c \
	lib/otfcc/src/table/BASE.c \
	lib/otfcc/src/table/COLR.c \
	lib/otfcc/src/table/OS_2.c \
	lib/otfcc/src/table/VORG.c \
	lib/otfcc/src/table/cmap.c \
	lib/otfcc/src/table/fvar.c \
	lib/otfcc/src/table/gasp.c \
	lib/otfcc/src/table/glyf/read.c \
	lib/otfcc/src/table/head.c \
	lib/otfcc/src/table/hhea.c \
	lib/otfcc/src/table/hmtx.c \
	lib/otfcc/src/table/maxp.c \
	lib/otfcc/src/table/meta/read.c \
	lib/otfcc/src/table/name.c \
	lib/otfcc/src/table/vhea.c \
	lib/otfcc/src/table/vmtx.c \
	lib/otfcc/src/table/otl/build.c \
	lib/otfcc/src/table/otl/parse.c \
	lib/otfcc/src/table/otl/subtables/chaining/read.c \
	lib/otfcc/src/table/otl/subtables/gpos-mark-to-single.c \
	lib/otfcc/src/table/vdmx/funcs.c \
	src/merge-otd.cpp \
	src/otfccbuild.cpp \
	src/otfccdump.cpp \
	src/stopwatch.cpp

for lang in zh_CN
do
	if [[ ! -f po/$lang.po ]]
	then
		msginit --locale=$lang --no-translator --input=po/unified.pot --output=po/$lang.po
	else
		msgmerge --update --sort-output po/$lang.po po/unified.pot
	fi
	msgfmt -o po/$lang.mo po/$lang.po
done
