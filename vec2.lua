local vec2  = require("class"):extend()
local sqrt  = math.sqrt
local sin   = math.sin
local cos   = math.cos
local atan2 = math.atan2

function vec2:create(x, y)
	if type(x) == "table" and x:is(vec2) then
		return vec2:new(x.x, x.y)
	elseif x ~= nil and y == nil then
		self.x = cos(x)
		self.y = sin(x)
	else
		self.x = x or 0 
		self.y = y or 0
	end
end

function vec2:unpack()
	return self.x, self.y
end

function vec2:__add(v)
	if type(v) == "number" then
		return vec2:new(self.x + v, self.y + v)
	end
	return vec2:new(self.x + v.x, self.y + v.y)
end

function vec2:__sub(v)
	if type(v) == "number" then
		return vec2:new(self.x - v, self.y - v)
	end
	return vec2:new(self.x - v.x, self.y - v.y)
end

function vec2:__mul(v)
	if type(v) == "number" then
		return vec2:new(self.x * v, self.y * v)
	end
	return vec2:new(self.x * v.x, self.y * v.y)
end

function vec2:__unm()
	return vec2:new(-self.x, -self.y)
end

function vec2:__div(v)
	if type(v) == "number" then
		v = 1 / v
		return vec2:new(self.x * v, self.y * v)
	end
	return vec2:new(self.x / v.x, self.y / v.y)
end

function vec2:len()
	return sqrt(self:dot(self))
end

function vec2:dot(v)
	return self.x * v.x + self.y * v.y
end

function vec2:norm()
	return self / self:len()
end

function vec2:lerp(v, t)
	return vec2:new(self.x + (v.x - self.x) * t, self.y + (v.y - self.y) * t)
end

function vec2:angle()
	return atan2(-self.y, self.x)
end

return vec2

