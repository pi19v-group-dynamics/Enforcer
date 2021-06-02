function love.load()
	-- load audio
	battle_source   = love.audio.newSource("assets/battle.mp3", "stream")
	gameover_source = love.audio.newSource("assets/gameover.mp3", "stream")
	mainmenu_source = love.audio.newSource("assets/mainmenu.mp3", "stream")
	victory_source  = love.audio.newSource("assets/victory.mp3", "stream")
	battle_source:setLooping(true)
	gameover_source:setLooping(true)
	mainmenu_source:setLooping(true)
	victory_source:setLooping(true)
	-- load font 
	font        = love.graphics.newImageFont("assets/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
	font_width  = 9
	font_height = 17
	love.graphics.setFont(font)
	-- load spritesheet
	spritesheet = love.graphics.newImage("assets/spritesheet.png")
	-- init scene
	scene = require("scenes/main_menu"):new()
end

function love.update(dt)
	love.window.setTitle("Enforcer (FPS: " .. love.timer.getFPS() .. ")")
	scene:update(dt)
end

love.graphics.setDefaultFilter("nearest", "nearest")
local canvas = love.graphics.newCanvas(WIDTH, HEIGHT)

function love.draw()
	love.graphics.setCanvas(canvas)
	love.graphics.clear()
	scene:draw()
	love.graphics.setCanvas()
	love.graphics.draw(canvas, 0, 0, 0, love.graphics.getWidth() / WIDTH, love.graphics.getHeight() / HEIGHT)
end
