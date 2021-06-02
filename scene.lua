local scene = require("class"):extend()

function scene:create()
	self.previous_scene = scene
	if self.previous_scene ~= nil then
		self.previous_scene:close()
	end
end

function scene:update(dt) end
function scene:draw() end
function scene:close() end 

return scene
