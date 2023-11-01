import("duplicate")

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
end