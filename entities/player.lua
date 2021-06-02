local player = require("entity"):extend()
local fire   = require("particles/fire")
local bullet = require("projectiles/bullet")
local enemy = require("entities/enemy")
local projectile = require("projectiles/projectile")
local vec2   = require("vec2")
local timer  = require("timer")
local min    = math.min
local max    = math.max
local abs    = math.abs
local floor  = math.floor
local halfpi = math.pi * 0.5
local lose = require("scenes/lose")

player.quad = nil

function player:create(x, y)
	player.__super.create(self, x, y, 8)
	if player.quad == nil then
		player.quad = love.graphics.newQuad(0, 0, 16, 16, spritesheet:getWidth(), spritesheet:getHeight())
	end
	self.xspeed = 100
	self.yspeed = 70
	self.rotation = 0
	self.reload = timer:new(0.5)
	self.shield = timer:new(2)
	self.intro = timer:new(2)
	self.destroy = nil 
	self.health = 3
	self.health_max = 3
end

local pi32 = 3 * math.pi / 2 

function player:on_collision(other)
	if other:is(enemy) or (other:is(projectile) and other.sender:is(enemy)) then
		self.health = self.health - 1
		if self.health <= 0 then
			self.exist = false
			scene = lose:new()
		end
		self.shield:set(1)
	end
end

function player:update(dt)
	player.__super.update(self, dt)	
	-- intro wait
	if not self.intro:out() then
		self.intro:update(dt)
		self.impulse = self.impulse + vec2:new(0, -self.yspeed)
		local dir = vec2:new(self.rotation + pi32)
		scene.world:add("particle", fire:new(self.pos.x - dir.x * self.size, self.pos.y - dir.y * self.size, 7, -dir))
		scene.world:add("particle", fire:new(self.pos.x - dir.x * self.size, self.pos.y - dir.y * self.size, 7, -dir))
		scene.world:add("particle", fire:new(self.pos.x - dir.x * self.size, self.pos.y - dir.y * self.size, 7, -dir))
		return
	end
	-- gas / brake
	if love.keyboard.isDown("up") then
		self.impulse.y = self.impulse.y - self.yspeed
	elseif love.keyboard.isDown("down") then
		self.impulse.y = self.impulse.y + self.yspeed
	end
	-- handling
	local rotation = 0.4 * dt
	if love.keyboard.isDown("left") then
		if self.rotation > 0 then
			self.rotation = self.rotation - rotation
		else
			self.rotation = max(-halfpi, self.rotation - rotation) 
		end
		self.impulse.x = self.impulse.x - self.xspeed
	elseif love.keyboard.isDown("right") then
		if self.rotation < 0 then
			self.rotation = self.rotation + rotation
		else
			self.rotation = min(halfpi, self.rotation + rotation) 
		end
		self.impulse.x = self.impulse.x + self.xspeed
	elseif self.rotation < 0 then
		self.rotation = self.rotation + rotation
	elseif self.rotation > 0 then
		self.rotation = self.rotation - rotation
	end

	if abs(self.rotation) < 0.001 then
		self.rotation = 0
	end

	-- update timers
	self.reload:update(dt)
	self.shield:update(dt)

	-- spawn particles
	local dir = vec2:new(self.rotation + pi32)
	scene.world:add("particle", fire:new(self.pos.x - dir.x * self.size, self.pos.y - dir.y * self.size, 4, -dir))
	-- handle shooting
	if self.reload:out() then
		if love.keyboard.isDown("space") then
			local e = bullet:new(self, self.pos.x, self.pos.y, dir)
			e.pos = e.pos + dir * (self.size + e.size)
			scene.world:add("entity", e)
			self.reload:set()
		end
	end
end

function player:draw(sb)
	if self.shield:out() or floor(love.timer.getTime() * 10) % 3 == 0 then
		sb:add(self.quad, self.pos.x, self.pos.y, self.rotation, 1, 1, 8, 8)
	end
	if not self.shield:out() then
		love.graphics.setColor(0.1, 0.1, 1)
		love.graphics.circle("line", self.pos.x, self.pos.y, self.size + 2)
		love.graphics.setColor(1, 1, 1)
	end
end

return player
