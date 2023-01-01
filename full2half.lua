import("core.base.option")

local options = {
    {nil, "file",  "vs", nil, "The file name."},
}


function main(...)
    local argv = option.parse({...}, options, "conv text.")
    local out = io.open(argv.file[1]..".conv", "wb")
    local has_frist = false
    local offset = 128;
    local inputFile = io.open(argv.file[1], "rb")
    local is_frist = false;
    for line in inputFile:lines() do
        local index = 1
        while(index <= #line) do
            local char = string.byte(line, index)
            if not is_frist and char > 0x80 and char ~= 0xff then
                is_frist = true
            elseif is_frist then
                is_frist = false
            end
            if char == 0xa3 and is_frist then
                if has_frist then
                    out:write(string.char(char))
                    out:write(string.char(char))
                end
                has_frist = true
            elseif ((char >= 0xc1 and char <= 0xda) or (char >= 0xe1 and char <= 0xfa)) and has_frist then
                -- 全角英文
                has_frist = false
                out:write(string.char(char - offset))
            elseif char >= 0xb0 and char <= 0xb9 then
                -- 全角数字
                has_frist = false
                out:write(string.char(char - offset))
            else
                if has_frist then
                    out:write(string.char(0xa3))
                end
                has_frist = false
                out:write(string.char(char))
            end
            -- printf("\\0x%x %s %s\n", char, tostring(is_frist), tostring(has_frist))
            index = index + 1
        end
    end
    out:close()
    inputFile:close()
end
