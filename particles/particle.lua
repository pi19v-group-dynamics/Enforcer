local particle = require("entity"):extend()

function particle:create(x, y, size, duration)
	particle.__super.create(self, x, y, size)
	self.duration = duration
end

function particle:update(dt)
	particle.__super.update(self, dt)
	self.duration = self.duration - dt
	if self.duration <= 0 then
		self.exist = false
	end
end

return particle
