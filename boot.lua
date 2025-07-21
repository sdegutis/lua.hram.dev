local doblit = 0x1000c

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

	doblit[0]=1
end

-- sandbox
local env = setmetatable({
    assert=assert,
    error=error,
    warn=warn,
	getmetatable=getmetatable,
    setmetatable=setmetatable,
	select=select,
	next=next,
    pairs=pairs,
    ipairs=ipairs,
	rawequal=rawequal,
    rawget=rawget,
    rawlen=rawlen,
    rawset=rawset,
	tonumber=tonumber,
    tostring=tostring,
    type=type,
	pcall=pcall,
    xpcall=xpcall,
    collectgarbage=collectgarbage,

	fullscreen=fullscreen,
	print=print,

	coroutine=coroutine,
	string=string,
	table=table,
	math=math,
	utf8=utf8,
	memory=memory,
	image=image,
	asm=asm,
	sync=sync,
}, {
	__index = _G,
	__newindex = _G,
})

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

	local fn, err = load(str, 'boot.lua', 't', env)
	if not fn then
		print(err)
	else
		xpcall(fn, function(err)
			print(err)
		end)
	end
end




--[[


sync

  newthread(fn<string>, ...) -> int|[nil,string]
    creates and returns a thread handle
    extra args are passed to fn
    returns error only on syntax errors

  sleep(ms)
    sleep the current thread for ms milliseconds

  newcritsec() -> int
    return a new critical section

  delcritsec(ms)
    delete a critical section

  entercritsec(ms)
    enter a critical section
    only one thread can enter at a time

  leavecritsec(ms)
    leave a critical section

  closehandle(int)
    frees resources and notifies waiters
    these must be manually closed:
      threads, semaphores

--]]
