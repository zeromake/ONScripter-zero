add_rules("mode.debug", "mode.release")

add_includedirs("src")
add_defines("USE_SIMD_X86_AVX2=1")
-- set_languages("cxx11")

if is_host("windows") then
    add_defines("XMD_H=1")
    add_cxflags("/utf-8")
    add_cxflags("/UNICODE")
    add_defines("UNICODE", "_UNICODE")
end

add_includedirs("3rd/include", "src", "src/onscripter", "src/reader")
add_linkdirs("3rd/lib-x64")

target("nsaconv")
    set_kind("binary")
    add_links("bzip2", "jpeg")
    add_files(
        "src/tools/nsaconv.cpp",
        "src/coding2utf16.cpp",
        "src/conv_shared.cpp",
        "src/gbk2utf16.cpp",
        "src/sjis2utf16.cpp",
        "src/reader/NsaReader.cpp",
        "src/reader/SarReader.cpp",
        "src/reader/DirectReader.cpp",
        "src/resize_image.cpp",
        "src/language/*.cpp"
    )

target("nsadec")
    set_kind("binary")
    add_links("bzip2")
    add_files(
        "src/tools/nsadec.cpp",
        "src/coding2utf16.cpp",
        "src/gbk2utf16.cpp",
        "src/sjis2utf16.cpp",
        "src/reader/NsaReader.cpp",
        "src/reader/DirectReader.cpp",
        "src/reader/SarReader.cpp",
        "src/language/*.cpp"
    )

target("nscriptdecode")
    set_kind("binary")
    add_files(
        "src/tools/nscriptdecode.cpp"
    )

target("sarconv")
    set_kind("binary")
    add_links("bzip2", "jpeg")
    add_files(
        "src/tools/sarconv.cpp",
        "src/coding2utf16.cpp",
        "src/reader/DirectReader.cpp",
        "src/reader/SarReader.cpp",
        "src/conv_shared.cpp",
        "src/resize_image.cpp"
    )

target("sardec")
    set_kind("binary")
    add_links("bzip2")
    add_files(
        "src/coding2utf16.cpp",
        "src/tools/sardec.cpp",
        "src/reader/SarReader.cpp",
        "src/reader/DirectReader.cpp"
    )

target("onscripter")
    set_kind("binary")
    add_links(
        "z",
        "bzip2",
        "jpeg",
        "png",
        "sdl2",
        "sdl2_image",
        "sdl2_ttf",
        "sdl2_mixer",
        "freetype",
        "brotli",
        "harfbuzz"
    )
    if is_host("macosx") then
        add_frameworks(
            "OpenGL",
            "CoreVideo",
            "CoreAudio",
            "AudioToolbox",
            "Carbon",
            "CoreGraphics",
            "ForceFeedback",
            "Metal",
            "AppKit",
            "IOKit",
            "CoreFoundation",
            "Foundation",
            "GameController",
            "CoreHaptics"
        )
        add_syslinks("iconv")
        add_defines("RENDER_COPY_RECT_FULL=1")
    elseif is_host("windows") then
        add_syslinks("gdi32", "user32", "winmm", "shell32", "setupapi", "advapi32", "ole32", "version", "imm32", "oleaut32")
    end
    add_packages("libsdl_ttf", "libsdl_mixer")
    add_defines("USE_BUILTIN_LAYER_EFFECTS=1", "USE_BUILTIN_EFFECTS=1", "USE_PARALLEL=1", "FMT_HEADER_ONLY=1")
    add_files("src/*.cpp", "src/renderer/*.cpp", "src/reader/*.cpp", "src/onscripter/*.cpp", "src/builtin_dll/*.cpp", "src/language/*.cpp")
    add_files("src/resource.rc")
    remove_files("src/AVIWrapper.cpp", "src/LUAHandler.cpp")
