local projectile = require("entity"):extend()
projectile.lifetime = 0

function projectile:create(sender, x, y, dir)
	projectile.__super.create(self, x, y)
	self.sender = sender
	self.dir = dir or vec2:new(1, 0)
	self.duration = self.lifetime
end

function projectile:update(dt)
	projectile.__super.update(self, dt)
	if self.duration <= 0 then self.exist = false end
	self.duration = self.duration - dt
end

return projectile
