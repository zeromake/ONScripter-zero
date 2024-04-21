/* -*- C++ -*-
 *
 *  ScriptHandler.cpp - Script manipulation class
 *
 *  Copyright (c) 2001-2018 Ogapee. All rights reserved.
 *            (C) 2014-2018 jh10001 <jh10001@live.cn>
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ScriptHandler.h"

#include <vector>

#include "coding2utf16.h"
#include "private/utils.h"

extern Coding2UTF16 *coding2utf16;

#define TMP_SCRIPT_BUF_LEN 4096

#define SKIP_SPACE(p) \
    while (*(p) == ' ' || *(p) == '\t') (p)++

ScriptHandler::ScriptHandler() {
    save_dir = NULL;
    num_of_labels = 0;
    script_buffer = NULL;
    kidoku_buffer = NULL;
    log_info[LABEL_LOG].filename = "NScrllog.dat";
    log_info[FILE_LOG].filename = "NScrflog.dat";
    clickstr_list = NULL;

    string_buffer = new char[STRING_BUFFER_LENGTH]{0};
    str_string_buffer = new char[STRING_BUFFER_LENGTH]{0};
    saved_string_buffer = new char[STRING_BUFFER_LENGTH]{0};

    variable_data = NULL;
    extended_variable_data = NULL;
    num_extended_variable_data = 0;
    max_extended_variable_data = 1;
    root_array_variable = NULL;

    screen_width = 640;
    screen_height = 480;
    variable_range = 0;
    global_variable_border = 0;
    screen_scale = onscripter::MakeShared<onscripter::ScaleManager>();
    user_scale = onscripter::MakeShared<onscripter::ScaleManager>();
}

ScriptHandler::~ScriptHandler() {
    reset();

    if (script_buffer) delete[] script_buffer;
    if (kidoku_buffer) delete[] kidoku_buffer;

    delete[] string_buffer;
    delete[] str_string_buffer;
    delete[] saved_string_buffer;
    if (variable_data) delete[] variable_data;
    for (int i = 0; i < 6; i++) {
        if (font_configs[i] != nullptr) {
            delete font_configs[i];
            font_configs[i] = nullptr;
        }
    }
}

void ScriptHandler::reset() {
    current_variable_data.reset(true);
    for (int i = 0; i < variable_range; i++) variable_data[i].reset(true);

    if (extended_variable_data) delete[] extended_variable_data;
    extended_variable_data = NULL;
    num_extended_variable_data = 0;
    max_extended_variable_data = 1;

    ArrayVariable *av = root_array_variable;
    while (av) {
        ArrayVariable *tmp = av;
        av = av->next;
        delete tmp;
    }
    root_array_variable = current_array_variable = NULL;

    // reset log info
    resetLog(log_info[LABEL_LOG]);
    resetLog(log_info[FILE_LOG]);

    // reset number alias
    Alias *alias;
    alias = root_num_alias.next;
    while (alias) {
        Alias *tmp = alias;
        alias = alias->next;
        delete tmp;
    };
    last_num_alias = &root_num_alias;
    last_num_alias->next = NULL;

    // reset string alias
    alias = root_str_alias.next;
    while (alias) {
        Alias *tmp = alias;
        alias = alias->next;
        delete tmp;
    };
    last_str_alias = &root_str_alias;
    last_str_alias->next = NULL;

    // reset misc. variables
    end_status = END_NONE;
    kidokuskip_flag = false;
    text_flag = true;
    linepage_flag = false;
    english_mode = false;
    skip_enabled = false;
    if (clickstr_list) {
        delete[] clickstr_list;
        clickstr_list = NULL;
    }

    is_internal_script = false;
    ScriptContext *sc = root_script_context.next;
    while (sc) {
        ScriptContext *tmp = sc;
        sc = sc->next;
        delete tmp;
    };
    last_script_context = &root_script_context;
    last_script_context->next = NULL;
}

void ScriptHandler::setSaveDir(const char *path) {
    if (save_dir) delete[] save_dir;
    save_dir = new char[strlen(path) + 1]{0};
    strcpy(save_dir, path);
}

FILE *ScriptHandler::fopen(const char *path,
                           const char *mode,
                           bool use_save_dir) {
    char filename[STRING_BUFFER_LENGTH] = {0};
    fpath(path, filename, use_save_dir);
    auto fp = ::fopen(filename, mode);
    return fp;
}

int ScriptHandler::fpath(const char *path, char *result, bool use_save_dir) {
    const char *prefix_dir =
        (use_save_dir && save_dir) ? save_dir : archive_path;
    const int prefix_dir_size = strlen(prefix_dir);
    if (onscripter::fs::path(path).is_absolute() || path[0] == '.') {
        strcpy(result, path);
    } else if (strncmp(path, prefix_dir, prefix_dir_size) != 0) {
        strcpy(result, (onscripter::fs::path(prefix_dir) / path).string().c_str());
    }
    char *buf = result;
    while (*buf != '\0') {
        if ((*buf == '/' || *buf == '\\') &&
            *buf != onscripter::fs::path::preferred_separator)
            *buf = onscripter::fs::path::preferred_separator;
        buf++;
    }
    strcpy(result, onscripter::fs::absolute(result).string().c_str());
    return 0;
}

void ScriptHandler::setKeyTable(const unsigned char *key_table) {
    int i;
    if (key_table) {
        key_table_flag = true;
        for (i = 0; i < 256; i++) this->key_table[i] = key_table[i];
    } else {
        key_table_flag = false;
        for (i = 0; i < 256; i++) this->key_table[i] = i;
    }
}

bool readVariableTextCompatible(const char *buf,
                                char *out,
                                int &variable_count) {
    int offset = 0;
    int outOffset = 0;
    out[outOffset] = buf[offset];
    offset++;
    outOffset++;
#define VERIABLE_TEXT_COMPATIBLE_CHAR(ch)                    \
    ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || \
     (ch >= 'a' && ch <= 'z') || (ch == '_'))
    if (!VERIABLE_TEXT_COMPATIBLE_CHAR(buf[offset])) {
        return false;
    }
    while (VERIABLE_TEXT_COMPATIBLE_CHAR(buf[offset])) {
        out[outOffset] = buf[offset];
        offset++;
        outOffset++;
    }
    variable_count = offset;
    out[outOffset] = '\0';
    return true;
}

bool readVariableText(const char *buf, char *out, int &variable_count) {
    int offset = 0;
    int outOffset = 0;
    out[outOffset] = buf[offset];
    offset++;
    outOffset++;
    char ch = ')';
    if (buf[offset] != '(' && buf[offset] != '{') {
        return readVariableTextCompatible(buf, out, variable_count);
    }
    if (buf[offset] == '{') {
        ch = '}';
    }
    out[outOffset] = '(';
    outOffset++;
    offset++;
    bool has = false;
    while (buf[offset] != ch) {
        if (buf[offset] == '\0' || buf[offset] == '\n') {
            return false;
        }
        has = true;
        out[outOffset] = buf[offset];
        offset++;
        outOffset++;
    }
    if (!has) {
        return false;
    }
    out[outOffset] = ')';
    outOffset++;
    variable_count = offset + 1;
    out[outOffset] = '\0';
    return true;
}

// basic parser function
const char *ScriptHandler::readToken() {
    current_script = next_script;
    wait_script = NULL;
    char *buf = current_script;
    end_status = END_NONE;
    current_variable.type = VAR_NONE;

    text_flag = false;

    SKIP_SPACE(buf);
    markAsKidoku(buf);

readTokenTop:
    string_counter = 0;
    char ch = *buf;
    if (ch == ';') {  // comment
        addStringBuffer(ch);
        do {
            ch = *++buf;
            addStringBuffer(ch);
        } while (ch != '\n' && ch != '\0');
    } else if (ch & 0x80 || (ch >= '0' && ch <= '9') || ch == '@' ||
               ch == '\\' || ch == '/' || ch == '%' || ch == '?' || ch == '$' ||
               ch == '[' || ch == '(' || ch == '<' || ch == '^' ||
#ifndef ENABLE_1BYTE_CHAR
               ch == '`' ||
#endif
               (!english_mode && ch == '>') || ch == '!' || ch == '#' ||
               ch == ',' || ch == '"') {  // text
        bool ignore_clickstr_flag = false;
        // 前四个有效字符（跳过半角空格，和缩进符号）里有双字节的情况这一行直接标记为，没有变量解析可以使用
        // $,%,? 符号。
        bool ignore_variable_flag = false;
        bool eng_flag = false;
        char variable_buff[256]{0};
        while (1) {
            if (IS_TWO_BYTE(ch)) {
                addStringBuffer(ch);
                ch = *++buf;
                if (ch == '\n' || ch == '\0') break;
                addStringBuffer(ch);
                buf++;
                if (!wait_script && !ignore_clickstr_flag &&
                    checkCountClickstr(buf - 2) > 0)
                    wait_script = buf;
                ignore_clickstr_flag = false;
            } else {
                ignore_clickstr_flag = false;
                if (!ignore_variable_flag &&
                    (ch == '%' || ch == '?' || ch == '$')) {
                    // 文本变量替换建议用 {} 或者 () 包裹;
                    int variable_count = 0;
                    if (readVariableText(buf, variable_buff, variable_count) &&
                        variable_count > 0) {
                        char *variable_buff_temp = variable_buff;
                        buf += variable_count;
                        if (ch == '%' || ch == '?') {
                            addIntVariable(&variable_buff_temp);
                            SKIP_SPACE(buf);
                        } else if (ch == '$') {
                            addStrVariable(&variable_buff_temp);
                            SKIP_SPACE(buf);
                        }
                    } else {
                        addStringBuffer(ch);
                        buf++;
                    }
                } else {
                    if (ch == '^') {
                        eng_flag = !eng_flag;
                        ignore_variable_flag = eng_flag;
                    }
                    if (ch == '\n' || ch == '\0') break;
                    addStringBuffer(ch);
                    buf++;
                    if (ch == '_') ignore_clickstr_flag = true;
                    if (!wait_script && (ch == '@' || ch == '\\'))
                        wait_script = buf;
                }
            }
            ch = *buf;
        }

        text_flag = true;
    }
#ifdef ENABLE_1BYTE_CHAR
    else if (ch == '`') {
        ch = *++buf;
        while (ch != '`' && ch != '\n' && ch != '\0') {
            if (IS_TWO_BYTE(ch)) {
                addStringBuffer(ch);
                ch = *++buf;
            }
            addStringBuffer(ch);
            ch = *++buf;
        }
        if (ch == '`') buf++;

        text_flag = true;
        end_status |= END_1BYTE_CHAR;
    }
#endif
    else if (english_mode && ch == '>') {
        ch = *++buf;
        while (1) {
            if (ch == '\n' || ch == '\0') break;

            if (ch != '\t') addStringBuffer(ch);
            ch = *++buf;
        }

        text_flag = true;
        end_status |= END_1BYTE_CHAR;
    } else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               ch == '_') {  // command
        do {
            if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
            addStringBuffer(ch);
            ch = *++buf;
        } while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                 (ch >= '0' && ch <= '9') || ch == '_');
    } else if (ch == '*') {  // label
        return readLabel();
    } else if (ch == '~' || ch == '\n' || ch == ':') {
        addStringBuffer(ch);
        markAsKidoku(buf++);
    } else if (ch != '\0') {
        utils::printError(
            "readToken: skip unknown heading character %c (%x)\n", ch, ch);
        buf++;
        goto readTokenTop;
    }

    next_script = checkComma(buf);

    // utils::printInfo("readToken [%s] len=%d [%c(%x)] %p\n", string_buffer,
    // strlen(string_buffer), ch, ch, next_script);

    return string_buffer;
}

const char *ScriptHandler::readLabel() {
    end_status = END_NONE;
    current_variable.type = VAR_NONE;

    current_script = next_script;
    SKIP_SPACE(current_script);
    char *buf = current_script;

    string_counter = 0;
    char ch = *buf;
    if (ch == '$') {
        addStrVariable(&buf);
    } else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               ch == '_' || ch == '*') {
        if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
        addStringBuffer(ch);
        buf++;
        if (ch == '*') SKIP_SPACE(buf);

        ch = *buf;
        while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '_') {
            if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
            addStringBuffer(ch);
            ch = *++buf;
        }
    }
    addStringBuffer('\0');

    next_script = checkComma(buf);

    return string_buffer;
}

const char *ScriptHandler::readStr() {
    end_status = END_NONE;
    current_variable.type = VAR_NONE;

    current_script = next_script;
    SKIP_SPACE(current_script);
    char *buf = current_script;

    string_buffer[0] = '\0';
    string_counter = 0;

    while (1) {
        parseStr(&buf);
        buf = checkComma(buf);
        string_counter += strlen(str_string_buffer);
        if (string_counter + 1 >= STRING_BUFFER_LENGTH)
            errorAndExit("readStr: string length exceeds 2048 bytes.");
        strcat(string_buffer, str_string_buffer);
        if (buf[0] != '+') break;
        buf++;
    }
    next_script = buf;

    return string_buffer;
}

int ScriptHandler::readInt() {
    string_counter = 0;
    string_buffer[string_counter] = '\0';

    end_status = END_NONE;
    current_variable.type = VAR_NONE;

    current_script = next_script;
    SKIP_SPACE(current_script);
    char *buf = current_script;

    int ret = parseIntExpression(&buf);

    next_script = checkComma(buf);

    return ret;
}

bool ScriptHandler::readColor(utils::uchar4 *color) {
    end_status = END_NONE;
    current_script = next_script;
    SKIP_SPACE(current_script);
    int offset = utils::readColor(current_script, color);
    if (offset == 0) {
        return false;
    }
    char *buf = current_script + offset;
    next_script = checkComma(buf);
    return true;
}

void ScriptHandler::nextParam() {
    end_status = END_NONE;
    current_variable.type = VAR_NONE;
    current_script = next_script;
    SKIP_SPACE(current_script);
    current_variable_data.reset(true);
    if (*current_script == '%' || *current_script == '$') {
        readVariable();
        auto variable_data = getVariableData(current_variable.var_no);
        if (current_variable.type == VAR_STR) {
            utils::setStr(&current_variable_data.str, variable_data.str, -1);
        } else {
            current_variable_data.num = variable_data.num;
        }
    } else if (*current_script >= '0' && *current_script <= '9') {
        current_variable_data.num = readInt();
        current_variable.type = VAR_INT;
    } else {
        utils::setStr(&current_variable_data.str, readStr(), -1);
        current_variable.type = VAR_STR;
    }
}

void ScriptHandler::skipToken() {
    SKIP_SPACE(current_script);
    char *buf = current_script;

    bool quat_flag = false;
    bool text_flag = false;
    while (1) {
        if (*buf == '\n' || *buf == 0 ||
            (!quat_flag && !text_flag && (*buf == ':' || *buf == ';')))
            break;
        if (*buf == '"') quat_flag = !quat_flag;
        if (IS_TWO_BYTE(*buf)) {
            buf += 2;
            if (!quat_flag) text_flag = true;
        } else
            buf++;
    }
    if (text_flag && *buf == '\n') buf++;

    next_script = buf;
}

// string access function
char *ScriptHandler::saveStringBuffer() {
    strcpy(saved_string_buffer, string_buffer);
    return saved_string_buffer;
}

// script address direct manipulation function
void ScriptHandler::setCurrent(char *pos) {
    current_script = next_script = pos;
}

void ScriptHandler::pushCurrent(char *pos) {
    pushed_current_script = current_script;
    pushed_next_script = next_script;

    current_script = pos;
    next_script = pos;
}

void ScriptHandler::popCurrent() {
    current_script = pushed_current_script;
    next_script = pushed_next_script;
}

void ScriptHandler::enterExternalScript(char *pos) {
    ScriptContext *sc = new ScriptContext;
    last_script_context->next = sc;
    sc->prev = last_script_context;
    last_script_context = sc;

    is_internal_script = true;
    sc->current_script = current_script;
    current_script = pos;
    sc->next_script = next_script;
    next_script = pos;
    sc->end_status = end_status;
    sc->current_variable = current_variable;
    sc->pushed_variable = pushed_variable;
}

void ScriptHandler::leaveExternalScript() {
    ScriptContext *sc = last_script_context;
    last_script_context = sc->prev;
    last_script_context->next = NULL;
    if (last_script_context->prev == NULL) is_internal_script = false;

    current_script = sc->current_script;
    next_script = sc->next_script;
    end_status = sc->end_status;
    current_variable = sc->current_variable;
    pushed_variable = sc->pushed_variable;
    delete sc;
}

bool ScriptHandler::isExternalScript() { return !is_internal_script; }

int ScriptHandler::getOffset(char *pos) { return pos - script_buffer; }

char *ScriptHandler::getAddress(int offset) { return script_buffer + offset; }

int ScriptHandler::getLineByAddress(char *address) {
    LabelInfo label = getLabelByAddress(address);

    char *addr = label.label_header;
    int line = 0;
    while (address > addr) {
        if (*addr == '\n') line++;
        addr++;
    }
    return line;
}

char *ScriptHandler::getAddressByLine(int line) {
    LabelInfo label = getLabelByLine(line);

    int l = line - label.start_line;
    char *addr = label.label_header;
    while (l > 0) {
        while (*addr != '\n' && *addr != '\0') addr++;
        if (*addr == '\0') {
            addr = label.label_header;
            utils::printError("save file is error goto start\n");
            break;
        }
        addr++;
        l--;
    }
    return addr;
}

ScriptHandler::LabelInfo ScriptHandler::getLabelByAddress(char *address) {
    int i;
    for (i = 0; i < num_of_labels - 1; i++) {
        if (label_info[i + 1].start_address > address) return label_info[i];
    }
    return label_info[i];
}

ScriptHandler::LabelInfo ScriptHandler::getLabelByLine(int line) {
    int i;
    for (i = 0; i < num_of_labels - 1; i++) {
        if (label_info[i + 1].start_line > line) return label_info[i];
    }
    return label_info[i];
}

bool ScriptHandler::isName(const char *name) {
    if (string_buffer[0] == '_')
        return (strncmp(name, string_buffer + 1, strlen(name)) == 0) ? true
                                                                     : false;
    return (strncmp(name, string_buffer, strlen(name)) == 0) ? true : false;
}

bool ScriptHandler::isText() { return text_flag; }

bool ScriptHandler::compareString(const char *buf) {
    SKIP_SPACE(next_script);
    unsigned int i, num = strlen(buf);
    for (i = 0; i < num; i++) {
        unsigned char ch = next_script[i];
        if ('A' <= ch && 'Z' >= ch) ch += 'a' - 'A';
        if (ch != buf[i]) break;
    }
    return (i == num) ? true : false;
}

void ScriptHandler::skipLine(int no) {
    for (int i = 0; i < no; i++) {
        while (*current_script != '\n') current_script++;
        current_script++;
    }
    next_script = current_script;
}

void ScriptHandler::setLinepage(bool val) { linepage_flag = val; }

// function for kidoku history
bool ScriptHandler::isKidoku() { return skip_enabled; }

void ScriptHandler::markAsKidoku(char *address) {
    if (!kidokuskip_flag || is_internal_script) return;

    int offset = current_script - script_buffer;
    if (address) offset = address - script_buffer;
    // utils::printInfo("mark (%c)%x:%x = %d\n", *current_script, offset /8,
    // offset%8, kidoku_buffer[ offset/8 ] & ((char)1 << (offset % 8)));
    if (kidoku_buffer[offset / 8] & ((char)1 << (offset % 8)))
        skip_enabled = true;
    else
        skip_enabled = false;
    kidoku_buffer[offset / 8] |= ((char)1 << (offset % 8));
}

void ScriptHandler::setKidokuskip(bool kidokuskip_flag) {
    this->kidokuskip_flag = kidokuskip_flag;
}

void ScriptHandler::saveKidokuData() {
    FILE *fp;

    if ((fp = fopen("kidoku.dat", "wb", true)) == NULL) {
        utils::printError("can't write kidoku.dat\n");
        return;
    }
    defer([&fp]{fclose(fp);});
    fwrite(kidoku_buffer, 1, script_buffer_length / 8, fp);
}

void ScriptHandler::loadKidokuData() {
    FILE *fp;

    setKidokuskip(true);
    kidoku_buffer = new char[script_buffer_length / 8 + 1]{0};
    memset(kidoku_buffer, 0, script_buffer_length / 8 + 1);

    if ((fp = fopen("kidoku.dat", "rb", true)) != NULL) {
        defer([&fp]{fclose(fp);});
        fread(kidoku_buffer, 1, script_buffer_length / 8, fp);
    }
}

void ScriptHandler::addIntVariable(char **buf) {
    const int buffer_size = 20;
    char num_buf[buffer_size];
    int no = parseInt(buf, true);
    int len = getStringFromInteger(num_buf, buffer_size, no, -1);
    for (int i = 0; i < len; i++) addStringBuffer(num_buf[i]);
}

void ScriptHandler::addStrVariable(char **buf) {
    (*buf)++;
    int no = parseInt(buf);
    VariableData &vd = getVariableData(no);
    if (vd.str) {
        for (unsigned int i = 0; i < strlen(vd.str); i++) {
            addStringBuffer(vd.str[i]);
        }
    }
}

void ScriptHandler::setClickstr(const char *list) {
    if (clickstr_list) delete[] clickstr_list;
    clickstr_list = new char[strlen(list) + 2]{0};
    memcpy(clickstr_list, list, strlen(list) + 1);
    clickstr_list[strlen(list) + 1] = '\0';
}

int ScriptHandler::checkCountClickstr(const char *buf, bool recursive_flag) {
    int result = checkClickstr(buf, recursive_flag);
    int offset = result;
    bool nextClickStr = false;
    if (offset > 0) {
        for (int i = 0; i < 5; i++) {
            if (buf[offset] == 0x00) {
                break;
            }
            int ret = checkClickstr(buf + offset);
            if (ret > 0) {
                nextClickStr = true;
                break;
            }
            if (IS_TWO_BYTE(buf[offset])) {
                offset += 2;
            } else {
                offset += 1;
            }
        }
    }
    if (nextClickStr) {
        result = 0;
    }
    return result;
}

int ScriptHandler::checkClickstr(const char *buf, bool recursive_flag) {
    if (buf[0] == '@' || buf[0] == '\\') return 1;

    if (clickstr_list == NULL) return 0;

    bool double_byte_check = true;
    char *click_buf = clickstr_list;
    while (click_buf[0]) {
#ifdef ENABLE_1BYTE_CHAR
        if (click_buf[0] == '`') {
            click_buf++;
            double_byte_check = false;
            continue;
        }
#endif
        if (double_byte_check) {
            if (click_buf[0] == buf[0] && click_buf[1] == buf[1]) {
                if (!recursive_flag && checkClickstr(buf + 2, true) > 0)
                    return 0;
                return 2;
            }
            click_buf += 2;
        } else {
            if (click_buf[0] == buf[0]) {
                if (!recursive_flag && checkClickstr(buf + 1, true) > 0)
                    return 0;
                return 1;
            }
            click_buf++;
        }
    }

    return 0;
}

int ScriptHandler::getIntVariable(VariableInfo *var_info) {
    if (var_info == NULL) var_info = &current_variable;

    if (var_info->type == VAR_INT)
        return getVariableData(var_info->var_no).num;
    else if (var_info->type == VAR_ARRAY)
        return *getArrayPtr(var_info->var_no, var_info->array, 0);
    return 0;
}

void ScriptHandler::skipAnyVariable() {
    end_status = END_NONE;
    current_script = next_script;
    char *buf = current_script;
    SKIP_SPACE(buf);
    while (*buf != ',' && *buf != '\n' && *buf != '\r' && *buf != '\0' &&
           *buf != ':' && *buf != ';')
        buf++;
    next_script = checkComma(buf);
}

void ScriptHandler::readVariable(bool reread_flag) {
    end_status = END_NONE;
    current_variable.type = VAR_NONE;
    if (reread_flag) next_script = current_script;
    current_script = next_script;
    char *buf = current_script;

    SKIP_SPACE(buf);

    bool ptr_flag = false;
    if (*buf == 'i' || *buf == 's') {
        ptr_flag = true;
        buf++;
    }

    if (*buf == '%') {
        buf++;
        current_variable.var_no = parseInt(&buf);
        current_variable.type = VAR_INT;
    } else if (*buf == '?') {
        ArrayVariable av;
        current_variable.var_no = parseArray(&buf, av);
        current_variable.type = VAR_ARRAY;
        current_variable.array = av;
    } else if (*buf == '$') {
        buf++;
        current_variable.var_no = parseInt(&buf);
        current_variable.type = VAR_STR;
    }

    if (ptr_flag) current_variable.type |= VAR_PTR;

    next_script = checkComma(buf);
}

void ScriptHandler::setInt(VariableInfo *var_info, int val, int offset) {
    if (var_info->type & VAR_INT) {
        setNumVariable(var_info->var_no + offset, val);
    } else if (var_info->type & VAR_ARRAY) {
        *getArrayPtr(var_info->var_no, var_info->array, offset) = val;
    } else {
        errorAndExit("setInt: no variables.");
    }
}

void ScriptHandler::pushVariable() { pushed_variable = current_variable; }

void ScriptHandler::setNumVariable(int no, int val) {
    VariableData &vd = getVariableData(no);
    if (vd.num_limit_flag) {
        if (val < vd.num_limit_lower)
            val = vd.num_limit_lower;
        else if (val > vd.num_limit_upper)
            val = vd.num_limit_upper;
    }
    vd.num = val;
}

int ScriptHandler::getStringFromInteger(char *buffer,
                                        const int buffer_size,
                                        int no,
                                        int num_column,
                                        bool is_zero_inserted) {
    int i, num_space = 0, num_minus = 0;
    if (no < 0) {
        num_minus = 1;
        no = -no;
    }
    int num_digit = 1, no2 = no;
    while (no2 >= 10) {
        no2 /= 10;
        num_digit++;
    }

    if (num_column < 0) num_column = num_digit + num_minus;
    if (num_digit + num_minus <= num_column)
        num_space = num_column - (num_digit + num_minus);
    else {
        for (i = 0; i < num_digit + num_minus - num_column; i++) no /= 10;
        num_digit -= num_digit + num_minus - num_column;
    }

#if defined(ENABLE_1BYTE_CHAR) && defined(FORCE_1BYTE_CHAR)
    if (num_minus == 1) no = -no;
    const int __size = 6;
    char format[__size];
    if (is_zero_inserted)
        snprintf(format, __size, "%%0%dd", num_column);
    else
        snprintf(format, __size, "%%%dd", num_column);
    snprintf(buffer, buffer_size, format, no);

    return num_column;
#else
    int c = 0;
    if (is_zero_inserted) {
        for (i = 0; i < num_space; i++) {
            buffer[c++] = coding2utf16->num_str[0];
            buffer[c++] = coding2utf16->num_str[1];
        }
    } else {
        for (i = 0; i < num_space; i++) {
            buffer[c++] = coding2utf16->space[0];
            buffer[c++] = coding2utf16->space[1];
        }
    }
    if (num_minus == 1) {
        buffer[c++] = coding2utf16->minus[0];
        buffer[c++] = coding2utf16->minus[1];
    }
    c = (num_column - 1) * 2;
    for (i = 0; i < num_digit; i++) {
        buffer[c] = coding2utf16->num_str[no % 10 * 2];
        buffer[c + 1] = coding2utf16->num_str[no % 10 * 2 + 1];
        no /= 10;
        c -= 2;
    }
    buffer[num_column * 2] = '\0';

    return num_column * 2;
#endif
}

int ScriptHandler::openScript(char *path) {
    if (readScript(path) < 0) return -1;
    readConfiguration();
    variable_data = new VariableData[variable_range];
    return labelScript();
}

struct ScriptHandler::LabelInfo ScriptHandler::lookupLabel(const char *label) {
    int i = findLabel(label);

    findAndAddLog(log_info[LABEL_LOG], label_info[i].name, true);
    return label_info[i];
}

struct ScriptHandler::LabelInfo ScriptHandler::lookupLabelNext(
    const char *label) {
    int i = findLabel(label);
    if (i + 1 < num_of_labels) {
        findAndAddLog(log_info[LABEL_LOG], label_info[i + 1].name, true);
        return label_info[i + 1];
    }

    return label_info[num_of_labels];
}

ScriptHandler::LogLink *ScriptHandler::findAndAddLog(LogInfo &info,
                                                     const char *name,
                                                     bool add_flag) {
    char capital_name[256];
    for (unsigned int i = 0; i < strlen(name) + 1; i++) {
        capital_name[i] = name[i];
        if ('a' <= capital_name[i] && capital_name[i] <= 'z')
            capital_name[i] += 'A' - 'a';
        else if (capital_name[i] == '/')
            capital_name[i] = '\\';
    }

    LogLink *cur = info.root_log.next;
    while (cur) {
        if (!strcmp(cur->name, capital_name)) break;
        cur = cur->next;
    }
    if (!add_flag || cur) return cur;

    LogLink *link = new LogLink();
    link->name = new char[strlen(capital_name) + 1]{0};
    strcpy(link->name, capital_name);
    info.current_log->next = link;
    info.current_log = info.current_log->next;
    info.num_logs++;

    return link;
}

void ScriptHandler::resetLog(LogInfo &info) {
    LogLink *link = info.root_log.next;
    while (link) {
        LogLink *tmp = link;
        link = link->next;
        delete tmp;
    }

    info.root_log.next = NULL;
    info.current_log = &info.root_log;
    info.num_logs = 0;
}

ScriptHandler::ArrayVariable *ScriptHandler::getRootArrayVariable() {
    return root_array_variable;
}

void ScriptHandler::addNumAlias(const char *str, int no) {
    Alias *p_num_alias = new Alias(str, no);
    last_num_alias->next = p_num_alias;
    last_num_alias = last_num_alias->next;
}

void ScriptHandler::addStrAlias(const char *str1, const char *str2) {
    Alias *p_str_alias = new Alias(str1, str2);
    last_str_alias->next = p_str_alias;
    last_str_alias = last_str_alias->next;
}

void ScriptHandler::errorAndExit(const char *str) {
    utils::printError(" **** Script error, %s [%s] ***\n", str, string_buffer);
    exit(-1);
}

void ScriptHandler::addStringBuffer(char ch) {
    if (string_counter + 1 == STRING_BUFFER_LENGTH)
        errorAndExit("addStringBuffer: string length exceeds 2048.");
    string_buffer[string_counter++] = ch;
    string_buffer[string_counter] = '\0';
}

ScriptHandler::VariableData &ScriptHandler::getVariableData(int no) {
    if (no >= 0 && no < variable_range) return variable_data[no];

    for (int i = 0; i < num_extended_variable_data; i++)
        if (extended_variable_data[i].no == no)
            return extended_variable_data[i].vd;

    num_extended_variable_data++;
    if (num_extended_variable_data == max_extended_variable_data) {
        ExtendedVariableData *tmp = extended_variable_data;
        extended_variable_data =
            new ExtendedVariableData[max_extended_variable_data * 2];
        if (tmp) {
            memcpy(extended_variable_data,
                   tmp,
                   sizeof(ExtendedVariableData) * max_extended_variable_data);
            delete[] tmp;
        }
        max_extended_variable_data *= 2;
    }

    extended_variable_data[num_extended_variable_data - 1].no = no;

    return extended_variable_data[num_extended_variable_data - 1].vd;
}

// ----------------------------------------
// Private methods

int ScriptHandler::readScript(char *path) {
    archive_path = new char[strlen(path) + 1]{0};
    strcpy(archive_path, path);

    FILE *fp = NULL;
    char filename[10];
    int i, encrypt_mode = 0;
    if ((fp = fopen("0.txt", "rb")) != NULL) {
        encrypt_mode = 0;
    } else if ((fp = fopen("00.txt", "rb")) != NULL) {
        encrypt_mode = 0;
    } else if ((fp = fopen("nscr_sec.dat", "rb")) != NULL) {
        encrypt_mode = 2;
    } else if ((fp = fopen("nscript.___", "rb")) != NULL) {
        encrypt_mode = 3;
    } else if ((fp = fopen("nscript.dat", "rb")) != NULL) {
        encrypt_mode = 1;
    } else if ((fp = fopen("onscript.nt", "rb")) != NULL) {
        encrypt_mode = 15;
    } else if ((fp = fopen("onscript.nt2", "rb")) != NULL) {
        encrypt_mode = 16;
    } else if ((fp = fopen("onscript.nt3", "rb")) != NULL) {
        encrypt_mode = 17;
    }

    if (fp == NULL) {
        utils::printError(
            "can't open any of 0.txt, 00.txt, nscript.dat and nscript.___\n");
        return -1;
    }

    size_t estimated_buffer_length;
    if (encrypt_mode != 0) {
        fseek(fp, 0, SEEK_END);
        estimated_buffer_length = ftell(fp) + 1;
    }

    onscripter::Vector<onscripter::String> unencrypt;
    onscripter::fs::path root_dir;
    if (archive_path && *archive_path) {
        root_dir = archive_path;
    } else {
        root_dir = ".";
    }
    if (encrypt_mode == 0) {
        fclose(fp);
        estimated_buffer_length = 1;

        for (auto f : onscripter::fs::directory_iterator(root_dir)) {
            auto _fp = f.path();
            if (f.is_regular_file() && _fp.extension() == ".txt") {
                auto str = _fp.filename().string();
                bool skip = false;
                size_t offset = 0;
                while (str.length() > offset && str.at(offset) != '.') {
                    unsigned char ch = str.at(offset);
                    if (ch > '9' || ch < '0') {
                        skip = true;
                        break;
                    }
                    offset++;
                }
                if (skip) {
                    continue;
                }
                unencrypt.push_back(_fp.filename().string());
                estimated_buffer_length += f.file_size();
            }
        }
        std::sort(unencrypt.begin(), unencrypt.end());
    }
    static size_t BUFFER_MAX = 1024 * 1024 * 100;
    if (estimated_buffer_length > BUFFER_MAX) {
        // 超过 100MB 的大小可能是 bug
        utils::printError("can't open buffer_length: %d > %d\n",
                          estimated_buffer_length,
                          BUFFER_MAX);
        return -1;
    }
    if (script_buffer) delete[] script_buffer;
    script_buffer = new char[estimated_buffer_length]{0};

    char *p_script_buffer;
    current_script = p_script_buffer = script_buffer;

    tmp_script_buf = new unsigned char[TMP_SCRIPT_BUF_LEN];
    if (encrypt_mode > 0) {
        fseek(fp, 0, SEEK_SET);
        readScriptSub(fp, &p_script_buffer, encrypt_mode);
        fclose(fp);
    } else {
        for (auto f : unencrypt) {
            fp = fopen(f.c_str(), "rb");
            if (fp) {
                readScriptSub(fp, &p_script_buffer, 0);
                fclose(fp);
            }
        }
    }
    delete[] tmp_script_buf;
    script_buffer_length = p_script_buffer - script_buffer;
    return 0;
}

unsigned long getfilesize(FILE *file_ptr) {
    unsigned long posCur = 0;
    unsigned long posEnd = 0;
    posCur = ftell(file_ptr);
    fseek(file_ptr, 0L, SEEK_END);
    posEnd = ftell(file_ptr);
    fseek(file_ptr, posCur, SEEK_SET);
    return posEnd;
}

int ScriptHandler::readScriptSub(FILE *fp, char **buf, int encrypt_mode) {
    unsigned char magic[5] = {0x79, 0x57, 0x0d, 0x80, 0x04};
    int magic_counter = 0;
    bool newline_flag = true;
    bool cr_flag = false;
    bool newlabel_flag = false;

    int size = getfilesize(fp);

    if (encrypt_mode == 3 && !key_table_flag)
        errorAndExit(
            "readScriptSub: the EXE file must be specified with --key-exe "
            "option.");

    size_t len = 0, count = 0;
    size_t offset = -1;
    unsigned char key_buf[4];
    int32_t key = 0;
    int32_t tmp = 0;
    size_t data_size = size - 0x920;
    while (1) {
        if (len == count) {
            len = fread(tmp_script_buf, 1, TMP_SCRIPT_BUF_LEN, fp);
            if (len == 0) {
                if (cr_flag) *(*buf)++ = '\n';
                break;
            }
            count = 0;
        }
        unsigned char ch = tmp_script_buf[count++];
        offset++;
        if (encrypt_mode == 1)
            ch ^= 0x84;
        else if (encrypt_mode == 15 || encrypt_mode == 16) {
            ch ^= 0x85 & 0x97;
            ch -= 1;
        } else if (encrypt_mode == 17) {
            if (offset < 0x91C) {
                continue;
            } else if (offset < 0x920) {
                int index = offset - 0x91C;
                key_buf[index] = ch;
                continue;
            } else {
                if (0x920 == offset) {
                    memcpy(&key, key_buf, 4);
                }
                key ^= ch;
                tmp = key + (ch) * (data_size + 1 - (offset - 0x91f)) +
                      0x5D588B65;
                key = tmp;
                ch ^= tmp;
            }
        } else if (encrypt_mode == 2) {
            ch = (ch ^ magic[magic_counter++]) & 0xff;
            if (magic_counter == 5) magic_counter = 0;
        } else if (encrypt_mode == 3) {
            ch = key_table[(unsigned char)ch] ^ 0x84;
        }

        // 有 \r 但是下一个字符不是 \n 强制补充一个 \n
        if (cr_flag && ch != '\n') {
            *(*buf)++ = '\n';
            newline_flag = true;
            cr_flag = false;
        }

        // 读取 * 开头的 label
        if (ch == '*' && newline_flag && !newlabel_flag) {
            num_of_labels++;
            newlabel_flag = true;
        } else {
            newlabel_flag = false;
        }

        if (ch == '\r') {
            cr_flag = true;
            continue;
        }
        if (ch == '\n') {
            *(*buf)++ = '\n';
            newline_flag = true;
            cr_flag = false;
        } else {
            *(*buf)++ = ch;
            // 空白字符不算作一行开头
            if (ch != ' ' && ch != '\t') newline_flag = false;
        }
    }

    *(*buf)++ = '\n';
    return 0;
}

int ScriptHandler::setFontConfig(const char *buf) {
    // printf("setFontConfig: %s\n", buf);
    ons_font::FONT_TYPE types = ons_font::GLOBAL_FONT;
    int offset = 0;
    if (buf[offset] >= '0' && buf[offset] <= '9') {
        types = ons_font::FONT_TYPE(buf[offset] - '0');
    }
    offset++;
    if (buf[offset] != ':') {
        return 0;
    }
    offset++;
    ons_font::FontConfig *cfg = nullptr;
    if (font_configs[types] == nullptr) {
        font_configs[types] = new ons_font::FontConfig;
    }
    cfg = font_configs[types];
    return ons_font::readFontConfig(buf, cfg);
}

void ScriptHandler::readConfiguration() {
    variable_range = 4096;
    global_variable_border = 200;

    char *buf = script_buffer;
    while (buf < script_buffer + script_buffer_length) {
        if (*buf == ';') break;
        if (IS_TWO_BYTE(*buf)) buf++;
        buf++;
    }

    while (++buf >= script_buffer + script_buffer_length) return;

    SKIP_SPACE(buf);
    bool config_flag = false;
    if (buf[0] == '$') {
        config_flag = true;
        buf++;
    }

    while (*buf && *buf != '\n') {
        SKIP_SPACE(buf);
        if (!strncmp(buf, "mode", 4)) {
            buf += 4;
            screen_width = 0;
            screen_height = 0;
            float height_ratio1 = 3.0f;
            float height_ratio2 = 4.0f;
            if (*buf == 'w') {
                buf++;
                float height_ratio1 = 9.0f;
                float height_ratio2 = 16.0f;
            }
            SKIP_SPACE(buf);
            while (*buf >= '0' && *buf <= '9')
                screen_width = screen_width * 10 + *buf++ - '0';
            SKIP_SPACE(buf);
            if (*buf == 'x' || *buf == '*') {
                buf += 1;
                SKIP_SPACE(buf);
                while (*buf >= '0' && *buf <= '9')
                    screen_height = screen_height * 10 + *buf++ - '0';
            }
            if (screen_height == 0) {
                switch (screen_width) {
                    case 800:
                        screen_height = 600;
                        break;
                    case 400:
                        screen_height = 300;
                        break;
                    case 320:
                        screen_height = 240;
                        break;
                    default:
                        screen_height =
                            (float)screen_width * height_ratio1 / height_ratio2;
                        break;
                }
            }
            if (*buf == '@') {
                buf++;
                int _screen_ratio = 0;
                while (*buf >= '0' && *buf <= '9')
                    _screen_ratio = _screen_ratio * 10 + *buf++ - '0';
                if (_screen_ratio > 0 && !init_screen_ratio) {
                    screen_scale->Update(_screen_ratio, 1);
                }
                if (*buf == 'x' || *buf == 'X') {
                    buf++;
                }
            }
        } else if (!strncmp(buf, "value", 5) || *buf == 'g' || *buf == 'G') {
            if (*buf == 'g' || *buf == 'G')
                buf++;
            else
                buf += 5;
            SKIP_SPACE(buf);
            global_variable_border = 0;
            while (*buf >= '0' && *buf <= '9')
                global_variable_border =
                    global_variable_border * 10 + *buf++ - '0';
        } else if (*buf == 'v' || *buf == 'V') {
            buf++;
            SKIP_SPACE(buf);
            variable_range = 0;
            while (*buf >= '0' && *buf <= '9')
                variable_range = variable_range * 10 + *buf++ - '0';
        } else if (*buf == 's' || *buf == 'S') {
            buf++;
            if (!(*buf >= '0' && *buf <= '9')) break;
            screen_width = 0;
            while (*buf >= '0' && *buf <= '9')
                screen_width = screen_width * 10 + *buf++ - '0';
            while (*buf == ',' || *buf == ' ' || *buf == '\t') buf++;
            screen_height = 0;
            while (*buf >= '0' && *buf <= '9')
                screen_height = screen_height * 10 + *buf++ - '0';
        } else if (*buf == 'l' || *buf == 'L') {
            buf++;
            SKIP_SPACE(buf);
            while (*buf >= '0' && *buf <= '9') buf++;
        } else if (*buf == 'f' || *buf == 'F') {
            buf++;
            buf += setFontConfig(buf);
        } else if (!strncmp(buf, "ratio", 5)) {
            int _user_ratio = 0;
            buf += 5;
            SKIP_SPACE(buf);
            while (*buf >= '0' && *buf <= '9')
                _user_ratio = _user_ratio * 10 + *buf++ - '0';
            if (_user_ratio > 0) {
                user_scale->Update(_user_ratio, 1);
            }
        } else if (*buf != ',') {
            break;
        }

        SKIP_SPACE(buf);
        if (!config_flag && *buf != ',') break;
        if (*buf == ',') buf++;
    }
    if (screen_scale->Has()) {
        screen_width = screen_scale->Scale(screen_width);
        screen_height = screen_scale->Scale(screen_height);
    }
    if (user_scale->Has()) {
        screen_width = user_scale->Scale(screen_width);
        screen_height = user_scale->Scale(screen_height);
    }
}

int ScriptHandler::labelScript() {
    int label_counter = -1;
    int current_line = 0;
    char *buf = script_buffer;
    label_info = new LabelInfo[num_of_labels + 1];

    while (buf < script_buffer + script_buffer_length) {
        SKIP_SPACE(buf);
        if (*buf == '*') {
            while (*(buf + 1) == '*') buf++;
            setCurrent(buf);
            readLabel();
            label_info[++label_counter].name =
                new char[strlen(string_buffer)]{0};
            strcpy(label_info[label_counter].name, string_buffer + 1);
            label_info[label_counter].label_header = buf;
            label_info[label_counter].num_of_lines = 1;
            label_info[label_counter].start_line = current_line;
            buf = getNext();
            if (*buf == '\n') {
                buf++;
                current_line++;
            }
            SKIP_SPACE(buf);
            label_info[label_counter].start_address = buf;
        } else {
            if (label_counter >= 0) label_info[label_counter].num_of_lines++;
            while (*buf != '\n') buf++;
            buf++;
            current_line++;
        }
    }

    label_info[num_of_labels].start_address = NULL;

    return 0;
}

int ScriptHandler::findLabel(const char *label) {
    int i;
    char capital_label[256];

    for (i = 0; i < (int)strlen(label) + 1; i++) {
        capital_label[i] = label[i];
        if ('A' <= capital_label[i] && capital_label[i] <= 'Z')
            capital_label[i] += 'a' - 'A';
    }
    for (i = 0; i < num_of_labels; i++) {
        if (!strcmp(label_info[i].name, capital_label)) return i;
    }
    int size = strlen(label) + 32;
    char *p = new char[size]{0};
    snprintf(p, size, "Label \"%s\" is not found.", label);
    errorAndExit(p);

    return -1;  // dummy
}

char *ScriptHandler::checkComma(char *buf) {
    SKIP_SPACE(buf);
    if (*buf == ',') {
        end_status |= END_COMMA;
        buf++;
        SKIP_SPACE(buf);
    }

    return buf;
}

void ScriptHandler::parseStr(char **buf) {
    SKIP_SPACE(*buf);

    if (**buf == '(') {
        (*buf)++;
        parseStr(buf);
        SKIP_SPACE(*buf);
        if ((*buf)[0] != ')') errorAndExit("parseStr: missing ')'.");
        (*buf)++;

        if (findAndAddLog(log_info[FILE_LOG], str_string_buffer, false)) {
            parseStr(buf);
            char *tmp_buf = new char[strlen(str_string_buffer) + 1]{0};
            strcpy(tmp_buf, str_string_buffer);
            parseStr(buf);
            strcpy(str_string_buffer, tmp_buf);
            delete[] tmp_buf;
        } else {
            parseStr(buf);
            parseStr(buf);
        }
        current_variable.type |= VAR_CONST;
    } else if (**buf == '$') {
        (*buf)++;
        int no;
        // 支持 int 表达式，例如在循环时的 $(%i + 10), $1 = 5 的情况下会被解释为
        // $15
        if (**buf == '(') {
            no = parseIntExpression(buf);
        } else {
            no = parseInt(buf);
        }
        VariableData &vd = getVariableData(no);

        if (vd.str)
            strcpy(str_string_buffer, vd.str);
        else
            str_string_buffer[0] = '\0';
        current_variable.type = VAR_STR;
        current_variable.var_no = no;
    } else if (**buf == '"') {
        int c = 0;
        (*buf)++;
        while (**buf != '"' && **buf != '\n')
            str_string_buffer[c++] = *(*buf)++;
        str_string_buffer[c] = '\0';
        if (**buf == '"') (*buf)++;
        current_variable.type |= VAR_CONST;
    }
#ifdef ENABLE_1BYTE_CHAR
    else if (**buf == '`') {
        int c = 0;
        str_string_buffer[c++] = *(*buf)++;
        while (**buf != '`' && **buf != '\n')
            str_string_buffer[c++] = *(*buf)++;
        str_string_buffer[c] = '\0';
        if (**buf == '`') (*buf)++;
        current_variable.type |= VAR_CONST;
        end_status |= END_1BYTE_CHAR;
    }
#endif
    else if (**buf == '#') {  // for color
        for (int i = 0; i < 7; i++) str_string_buffer[i] = *(*buf)++;
        str_string_buffer[7] = '\0';
        current_variable.type = VAR_NONE;
    } else if (**buf == '*') {  // label
        int c = 0;
        str_string_buffer[c++] = *(*buf)++;
        SKIP_SPACE(*buf);
        char ch = **buf;
        while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '_') {
            if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
            str_string_buffer[c++] = ch;
            ch = *++(*buf);
        }
        str_string_buffer[c] = '\0';
        current_variable.type |= VAR_CONST;
    } else {  // str alias
        char ch, alias_buf[512];
        int alias_buf_len = 0;
        bool first_flag = true;

        while (1) {
            if (alias_buf_len == 511) break;
            ch = **buf;

            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                ch == '_') {
                if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
                first_flag = false;
                alias_buf[alias_buf_len++] = ch;
            } else if (ch >= '0' && ch <= '9') {
                if (first_flag)
                    errorAndExit(
                        "parseStr: number is not allowed for the first letter "
                        "of str alias.");
                first_flag = false;
                alias_buf[alias_buf_len++] = ch;
            } else
                break;
            (*buf)++;
        }
        alias_buf[alias_buf_len] = '\0';

        if (alias_buf_len == 0) {
            str_string_buffer[0] = '\0';
            current_variable.type = VAR_NONE;
            return;
        }

        Alias *p_str_alias = root_str_alias.next;

        while (p_str_alias) {
            if (!strcmp(p_str_alias->alias, (const char *)alias_buf)) {
                strcpy(str_string_buffer, p_str_alias->str);
                break;
            }
            p_str_alias = p_str_alias->next;
        }
        if (!p_str_alias) {
            utils::printInfo("can't find str alias for %s...\n", alias_buf);
            exit(-1);
        }
        current_variable.type |= VAR_CONST;
    }
}

int ScriptHandler::parseInt(char **buf, bool ignore_exit) {
    int ret = 0;

    SKIP_SPACE(*buf);

    if (**buf == '%') {
        (*buf)++;
        int no;
        if (**buf == '(') {
            // 支持 int 表达式，例如在循环时的 %(%i + 10), $1 = 5
            // 的情况下会被解释为 %15
            no = parseIntExpression(buf);
        } else {
            no = parseInt(buf);
        }
        current_variable.var_no = no;
        current_variable.type = VAR_INT;
        return getVariableData(current_variable.var_no).num;
    } else if (**buf == '(') {
        return parseIntExpression(buf);
    } else if (**buf == '?') {
        ArrayVariable av;
        current_variable.var_no = parseArray(buf, av, ignore_exit);
        current_variable.type = VAR_ARRAY;
        current_variable.array = av;
        int *ret = getArrayPtr(
            current_variable.var_no, current_variable.array, 0, ignore_exit);
        if (!ret) {
            return 0;
        }
        return *ret;
    } else {
        char ch, alias_buf[256];
        int alias_buf_len = 0, alias_no = 0;
        bool direct_num_flag = false;
        bool num_alias_flag = false;

        char *buf_start = *buf;
        while (1) {
            ch = **buf;

            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                ch == '_') {
                if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
                if (direct_num_flag) break;
                num_alias_flag = true;
                alias_buf[alias_buf_len++] = ch;
            } else if (ch >= '0' && ch <= '9') {
                if (!num_alias_flag) direct_num_flag = true;
                if (direct_num_flag)
                    alias_no = alias_no * 10 + ch - '0';
                else
                    alias_buf[alias_buf_len++] = ch;
            } else
                break;
            (*buf)++;
        }

        if (*buf - buf_start == 0) {
            current_variable.type = VAR_NONE;
            return 0;
        }

        /* ---------------------------------------- */
        /* Solve num aliases */
        if (num_alias_flag) {
            alias_buf[alias_buf_len] = '\0';
            Alias *p_num_alias = root_num_alias.next;

            while (p_num_alias) {
                if (!strcmp(p_num_alias->alias, (const char *)alias_buf)) {
                    alias_no = p_num_alias->num;
                    break;
                }
                p_num_alias = p_num_alias->next;
            }
            if (!p_num_alias) {
                // utils::printInfo("can't find num alias for %s... assume
                // 0.\n", alias_buf );
                current_variable.type = VAR_NONE;
                *buf = buf_start;
                return 0;
            }
        }
        current_variable.type = VAR_INT | VAR_CONST;
        ret = alias_no;
    }

    SKIP_SPACE(*buf);

    return ret;
}

