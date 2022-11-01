add_rules("mode.debug", "mode.release")

set_languages("c++17")

local arch = os.getenv("ARCH") or "x64"
if arch == "x86_64" then
    arch = "x64"
end
local is32bit = arch ~= "x64" and arch ~= "arm64"
local isarm64 = arch == "arm64"

local targets = {
    bzip2 = true,
    sdl2 = true,
    jpeg = true,
    z = true,
    png = true,
}


if is_host("macosx") then
    targets = {
        bzip2 = true,
        sdl2 = true,
        jpeg = true,
        z = true,
        png = true,
    }
end


local freetypePath = path.join(os.scriptdir(), "freetype/freetype-VER-2-12-1")
local bzip2Path = path.join(os.scriptdir(), "bzip2/bzip2-1.0.6")

if is_host("windows") then
    add_cxflags("/utf-8")
end

if targets["bzip2"] == true then

target("bzip2")
    set_kind("static")
    for _, f in ipairs({
        "blocksort.c",
        "huffman.c",
        "crctable.c",
        "randtable.c",
        "compress.c",
        "decompress.c",
        "bzlib.c",
    }) do
        add_files(path.join(bzip2Path, f))
    end

end

local sdlPath = path.join(os.scriptdir(), "libsdl/SDL2-2.24.1")

if targets["sdl2"] == true then
local sdlSrc = {
    "src/*.c",
    "src/atomic/*.c",
    "src/audio/*.c",
    "src/cpuinfo/*.c",
    "src/dynapi/*.c",
    "src/events/*.c",
    "src/file/*.c",
    "src/haptic/*.c",
    "src/hidapi/*.c",
    "src/joystick/*.c",
    "src/libm/*.c",
    "src/misc/*.c",
    "src/power/*.c",
    "src/render/*.c",
    "src/render/direct3d/*.c",
    "src/render/direct3d11/*.c",
    "src/render/direct3d12/*.c",
    "src/render/opengl/*.c",
    "src/render/opengles/*.c",
    "src/render/opengles2/*.c",
    "src/render/ps2/*.c",
    "src/render/software/*.c",
    "src/render/vitagxm/*.c",
    "src/sensor/*.c",
    "src/stdlib/*.c",
    "src/thread/*.c",
    "src/timer/*.c",
    "src/video/*.c",
    "src/video/yuv2rgb/*.c",
    "src/locale/*.c",
    "src/video/dummy/*.c",
    "src/audio/disk/*.c",
    "src/audio/dummy/*.c",
    "src/video/x11/*.c",
    "src/sensor/dummy/*.c",
    "src/joystick/hidapi/*.c",
    "src/joystick/virtual/*.c",
}


if is_host("macos") then
    table.join2(sdlSrc, {
        "src/power/macosx/*.c",
        "src/video/cocoa/*.m",
        "src/render/metal/*.m",
        "src/misc/macosx/*.m",
        "src/locale/macosx/*.m",
        "src/audio/coreaudio/*.m",
        "src/joystick/iphoneos/*.m",
        "src/filesystem/cocoa/*.m",
        "src/file/cocoa/*.m",
        "src/thread/pthread/*.c",
        "src/core/unix/*.c",
        "src/timer/unix/*.c",
        "src/loadso/dlopen/*.c",
        "src/haptic/darwin/*.c",
        "src/joystick/darwin/*.c",
    })
elseif is_host("windows") then
    table.join2(sdlSrc, {
        "src/video/windows/*.c",
        "src/misc/windows/*.c",
        "src/audio/wasapi/*.c",
        "src/audio/winmm/*.c",
        "src/audio/directsound/*.c",
        "src/filesystem/windows/*.c",
        "src/thread/generic/*.c",
        "src/thread/windows/*.c",
        "src/core/windows/*.c",
        "src/timer/windows/*.c",
        "src/loadso/windows/*.c",
        "src/haptic/windows/*.c",
        "src/joystick/windows/*.c",
        "src/power/windows/*.c",
        "src/sensor/windows/*.c",
        "src/locale/windows/*.c",
        "src/power/windows/*.c",
    })
