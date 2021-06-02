local fire = require("particles/particle"):extend()
local vec2 = require("vec2")
local rand = math.random

function fire:create(x, y, size, dir)
	fire.__super.create(self, x, y, size, size)
	self.dir = dir or vec2:new(rand(0, 7))
	self.orig_size = self.size
	self.speed = 100
	self.reduce = rand(6, 10)
end

function fire:update(dt)
	fire.__super.update(self, dt)
	self.impulse = self.dir * self.speed
	self.size = self.size - self.size * self.reduce * dt
end

function fire:draw()
	love.graphics.setColor(1, self.size / self.orig_size, 0)
	love.graphics.circle("fill", self.pos.x, self.pos.y, self.size)
	love.graphics.setColor(1, 1, 1)
end

return fire
