
local keys = {
    "space",
    "minus",
    "bracket",
    "num_str",
    "DEFAULT_START_KINSOKU",
    "DEFAULT_END_KINSOKU",
    "DEFAULT_SAVE_MENU_NAME",
    "DEFAULT_LOAD_MENU_NAME",
    "DEFAULT_SAVE_ITEM_NAME",
    "MESSAGE_SAVE_EXIST",
    "MESSAGE_SAVE_EMPTY",
    "MESSAGE_SAVE_CONFIRM",
    "MESSAGE_LOAD_CONFIRM",
    "MESSAGE_RESET_CONFIRM",
    "MESSAGE_END_CONFIRM",
    "MESSAGE_YES",
    "MESSAGE_NO",
    "MESSAGE_OK",
    "MESSAGE_CANCEL",
}

local typesTo = {
    gbk = "GBK2UTF16",
    sjis = "SJIS2UTF16",
}
for _, filepath in ipairs(os.files("src/language/*.txt")) do
    if filepath:endswith(".txt") then
        local typesName = path.basename(filepath)
        local types = typesTo[typesName]
        local out = string.sub(filepath, 0, #filepath-4)..".cpp"
        local outFile = io.open(out, "w")
        local offset = 1;
        outFile:writef("#include \"%s2utf16.h\"\n#include <string.h>\n\n", typesName)
        outFile:writef("void %s::init_static_string() {\n", types)
        local inputFile = io.open(filepath, "rb")
        for line in inputFile:lines() do
            local index = 1
            outFile:writef("    strcpy(%s, \"", keys[offset])
            local first = true
            while(index <= #line-1) do
                local char = string.byte(line, index)
                outFile:writef("\\x%x", char)
                index = index+1
            end
            outFile:writef("\");\n")
            offset = offset + 1
        end
        outFile:write("}\n")
        outFile:close()
        inputFile:close()
    end
end
