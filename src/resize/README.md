# resize

> 移植的版本为 [GraphicsMagick 1.3.42](https://sourceforge.net/projects/graphicsmagick/files/graphicsmagick/1.3.42/)

从 [GraphicsMagick](http://www.graphicsmagick.org/) 的 magick/resize.c 文件移植的纯 c 实现。
由于 GraphicsMagick 的许可看起来比较复杂……，先放了一个 MIT，有空再改了。

## 一、特点

- 暂时仅支持 32 位的 rgba 的图片缩放（可以手动指定 rbga 的排序）。
- 纯 c 实现，无第三方依赖，外部库暂时只适配了 sdl 的图片。
- 由于是 GraphicsMagick 移植，后面 GraphicsMagick 添加了滤镜算法可以直接拷贝过来。

## 二、任务列表

- [ ] openmp 支持（移植时为了简单没有把 openmp 的实现也移植过来）。
- [ ] 支持 24 位的 rgb 图片。
- [ ] 支持 8 位的灰度图片。
