add_rules("mode.debug", "mode.release")

add_includedirs("src")
set_languages("c++17")

local VERSION = os.getenv("VERSION") or "0.8.0"
if VERSION:startswith("v") then
    VERSION = VERSION:sub(2)
end

add_defines(
    "ONS_JH_VERSION=\""..VERSION.."\"",
    "ONS_VERSION=\"20181218\"",
    "NSC_VERSION=296"
)

if is_plat("windows") then
    add_defines("XMD_H=1")
    add_cxflags("/utf-8")
    add_cxflags("/UNICODE")
    add_defines("UNICODE", "_UNICODE", "WINVER=0x0606")
end

add_includedirs("src", "src/onscripter", "src/reader", "include")

local deps = {
    "zlib",
    "bzip2",
    "jpeg",
    "png",
    "webp",
    -- "sdl2",
    "sdl2_ttf",
    "sdl2_mixer",
    "brotli",
    "ghc_filesystem"
}
local sdlConfigs = {}

if is_plat("android") then
    table.insert(deps, "ndk-cpufeatures")
    sdlConfigs["shared"] = true
end

local dep_opt = {system=false}
for _, dep_name in ipairs(deps) do
    add_requires(dep_name, dep_opt)
end

add_requires("sdl2", {system=false, configs=sdlConfigs})
add_requires("freetype", {system=false,configs={
    zlib=true,
    bzip2=true,
    brotli=true,
    png=true,
    harfbuzz=true
}, lazy_options={system=false,configs={harfbuzz=false}}})
add_requires("sdl2_image", {system=false,configs={
    png=true,
    jpeg=true,
    webp=true
}})
add_requires("harfbuzz", {system=false,configs={freetype=true}})
add_requires("sdl2_ttf", {system=false,configs={harfbuzz=true}})

target("nsaconv")
    set_kind("binary")
    add_packages("bzip2", "jpeg")
    if is_plat("mingw") then
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
    if is_plat("mingw") then
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
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files(
        "src/tools/nscriptdecode.cpp"
    )

target("sarconv")
    set_kind("binary")
    add_packages("bzip2", "jpeg")
    if is_plat("mingw") then
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
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files(
        "src/coding2utf16.cpp",
        "src/tools/sardec.cpp",
        "src/reader/SarReader.cpp",
        "src/reader/DirectReader.cpp"
    )

target("nsaenc")
    set_kind("binary")
    add_packages("bzip2", "jpeg")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files(
        "src/tools/nsaenc.cpp",
        "src/coding2utf16.cpp",
        "src/gbk2utf16.cpp",
        "src/reader/DirectReader.cpp",
        "src/reader/NsaReader.cpp",
        "src/reader/SarReader.cpp",
        "src/conv_shared.cpp",
        "src/resize_image.cpp",
        "src/language/*.cpp"
    )

target("nt3dec")
    set_kind("binary")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files("src/tools/nt3dec.c")


target("va")
    set_kind("binary")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files("demo/va.cpp")

target("onscripter")
    if is_plat("android") then
        set_kind("shared")
    else
        set_kind("binary")
    end
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
        "brotli",
        "ghc_filesystem"
    )
    if is_plat("windows") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    if is_plat("macosx") then
        add_files("src/entry/onscripter_main.mm")
        add_defines("RENDER_COPY_RECT_FULL=1")
        add_frameworks("AudioToolbox", "Cocoa")
    elseif is_plat("iphoneos") then
        add_files("src/entry/onscripter_main.mm")
        add_defines("IOS", "INFRA_FORCE_GHC_FS", "GLES_SILENCE_DEPRECATION")
    elseif is_plat("windows") then
        add_files("src/resource.rc", "src/entry/onscripter_main.cpp")
    elseif is_plat("android") then
        add_packages("ndk-cpufeatures")
        add_files("src/entry/onscripter_main.cpp")
        add_defines("SDL_JAVA_PACKAGE_PATH=com_zeromake_onscripter", "ANDROID")
        add_cxflags("-fPIC", {force = true})
        add_ldflags("-fPIC", {force = true})
        add_cxflags("-funwind-tables", {force = true})
    else
        add_files("src/entry/onscripter_main.cpp")
    end

    if is_arch("x86", "x64", "i386", "x86_64") then
        add_vectorexts("avx2")
        add_defines("USE_SIMD=1", "USE_SIMD_X86_AVX2=1")
    elseif is_arch("arm.*") then
        add_vectorexts("neon")
    end
    add_defines("USE_BUILTIN_LAYER_EFFECTS=1", "USE_BUILTIN_EFFECTS=1", "USE_PARALLEL=1", "FMT_HEADER_ONLY=1")
    add_files("src/*.cpp", "src/renderer/*.cpp", "src/reader/*.cpp", "src/onscripter/*.cpp", "src/builtin_dll/*.cpp", "src/language/*.cpp")
    remove_files("src/AVIWrapper.cpp", "src/LUAHandler.cpp")
    after_build(function (target)
        if target:is_plat("android") then
            local outDir = "project/android/app/libs/"..target:arch().."/"
            for _, pkg in pairs(target:pkgs()) do
                if pkg:has_shared() then
                    os.cp(pkg:libraryfiles(), outDir)
                    print("cp "..pkg:libraryfiles().." "..outDir)
                end
            end
            os.cp(target:targetfile(), outDir)
            return
        end
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
        if target:is_plat("windows", "mingw") then
            out_name = out_name..".exe"
        end
        os.cp(target:targetfile(),  path.join(os.scriptdir(), "dist", out_name))
    end)

target("main")
    set_kind("shared")
    add_packages(
        "sdl2"
    )
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    if is_plat("windows") then
        add_files("src/resource.rc")
    end
    if is_plat("android") then
        add_defines("ANDROID")
    elseif is_plat("iphoneos") then
        add_defines("IOS")
    end
    add_files("demo/hello.c")

target("example2")
    set_kind("binary")
    add_packages("freetype")
    add_files("demo/example2.cpp")
-- target("demo")
--     set_kind("binary")
--     add_packages(
--         "zlib",
--         "bzip2",
--         "jpeg",
--         "png",
--         "sdl2",
--         "sdl2_image",
--         "sdl2_ttf",
--         "sdl2_mixer",
--         "freetype",
--         "brotli",
--         "harfbuzz",
--         "webp"
--     )
--     if is_plat("mingw") then
--         add_ldflags("-static-libgcc", "-static-libstdc++")
--     end
--     if is_plat("windows") then
--         add_files("src/resource.rc")
--     end
--     add_files("demo.cpp")
