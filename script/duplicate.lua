local _instance = {}

function _instance.new()
    local instance = table.inherit(_instance)
    instance.duplicate = {}
    instance.value = {}
    return instance
end

function _instance:add(item, key, count)
    if key == nil then
        key = item
    end
    if count == nil then
        count = 0
    end
    if self.duplicate[key] == nil or self.duplicate[key][1] < count then
        if self.duplicate[key] == nil then
            self.duplicate[key] = {count, (#self.value)+1}
            table.insert(self.value, item)
        else
            self.duplicate[key][1] = count
            self.value[self.duplicate[key][2]] = item
        end
    end
end

function new()
    return _instance.new()
end