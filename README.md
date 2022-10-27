ONScripter
=============

Fork to [ONScripter-Jh](https://github.com/jh10001/ONScripter-Jh)

## bugfix

- [x] 修复高版本的 sdl 的音乐播放回调会取消音频播放（点击跳过上一个音频时会导致下一个音频无法播放）。
- [x] 修复 osx 系统上无法编译 `gbk`，`shift-jis` 编码的 `cpp` 源码（使用 lua 脚本直接生成 \x 转义字符串）。
- [x] 修复 osx 上的 sdl 的 `render` 在高 `dpi` 模式下被放大了 `dpi` 的倍数导致文字渲染模糊（通过 oc 代码获得 dpi 倍数把 render 缩小）。
- [ ] 自动播放会导致音频播放不完整。

## feture

- [x] 使用 `xmake` 编译，支持 `windows`，`osx`。
- [x] 内置所有必须依赖，在 3rd 文件夹中，使用 `xmake` 编译。
- [x] 日志加入使用的 `render` 和 `audio` 的驱动。
- [x] `language.lua` 会把 `src/language/*.txt` 转换为带转义文字的 `cpp` 文件。
- [x] `nscriptdecode` 工具改为流模式，支持更多参数，支持三种的加解密（由于用的是 xor 模式即是加密也是解密）。
- [x] `nsadec` 工具支持 `-out` 设置输出目录, `-lower` 设置输出文件名是否为小写，`-v` 显示详细输出，默认也加入了进度查看。

## 编译指南

**windows**
> 需要安装 Visual Studio

``` powershell
./build-3rd.ps1
xmake f -c
xmake build onscripter
```

**osx**
> 需要安装 xcode

``` bash
./build-3rd.sh
xmake f -c
xmake build onscripter
```

## 吐槽

看了代码以后才知道为啥只支持 `gbk`，`shift-jis`，这一种两字节编码的格式，代码里充满了大量的 `IS_TWO_BYTE` 判断，而且 ui 文字渲染也是走的这个逻辑，导致 ui 的文字必须也对应脚本的格式。
而且 ui 的文字里竟然还包含解析脚本的 token 关键字，所以说，ui 文字和 token 关键字需要拆开。
