#!/bin/sh

set -e
echo $ARCH
if [[ "$ARCH" == 'arm64' ]]; then
    xmake f -a arm64 --target_minver=10.12 -c --file=./3rd/xmake.lua
else
    xmake f -a x86_64 --target_minver=10.12 -c --file=./3rd/xmake.lua
fi
xmake lua ./3rd/fetch.lua
xmake build -vD --file=./3rd/xmake.lua
xmake lua ./3rd/copy.lua
