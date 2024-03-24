import("lib.detect.find_tool")
import("core.base.bytes")

local charset_list = {
    ['gb18030_2022'] = 'https://raw.githubusercontent.com/unicode-org/icu/main/icu4c/source/data/mappings/gb18030-2022.ucm',
    ['shift_jis'] = 'https://raw.githubusercontent.com/unicode-org/icu/main/icu4c/source/data/mappings/ibm-943_P15A-2003.ucm',
}

local function ucs4_tostring(ch)
    -- 0x2028 0x2029 vscode 会认为是代码里有不正确的换行
    if ch == 0x2028 or ch == 0x2029 then
        ch = 0xFFFD
    end
    local n = 0
    -- 创建一个 7 长度的 byte 数组
    local out_bytes = bytes(7, 0)
    if ch <= 0x0000007f then
        out_bytes[1] = ch
        n = 1
    elseif ch <= 0x000007ff then
        out_bytes[1] = ((ch >> 6) & 0x1f) | 0xc0
        out_bytes[2] = (ch & 0x3f) | 0x80
        n = 2
    elseif ch <= 0x0000ffff then
        out_bytes[1] = ((ch >> 12) & 0x0f) | 0xe0
        out_bytes[2] = ((ch >> 6) & 0x3f) | 0x80
        out_bytes[3] = (ch & 0x3f) | 0x80
        n = 3
    elseif ch <= 0x001fffff then
        out_bytes[1] = ((ch >> 18) & 0x07) | 0xf0
        out_bytes[2] = ((ch >> 12) & 0x3f) | 0x80
        out_bytes[3] = ((ch >> 6) & 0x3f) | 0x80
        out_bytes[4] = (ch & 0x3f) | 0x80
        n = 4
    elseif ch <= 0x03ffffff then
        out_bytes[1] = ((ch >> 24) & 0x03) | 0xf8
        out_bytes[2] = ((ch >> 18) & 0x3f) | 0x80
        out_bytes[3] = ((ch >> 12) & 0x3f) | 0x80
        out_bytes[4] = ((ch >> 6) & 0x3f) | 0x80
        out_bytes[5] = (ch & 0x3f) | 0x80
        n = 5
    elseif ch <= 0x7fffffff then
        out_bytes[1] = ((ch >> 30) & 0x01) | 0xfc
        out_bytes[2] = ((ch >> 24) & 0x3f) | 0x80
        out_bytes[3] = ((ch >> 18) & 0x3f) | 0x80
        out_bytes[4] = ((ch >> 12) & 0x3f) | 0x80
        out_bytes[5] = ((ch >> 6) & 0x3f) | 0x80
        out_bytes[6] = (ch & 0x3f) | 0x80
        n = 6
    end
    return out_bytes:str(1, n)
end

