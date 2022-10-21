add_rules("mode.debug", "mode.release")

set_languages("c++11")
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
}

if is_host("macosx") then
    targets = {
        bzip2 = true,
        sdl2 = true,
        jpeg = true,
    }
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
        add_files(path.join(os.scriptdir(), "bzip2/bzip2-1.0.6", f))
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
    "src/loadso/dlopen/*.c",
    "src/video/cocoa/*.m",
    "src/render/metal/*.m",
    "src/video/x11/*.c",
    "src/thread/pthread/*.c",
    "src/joystick/hidapi/*.c",
    "src/misc/macosx/*.m",
    "src/locale/macosx/*.m",
    "src/audio/coreaudio/*.m",
    "src/joystick/darwin/*.c",
    "src/joystick/iphoneos/*.m",
    "src/haptic/darwin/*.c",
    "src/power/macosx/*.c",
    "src/filesystem/cocoa/*.m",
    "src/timer/unix/*.c",
    "src/file/cocoa/*.m",
    "src/core/unix/*.c",
    "src/joystick/virtual/*.c",
    "src/sensor/dummy/*.c",
}


target("sdl2")
    set_kind("static")
    add_includedirs(path.join(sdlPath, "include"), path.join(sdlPath, "src/video/khronos"))
    for _, f in ipairs(sdlSrc) do
        add_files(path.join(sdlPath, f))
    end
end



if targets["jpeg"] == true then

local jpegSourceDir = path.join(os.scriptdir(), "jpeg/jpeg-9e")

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
    add_includedirs(path.join(sdlPath, "include"))
    add_defines(
        -- "HAVE_STDIO_H=1",
        -- "HAVE_STDLIB_H=1",
        -- "HAVE_STRING_H=1",
        -- "HAVE_INTTYPES_H=1",
        -- "HAVE_STDINT_H=1",
        -- "HAVE_STRINGS_H=1",
        -- "HAVE_SYS_STAT_H=1",
        -- "HAVE_SYS_TYPES_H=1",
        -- "HAVE_UNISTD_H=1",
        -- "STDC_HEADERS=1",
        -- "HAVE_DLFCN_H=1",
        "USE_STBIMAGE=1",
        "USE_IMAGEIO=1",
        -- "LOAD_AVIF=1",
        "LOAD_JPG=1",
        -- "LOAD_JXL=1",
        "LOAD_PNG=1",
        -- "LOAD_WEBP=1",
        "LOAD_BMP=1",
        -- "LOAD_GIF=1",
        -- "LOAD_LBM=1",
        -- "LOAD_PCX=1",
        -- "LOAD_PNM=1",
        "LOAD_SVG=1",
        -- "LOAD_TGA=1",
        -- "LOAD_XCF=1",
        -- "LOAD_XPM=1",
        -- "LOAD_XV=1",
        -- "LOAD_QOI=1",
        "SDL_IMAGE_SAVE_PNG=1",
        "SDL_IMAGE_SAVE_JPG=1"
    )
    if is_host("macosx") then
        set_values("objc.build.arc", false)
        add_mxxflags("-fno-objc-arc")
    end
    for _, f in ipairs({
        "*.c",
        "*.m",
    }) do
        add_files(path.join(sdlImagePath, f))
    end
