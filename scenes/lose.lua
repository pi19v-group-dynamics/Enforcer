local lose = require("scene"):extend()
local timer = require("timer")

function lose:create()
	lose.__super.create(self)
	self.msg = tostring(total_score)
	self.timer = timer:new(5, function() scene = require("scenes/main_menu"):new() end)
	love.audio.stop()
	victory_source:play()
end

local youlose = "You lose"
local sin = math.sin

function lose:update(dt)
	self.timer:update(dt)
end

function lose:draw()
	love.graphics.print(youlose, (WIDTH - 8 * font_width) / 2, HEIGHT / 2)
end

return lose

