import("net.http")
import("utils.archive")
import("lib.detect.find_program")

local downloads = {
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
}

local downloadDir = path.join(os.scriptdir(), "download")
local curlBin = find_program("curl")
local proxyUrl = nil --"socks5://127.0.0.1:10800"

os.mkdir(downloadDir)

local function curlDowload(url, f)
    local argv = {"-L", "-o", f}
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
    end
end
