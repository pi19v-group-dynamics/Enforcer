VERSION = "v0.8"
WIDTH  = 240
HEIGHT = 184

function love.conf(t)
	t.window.width  = WIDTH  * 4
	t.window.height = HEIGHT * 4
	t.window.resizable = true
end
