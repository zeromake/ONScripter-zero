#!/usr/bin/env sh
files=`find . -type f -name '*.png'`
set -- $files

for file in "$@"; do
	name=`echo $file|sed 's/\.png//1'`
	echo "generate ${name}.webp"
	realcugan-ncnn-vulkan -s 4 -o $name.webp -i $file -n 0 -x -j 16:16:16
	if [ $? -eq 0 ]; then
		rm $file
	else
		waifu2x-ncnn-vulkan -s 4 -o $name.webp -i $file
		rm $file
	fi
done
