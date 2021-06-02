local win = require("scene"):extend()
local timer = require("timer")

function win:create(msg)
	win.__super.create(self)
	self.msg = msg or ""
	self.timer = timer:new(2, function() scene = require("scenes/main_menu"):new() end)
	love.audio.stop()
	victory_source:play()
end

local youwin = "You win!"
local sin = math.sin

function win:update(dt)
	self.timer:update(dt)
end

function win:draw()
	love.graphics.print(youwin, (WIDTH - 8* font_width) / 2, font_height / 2)
	for i = 1, #self.msg do
		love.graphics.print(self.msg:sub(i, i), (WIDTH - #self.msg * font_width) / 2 + i * font_width, HEIGHT / 2 + sin(love.timer.getTime() * 8 + i) * 4)
	end
end

return win