end

target("sdl2")
    set_kind("static")
    add_defines("HAVE_LIBC=1")
    add_includedirs(path.join(sdlPath, "include"), path.join(sdlPath, "src/video/khronos"))
    for _, f in ipairs(sdlSrc) do
        add_files(path.join(sdlPath, f))
    end
end



local pngPath = path.join(os.scriptdir(), "png/libpng-1.6.38")
local jpegSourceDir = path.join(os.scriptdir(), "jpeg/jpeg-9e")

if targets["jpeg"] == true then

target("jpeg")
    set_kind("static")
    -- set_kind("shared")
    on_config(function ()
        local jconfig = path.join(jpegSourceDir, "jconfig.h")
        if not os.exists(jconfig) then
            local jconfigSource = path.join(jpegSourceDir,"jconfig.vc")
            if is_plat("macosx") then
                jconfigSource = path.join(os.scriptdir(), "pre/jpeg/jconfig.h")
            end
            os.cp(jconfigSource, jconfig)
        end
    end)
    for _, f in ipairs({
        "jaricom.c",
        "jcapimin.c",
        "jcapistd.c",
        "jcarith.c",
        "jccoefct.c",
        "jccolor.c",
        "jcdctmgr.c",
        "jchuff.c",
        "jcinit.c",
        "jcmainct.c",
        "jcmarker.c",
        "jcmaster.c",
        "jcomapi.c",
        "jcparam.c",
        "jcprepct.c",
        "jcsample.c",
        "jctrans.c",
        "jdapimin.c",
        "jdapistd.c",
        "jdarith.c",
        "jdatadst.c",
        "jdatasrc.c",
        "jdcoefct.c",
        "jdcolor.c",
        "jddctmgr.c",
        "jdhuff.c",
        "jdinput.c",
        "jdmainct.c",
        "jdmarker.c",
        "jdmaster.c",
        "jdmerge.c",
        "jdpostct.c",
        "jdsample.c",
        "jdtrans.c",
        "jerror.c",
        "jfdctflt.c",
        "jfdctfst.c",
        "jfdctint.c",
        "jidctflt.c",
        "jidctfst.c",
        "jidctint.c",
        "jquant1.c",
        "jquant2.c",
        "jutils.c",
        "jmemmgr.c",
        "jmemansi.c",
    }) do
        add_files(path.join(jpegSourceDir, f))
    end

end

local sdlImagePath = "libsdl_image/SDL2_image-2.6.2"
target("sdl2_image")
    set_kind("static")
    add_includedirs(
        path.join(sdlPath, "include"),
        "libwebp/libwebp-1.2.4/src",
        jpegSourceDir,
        pngPath
    )
    add_defines(
        "HAVE_STDIO_H=1",
        "HAVE_STRING_H=1",
        "HAVE_STDINT_H=1",
        "LOAD_JPG=1",
        "LOAD_PNG=1",
        "LOAD_SVG=1",
        "LOAD_WEBP=1",
        "LOAD_GIF=1",
        "LOAD_BMP=1",
        "LOAD_LBM=1",
        "LOAD_PCX=1",
        "LOAD_PNM=1",
        "LOAD_XCF=1",
        "LOAD_XPM=1",
        "LOAD_XV=1",
        "LOAD_TGA=1",
        "SDL_IMAGE_SAVE_PNG=1",
        "SDL_IMAGE_SAVE_JPG=1"
    )
    if is_host("macosx") then
        set_values("objc.build.arc", false)
        add_mflags("-fno-objc-arc")
    end
    local files = {
        "*.c",
    }
    if is_host("macosx") then
        table.join2(files, {"IMG_ImageIO.m"})
    end
    for _, f in ipairs(files) do
        add_files(path.join(sdlImagePath, f))
    end

