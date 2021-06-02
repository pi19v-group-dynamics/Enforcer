local shooter = require("entities/alien"):extend()
local bullet = require("projectiles/bullet")
local vec2  = require("vec2")
local rect  = require("rect")

shooter.reward = 250
shooter.shoot_timer_max = 1
shooter.zone = zone or rect:new(0, 0, WIDTH, HEIGHT / 2)
shooter.change_time = 0.2

function shooter:create(x, y, zone)
	shooter.__super.create(self, x, y, zone)
	self.shoot_timer = 0
	shooter.quad = love.graphics.newQuad(144, 32, 16, 16, spritesheet:getWidth(), spritesheet:getHeight())
end

function shooter:update(dt)
	shooter.__super.update(self, dt)
	if self.shoot_timer <= 0 then
		scene.world:add("entity", bullet:new(self, self.pos.x, self.pos.y, (player0.pos - self.pos):norm()))
		self.shoot_timer = self.shoot_timer_max
	else
		self.shoot_timer = self.shoot_timer - dt
	end
end

return shooter

