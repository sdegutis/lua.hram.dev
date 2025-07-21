-- default loop
function int()
	local sysdata = 0x10000
	local event = sysdata[0]
	local arg = sysdata[4]

	if event == 6 and arg == 122 then
		fullscreen()
	end
end

-- setup print
local lasty=0
local lastx=0
function print(str, startx, starty)
	startx = startx or lastx
	starty = starty or lasty

	local x = startx
	local y = starty

	for i=1,#str do
		local idx = str:byte(i)-32

		if idx == 0xa-32 then
			x = startx
			y = y + 6
			goto continue
		end

		if idx < 0 or idx > 16*6 then
			goto continue
		end

		local fx = (idx %  16) * 4
		local fy = (idx // 16) * 6
		image.copy(#0x10100, #0x10120, x, y, fx, fy, 4, 6)
		x = x + 4

		::continue::
	end

	lastx = startx
	lasty = y+6
end

-- boot
local userfile = userdir..'\\hram\\boot.lua'
print("welcome to HRAM, the Hand-Rolled Assembly Machine!", 3, 2)
print("loading "..userfile)

local file, err = io.open(userfile)
if not file then
	print(err)
else
	local str = file:read('a')
	file:close()

	local fn, err = load(str, 'boot.lua')
	if not fn then
		print(err)
	else
		xpcall(fn, function(err)
			print(err)
		end)
	end
end
