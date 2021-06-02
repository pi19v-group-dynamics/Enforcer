local class = {}
class.__index = class

function class:create() end
function class:destroy() end

function class:extend()
	local c = {}
	for k, v in ipairs(self) do
		if k:find("__") == 1 then
			c[k] = v
		end
	end
	c.__index = c
	c.__super = self
	setmetatable(c, self)
	return c
end

function class:implement(...)
	for k, v in ipairs{...} do
		if self[k] == nil and type(v) == "function" then
			self[k] = v
		end
	end
end

function class:is(t)
	local mt = getmetatable(self)
	while mt do
		if mt == t then return true end
		mt = getmetatable(mt)
	end
	return false
end

function class:__gc()
	self:destroy()
end

function class:new(...)
	local o = setmetatable({}, self)
	o:create(...)
	return o
end

return class
