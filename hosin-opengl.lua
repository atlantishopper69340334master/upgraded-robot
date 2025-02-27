WebSocket = WebSocket or {}
WebSocket.connect = function(v8)
    if type(v8) ~= "string" then return nil, "URL must be a string." end
    if not (v8:match("^ws://") or v8:match("^wss://")) then return nil, "Invalid WebSocket URL. Must start with 'ws://' or 'wss://'." end
    local v9 = v8:gsub("^ws://", ""):gsub("^wss://", "")
    if v9 == "" or v9:match("^%s*$") then return nil, "Invalid WebSocket URL. No host specified." end
    return {
        Send = function(v51) end,
        Close = function() end,
        OnMessage = {},
        OnClose = {}
    }
end

local metaStore = {}
local function enhancedSetMetaTable(target, meta)
    local newMeta = setmetatable(target, meta)
    metaStore[newMeta] = meta
    return newMeta
end

setmetatable = enhancedSetMetaTable

function getrawmetatable(v14)
    return metaStore[v14]
end

function setrawmetatable(v15, v16)
    local v17 = getrawmetatable(v15)
    for k, v in pairs(v16) do v17[k] = v end
    return v15
end

local hiddenProps = {}

function sethiddenproperty(obj, prop, value)
    assert(obj and type(prop) == "string", "Invalid property set operation.")
    hiddenProps[obj] = hiddenProps[obj] or {}
    hiddenProps[obj][prop] = value
    return true
end

function gethiddenproperty(obj, prop)
    assert(obj and type(prop) == "string", "Invalid property get operation.")
    return (hiddenProps[obj] and hiddenProps[obj][prop]) or ((prop == "size_xml") and 5), true
end

function hookmetamethod(obj, method, newFunc)
    assert(type(obj) == "table" or type(obj) == "userdata", "Invalid first argument")
    assert(type(method) == "string", "Invalid second argument")
    assert(type(newFunc) == "function", "Invalid third argument")
    local meta = debug.getmetatable(obj) or {}
    local oldFunc = meta[method]
    meta[method] = newFunc
    return oldFunc
end

debug.getproto = function(fn, num, full)
    local f = function() return true end
    return full and {f} or f
end

debug.getconstant = function(fn, index)
    local constants = { [1] = "print", [2] = nil, [3] = "Hello, world!" }
    return constants[index]
end

debug.getupvalues = function(fn)
    local val
    setfenv(fn, { print = function(v) val = v end })
    fn()
    return { val }
end

debug.getupvalue = function(fn, index)
    local val
    setfenv(fn, { print = function(v) val = v end })
    fn()
    return val
end

table.freeze = function(tbl) end

function setreadonly() end

function isreadonly(tbl)
    assert(type(tbl) == "table", "Expected table, got " .. type(tbl))
    return true
end

function getcallbackvalue(obj, key)
    return obj[key]
end

local InstanceBackup = Instance
Instance = table.clone(Instance)

Instance.new = function(className, parent)
    if className == "BindableFunction" then
        local bindFunc = InstanceBackup.new("BindableFunction", parent)
        return setmetatable({}, {
            __index = function(_, key)
                if key == "OnInvoke" then return hiddenProps[bindFunc] end
                return bindFunc[key]
            end,
            __newindex = function(_, key, value)
                if key == "OnInvoke" then
                    hiddenProps[bindFunc] = value
                    bindFunc.OnInvoke = value
                else
                    bindFunc[key] = value
                end
            end
        })
    else
        return InstanceBackup.new(className, parent)
    end
end

local userAgent = "MoonAPI"
local oldRequest = request
getgenv().request = function(options)
    options.Headers = options.Headers or {}
    options.Headers["User-Agent"] = userAgent
    return oldRequest(options)
end

function printidentity()
    print("Current identity is 7")
end
