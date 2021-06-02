local rect = require("class"):extend()

function rect:create(x, y, w, h)
	self.x = x
	self.y = y
	self.w = w
	self.h = h
end

local function rect_point(x0, y0, x, y, w, h)
	return x0 >= x and y0 >= y and x0 < x + w and y0 < y + h
end

function rect:overlap(x, y, w, h)
	if not h then -- rect vs circle?
		if not w then -- rect vs point?
			return rect_point(x, y, self.x, self.y, self.w, self.h)
		else
			local x0 = self.x + self.w * 0.5
			local y0 = self.y + self.h * 0.5
			local wr = self.w / w
			local hr = self.h / w
			return rect_point(x, y, x0 - wr, y0 - wr, wr * 2, hr * 2)
		end
	end
	-- rect vs rect
	return x < self.x + self.w and y < self.y + self.h and x + w >= self.x and y + h >= self.y
end

return rect