local harfbuzzPath = path.join(os.scriptdir(), "harfbuzz/harfbuzz-5.3.1")

target("harfbuzz")
    set_kind("static")
    -- set_kind("shared")
    add_includedirs(path.join(freetypePath, "include"))
    add_defines("HAVE_ATEXIT=1", "HAVE_ISATTY=1", "HAVE_STDBOOL_H=1", "HAVE_FREETYPE")
    if is_host("windows") then
        add_defines("HAVE_DIRECTWRITE=1")
    end
    for _, f in ipairs({
        "src/harfbuzz.cc"
    }) do
        add_files(path.join(harfbuzzPath, f))
    end

local zlibPath = "zlib/zlib-1.2.13"

if targets["z"] == true then

target("z")
    set_kind("static")
    on_config(function()
        local libconfig = path.join(os.scriptdir(), zlibPath, "zconf.h")
        if not os.exists(libconfig) then
            os.cp(path.join(os.scriptdir(), zlibPath, "zconf.h.in"), libconfig)
        end
    end)
    for _, f in ipairs({
        "adler32.c",
        "crc32.c",
        "deflate.c",
        "infback.c",
        "inffast.c",
        "inflate.c",
        "inftrees.c",
        "trees.c",
        "zutil.c",
        "compress.c",
        "uncompr.c",
        "gzclose.c",
        "gzlib.c",
        "gzread.c",
        "gzwrite.c",
    }) do
        add_files(path.join(os.scriptdir(), zlibPath, f))
    end
    if is_host("macosx") then
        add_defines("Z_HAVE_UNISTD_H=1")
    end
end



if targets["png"] == true then

target("png")
    set_kind("static")
    add_deps("z")
    add_includedirs(path.join(os.scriptdir(), zlibPath))
    on_config(function()
        local libconfig = path.join(pngPath, "pnglibconf.h")
        if not os.exists(libconfig) then
            os.cp(path.join(os.scriptdir(), "pre/png/pnglibconf.h"), libconfig)
        end
    end)
    for _, f in ipairs({
        "png.c",
        "pngerror.c",
        "pngget.c",
        "pngmem.c",
        "pngpread.c",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c",
    }) do
        add_files(path.join(pngPath, f))
    end

    if is_arch("arm*") then
        for _, f in ipairs({
            "arm/arm_init.c",
            "arm/filter_neon.S",
            "arm/filter_neon_intrinsics.c",
            "arm/palette_neon_intrinsics.c",
        }) do
            add_files(path.join(pngPath, f))
        end
    end

    if is_arch("mips*") then
        for _, f in ipairs({
            "mips/mips_init.c",
            "mips/filter_msa_intrinsics.c",
        }) do
            add_files(path.join(pngPath, f))
        end
    end

    if is_arch("x86_64", "i386") then
        for _, f in ipairs({
            "intel/intel_init.c",
            "intel/filter_sse2_intrinsics.c",
        }) do
            add_files(path.join(pngPath, f))
        end
    end

    if is_arch("powerpc*") then
        for _, f in ipairs({
            "powerpc/powerpc_init.c",
            "powerpc/filter_vsx_intrinsics.c",
        }) do
            add_files(path.join(pngPath, f))
        end
    end
end


local brotliPath = path.join(os.scriptdir(), "brotli/brotli-1.0.9")

target("brotli")
    set_kind("static")
    -- set_kind("shared")
    add_includedirs(path.join(brotliPath, "c/include"))
    for _, f in ipairs({
        "c/common/*.c",
        "c/dec/*.c",
        "c/enc/*.c"
    }) do
        add_files(path.join(brotliPath, f))
    end

