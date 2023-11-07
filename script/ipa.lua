
import("core.base.option")
import("utils.archive")

local options = {
    {nil, "input",  "v", nil, "app 文件夹路径."},
    {nil, "output",  "v", nil, "输出 ipa 文件路径."},
}

function main(...)
    local argv = option.parse({...}, options, "打包 ipa 文件")
    if not argv.input then
        argv.help()
        return
    end
    local output = path.absolute(argv.output and argv.output or argv.input:sub(1, -4).."ipa")
    local swap = output:sub(1, -4).."zip"
    local swap_dir = path.absolute(path.join(os.scriptdir(), "../build/ipa_swap"))
    -- os.tryrm(swap_dir)
    -- os.mkdir(path.join(swap_dir, "Payload"))
    -- os.trycp(argv.input, path.join(swap_dir, "Payload"))
    archive.archive(swap, "Payload", {recurse = true, curdir = swap_dir})
    os.mv(swap, output)
    print(output, "done")
end
