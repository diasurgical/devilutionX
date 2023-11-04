function Events:RegisterEvent(eventName)
    self[eventName] = {
        Functions = {},
        Add = function(func)
            table.insert(self[eventName].Functions, func)
        end,
        Remove = function(func)
            for i, f in ipairs(self[eventName].Functions) do
                if f == func then
                    table.remove(self[eventName].Functions, i)
                    break
                end
            end
        end,
        Trigger = function()
            for _, func in ipairs(self[eventName].Functions) do
                func()
            end
        end,
    }
end

Events:RegisterEvent("OnGameBoot")
Events:RegisterEvent("OnGameStart")
Events:RegisterEvent("OnGameDrawComplete")
