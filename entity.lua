local entity = require("class"):extend()
local vec2   = require("vec2")

function entity:create(x, y, size)
	self.pos     = vec2:new(x, y)
	self.impulse = vec2:new()
	self.size    = size
	self.exist   = true
end

function entity:update(dt)
	self.pos       = self.pos + self.impulse * dt
	self.impulse.x = 0
	self.impulse.y = 0
end

function entity:on_collision(other) end

function entity:collision(e)
	local dir = e.pos - self.pos
	local len = dir:len()
	if len < self.size + e.size then
		local temp = len == 0 and vec2:new() or dir / len
		len = len * 0.5
		self.impulse = self.impulse + temp * len	
		e.impulse = e.impulse - temp * len	
		self:on_collision(e)
		e:on_collision(self)
	end
end

function entity:draw()
	-- draw debug render
	love.graphics.setColor(1, 0, 1)
	love.graphics.circle("line", self.pos.x, self.pos.y, self.size)
end

return entity
