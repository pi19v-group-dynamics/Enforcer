local play  = require("scene"):extend()
local world = require("world")
local alien = require("entities/alien")
local shooter = require("entities/shooter")
local win   = require("scenes/win")
local min   = math.min
local max   = math.max
local rand  = math.random
local floor = math.floor
local timer = require("timer")

play.max_score = 10000000
play.max_distance = 100000
play.max_speed = 333
play.max_stars = 200

function play:create()
	play.__super.create(self)
	-- state variables
	self.score = 0
	self.distance = 0
	self.speed = 0.001
	self.world = world:new()
	-- spawn timer
	self.spawn = timer:new(1, function(self)
		local choice = rand(0, 100)
		if choice < 50 then
			self.world:add("entity", alien:new(rand(0, WIDTH), rand(-HEIGHT / 4, -10)))
		else
			self.world:add("entity", shooter:new(rand(0, WIDTH), rand(-HEIGHT / 4, -10)))
		end
		self.spawn:set()
	end, self)
	-- stars
	self.stars = {}
	for i = 1, play.max_stars do
		local c = (i - 1) * 2
		self.stars[c + 1] = rand(WIDTH - 1)
		self.stars[c + 2] = rand(HEIGHT - 1)
	end
	-- cosmetic
	self.blink_score_timer = 0
	-- test spawn
	player0 = require("entities/player"):new(WIDTH / 2, HEIGHT + HEIGHT / 2)
	self.world:add("entity", player0)
	-- set audio
	love.audio.stop()
	battle_source:play()
end

function play:update(dt)
	-- update stars
	love.graphics.setColor(1, 1, 1)
	for i = 1, play.max_stars  do
		local c = (i - 1) * 2
		if self.stars[c + 2] >= HEIGHT then
			self.stars[c + 1] = rand(WIDTH - 1)
			self.stars[c + 2] = 0
		else
			self.stars[c + 2] = self.stars[c + 2] + (1 + self.speed) * dt
		end
	end
	-- update world
	self.world:update(dt)
	self.spawn:update(dt)
	-- update state variables
	self.distance = self.distance + self.speed * dt
	self.speed = min(self.max_speed, self.speed + 0.2 * dt)
	if self.distance >= self.max_distance then
		scene = win:new("Max distance reached!")
	end
	-- update cosmetic
	if self.blink_score_timer > 0 then
		self.blink_score_timer = self.blink_score_timer - dt
	end
end

local function fill_to(n, str)
	str = tostring(str)
	return ("0"):rep(n - #str) .. str
end

function play:draw()
	-- draw stars
	love.graphics.setColor(1, 1, 1)
	love.graphics.points(self.stars)
	-- draw world
	self.world:draw()
	-- draw consmetics
	if self.blink_score_timer > 0 then
		love.graphics.setColor(1, 0.5, 0.25)
	else
		love.graphics.setColor(1, 1, 1)
	end
	local str = fill_to(8, self.score)
	love.graphics.print(str, (WIDTH - #str * font_width) / 2, 0)
	love.graphics.setColor(1, 1, 1)
	-- show meters
	love.graphics.print(fill_to(6, floor(self.distance)) .. "km", 0, HEIGHT - font_height)
end

function play:add_score(n)
	self.score = min(self.max_score, self.score + n)
	total_score = self.score
	self.blink_score_timer = 1
	if self.score >= self.max_score then
		scene = win:new("Max score points got!")
	end
end

function play:close()
	player0 = nil	
end

return play
