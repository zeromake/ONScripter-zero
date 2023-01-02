#!/usr/bin/env sh
set -e
files=`find . -type f -name '*.webp'`
set -- $files

for file in "$@"; do
	mkdir -p $(dirname ../resize/$file)
	magick $file -resize 50% ../resize/$file
	echo "resize/$file"
done
