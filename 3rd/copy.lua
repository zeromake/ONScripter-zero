local arch = os.getenv("ARCH") or "x64"
if arch == "x86_64" then
    arch = "x64"
end
local is32bit = arch ~= "x64" and arch ~= "arm64"
local isarm64 = arch == "arm64"

local includeDir = path.join(os.scriptdir(), "include")
local libDir = path.join(os.scriptdir(), "lib-"..arch)

os.mkdir(includeDir)
os.mkdir(libDir)

if is_host("macosx") then
    if isarm64 then
        os.cp(path.join(os.scriptdir(), "../build/macosx/arm64/release/*.a"), libDir)
    else
        os.cp(path.join(os.scriptdir(), "../build/macosx/x86_64/release/*.a"), libDir)
    end
elseif is_host("windows") then
    if is32bit then
        os.cp(path.join(os.scriptdir(), "build/mingw/x86/release/*.a"), libDir)
        os.cp(path.join(os.scriptdir(), "build/windows/x86/release/*.lib"), libDir)
    else
        os.cp(path.join(os.scriptdir(), "build/mingw/x86_64/release/*.a"), libDir)
        os.cp(path.join(os.scriptdir(), "build/windows/x64/release/*.lib"), libDir)
    end
end

local headerFiles = {
    {
        "bzip2/bzip2-1.0.6/",
        {
            "bzlib.h",
        }
    },
    {
        "jpeg/jpeg-9e/",
        {
            "jconfig.h",
            "jpeglib.h",
            "jmorecfg.h",
            "jpegint.h",
            "jerror.h",
        },
    },
    {
        "libsdl/SDL2-2.24.1/",
        {
            "include/*.h",
        },
    },
    {
        "libsdl_image/SDL2_image-2.6.2/",
        {
            "SDL_image.h",
        },
    },
}

for _, item in ipairs(headerFiles) do
    local target = includeDir.."/"
    if #item > 2 then
        target = target..item[3]
    end
    for _, f in ipairs(item[2]) do
        local source = os.scriptdir().."/"..item[1]..f
        if os.exists(source) or f:endswith("*.h") then
            printf("cp %s %s\n", source, target)
            os.cp(source, target)
        end
    end
end