local freetypeFiles = {
    "src/autofit/autofit.c",
    "src/base/ftbase.c",
    "src/base/ftbbox.c",
    "src/base/ftbdf.c",
    "src/base/ftbitmap.c",
    "src/base/ftcid.c",
    "src/base/ftfstype.c",
    "src/base/ftgasp.c",
    "src/base/ftglyph.c",
    "src/base/ftgxval.c",
    "src/base/ftinit.c",
    "src/base/ftmm.c",
    "src/base/ftotval.c",
    "src/base/ftpatent.c",
    "src/base/ftpfr.c",
    "src/base/ftstroke.c",
    "src/base/ftsynth.c",
    "src/base/fttype1.c",
    "src/base/ftwinfnt.c",
    "src/bdf/bdf.c",
    "src/cache/ftcache.c",
    "src/cff/cff.c",
    "src/cid/type1cid.c",
    "src/dlg/dlgwrap.c",
    "src/gzip/ftgzip.c",
    "src/bzip2/ftbzip2.c",
    "src/lzw/ftlzw.c",
    "src/pcf/pcf.c",
    "src/pfr/pfr.c",
    "src/psaux/psaux.c",
    "src/pshinter/pshinter.c",
    "src/psnames/psmodule.c",
    "src/raster/raster.c",
    "src/sfnt/sfnt.c",
    "src/smooth/smooth.c",
    "src/sdf/sdf.c",
    "src/svg/svg.c",
    "src/truetype/truetype.c",
    "src/type1/type1.c",
    "src/type42/type42.c",
    "src/winfonts/winfnt.c",
}

if is_host("windows") then
    table.join2(freetypeFiles, {
        "builds/windows/ftdebug.c",
        "builds/windows/ftsystem.c",
    })
elseif is_host("macosx") then
    table.join2(freetypeFiles, {
        "builds/mac/ftmac.c",
        "src/base/ftdebug.c",
        "builds/unix/ftsystem.c",
    })
end

target("freetype")
    set_kind("static")
    add_includedirs(
        path.join(freetypePath, "include"),
        path.join(freetypePath, "src/base"),
        path.join(harfbuzzPath, "src"),
        path.join(brotliPath, "c/include"),
        bzip2Path,
        zlibPath,
        pngPath
    )
    -- add_linkdirs(path.join(os.scriptdir(), "build/windows/x64/release"))
    -- add_links("harfbuzz", "brotli", "z", "bzip2")
    add_defines(
        "FT2_BUILD_LIBRARY",
        "_LIB",
        "FT_CONFIG_OPTION_USE_ZLIB",
        "FT_CONFIG_OPTION_SYSTEM_ZLIB",
        "FT_CONFIG_OPTION_USE_HARFBUZZ",
        "FT_CONFIG_OPTION_USE_BZIP2",
        "FT_CONFIG_OPTION_USE_BROTLI",
        "FT_CONFIG_OPTION_USE_PNG"
    )
    if is_host("macosx") then
        add_defines("HAVE_UNISTD_H=1", "HAVE_FCNTL_H=1")
    end
    for _, f in ipairs(freetypeFiles) do
        add_files(path.join(freetypePath, f))
    end

local sdlTTFPath = path.join(os.scriptdir(), "libsdl_ttf/SDL2_ttf-2.20.1")

target("sdl2_ttf")
    set_kind("static")
    -- set_kind("shared")
    add_includedirs(
        path.join(sdlPath, "include"),
        path.join(freetypePath, "include"),
        path.join(harfbuzzPath, "src")
    )
    -- add_linkdirs(path.join(os.scriptdir(), "build/windows/x64/release"))
    -- add_links("harfbuzz", "brotli", "z", "bzip2", "freetype", "sdl2")
    add_defines("TTF_USE_HARFBUZZ=1")
    for _, f in ipairs({
        "SDL_ttf.c"
    }) do
        add_files(path.join(sdlTTFPath, f))
    end

local smpegFiles = {
    "*.cpp",
    "video/*.cpp",
    "audio/*.cpp",
}

local smpegPath = path.join(os.scriptdir(), "smpeg/smpeg-main")

