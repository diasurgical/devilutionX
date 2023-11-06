-- Gets the signature of native (non-C) Lua function
-- Based on https://stackoverflow.com/a/24216007/181228

local function getlocals(l)
  local i = 0
  local direction = 1
  return function()
    i = i + direction
    local k, v = debug.getlocal(l, i)
    if (direction == 1 and (k == nil or k.sub(k, 1, 1) == '(')) then
      i = -1
      direction = -1
      k, v = debug.getlocal(l, i)
    end
    return k, v
  end
end

local function dumpsig(f)
  assert(type(f) == 'function', "bad argument #1 to 'dumpsig' (function expected)")
  local p = {}
  pcall(function()
    local oldhook
    local hook = function(event, line)
      for k, v in getlocals(3) do
        if k == "(*vararg)" then
          table.insert(p, "...")
          break
        end
        table.insert(p, k)
      end
      debug.sethook(oldhook)
      error('aborting the call')
    end
    oldhook = debug.sethook(hook, "c")
    f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)
  end)
  return "(" .. table.concat(p, ", ") .. ")"
end

return dumpsig
