add_rules("mode.debug", "mode.release")

add_requires("libsdl_ttf", "libsdl_mixer", "luajit")
add_includedirs("src")
add_defines("USE_SIMD_X86_AVX2=1")
set_languages("c++11")

if is_host("windows") then
    add_defines("XMD_H=1")
    add_cxxflags("/utf-8", "/UNICODE")
    add_defines("UNICODE", "_UNICODE")
end

add_includedirs("3rd/include")
add_linkdirs("3rd/lib-x64")

target("nsaconv")
    set_kind("binary")
    add_links("bzip2", "jpeg")
    add_files(
        "tools/nsaconv.cpp",
        "src/coding2utf16.cpp",
        "src/conv_shared.cpp",
        "src/gbk2utf16.cpp",
        "src/NsaReader.cpp",
        "src/SarReader.cpp",
        "src/DirectReader.cpp",
        "src/resize_image.cpp"
    )

target("nsadec")
    set_kind("binary")
    add_links("bzip2")
    add_files(
        "tools/nsadec.cpp",
        "src/coding2utf16.cpp",
        "src/NsaReader.cpp",
        "src/DirectReader.cpp",
        "src/SarReader.cpp"
    )

target("nscriptdecode")
    set_kind("binary")
    add_files(
        "tools/nscriptdecode.cpp"
    )

target("sarconv")
    set_kind("binary")
    add_links("bzip2", "jpeg")
    add_files(
        "tools/sarconv.cpp",
        "src/coding2utf16.cpp",
        "src/DirectReader.cpp",
        "src/SarReader.cpp",
        "src/conv_shared.cpp",
        "src/resize_image.cpp"
    )

target("sardec")
    set_kind("binary")
    add_links("bzip2")
    add_files(
        "tools/sardec.cpp",
        "src/SarReader.cpp",
        "src/coding2utf16.cpp",
        "src/DirectReader.cpp"
    )

target("onscripter")
    set_kind("binary")
    add_links("bzip2", "jpeg", "sdl2", "sdl2_image")
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
    end
    add_packages("libsdl_ttf", "libsdl_mixer", "luajit")
    add_defines("USE_BUILTIN_LAYER_EFFECTS=1", "USE_BUILTIN_EFFECTS=1", "USE_LUA=1", "USE_PARALLEL=1")
    add_files("src/*.cpp", "src/renderer/*.cpp", "src/builtin_dll/*.cpp")
    remove_files("src/AVIWrapper.cpp")
