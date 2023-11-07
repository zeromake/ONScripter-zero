import("duplicate")
import("core.base.bytes")


local function to_hash(name)
    return string.upper(string.sub(hash.md5(bytes(name)), 1, 24))
end

local function render_pbxproj(context, input_path, output_path)
    local output = io.open(output_path, 'wb')
    for line in io.lines(input_path) do
        local space, variable = line:match('(\t*)%$%{([%w_]+)%}')
        if variable then
            local variables = context[variable]
            variables = type(variables) == "table" and variables or {variables}
            local buffer = ""
            local size = #variables
            for i, item in ipairs(variables) do
                buffer = buffer..space..item..'\n'
            end
            line = line:gsub('(\t*)%$%{([%w_]+)%}', buffer)
            output:write(line)
        else
            output:write(line..'\n')
        end
    end
    output:close()
end

function main(target, version)
    local links = duplicate.new()
    local link_dirs = duplicate.new()
    local include_dirs = duplicate.new()
    local path_sep = os.host() == 'windows' and '\\' or '/'
    local frameworks = duplicate.new()
    for _, framework in ipairs(target:get("frameworks")) do
        frameworks:add(framework)
    end
    for _, link in ipairs(target:get("syslinks")) do
        links:add(link)
    end
    for _, link in ipairs(target:get("links")) do
        links:add(link)
    end
    for name, pkg in pairs(target:pkgs()) do
        local priority = 0
        local pkg_links = pkg:get("links")
        if pkg_links ~= nil then
            pkg_links = type(pkg_links) == "table" and pkg_links or {pkg_links}
            priority = #pkg_links
            for _, link in ipairs(pkg_links) do
                links:add(link)
            end
        end
        local pkg_link_dirs = pkg:get("linkdirs")
        if pkg_link_dirs ~= nil then
            pkg_link_dirs = type(pkg_link_dirs) == "table" and pkg_link_dirs or {pkg_link_dirs}
            for _, link_dir in ipairs(pkg_link_dirs) do
                local k = link_dir:split(path_sep, {plain = true})[6]
                link_dirs:add(link_dir, k, priority)
            end
        end
        local pkg_includes = pkg:get("sysincludedirs")
        if pkg_includes ~= nil then
            pkg_includes = type(pkg_includes) == "table" and pkg_includes or {pkg_includes}
            for _, include in ipairs(pkg_includes) do
                local k = include:split(path_sep, {plain = true})[6]
                include_dirs:add(include, k, priority)
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

    table.sort(link_dirs.value)
    table.sort(include_dirs.value)
    links:add("onscripter")
    link_dirs:add(path.absolute(target:targetdir()))

    local app_name = "onscripter.app"
    local ref_hash = {}
    local source_files = {
        "ViewController.h",
        "ViewController.m",
        "Assets.xcassets",
        "Info.plist",
        "main.mm",
        "Main.storyboard",
    }
    -- local
    local app_hash = to_hash(app_name)
    ref_hash[app_name] = app_hash

    local SOURCE_REFS = {}
    table.insert(
        SOURCE_REFS,
        string.format(
            '%s /* %s */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = %s; sourceTree = BUILT_PRODUCTS_DIR; };',
            app_hash,
            app_name,
            app_name
        )
    )
    for _, f in ipairs(source_files) do
        local lastKnownFileType = "sourcecode.c.h"
        if f:endswith(".m") then
            lastKnownFileType = "sourcecode.c.objc"
        elseif f:endswith(".mm") then
            lastKnownFileType = "sourcecode.c.objcpp"
        elseif f:endswith(".plist") then
            lastKnownFileType = "text.plist.xml"
        elseif f:endswith(".xcassets") then
            lastKnownFileType = "folder.assetcatalog"
        elseif f:endswith(".storyboard") then
            lastKnownFileType = "file.storyboard"
        end
        local current_hash = to_hash(bytes(f))
        ref_hash[f] = current_hash
        table.insert(
            SOURCE_REFS,
            string.format(
                '%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = %s; path = %s; sourceTree = "<group>"; };',
                current_hash,
                f,
                lastKnownFileType,
                f
            )
        )
    end

    for _, f in ipairs(frameworks.value) do
        local k = f..".framework"
        local current_hash = to_hash(bytes(k))
        ref_hash[k] = current_hash

        table.insert(
            SOURCE_REFS,
            (string.format(
                '%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %s; path = System/Library/Frameworks/%s; sourceTree = SDKROOT; };',
                current_hash,
                k,
                k,
                k
            ))
        )
    end
    table.sort(SOURCE_REFS)

    local BUILD_SECTION_FILES = {}
    local build_section_template = '%s /* %s */ = {isa = PBXBuildFile; fileRef = %s /* %s */; };'
    local build_source_files = {}
    for _, name in ipairs(source_files) do
        if name:endswith('.m') or name:endswith('.mm') or name:endswith('.storyboard') then
            table.insert(
                build_source_files,
                name
            )
        end
    end

    for _, name in ipairs(build_source_files) do
        local k = name..".build_section"
        local current_hash = to_hash(k)
        table.insert(BUILD_SECTION_FILES, string.format(
            build_section_template,
            current_hash,
            name.." in Sources",
            ref_hash[name],
            name
        ))
        ref_hash[k] = current_hash
    end

    for _, name in ipairs(frameworks.value) do
        name = name..".framework"
        local k = name..".build_section"
        local current_hash = to_hash(k)
        table.insert(BUILD_SECTION_FILES, string.format(
            build_section_template,
            current_hash,
            name.." in Frameworks",
            ref_hash[name],
            name
        ))
        ref_hash[k] = current_hash
    end
    table.sort(BUILD_SECTION_FILES)

    local FRAMEWORKS_BUILD_PHASE = {}
    local framework_build_phase_template = '%s /* %s in Frameworks */,'
    for _, name in ipairs(frameworks.value) do
        name = name..".framework"
        local k = name..".build_section"
        table.insert(FRAMEWORKS_BUILD_PHASE, string.format(
            framework_build_phase_template,
            ref_hash[k],
            name
        ))
    end

    local PRODUCTS_GROUP = {}

    table.insert(
        PRODUCTS_GROUP,
        string.format(
            '%s /* %s */,',
            ref_hash[app_name],
            app_name
        )
    )

    local SOURCE_FILES_GROUP = {}

    for _, name in ipairs(source_files) do
        table.insert(
            SOURCE_FILES_GROUP,
            string.format(
                '%s /* %s */,',
                ref_hash[name],
                name
            )
        )
    end

    local FRAMEWORKS_GROUP = {}
    for _, name in ipairs(frameworks.value) do
        name = name..".framework"
        table.insert(
            FRAMEWORKS_GROUP,
            string.format(
                '%s /* %s */,',
                ref_hash[name],
                name
            )
        )
    end

    local PRODUCT_REFERENCE = {
        string.format(
            'productReference = %s /* %s */;',
            ref_hash[app_name],
            app_name
        )
    }

    local SOURCES_BUILD_PHASE = {}
    local sources_build_phase_template = '%s /* %s in Sources */,'
    for _, name in ipairs(build_source_files) do
        table.insert(
            SOURCES_BUILD_PHASE,
            string.format(
                sources_build_phase_template,
                ref_hash[name..".build_section"],
                name
            )
        )
    end

    local VERSION = string.format('"%s";', version)
    local HEADER_PATHS = {}
    for _, include_dir in ipairs(include_dirs.value) do
        table.insert(HEADER_PATHS, string.format('%s,', include_dir))
    end

    local LIBRARY_PATHS = {}
    for _, link_dir in ipairs(link_dirs.value) do
        table.insert(LIBRARY_PATHS, string.format('%s,', link_dir))
    end
    local LDFLAGS = {}
    for _, link in ipairs(links.value) do
        table.insert(LDFLAGS, string.format('"-l%s",', link))
    end

    local IDENTIFIER = '"com.zeromake.onscripter-zero";'
    local context = {
        SOURCE_REFS = SOURCE_REFS,
        BUILD_SECTION_FILES = BUILD_SECTION_FILES,
        FRAMEWORKS_BUILD_PHASE = FRAMEWORKS_BUILD_PHASE,
        PRODUCTS_GROUP = PRODUCTS_GROUP,
        SOURCE_FILES_GROUP = SOURCE_FILES_GROUP,
        FRAMEWORKS_GROUP = FRAMEWORKS_GROUP,
        PRODUCT_REFERENCE = PRODUCT_REFERENCE,
        SOURCES_BUILD_PHASE = SOURCES_BUILD_PHASE,
        HEADER_PATHS = HEADER_PATHS,
        LIBRARY_PATHS = LIBRARY_PATHS,
        LDFLAGS = LDFLAGS,
        VERSION = VERSION,
        IDENTIFIER = IDENTIFIER,
    }
    render_pbxproj(
        context,
        path.join(os.scriptdir(), '../project/ios/onscripter-oc/project.pbxproj.in'),
        path.join(os.scriptdir(), '../project/ios/onscripter-oc/onscripter.xcodeproj/project.pbxproj')
    )
end
