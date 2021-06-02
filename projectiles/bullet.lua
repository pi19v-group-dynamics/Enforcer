local bullet = require("projectiles/projectile"):extend()

bullet.speed = 300
bullet.size = 3
bullet.quad = nil 
bullet.lifetime = 1

function bullet:create(sender, x, y, dir)
	bullet.__super.create(self, sender, x, y, dir)
	if bullet.quad == nil then
		bullet.quad = love.graphics.newQuad(0, 16, 16, 16, spritesheet:getWidth(), spritesheet:getHeight())
	end
end

function bullet:on_collision(other)
	if other ~= self.sender then
		self.exist = false
	end
end

function bullet:update(dt)
	bullet.__super.update(self, dt)
	self.impulse = self.dir * self.speed
end

function bullet:draw(sb)
	sb:add(bullet.quad, self.pos.x, self.pos.y, 0, 1, 1, 8, 8)
end

return bullet
