add_rules("mode.debug", "mode.release")

add_includedirs("src")
set_languages("c++17")

local VERSION = os.getenv("VERSION") or "0.9.2"
if VERSION:startswith("v") then
    VERSION = VERSION:sub(2)
end

add_repositories("xrepo https://github.com/zeromake/xrepo.git")

option("simd")
    set_default(true)
    set_showmenu(true)
    set_description('开启 simd 优化')
option_end()

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
    add_includedirs("include/windows")
end

add_includedirs("src", "src/onscripter", "src/reader", "include")

local deps = {
    "zlib",
    "bzip2",
    "jpeg",
    "png",
    "webp",
    "sdl2_mixer",
    "brotli",
    "ghc_filesystem",
    "luajit"
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

add_requireconfs("sdl2", {system=false, configs=sdlConfigs})
add_requireconfs("**.sdl2", {system=false, configs=sdlConfigs})
add_requireconfs("**.harfbuzz", {system=false, configs={freetype=true}})
add_requires("freetype", {
    system=false,
    configs={
        harfbuzz=true,
        zlib=true,
        bzip2=true,
        brotli=true,
        png=true
    }
})
add_requires("sdl2_ttf", {system=false,configs={harfbuzz=true}})
add_requires("sdl2_image", {system=false,configs={
    png=true,
    jpeg=(not is_plat("android")),
    webp=true
}})

set_rundir("$(projectdir)")

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

target("nsdecode")
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

target("arcmake")
    set_kind("binary")
    add_packages("bzip2", "jpeg")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files(
        "src/tools/arcmake.cpp",
        "src/coding2utf16.cpp",
        "src/gbk2utf16.cpp",
        "src/reader/DirectReader.cpp",
        "src/reader/NsaReader.cpp",
        "src/reader/SarReader.cpp",
        "src/conv_shared.cpp",
        "src/resize_image.cpp",
        "src/language/*.cpp"
    )

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
        "ghc_filesystem",
        "luajit"
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
    if get_config('simd') then
        add_defines("USE_SIMD=1")
        if is_plat("macosx") then
            add_cxflags("-march=knm")
        else
            add_vectorexts('all')
        end
    end
    add_defines(
        "USE_LUA=1",
        "USE_BUILTIN_LAYER_EFFECTS=1",
        "USE_BUILTIN_EFFECTS=1",
        "USE_PARALLEL=1",
        "FMT_HEADER_ONLY=1"
    )
    add_files(
        "src/*.cpp|conv_shared.cpp",
        "src/SDL2_rotozoom.c",
        "src/renderer/*.cpp",
        "src/reader/*.cpp",
        "src/onscripter/*.cpp",
        "src/builtin_dll/*.cpp",
        "src/language/*.cpp"
    )
    remove_files("src/AVIWrapper.cpp")
    after_build(function (target)
        if target:is_plat("android") then
            local outDir = "project/android/app/libs/"..target:arch().."/"
            local shareds = {}
            for name, pkg in pairs(target:pkgs()) do
                local libfiles = pkg:libraryfiles()
                for _, libfile in ipairs(libfiles) do
                    if libfile:endswith(".so") then
                        shareds[libfile] = 1
                    end
                end
            end
            for libfile, _ in pairs(shareds) do
                print("cp "..libfile.." "..outDir)
                os.cp(libfile, outDir)
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


-- target("demo/cache")
--     set_kind("binary")
--     add_files(
--         "demo/cache.cpp",
--         "src/murmurhash.c"
--     )
--     if is_plat("mingw") then
--         add_ldflags("-static-libgcc", "-static-libstdc++")
--     end
--     if is_plat("windows") then
--         add_files("src/resource.rc")
--     end
target("demo/demo")
    set_kind("binary")
    add_packages("sdl2", "sdl2_ttf", "freetype", "harfbuzz")
    add_files("demo/demo.cpp")
    add_files("demo/ren-font.cpp")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    if is_plat("windows") then
        add_files("src/resource.rc")
    elseif  is_plat("macosx") then
        add_frameworks("AudioToolbox", "Cocoa")
    end
target("demo/demo1")
    set_kind("binary")
    add_packages("sdl2", "sdl2_ttf", "freetype", "harfbuzz")
    add_files("demo/demo1.cpp")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    if is_plat("windows") then
        add_files("src/resource.rc")
    elseif  is_plat("macosx") then
        add_frameworks("AudioToolbox", "Cocoa")
    end

target("demo/print")
    set_kind("binary")
    add_files("demo/print.c")

target("saveconv")
    set_kind("binary")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_files("src/tools/saveconv.cpp")
