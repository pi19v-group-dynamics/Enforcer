local world = require("class"):extend()

function world:create()
	self.entity = {}
	self.particle = {}
	self.spritebatch = love.graphics.newSpriteBatch(spritesheet)
end

local insert = table.insert
local remove = table.remove

function world:update(dt)
	-- update entity
	for i = #self.entity, 1, -1 do
		local this = self.entity[i]
		if this.exist then
			this:update(dt)
			for j = i + 1, #self.entity do
				local other = self.entity[j]
				if other.exist then
					this:collision(other)
				end
			end
		else
			remove(self.entity, i)
		end
	end
	-- update particle
	for i = #self.particle, 1, -1 do
		local this = self.particle[i]
		if this.exist then
			this:update(dt)
		else
			remove(self.particle, i)
		end
	end
end

function world:add(what, ent)
	local i = 1
	local arr = self[what]
	while arr[i] ~= nil do
		i = i + 1
	end
	ent.id = i
	arr[i] = ent
	return i
end

function world:draw()
	-- draw entity
	self.spritebatch:clear()
	for i = 1, #self.entity do
		local e = self.entity[i]
		e:draw(self.spritebatch)
	end
	-- draw particle
	for i = 1, #self.particle do
		local e = self.particle[i]
		e:draw(self.spritebatch)
	end
	-- flush to GPU, display
	self.spritebatch:flush()
	love.graphics.draw(self.spritebatch)
end

return world