int ScriptHandler::parseIntExpression(char **buf) {
    int num[3], op[2];  // internal buffer

    SKIP_SPACE(*buf);

    readNextOp(buf, NULL, &num[0]);

    readNextOp(buf, &op[0], &num[1]);
    if (op[0] == OP_INVALID) return num[0];

    while (1) {
        readNextOp(buf, &op[1], &num[2]);
        if (op[1] == OP_INVALID) break;

        if (!(op[0] & 0x04) &&
            (op[1] & 0x04)) {  // if priority of op[1] is higher than op[0]
            num[1] = calcArithmetic(num[1], op[1], num[2]);
        } else {
            num[0] = calcArithmetic(num[0], op[0], num[1]);
            op[0] = op[1];
            num[1] = num[2];
        }
    }
    return calcArithmetic(num[0], op[0], num[1]);
}

/*
 * Internal buffer looks like this.
 *   num[0] op[0] num[1] op[1] num[2]
 * If priority of op[0] is higher than op[1], (num[0] op[0] num[1]) is computed,
 * otherwise (num[1] op[1] num[2]) is computed.
 * Then, the next op and num is read from the script.
 * Num is an immediate value, a variable or a bracketed expression.
 */
void ScriptHandler::readNextOp(char **buf, int *op, int *num) {
    bool minus_flag = false;
    SKIP_SPACE(*buf);
    char *buf_start = *buf;

    if (op) {
        if ((*buf)[0] == '+')
            *op = OP_PLUS;
        else if ((*buf)[0] == '-')
            *op = OP_MINUS;
        else if ((*buf)[0] == '*')
            *op = OP_MULT;
        else if ((*buf)[0] == '/')
            *op = OP_DIV;
        else if ((*buf)[0] == 'm' && (*buf)[1] == 'o' && (*buf)[2] == 'd' &&
                 ((*buf)[3] == ' ' || (*buf)[3] == '\t' || (*buf)[3] == '$' ||
                  (*buf)[3] == '%' || (*buf)[3] == '?' ||
                  ((*buf)[3] >= '0' && (*buf)[3] <= '9')))
            *op = OP_MOD;
        else {
            *op = OP_INVALID;
            return;
        }
        if (*op == OP_MOD)
            *buf += 3;
        else
            (*buf)++;
        SKIP_SPACE(*buf);
    }

    SKIP_SPACE(*buf);
    if ((*buf)[0] == '-') {
        minus_flag = true;
        (*buf)++;
        SKIP_SPACE(*buf);
    }

    if ((*buf)[0] == '(') {
        (*buf)++;
        *num = parseIntExpression(buf);
        if (minus_flag) *num = -*num;
        SKIP_SPACE(*buf);
        if ((*buf)[0] != ')') errorAndExit("Missing ')' in expression");
        (*buf)++;
    } else {
        *num = parseInt(buf);
        if (minus_flag) *num = -*num;
        if (current_variable.type == VAR_NONE) {
            if (op) *op = OP_INVALID;
            *buf = buf_start;
        }
    }
}

