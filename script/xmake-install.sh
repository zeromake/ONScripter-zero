#!/bin/sh

RELEASE_URL=https://github.com/xmake-io/xmake/releases
VERSION=$(curl -w '%{url_effective}' -I -L -s -S ${RELEASE_URL}/latest -o /dev/null | sed -e 's|.*/v||')

echo "${VERSION}"

curl -L -o $1 "https://launchpad.net/~xmake-io/+archive/ubuntu/xmake/+files/xmake_${VERSION}+1jammy_amd64.deb"
