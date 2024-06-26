#!/bin/bash

set -e

xgettext --c++ --from-code=UTF-8 --keyword=_ --no-location --omit-header --output=po/unified.pot \
	src/merge-otd.cpp \
	src/otfccbuild.c \
	src/otfccdump.c \
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
