local main_menu = require("scene"):extend()
local play = require("scenes/play")

local menus = -- menu items
{
	{ -- start selection
		label = "Start",
		fn = function()
			scene = play:new()
		end
	},
	{ -- options selection
		label = "Settings",
		fn = function(menus)
			menus[2].label = "Not implemented"
		end
	},
	{ -- exit selection
		label = "Exit",
		fn = function()
			love.event.quit()
		end
	}
}

function main_menu:create()
	main_menu.__super.create(self)
	self.t = -100
	self.current = 1
	self.pressed = false
	love.audio.stop()	
	mainmenu_source:play()
end

function main_menu:update(dt)
	self.t = self.t - self.t * 8 * dt
	if love.keyboard.isDown("up") then
		if not self.pressed then
			self.current = math.max(1, self.current - 1)
			self.pressed = true
		end
	elseif love.keyboard.isDown("down") then
		if not self.pressed then
			self.current = math.min(self.current + 1, #menus)
			self.pressed = true
		end
	else
		self.pressed = false
	end
	if love.keyboard.isDown("space") then
		menus[self.current].fn(menus)
	end
end

function main_menu:draw()
	love.graphics.print(VERSION, WIDTH - #VERSION * font_width, HEIGHT - font_height)
	love.graphics.print("Enforcer", (WIDTH - 8 * font_width) / 2, 10)
	love.graphics.translate(10, HEIGHT - #menus * font_height * 2)
	for i = 1, #menus do
		local m = menus[i]
		if self.current == i then
			love.graphics.setColor(0, 0.5, 1)
			love.graphics.rectangle("fill", -10, i * font_height, WIDTH, font_height)
		end
		love.graphics.setColor(1, 1, 1)
		if self.current == i then
			for j = 1, #m.label do
				love.graphics.print(m.label:sub(j, j), self.t * i + (j - 1) * font_width, i * font_height + math.sin(love.timer.getTime() * 4 + j) * 4)
			end	
		else
			love.graphics.print(m.label, self.t * i, i * font_height)	
		end
	end
	love.graphics.origin()
end

return main_menu
