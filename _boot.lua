
-- setup print
local doblit = 0x1000c
local lasty=0
local lastx=0
function print(str, startx, starty)
    str = tostring(str)
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
		image.copy(#0x10100, #0x10118, x, y, fx, fy, 4, 6)
		x = x + 4

		::continue::
	end

	lastx = startx
	lasty = y+6

	doblit[0]=1
end


-- sandbox
local env = {
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
	lpeg=lpeg,
}


-- welcome screen
print('', 95, 50)

local welcome = [[
  _   _   ___     _     __  __  
 | |_| | | _ \   / \   |  \/  | 
 | ___ | |   /  / ^ \  | |\/| | 
 |_| |_| |_\_\ /_/ \_\ |_|  |_| 
                                
THE HAND ROLLED ASSEMBLY MACHINE
================================
                                
///  program like it's 1979! ///
]]

local co = coroutine.create(function()
	for i = 1, 5 do
		coroutine.yield()
	end
	for i = 1, #welcome, 33 do
		local s = welcome:sub(i, i+31)
		print(s)
		coroutine.yield()
	end
end)

coroutine.resume(co)

function errfn(err)
	print(tostring(err))
end

local total = 0
function sig()
	local sysdata = 0x10000
	local event = sysdata[0]
	if event == 0 then total = total + 1 end
	if total % 2 == 0 then coroutine.resume(co) end
	if total < 50 then return end

	_G.sig = function()
		if env.sig then
			xpcall(env.sig, errfn)
		end
	end

	-- user boot
	print("loading boot.lua", 3, 2)

	local found, err = package.searchpath('boot', userdir..'\\hram\\?.lua;.\\?.lua')
	if not found then
		print('error loading boot.lua:\n'..err)
	else
		print('found: '..found)
		local fn = loadfile(found, 't', env)
		xpcall(fn, errfn)
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




--[[
	local sysdata = 0x10000
	local event = sysdata[0]
	local arg = sysdata[4]

	if event == 6 and arg == 122 then
		fullscreen()
	end

--]]
