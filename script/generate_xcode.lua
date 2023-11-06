import("duplicate")
import("core.base.bytes")


local function to_hash(name)
    return string.upper(string.sub(hash.md5(bytes(name)), 1, 24))
end

function main(target)
    local links = duplicate.new()
    local link_dirs = duplicate.new()
    local include_dirs = duplicate.new()
    local path_sep = os.host() == 'windows' and '\\' or '/'
    local frameworks = duplicate.new()
    for _, framework in ipairs(target:get("frameworks")) do
        frameworks:add(framework)
    end
    for _, includedir in ipairs(target:get("includedirs")) do
        include_dirs:add(includedir)
    end
    for _, link in ipairs(target:get("syslinks")) do
        links:add(link)
    end
    for _, link in ipairs(target:get("links")) do
        links:add(link)
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
            app_name,
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
                '%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = %s; path = %s; sourceTree = "<group>"; };\n',
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
                '%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = %s; path = System/Library/Frameworks/%s; sourceTree = SDKROOT; };\n',
                current_hash,
                k,
                k,
                k
            ))
        )
    end

    local BUILD_SECTION_FILES = {}
    local build_section_template = '%s /* %s */ = {isa = PBXBuildFile; fileRef = %s; };\n'

    for _, name in ipairs(source_files) do
        if name:endswith('.m') or name:endswith('.mm') or name:endswith('.storyboard') then
            local k = name..".build_section"
            local current_hash = to_hash(k)
            table.insert(BUILD_SECTION_FILES, string.format(
                build_section_template,
                current_hash,
                name.." in Sources",
                ref_hash[name]
            ))
            ref_hash[k] = current_hash
        end
    end

    for _, name in ipairs(frameworks.value) do
        name = name..".framework"
        local k = name..".build_section"
        local current_hash = to_hash(k)
        table.insert(BUILD_SECTION_FILES, string.format(
            build_section_template,
            current_hash,
            name.." in Frameworks",
            ref_hash[name]
        ))
        ref_hash[k] = current_hash
    end

    local FRAMEWORKS_BUILD_PHASE = {}
    local framework_build_phase_template = '%s /* %s in Frameworks */,\n'
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

    print("SOURCE_REFS:\n", SOURCE_REFS)
    print("BUILD_SECTION_FILES:\n", BUILD_SECTION_FILES)
    print("FRAMEWORKS_BUILD_PHASE:\n", FRAMEWORKS_BUILD_PHASE)
    print("PRODUCTS_GROUP:\n", PRODUCTS_GROUP)
    print("SOURCE_FILES_GROUP:\n", SOURCE_FILES_GROUP)
    print("FRAMEWORKS_GROUP:\n", FRAMEWORKS_GROUP)
    print(ref_hash)
end