target("smpeg")
    set_kind("static")
    add_includedirs(
        smpegPath,
        path.join(sdlPath, "include")
    )
    add_defines(
        "memset=SDL_memset",
        "malloc=SDL_malloc",
        "memmove=SDL_memmove",
        "memcpy=SDL_memcpy",
        "realloc=SDL_realloc",
        "free=SDL_free"
    )
    add_defines(
        -- "USE_MMX=1",
        "USE_TIMESTAMP_SYNC=1",
        "USE_SYSTEM_THREAD=1",
        "THREADED_AUDIO=1"
    )
    for _, f in ipairs(smpegFiles) do
        add_files(path.join(smpegPath, f))
    end

local libFDKPath = path.join(os.scriptdir(), "fdk-aac/fdk-aac-2.0.2")

local libFDKsubDirs = {
    "libAACdec",
    "libSBRdec",
    "libSACdec",
    "libMpegTPDec",
    "libDRCdec",
    "libArithCoding",
    "libPCMutils",
    "libSYS",
    "libFDK"
}
target("fdk-aac")
    set_kind("static")
    for _, dir in ipairs(libFDKsubDirs) do
        add_includedirs(path.join(libFDKPath, dir, "include"))
        add_files(path.join(libFDKPath, dir, "src/*.cpp"))
    end

local sdlMixerFiles = {
    "src/*.c",
    "src/codecs/*.c",
    "src/codecs/native_midi/*.c",
}

local sdlMixerPath = path.join(os.scriptdir(), "libsdl_mixer/SDL2_mixer-2.6.2")

target("sdl2_mixer")
    set_kind("static")
    add_includedirs(
        path.join(sdlMixerPath, "include"),
        path.join(sdlMixerPath, "src"),
        path.join(sdlMixerPath, "src/codecs"),
        path.join(sdlPath, "include")
    )
    add_defines(
        "MUSIC_WAV=1",
        "MUSIC_OGG=1",
        "OGG_USE_STB=1",
        "MUSIC_MP3_DRMP3=1",
        "MUSIC_FLAC_DRFLAC=1",
        "MUSIC_MID_NATIVE=1"
        -- "MUSIC_OPUS=1",
        -- "MUSIC_MP3_MPG123=1",
        -- "MUSIC_MID_TIMIDITY=1",
        -- "MUSIC_MOD_XMP=1"
    )
    for _, f in ipairs(sdlMixerFiles) do
        add_files(path.join(sdlMixerPath, f))
    end

-- local de256Path = path.join(os.scriptdir(), "libde265/libde265-1.0.9")
-- local de256Files = {
--     "libde265/*.cc",
--     "libde265/encoder/*.cc",
--     "libde265/encoder/algo/*.cc",
--     "libde265/x86/*.cc",
-- }
-- if is_host("windows") then
--     table.join2(de256Files, {
--         "extra/win32cond.cc"
--     })
-- end

-- target("de265")
--     set_kind("shared")
--     if not is_host("windows") then
--         add_cxflags("-msse2 -mssse3 -msse4.1")
--         add_defines("HAVE_POSIX_MEMALIGN=1")
--     end
--     add_includedirs(de256Path, path.join(de256Path, "libde265"))
--     add_defines("HAVE_SSE4_1=1", "HAVE_STDINT_H=1")
--     for _, f in ipairs(de256Files) do
--         add_files(path.join(de256Path, f))
--     end

local webpPath = path.join(os.scriptdir(), "libwebp/libwebp-1.2.4")
local webpDirs = {
    "sharpyuv",
    "src/dec",
    "src/demux",
    "src/mux",
    "src/dsp",
    "src/enc",
    "src/utils",
}

target("webp")
    set_kind("static")
    add_includedirs(webpPath)
    for _, f in ipairs(webpDirs) do
        add_files(path.join(webpPath, f, "*.c"))
    end