local function generate_table(name, input, output)
    local outputFile = io.open(output, 'wb')
    local outputDuplicateFile = io.open(output..".txt", 'wb')
    outputFile:write('#include <stdint.h>\n\n')
    local ucs4_keys = {}
    local charset_keys = {}
    local ucs4_map = {}
    local charset_map = {}
    local charset_duplicate = {}
    local ucs4_duplicate = {}
    for line in io.lines(input) do
        if line:startswith("<U") then
            local ucs4_start = 3
            local ucs4_end = line:find(">", ucs4_start)-1
            local byte_start = line:find("\\", ucs4_end)
            local byte_end = line:find(" ", byte_start)-1
            local ucs4_hex = tonumber("0x"..line:sub(ucs4_start, ucs4_end))
            local charset_hex = tonumber("0x"..line:sub(byte_start, byte_end):gsub("\\x", ""))
            if ucs4_hex >= 0x80 and charset_hex >= 0x80 then
                if ucs4_map[ucs4_hex] == nil then
                    ucs4_map[ucs4_hex] = charset_hex
                    table.insert(ucs4_keys, ucs4_hex)
                else
                    if ucs4_duplicate[ucs4_hex] == nil then
                        ucs4_duplicate[ucs4_hex] = {}
                    end
                    ucs4_duplicate[ucs4_hex][charset_hex] = 0
                    ucs4_duplicate[ucs4_hex][ucs4_map[ucs4_hex]] = 1
                end
                if charset_map[charset_hex] == nil then
                    charset_map[charset_hex] = ucs4_hex
                    table.insert(charset_keys, charset_hex)
                else
                    if charset_duplicate[charset_hex] == nil then
                        charset_duplicate[charset_hex] = {}
                    end
                    charset_duplicate[charset_hex][ucs4_hex] = 0
                    charset_duplicate[charset_hex][charset_map[charset_hex]] = 1
                end
            end
        end
    end
    table.sort(ucs4_keys)
    table.sort(charset_keys)
    outputFile:writef('static uint32_t g_charset_ucs4_to_%s_table_data[] = {\n', name)
    local prev_k = nil
    local prev_start_k = nil
    local prev_start_index = nil
    local search = {}
    local merge_count = 128
    local index = 0
    local insert_search = function (_start, _end, force)
        local offset = _end - _start
        if offset >= merge_count or force then
            table.insert(search, {prev_start_k, prev_k, prev_start_index})
            -- outputFile:writef('    /* ----------------- 0x%08X -> 0x%08X %d ----------------- */\n', prev_start_k, prev_k, prev_start_index)
            prev_start_k = _end
            prev_start_index = index
        else
            for kk = _start, _end - 1 do
                outputFile:writef('    0x%08X,\n', 0)
                index = index + 1
            end
        end
    end
    for _, k in ipairs(ucs4_keys) do
        if prev_start_k == nil then
            prev_start_k = k
            prev_start_index = index
        end
        if prev_k ~= nil and prev_k < k then
            local s = prev_k
            local d = k
            insert_search(prev_k, k)
        end
        outputFile:writef('    0x%08X, // `%s` %d -> 0x%08X\n', ucs4_map[k], ucs4_tostring(k), index, k)
        prev_k = k + 1
        index = index + 1
    end
    insert_search(0, 0, true)
    outputFile:write('};\n')
    outputFile:writef('static uint32_t g_charset_ucs4_to_%s_search_data[][3] = {\n', name)
    for _, item in ipairs(search) do
        outputFile:writef('    {0x%08X, 0x%08X, %d},\n', item[1], item[2], item[3])
    end
    outputFile:write('};\n')
    outputFile:writef('static uint32_t g_charset_%s_to_ucs4_table_data[] = {\n', name)
    prev_start_k = nil
    prev_start_index = nil
    search = {}
    index = 0
    for _, k in ipairs(charset_keys) do
        if prev_start_k == nil then
            prev_start_k = k
            prev_start_index = index
        end
        if prev_k ~= nil and prev_k < k then
            local s = prev_k
            local d = k
            insert_search(prev_k, k)
        end
        outputFile:writef('    0x%08X, // `%s` %d -> 0x%08X\n', charset_map[k], ucs4_tostring(charset_map[k]), index, k)
        prev_k = k + 1
        index = index + 1
    end
    insert_search(0, 0, true)
    outputFile:write('};\n')
    outputFile:writef('static uint32_t g_charset_%s_to_ucs4_search_data[][3] = {\n', name)
    for _, item in ipairs(search) do
        outputFile:writef('    {0x%08X, 0x%08X, %d},\n', item[1], item[2], item[3])
    end
    outputFile:write('};\n')
    outputDuplicateFile:writef('# charset %s_to_ucs4 duplicate\n', name)
    for k, v in pairs(charset_duplicate) do
        outputDuplicateFile:writef('%d ', k)
        for kk, vv in pairs(v) do
            outputDuplicateFile:writef('%d ', kk)
        end
        outputDuplicateFile:write('\n');
    end
    outputDuplicateFile:writef('# charset ucs4_to_%s duplicate\n', name)
    for k, v in pairs(ucs4_duplicate) do
        outputDuplicateFile:writef('%d ', k)
        for kk, vv in pairs(v) do
            outputDuplicateFile:writef('%d ', kk)
        end
        outputDuplicateFile:write('\n');
    end
    outputFile:close()
    outputDuplicateFile:close()
end

function main()
    if not os.exists("build/download") then
        os.mkdir("build/download")
    end
    local curl = find_tool("curl")
    for k, ucm_url in pairs(charset_list) do
        local ucm_path = path.join("build/download", k..".ucm")
        if not os.exists(ucm_path) then
            os.vexecv(curl.program, {"-L", "-o", ucm_path, ucm_url})
        end
        generate_table(k, ucm_path, path.join("src/charset", k..".g"))
    end
end
