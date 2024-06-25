#!/bin/bash

set -e

xmake config --clean
xmake clean
xmake build

source build/config.sh
VERSION=$VERSION-dev

po/update.sh
source package/common.sh

export _platform="unix"
export _archive=""
export _url_extension="desktop"
export _script_extension="sh"

_Dist="字体合并补全工具-简体中文-$VERSION" \
_cjk="WFM-Sans-CJK-SC-Medium" \
_latin="WFM-Sans-LCG-Medium" \
package_sc
