#!/bin/bash

set -e

xmake config --clean --plat=mingw --arch=i386 --policies=build.optimization.lto
xmake clean
xmake build

source build/config.sh
VERSION=$VERSION-windows-32

source package/common.sh

export _platform="windows"
export _archive="7z"
export _url_extension="url"

_Dist="字体合并补全工具-压缩字库-$VERSION" \
_dist="WarFontMerger-XS-$VERSION" \
_cjk="WFM-Sans-CJK-XS-Regular" \
_latin="WFM-Sans-LCG-Apache-Regular" \
package_sc
