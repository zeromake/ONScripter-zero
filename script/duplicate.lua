local _instance = {}

function _instance.new()
    local instance = table.inherit(_instance)
    instance.duplicate = {}
    instance.value = {}
    return instance
end

function _instance:add(item, key)
    if key == nil then
        key = item
    end
    if self.duplicate[key] == nil then
        self.duplicate[key] = true
        table.insert(self.value, item)
    end
end

function new()
    return _instance.new()
end