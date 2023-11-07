add_rules("mode.debug", "mode.release")

add_includedirs("src")
set_languages("c++17")

local VERSION = os.getenv("VERSION") or "0.9.10-development"
if VERSION:startswith("v") then
    VERSION = VERSION:sub(2)
end

add_repositories("xrepo https://github.com/zeromake/xrepo.git")

option("simd")
    set_default(true)
    set_showmenu(true)
    set_description('开启 simd 优化')
option_end()

option("omp")
    set_default(false)
    set_showmenu(true)
    set_description('使用 opennmp 进行并行')
option_end()

add_defines(
    "ONS_ZERO_VERSION=\""..VERSION.."\"",
    "ONS_JH_VERSION=\"0.8.0\"",
    "ONS_VERSION=\"20181218\"",
    "NSC_VERSION=296"
)

if is_plat("windows") then
    add_defines("XMD_H=1")
    add_cxflags("/utf-8")
    add_cxflags("/UNICODE")

    if get_config("omp") then
        add_cxflags("/openmp")
        add_ldflags("/openmp")
    end
    add_defines("UNICODE", "_UNICODE", "WINVER=0x0606")
    add_includedirs("include/windows")
elseif is_plat("mingw") then
    if get_config("omp") then
        add_cxflags("-fopenmp")
        add_ldflags("-fopenmp")
    end
    add_defines("UNICODE", "_UNICODE", "WINVER=0x0606")
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
add_requireconfs("**.freetype", {system=false})
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
local sdl2_image_config = {
    webp=true
}
if is_plat("windows", "mingw") then
    sdl2_image_config["backend"] = "wic"
elseif is_plat("macosx", "iphoneos") then
    sdl2_image_config["backend"] = "imageio"
elseif is_plat("android") then
    -- 安卓 jpeg 库有 bug 无法显示，改用 stb 替代
    sdl2_image_config["backend"] = "stb"
else
    -- 没有 backend，需要外部库
    sdl2_image_config["backend"] = "common"
    sdl2_image_config["png"] = true
    sdl2_image_config["jpeg"] = true
end
add_requires("sdl2_image", {system=false, configs=sdl2_image_config})

set_rundir("$(projectdir)")

local function use_binary() 
    set_kind("binary")
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
end

