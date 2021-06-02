local timer = require("class"):extend()

function timer:create(max, fn, ...)
	self.max = max
	self.cur = 0 
	self.is_out = false 
	self.fn = fn or function() end
	self.params = ...
end

function timer:set(max)
	if max then self.max = max end
	self.cur = 0
	self.is_out = false
end

function timer:stop()
	self.cur = self.max
	self.is_out = true
end

function timer:update(dt)
	if self.cur < self.max then
		self.cur = self.cur + dt
	elseif not self.is_out then
		self.is_out = true
		self.fn(self.params)
	end
end

function timer:out()
	return self.is_out
end

return timer
