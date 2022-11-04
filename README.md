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
- [ ] 比较合并上游的 [onscripter](http://onscripter.osdn.jp/onscripter.html.en#package-source) 的 `20220816` 版本。

## 编译指南

**windows**
> 需要安装 Visual Studio

``` powershell
.\build-3rd.ps1
xmake f -c
xmake build onscripter
```

**windows by mingw**
> 安装 [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases) 或者其它 `mingw` 的版本。
``` powershell
$MINGW="D:\mingw64"
.\build-3rd.ps1 "-p mingw --mingw=$MINGW"
xmake f -p mingw --mingw=$MINGW -c
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

上游的 `20220816` 实际上已经支持了 `utf8` 不过在我看来还是有些无语（代码里全是 utf8 判断），我感觉直接在脚本载入阶段全部转为 `utf8` 代码除了读取脚本之外全部只适配 `utf8`会更简单。

顺便一提 GPLv2 的 `ONScripter` 天生不能商用（说是用了必须要以 GPL 开源就可以了，但是有哪个兄弟卖游戏还给大家分发游戏源码的），但是不和引擎打包在一起加密就是等于没有（虽然 ONScripter 的加密本身也和没有差不多了），必须要修改引擎去支持更高级的加密方式，所以 `ONScripter` 也就只能做成执行器去加载游戏了，开源加载器即可，天生就是给移植用的，IOS 那边的 (ONSPlayer)[https://apps.apple.com/cn/app/id1388250129] 闭源也不知道是不是找了 [onscripter原开发者](http://onscripter.osdn.jp) 买了商用授权。

商用的话比较活跃的就剩 [renpy](https://www.renpy.org/) ，主要是 `MIT` 协议，有些库是 `LGPL`，有各种平台的支持。

还有就是 [krkrz](https://github.com/krkrz/krkrz)，windows 下还挺好用的，协议主要是 krkr 自定义的免责声明，和一把依赖库的协议多为 zlib bsd 之类的，手机的话虽然有 `吉里吉里2模拟器` 这种东西但是并不是全部开源的可以看看 [Kirikiroid2#103](https://github.com/zeas2/Kirikiroid2/issues/103) 依赖库和打包全部没有，前面的 issue 有人搞了一个 [Kirikiroid2](https://github.com/ningshanwutuobang/Kirikiroid2) 不过听说内存还是有问题。

看了 gpl，以后要是碰上 gpl 的库恐怕只会选择绕道走了，如果是直接面向用户的产品还可以考虑，库的话就有点真 gpl 病毒的感觉了。

## avg 开源引擎比较

| 名称  | 官网 | 商用 | pc | mobile | 文档 | 个人的评价 |
|------|------|-----|----|--------|------|---------|
| krkrz | https://krkrz.github.io | ✓ | windows | ✕ | 没有统一的官网文档，大量的 kag 映射没用地方查 | 很强的一个引擎可惜没落了 |
| renpy | https://www.renpy.org | ✓ | windows,linux,MacOSX | Android,IOS | 有统一的渐进式的官方文档 | 未来之星 krkr 有力替代者 |
| ONScripter | https://onscripter.osdn.jp | ✕ | windows,linux,MacOSX | Android,IOS | 由于并没有特别的语法和插件，一个指令列表就足够了 | 超轻量级引擎(全静态 upx 后 2-3 mb)，免费游戏的选择 |