int ScriptHandler::calcArithmetic(int num1, int op, int num2) {
    int ret = 0;

    if (op == OP_PLUS)
        ret = num1 + num2;
    else if (op == OP_MINUS)
        ret = num1 - num2;
    else if (op == OP_MULT)
        ret = num1 * num2;
    else if (op == OP_DIV)
        ret = num1 / num2;
    else if (op == OP_MOD)
        ret = num1 % num2;

    current_variable.type = VAR_INT | VAR_CONST;

    return ret;
}

int ScriptHandler::parseArray(char **buf,
                              struct ArrayVariable &array,
                              bool ignore_exit) {
    SKIP_SPACE(*buf);

    (*buf)++;  // skip '?'
    int no = parseInt(buf);

    SKIP_SPACE(*buf);
    array.num_dim = 0;
    while (**buf == '[') {
        (*buf)++;
        array.dim[array.num_dim] = parseIntExpression(buf);
        array.num_dim++;
        SKIP_SPACE(*buf);
        if (**buf != ']') {
            if (!ignore_exit) {
                errorAndExit("parseArray: missing ']'.");
            } else {
                return no;
            }
        }
        (*buf)++;
    }
    for (int i = array.num_dim; i < 20; i++) array.dim[i] = 0;

    return no;
}

int *ScriptHandler::getArrayPtr(int no,
                                ArrayVariable &array,
                                int offset,
                                bool ignore_exit) {
    ArrayVariable *av = root_array_variable;
    while (av) {
        if (av->no == no) break;
        av = av->next;
    }
    if (av == NULL) {
        if (!ignore_exit) {
            errorAndExit("Array No. is not declared.");
        } else {
            return NULL;
        }
    }

    int dim = 0, i;
    for (i = 0; i < av->num_dim; i++) {
        if (av->dim[i] <= array.dim[i]) errorAndExit("dim[i] <= array.dim[i].");
        dim = dim * av->dim[i] + array.dim[i];
    }
    if (av->dim[i - 1] <= array.dim[i - 1] + offset)
        errorAndExit("dim[i-1] <= array.dim[i-1] + offset.");

    return &av->data[dim + offset];
}

void ScriptHandler::declareDim() {
    current_script = next_script;
    char *buf = current_script;

    if (current_array_variable) {
        current_array_variable->next = new ArrayVariable();
        current_array_variable = current_array_variable->next;
    } else {
        root_array_variable = new ArrayVariable();
        current_array_variable = root_array_variable;
    }

    ArrayVariable array;
    current_array_variable->no = parseArray(&buf, array);

    int dim = 1;
    current_array_variable->num_dim = array.num_dim;
    for (int i = 0; i < array.num_dim; i++) {
        current_array_variable->dim[i] = array.dim[i] + 1;
        dim *= (array.dim[i] + 1);
    }
    current_array_variable->data = new int[dim];
    memset(current_array_variable->data, 0, sizeof(int) * dim);

    next_script = buf;
}
