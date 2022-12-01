add_rules("mode.debug", "mode.release")

add_includedirs("src")
set_languages("c++17")
add_defines(
    "USE_SIMD_X86_AVX2=1",
    "ONS_JH_VERSION=\"0.8.0\"",
    "ONS_VERSION=\"20181218\"",
    "NSC_VERSION=296",
    "WINVER=0x0606"
)

if is_host("windows") then
    add_defines("XMD_H=1")
    add_cxflags("/utf-8")
    add_cxflags("/UNICODE")
    add_defines("UNICODE", "_UNICODE")
end

add_includedirs("src", "src/onscripter", "src/reader")

add_requires(
    "zlib",
    "bzip2",
    "jpeg",
    "png",
    "webp",
    "sdl2",
    "sdl2_ttf",
    "sdl2_mixer",
    "brotli"
)
add_requires("freetype", {configs={
    zlib=true,
    bzip2=true,
    brotli=true,
    png=true,
    harfbuzz=true
}, lazy_options={configs={harfbuzz=false}}})
add_requires("sdl2_image", {configs={
    png=true,
    jpeg=true,
    webp=true
}})
add_requires("harfbuzz", {configs={freetype=true}})
add_requires("sdl2_ttf", {configs={harfbuzz=true}})

target("nsaconv")
    set_kind("binary")
    add_packages("bzip2", "jpeg")
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
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
    add_packages("bzip2")
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
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
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files(
        "src/tools/nscriptdecode.cpp"
    )

target("sarconv")
    set_kind("binary")
    add_packages("bzip2", "jpeg")
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
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
    add_packages("bzip2")
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files(
        "src/coding2utf16.cpp",
        "src/tools/sardec.cpp",
        "src/reader/SarReader.cpp",
        "src/reader/DirectReader.cpp"
    )

target("onscripter")
    set_kind("binary")
    add_packages(
        "zlib",
        "bzip2",
        "jpeg",
        "png",
        "webp",
        "freetype",
        "harfbuzz",
        "sdl2",
        "sdl2_image",
        "sdl2_ttf",
        "sdl2_mixer",
        "brotli"
    )
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    if is_host("macosx") then
        add_files("src/entry/onscripter_main.mm")
        add_defines("RENDER_COPY_RECT_FULL=1")
    elseif is_host("windows") then
        add_files("src/resource.rc", "src/entry/onscripter_main.cpp")
    end
    add_defines("USE_BUILTIN_LAYER_EFFECTS=1", "USE_BUILTIN_EFFECTS=1", "USE_PARALLEL=1", "FMT_HEADER_ONLY=1")
    add_files("src/*.cpp", "src/renderer/*.cpp", "src/reader/*.cpp", "src/onscripter/*.cpp", "src/builtin_dll/*.cpp", "src/language/*.cpp")
    remove_files("src/AVIWrapper.cpp", "src/LUAHandler.cpp")
    after_build(function (target)
        local out_name = "onscripter"
        if target:is_plat("macosx") then
            out_name = out_name.."-darwin"
        elseif target:is_plat("windows", "mingw") then
            out_name = out_name.."-windows"
        end
        if target:is_arch("x64", "x86_64") then
            out_name = out_name.."-x86_64"
        elseif target:is_arch("arm64") then
            out_name = out_name.."-arm64"
        end
        os.cp(target:targetfile(), "dist/"..out_name)
    end)

target("demo")
    set_kind("binary")
    add_packages(
        "zlib",
        "bzip2",
        "jpeg",
        "png",
        "sdl2",
        "sdl2_image",
        "sdl2_ttf",
        "sdl2_mixer",
        "freetype",
        "brotli",
        "harfbuzz",
        "webp"
    )
    if is_plat("windows") ~= true and is_host("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    if is_plat("windows") then
        add_files("src/resource.rc")
    end
    add_files("demo.cpp")