target("onscripter")
    if is_plat("android") then
        set_kind("shared")
    elseif is_plat("iphoneos") then
        -- codesign --force --deep --sign - xxx.app
        -- ideviceinstaller -i xxx.app
        -- ideviceinstaller -i xxx.ipa
        set_kind("static")
        add_cxflags("-fembed-bitcode")
        add_mxflags("-fembed-bitcode")
        add_defines("ONSCRIPTER_MAIN_RENAME=onscripter_main")
        add_defines("ONSCRIPTER_MAIN_EXTREN=extern \"C\"")
    else
        use_binary()
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
    add_defines("ONSCRIPTER_EXTEND_INIT=1")
    add_files("src/entry/onscripter_main.cpp")
    add_links("sdl2_main")
    if is_plat("macosx") then
        add_files("src/entry/*.mm")
        add_defines("RENDER_COPY_RECT_FULL=1")
        add_frameworks("AudioToolbox", "Cocoa")
        if get_config("target_minver") then
            local target_minver = tonumber(get_config("target_minver"))
            if target_minver < 10.15 then
                add_defines("INFRA_FORCE_GHC_FS")
            end
        end
    elseif is_plat("iphoneos") then
        add_files("src/entry/*.mm")
        add_defines("RENDER_COPY_RECT_FULL=1")
        if get_config("target_minver") then
            local target_minver = tonumber(get_config("target_minver"))
            if target_minver < 13.0 then
                add_defines("INFRA_FORCE_GHC_FS")
            end
        end
        add_defines("IOS", "GLES_SILENCE_DEPRECATION")
    elseif is_plat("windows", "mingw") then
        add_files("src/resource.rc")
        add_syslinks("windowscodecs")
    elseif is_plat("android") then
        add_packages("ndk-cpufeatures")
        add_defines("SDL_JAVA_PACKAGE_PATH=com_zeromake_onscripter", "ANDROID")
        add_cxflags("-fPIC", {force = true})
        add_ldflags("-fPIC", {force = true})
        add_cxflags("-funwind-tables", {force = true})
    end
    if get_config('simd') then
        add_defines("USE_SIMD=1")
        if is_arch("x86", "x64", "i386", "x86_64") then
            if is_plat("macosx") then
                add_cxflags("-march=knl")
            else
                add_vectorexts("avx2")
            end
        elseif is_arch("arm.*") then
            add_vectorexts("neon")
        else
            add_vectorexts("all")
        end
    end
    add_defines(
        "USE_LUA=1",
        "USE_BUILTIN_LAYER_EFFECTS=1",
        "USE_BUILTIN_EFFECTS=1",
        "USE_PARALLEL=1",
        "FMT_HEADER_ONLY=1"
    )
    if get_config("omp") then
        add_defines("USE_OMP_PARALLEL=1")
    end
    add_files(
        "src/*.cpp",
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
        elseif target:is_plat("iphoneos") then
            import("script.generate_xcode")(target, VERSION)
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


-- target("saveconv")
--     set_kind("binary")
--     if is_plat("mingw") then
--         add_ldflags("-static-libgcc", "-static-libstdc++")
--     end
--     add_files("src/tools/saveconv.cpp")

target("nsdec")
    use_binary()
    add_files(
        "src/tools/nscriptdecode.cpp"
    )

target("sardec")
    use_binary()
    add_packages("bzip2")
    add_files(
        "src/coding2utf16.cpp",
        "src/tools/sardec.cpp",
        "src/reader/SarReader.cpp",
        "src/reader/DirectReader.cpp"
    )

target("nsaenc")
    use_binary()
    add_packages("bzip2", "jpeg")
    add_files(
        "src/tools/nsaenc.cpp",
        "src/coding2utf16.cpp",
        "src/gbk2utf16.cpp",
        "src/reader/DirectReader.cpp",
        "src/reader/NsaReader.cpp",
        "src/reader/SarReader.cpp",
        "src/tools/conv_shared.cpp",
        "src/resize_image.cpp",
        "src/language/*.cpp"
    )

target("nsadec")
    use_binary()
    add_packages("bzip2")
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

target("arcmake")
    use_binary()
    add_packages("bzip2", "jpeg")
    add_files(
        "src/tools/arcmake.cpp",
        "src/coding2utf16.cpp",
        "src/gbk2utf16.cpp",
        "src/reader/DirectReader.cpp",
        "src/reader/NsaReader.cpp",
        "src/reader/SarReader.cpp",
        "src/tools/conv_shared.cpp",
        "src/resize_image.cpp",
        "src/language/*.cpp"
    )


-- target("sarconv")
--     use_binary()
--     add_packages("bzip2", "jpeg")
--     add_files(
--         "src/tools/sarconv.cpp",
--         "src/coding2utf16.cpp",
--         "src/reader/DirectReader.cpp",
--         "src/reader/SarReader.cpp",
--         "src/tools/conv_shared.cpp",
--         "src/resize_image.cpp"
--     )
-- target("nsaconv")
--     add_packages("bzip2", "jpeg")
--     use_binary()
--     add_files(
--         "src/tools/nsaconv.cpp",
--         "src/coding2utf16.cpp",
--         "src/tools/conv_shared.cpp",
--         "src/gbk2utf16.cpp",
--         "src/sjis2utf16.cpp",
--         "src/reader/NsaReader.cpp",
--         "src/reader/SarReader.cpp",
--         "src/reader/DirectReader.cpp",
--         "src/resize_image.cpp",
--         "src/language/*.cpp"
--     )