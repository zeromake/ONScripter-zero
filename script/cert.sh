#!/usr/bin/env sh
set -e

printf "%s" $1 | base64 -d > $2
