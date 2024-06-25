#!/bin/bash

set -e

xmake config --clean --plat=macosx --arch=x86_64 --cxflags="-target x86_64-apple-macos10.9" --ldflags="-target x86_64-apple-macos10.9" --policies=build.optimization.lto
xmake clean
xmake build

source build/config.sh
VERSION=$VERSION-mac-x86

po/update.sh
source package/common.sh

export _platform="unix"
export _archive="tar.xz"
export _url_extension="url"
export _script_extension="command"

_Dist="字体合并补全工具-压缩字库-$VERSION" \
_dist="WarFontMerger-XS-$VERSION" \
_cjk="WFM-Sans-CJK-XS-Regular" \
_latin="WFM-Sans-LCG-Apache-Regular" \
package_sc

_Dist="字体合并补全工具-简体中文-$VERSION" \
_dist="WarFontMerger-SC-$VERSION" \
_cjk="WFM-Sans-CJK-SC-Medium" \
_latin="WFM-Sans-LCG-Medium" \
package_sc

_Dist="字型合併補全工具-傳統字形-$VERSION" \
_dist="WarFontMerger-CL-$VERSION" \
_cjk="WFM-Sans-CJK-CL-Medium" \
_latin="WFM-Sans-LCG-Medium" \
package_tc
