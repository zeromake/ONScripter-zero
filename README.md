ONScripter
=============

Fork to [ONScripter-Jh](https://github.com/jh10001/ONScripter-Jh)

## bugfix

- [x] 修复高版本的 sdl 的音乐播放回调会取消音频播放（点击跳过上一个音频时会导致下一个音频无法播放）。
- [x] 修复 osx 系统上无法编译 `gbk`，`shift-jis` 编码的 `cpp` 源码（使用 lua 脚本直接生成 \x 转义字符串）。
- [x] 修复 osx 上的 sdl 的 `render` 在高 `dpi` 模式下被放大了 `dpi` 的倍数导致文字渲染模糊（通过 oc 代码获得 dpi 倍数把 render 缩小）。
- [x] 修复在 osx 上的 `savescreenshot` 命令是向执行目录下写的，并修复脚本里的是 win 的风格路径。
- [x] 修复 windows 上调用外部播放器是同步阻塞的会导致游戏卡死，开了一个线程去调其系统命令。
- [x] 修复 android 下的 finger 触摸事件和鼠标点击事件冲突，android 的 sdl 官方模板触摸事件触发会同时触发 finger 和鼠标点击，手动在 c++ 这边适配了。
- [x] 修复触摸事件模拟点击效果不佳，点击的还是使用鼠标点击事件，去掉 finger 事件里的 mouseMove, mouseButton 的模拟(点击一下跑的没边了)，finger 事件重写了两指和三指事件。
- [x] 修复 android 下的后台崩溃。
- [x] 修复并支持某些游戏的换页判断: `。]\`，`。` 和 `\` 都是换页符号，现在会检查换页符号后 5 个字符是否还有换页符号，如果有跳过该换页符号。
- [x] 修复 android 的 aaudio 音频驱动方式有问题，会出现音乐断续，播放完一个以后会变正常，现在先切换到默认 openslES 后正常驱动。
- [x] 修复 android 下后台锁屏后黑屏，缺少画布大小变更事件监听后刷新绘制。
- [x] 修复 osx 上 simd 动画计算加速，xmake 的 `add_vectorexts("avx2")` 选项的 `-mavx2` 编译参数有奇怪的动画加载 bug，改用 `-march=knl` 就没事(黑苹果 bug)。
- [x] 修复各种奇怪的文字截断，主要是字体并非是正方形也不是等宽和等高的，但是 FontInfo 是完全以字体大小来计算偏移和精灵文字的绘制范围。
- [x] 修复文字带有描边会被截断，描边字体并未被算做绘制区域强制加上描边长度到绘制区域。
- [x] 修复 xmake 的 android 打包问题，新版 xmake 的依赖 api 有部分变化。
- [x] 修复 nt3 脚本解密不完全的问题。
- [x] 修复结束的文本不需要做强制点击换页。
- [x] 修复文本渲染里的变量替换无法支持别名的情况（初音岛1）。
- [x] 修复文本渲染有些游戏会做每行文本最后添加一个 / 符号，需要跳过该符号且跳过 processEOT 里的自动换行（3day）。
- [x] 修复方法调用的参数没有消费被渲染为文本。
- [x] 修复 getparam 的多余声明参数没有消费被渲染为文本。
- [x] 修复 playSound 里的 SDL_RW 没有正确回收。
- [x] 修复多层 for 的 break 无法正确的消费 next 命令
- [ ] 自动播放会导致音频播放不完整。

## feture

- [x] 使用 `xmake` 编译，支持 `windows`，`osx`。
- [x] 内置所有必须依赖，在 3rd 文件夹中，使用 `xmake` 编译。
- [x] 日志加入使用的 `render` 和 `audio` 的驱动。
- [x] `language.lua` 会把 `src/language/*.txt` 转换为带转义文字的 `cpp` 文件。
- [x] `nscriptdecode` 工具改为流模式，支持更多参数，支持三种的加解密（由于用的是 xor 模式即是加密也是解密）。
- [x] `nsadec` 工具支持 `-out` 设置输出目录, `-lower` 设置输出文件名是否为小写，`-v` 显示详细输出，默认也加入了进度查看。
- [x] 修复在 `超级糖果` 的 `ons` 移植版的各种指令问题。
    - [x] 添加 `else`, `elseif`, `elif` 指令，与 `if`, `notif` 指令配合使用。
    - [x] 添加 `sefadetime` 指令，可以控制语音的淡入时间支持，播放一个语音后自动设置回 0。
    - [x] 添加 `checksp` 指令，检查精灵是否显示。
    - [x] 修改 `csp` 指令支持多参数，`csp 1,10` 代表清理 `1-10` 的精灵。
    - [x] 添加 `sprintf` 指令，参考 `c` 的 `sprintf`，示例 `mov %1,100:sprintf $1,"%d_xxx.jpg",%1`，参数上限为 32 个。
    - [x] 支持 onscript.nt 的脚步格式，支持版本 1-3。
- [x] 修改并支持 sdl2 的官方 android 模板
    - [x] 参考面包工房写了一个启动页
    - [x] 先使用 MANAGE_EXTERNAL_STORAGE 权限来读写 ons 目录
- [x] android 版完成。
- [x] 绘制画布在 android 上自动缩放到屏幕大小，否则字体渲染会发虚。
- [x] 支持对各种情况的字体进行单独设置
    - [x] 描边支持设置描边颜色，和描边宽度。
    - [x] 文本字体支持缩放比例。
- [x] 无需在编译脚本里指定 simd 使用的后端，`simd/simd.h` 会根据编译器预定义头自动选择后端，如果没有可用后端会自动关闭 `simd` 支持。
- [x] 文本渲染强制换行和切页，防止渲染到窗口外（配合自定义字体切换配置）。
- [x] 所有平台支持通过 ci 打包分发到 release。
- [x] 不再对文本里的单字节符号英文做特殊处理，支持中英混合，支持变量插入。
- [x] 强制对超过文本框的内容，做强制点击换页（有 bug 如果是和选项一起出现会导致点击换页事件抢掉选项事件注册）。
- [x] lua 的 NSCOM 调用现在可以在 ns 脚本里传入 int, string 参数
- [x] lua 的 NSSpLoad 支持传入位置 int 参数（可选），不用再调用一次 NSSpMove。
- [x] lua 增加一个 NSReadFile，用来支持读取 csv 文件之类的纯文本。
- [x] ns2 打包和解包工具支持 utf8 文件名打包（修复 unix 环境下无法解包带 gbk 的 nsa 文件），ons 引擎支持对该 ns2 的兼容读取，现在取文件会比较两次 gbk, utf8 的文件名。
    - [x] ons 的内置 gbk 转码修正 utf8 到 gbk 不全的情况。
- [x] ons 的 trans_mode 默认为 AnimationInfo::TRANS_NONE。
- [x] ons trans_mode 添加 AnimationInfo::TRANS_MASK_{TOP,BOTTOM,LEFT,RIGHT} 用来替代 AnimationInfo::TRANS_ALPHA。
    - [x] 写了第一个版本渲染有问题，等待修复。
- [x] ons 的 loadImage 做一个文件内容 lru 缓存，默认缓存 64 个图片内容。
- [x] ons 的内建纯色矩阵支持透明色，示例：`>800,200,#ff00005a`。
    - [x] 颜色支持 `#fff`, `#ffff`, `#ffffff`, `#ffffffff` 的类 css 变长写法（仅限在 sp 的 > 内建矩阵里有限）。
- [x] bg 支持加载带有透明颜色的图片，配合 clear_color 可以设置默认底色。
- [x] DirectReader 的多字节数字的读写方法改为 `readLongBE`, `readShortBE`, `writeLongBE`, `writeShortBE`
- [x] DirectReader 的多字节数字添加新方法 `readLongLE`, `readShortLE`, `writeLongLE`, `writeShortLE`
- [x] DirectReader 的多字节数字的读写方法会根据当前系统大小端进行自动优化（改为 memcpy）。
- [x] DirectReader 各个地方的调用都正确的使用 `LE`, `BE` 的方法。
- [x] 为 sp 添加新的图片处理器语法 `@expr:param1 param2 param3……|expr:param1 param2 param3……`。
    - [x] `composite` 参考 renpy 的 `Composite`，混合多张图片，示例 `@composite:xxx.png[ dest.x dest.y][ src.x src.y src.w src.h]……`
    - [x] `alpha`，对图片做 alpha 效果，示例 `@alpha:xxx.png 128` 或者 `@xxx.png|alpha:128`
    - [x] `crop`，对图片做矩形切割，示例 `@crop:xxx.png 0 0 48 48` 或者 `@xxx.png|crop:0 0 48 48` （来自管道时不能再由 crop 加载图片）
    - [x] 所有表达式支持管道式操作，前一个会的结果被作为后一个的输入
    - [x] 需要重新处理 std::stringstream 对下一个输出的类型的判断方式，否则会导致 `1.png` 这个当作数字。
- [x] 为 for 添加 `continue` 指令支持（什么也不用做直接把 next 做一个 continue 别名即可）。
- [ ] 文字渲染范围改为一个矩形坐标，不再使用横向纵向多少个字符的方式(设置依旧生效但是会转化为一个矩形坐标)。
- [ ] 脚本全部切换到 utf-8 的处理，仅在读取时做 gbk,shift_jis 到 utf-8 转换。
- [ ] 支持为字体设置多个 fallback 字体，并默认内置系统字体 fallback。
- [ ] android 需要一个配置页面对游戏进行一个自定义或者全局配置。
- [ ] 查找动画在 android 有明显卡顿的问题。
- [ ] 抽象文件操作(文件读写，文件目录遍历，文件路径自动拼接，文件是否存在判断，目录创建)，以便于移植到各种平台下，考虑使用 `c++17` 的 `std::filesystem` 的通用方法，移动端平台可能需要手动编写。
- [ ] 支持读取压缩包里的 ons 游戏，无需解压。
- [ ] 代码里描述指令说明并生成文档, [onscripter-api](https://07th-mod.github.io/ponscripter-fork/api/), [NScrAPI](http://senzogawa.s90.xrea.com/reference/NScrAPI.html)。
- [ ] 比较合并上游的 [onscripter](http://onscripter.osdn.jp/onscripter.html.en#package-source) 的 `20220816` 版本 https://web.archive.org/web/20230205122538/https://onscripter.osdn.jp/onscripter-20220816.tar.gz。
- [ ] surface 支持 gpu 着色器缩放(静态内置)
    - [ ] [Magpie's Effect](https://github.com/Blinue/Magpie/tree/main/src/Effects)
    - [ ] [FSR-GLES-Demo](https://github.com/elecro/FSR-GLES-Demo)
    - [ ] [第三方着色器介绍](https://hooke007.github.io/mpv-lazy/%5B01%5D_%E7%AC%AC%E4%B8%89%E6%96%B9%E7%9D%80%E8%89%B2%E5%99%A8%E4%BB%8B%E7%BB%8D.html)
    - [ ] opengl 支持
    - [ ] dx11,dx12 支持
    - [ ] metal 支持
## 编译指南

**预先工作**
~~> 由于 freetype, harfbuzz 的循环依赖，请使用我的 xmake 分支，已经向 xmake-io 提出 [issuse](https://github.com/xmake-io/xmake/issues/3118) 了，但是维护者没有空处理，我这边只是一个比较简单的实现。~~

已经不需要手动处理最新版的 xmake v2.8.3 已经正常支持。

**windows**
> 需要安装 Visual Studio

``` powershell
xmake f -c -y
xmake build -y onscripter
```

**windows by mingw**

> 安装 [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases) 或者其它 `mingw` 的版本。

``` powershell
$MINGW="D:\mingw64"
xmake f -p mingw --mingw=$MINGW -y -c
xmake build -y onscripter
```

**osx**

> 需要安装 xcode

``` bash
xmake f -y -c
xmake build -y onscripter
```

**android**

> 需要 android sdk + ndk + jdk
> 顺便一提 luajit 需要编译机能编译 32 位的，所以 osx 编译不了 arm 的。

``` bash
NDK_SDK=~/Library/Android/sdk/ndk/25.0.8775105
xmake f -p android -a arm64-v8a --ndk_sdkver=21 --ndk=${NDK_SDK} -y -c
xmake build -y onscripter
cd project/android
./gradlew assembleDebug
adb install ./app/build/outputs/apk/debug/app-debug.apk
```

## 吐槽

看了代码以后才知道为啥只支持 `gbk`，`shift-jis`，这一种两字节编码的格式，代码里充满了大量的 `IS_TWO_BYTE` 判断，而且 ui 文字渲染也是走的这个逻辑，导致 ui 的文字必须也对应脚本的格式。
而且 ui 的文字里竟然还包含解析脚本的 token 关键字，所以说，ui 文字和 token 关键字需要拆开。

上游的 `20220816` 实际上已经支持了 `utf8` 不过在我看来还是有些无语（代码里全是 utf8 判断），我感觉直接在脚本载入阶段全部转为 `utf8` 代码除了读取脚本之外全部只适配 `utf8`会更简单。

顺便一提 GPLv2 的 `ONScripter` 天生不能商用（说是用了必须要以 GPL 开源就可以了，但是有哪个兄弟卖游戏还给大家分发游戏源码的），但是不和引擎打包在一起加密就是等于没有（虽然 ONScripter 的加密本身也和没有差不多了），必须要修改引擎去支持更高级的加密方式，所以 `ONScripter` 也就只能做成执行器去加载游戏了，开源加载器即可，天生就是给移植用的，IOS 那边的 [ONSPlayer](https://apps.apple.com/cn/app/id1388250129) 闭源也不知道是不是找了 [onscripter原开发者](http://onscripter.osdn.jp) 买了商用授权。

商用的话比较活跃的就剩 [renpy](https://www.renpy.org/) ，主要是 `MIT` 协议，有些库是 `LGPL`，有各种平台的支持。

还有就是 [krkrz](https://github.com/krkrz/krkrz)，windows 下还挺好用的，协议主要是 krkr 自定义的免责声明，和一把依赖库的协议多为 zlib bsd 之类的，手机的话虽然有 `吉里吉里2模拟器` 这种东西但是并不是全部开源的可以看看 [Kirikiroid2#103](https://github.com/zeas2/Kirikiroid2/issues/103) 依赖库和打包全部没有，前面的 issue 有人搞了一个 [Kirikiroid2](https://github.com/ningshanwutuobang/Kirikiroid2) 不过听说内存还是有问题。

看了 gpl，以后要是碰上 gpl 的库恐怕只会选择绕道走了，如果是直接面向用户的产品还可以考虑，库的话就有点真 gpl 病毒的感觉了。

## avg 开源引擎比较

| 名称  | 官网 | 商用 | pc | mobile | 文档 | 个人的评价 |
|------|------|-----|----|--------|------|---------|
| krkrz | https://krkrz.github.io | ✓ | windows | ✕ | 没有统一的官网文档，大量的 kag 映射没用地方查 | 很强的一个引擎可惜没落了 |
| renpy | https://www.renpy.org | ✓ | windows,linux,MacOSx | Android,IOS | 有统一的渐进式的官方文档 | 未来之星 krkr 有力替代者 |
| ONScripter | https://onscripter.osdn.jp | ✕ | windows,linux,MacOSx | Android,IOS | 由于并没有特别的语法和插件，一个指令列表就足够了 | 超轻量级引擎(全静态 upx 后 2-3 mb)，免费游戏的选择 |

## 参考资料

- [shift_jis码表](https://uic.io/zh/charset/show/shift_jis/)
- [gb2312码表](https://uic.io/zh/charset/show/gb2312/)
- [gb18030码表](https://uic.io/zh/charset/show/gb18030/)
- [SDL2_Tutorials](https://lazyfoo.net/tutorials/SDL/index.php)
- [C++开源库列表](https://www.cnblogs.com/chendeqiang/p/12861712.html)
