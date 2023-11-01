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
    if is_plat("mingw") then
        add_ldflags("-static-libgcc", "-static-libstdc++")
    end
    add_defines("ONSCRIPTER_EXTEND_INIT=1")
    add_files("src/entry/onscripter_main.cpp")
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
        "src/*.cpp|conv_shared.cpp",
        "src/SDL2_rotozoom.c",
        "src/renderer/*.cpp",
        "src/reader/*.cpp",
        "src/onscripter/*.cpp",
        "src/builtin_dll/*.cpp",
        "src/language/*.cpp"
    )
    remove_files("src/AVIWrapper.cpp")
    on_config(function (target)
        import("script.duplicate")
        local links = duplicate.new()
        local link_dirs = duplicate.new()
        local include_dirs = duplicate.new()
        local path_sep = os.host() == 'windows' and '\\' or '/'
        local frameworks = duplicate.new()
        for _, framework in ipairs(target:get("frameworks")) do
            frameworks:add(framework)
        end
        for name, pkg in pairs(target:pkgs()) do
            local pkg_links = pkg:get("links")
            if pkg_links ~= nil then
                pkg_links = type(pkg_links) == "table" and pkg_links or {pkg_links}
                for _, link in ipairs(pkg_links) do
                    links:add(link)
                end
            end
            local pkg_link_dirs = pkg:get("linkdirs")
            if pkg_link_dirs ~= nil then
                pkg_link_dirs = type(pkg_link_dirs) == "table" and pkg_link_dirs or {pkg_link_dirs}
                for _, link_dir in ipairs(pkg_link_dirs) do
                    local k = link_dir:split(path_sep, {plain = true})[6]
                    link_dirs:add(link_dir, k)
                end
            end
            local pkg_includes = pkg:get("sysincludedirs")
            if pkg_includes ~= nil then
                pkg_includes = type(pkg_includes) == "table" and pkg_includes or {pkg_includes}
                for _, include in ipairs(pkg_includes) do
                    local k = include:split(path_sep, {plain = true})[6]
                    include_dirs:add(include, k)
                end
            end
            
            local pkg_frameworks = pkg:get("frameworks")
            if pkg_frameworks ~= nil then
                pkg_frameworks = type(pkg_frameworks) == "table" and pkg_frameworks or {pkg_frameworks}
                for _, framework in ipairs(pkg_frameworks) do
                    frameworks:add(framework)
                end
            end
        end
        print("links:", links.value)
        print("link_dirs:", link_dirs.value)
        print("include_dirs:", include_dirs.value)
        print("frameworks:", frameworks.value)
    end)
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
