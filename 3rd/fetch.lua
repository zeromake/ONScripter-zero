import("net.http")
import("utils.archive")
import("lib.detect.find_program")

local downloads = {
    {
        "zlib",
        "https://github.com/madler/zlib/releases/download/v1.2.13/zlib1213.zip",
    },
    {
        "libpng-apng",
        "https://download.sourceforge.net/apng/libpng/libpng16/libpng-1.6.38-apng.zip",
    },
    {
        "png",
        "https://download.sourceforge.net/libpng/libpng-1.6.38.tar.gz",
    },
    {
        "bzip2",
        "https://download.sourceforge.net/bzip2/bzip2-1.0.6.tar.gz"
    },
    {
        "jpeg",
        "http://www.ijg.org/files/jpegsr9e.zip",
    },
    {
        "libsdl",
        "https://github.com/libsdl-org/SDL/releases/download/release-2.24.1/SDL2-2.24.1.tar.gz",
    },
    {
        "libsdl_image",
        "https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.2/SDL2_image-2.6.2.tar.gz",
    },
    {
        "libsdl_ttf",
        "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.1/SDL2_ttf-2.20.1.tar.gz",
    },
    {
        "freetype",
        "https://gitlab.freedesktop.org/freetype/freetype/-/archive/VER-2-12-1/freetype-VER-2-12-1.tar.bz2",
        "freetype-2.12.1.tar.bz2",
    },
    {
        "harfbuzz",
        "https://github.com/harfbuzz/harfbuzz/releases/download/5.3.1/harfbuzz-5.3.1.tar.xz",
    },
    {
        "brotli",
        "https://github.com/google/brotli/archive/refs/tags/v1.0.9.tar.gz",
        "brotli-1.0.9.tar.gz",
    },
    {
        "fmt",
        "https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip",
    },
    {
        "smpeg",
        "https://github.com/icculus/smpeg/archive/refs/heads/main.zip",
    },
    {
        "libsdl_mixer",
        "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.2/SDL2_mixer-2.6.2.tar.gz",
    },
    {
        "libde265",
        "https://github.com/strukturag/libde265/releases/download/v1.0.9/libde265-1.0.9.tar.gz",
    },
    {
        "mp4v2",
        "https://github.com/enzo1982/mp4v2/releases/download/v2.1.1/mp4v2-2.1.1.tar.bz2",
    },
    {
        "fdk-aac",
        "https://sourceforge.net/projects/opencore-amr/files/fdk-aac/fdk-aac-2.0.2.tar.gz/download",
        "fdk-aac-2.0.2.tar.gz",
    },
    {
        "libwebp",
        "https://github.com/webmproject/libwebp/archive/refs/tags/v1.2.4.tar.gz",
        "libwebp-1.2.4.tar.gz",
    },
}

local downloadDir = path.join(os.scriptdir(), "download")
local curlBin = find_program("curl")
local patchBin = find_program("patch")
local proxyUrl = nil --"socks5://127.0.0.1:10800"

os.mkdir(downloadDir)

local function curlDowload(url, f)
    local argv = {"-L", "--ssl-no-revoke", "-o", f}
    if proxyUrl ~= nil then
        table.insert(argv, "-x")
        table.insert(argv, proxyUrl)
    end
    table.insert(argv, url)
    os.execv("curl", argv)
end

for _, item in ipairs(downloads) do
    local f = path.join(downloadDir, path.filename(item[2]))
    if #item > 2 then
        f = path.join(downloadDir, item[3])
    end
    if not os.exists(f) then
        print("download "..f.." ing")
        if curlBin ~= nil then
            curlDowload(item[2], f)
        else
            http.download(item[2], f)
        end
        print("download "..f.." done")
    end
    local target = path.join(os.scriptdir(), item[1])
    if not os.exists(target) then
        if item[2]:endswith(".h") then
            os.mkdir(target)
            os.cp(f, target.."/")
        else
            archive.extract(f, target)
        end
        if item[1] == "png" and os.exists(path.join(os.scriptdir(), "libpng-apng/libpng-1.6.38-apng.patch")) and patchBin ~= nil then
            os.cd(path.join(target, "libpng-1.6.38"))
            os.exec("patch -p 0 -i ../../libpng-apng/libpng-1.6.38-apng.patch")
            os.cd("-")
        end
    end
end
