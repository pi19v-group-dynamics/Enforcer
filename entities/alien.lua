local alien = require("entities/enemy"):extend()
local vec2  = require("vec2")
local rect  = require("rect")
local rand  = math.random
local timer = require("timer")
local twopi = math.pi * 2
local projectile = require("projectiles/projectile")
local fire = require("particles/fire")

alien.reward = 100
alien.quad = nil
alien.zone = zone or rect:new(WIDTH / 2 - WIDTH / 4, HEIGHT / 2 - HEIGHT / 4, WIDTH / 2, HEIGHT / 2)
alien.change_time = 0.3

function alien:create(x, y, zone)
	alien.__super.create(self, x, y, 8)
	self.dir = vec2:new(1, 0)
	self.speed = 100
	self.change_dir = timer:new(self.change_time, function(self)
		if not self.zone:overlap(self.pos.x, self.pos.y) then
			self.dir = (vec2:new(self.zone.x + self.zone.w / 2, self.zone.y + self.zone.h / 2) - self.pos):norm()
		else
			self.dir = self.dir:lerp(vec2:new(self.dir:angle()), 0.1)
		end
		self.change_dir:set()
	end, self)
	if alien.quad == nil then
		alien.quad = love.graphics.newQuad(128, 32, 16, 16, spritesheet:getWidth(), spritesheet:getHeight())
	end
end

function alien:on_collision(other)
	if other:is(projectile) and other.sender == player0 then
		scene.world:add("particle", fire:new(self.pos.x, self.pos.y, 6))
		scene.world:add("particle", fire:new(self.pos.x, self.pos.y, 7))
		scene.world:add("particle", fire:new(self.pos.x, self.pos.y, 6))
		scene.world:add("particle", fire:new(self.pos.x, self.pos.y, 6))
		scene.world:add("particle", fire:new(self.pos.x, self.pos.y, 7))
		scene.world:add("particle", fire:new(self.pos.x, self.pos.y, 6))
		alien.__super.on_collision(self, other)
	end
end

local t = 0.01

function alien:update(dt)
	alien.__super.update(self, dt)
	self.change_dir:update(dt)
	self.impulse = self.impulse + self.dir * self.speed 
end

function alien:draw(sb)
	sb:add(self.quad, self.pos.x, self.pos.y, 0, 1, 1, 8, 8)
end

return alien
