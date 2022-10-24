#!/bin/sh

set -e
echo $ARCH
if [[ "$ARCH" == 'arm64' ]]; then
    xmake f -a arm64 -c --file=./3rd/xmake.lua
else
    xmake f -a x86_64 -c --file=./3rd/xmake.lua
fi
xmake lua ./3rd/fetch.lua
xmake build --file=./3rd/xmake.lua
xmake lua ./3rd/copy.lua
