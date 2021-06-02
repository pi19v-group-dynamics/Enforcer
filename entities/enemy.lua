local enemy = require("entity"):extend()
enemy.reward = 0

function enemy:create(x, y, zone)
	enemy.__super.create(self, x, y, 8)
end

function enemy:on_collision(other)
	self.exist = false
	scene:add_score(self.reward)
end

return enemy

