add_rules("mode.debug", "mode.release")

add_requires("libsdl", "libsdl_image", "libsdl_ttf", "libsdl_mixer", "bzip2", "libjpeg")
add_includedirs("src")
add_defines("XMD_H=1", "USE_SIMD_X86_AVX2=1")
add_cxxflags("/UNICODE")
add_defines("UNICODE", "_UNICODE")

target("nsaconv")
    set_kind("binary")
    add_packages("bzip2", "libjpeg")
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
    add_packages("bzip2")
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
    add_packages("bzip2", "libjpeg")
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
    add_packages("bzip2")
    add_files(
        "tools/sardec.cpp",
        "src/SarReader.cpp",
        "src/coding2utf16.cpp",
        "src/DirectReader.cpp"
    )

target("onscripter")
    set_kind("binary")
    add_packages("bzip2", "libjpeg")
    add_packages("libsdl", "libsdl_image", "libsdl_ttf", "libsdl_mixer")
    add_defines("USE_BUILTIN_LAYER_EFFECTS=1", "USE_BUILTIN_EFFECTS=1")
    add_files("src/*.cpp", "src/renderer/*.cpp", "src/builtin_dll/*.cpp")
    remove_files("src/AVIWrapper.cpp", "src/LUAHandler.cpp")
