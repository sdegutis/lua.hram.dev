local int    = 0x30004
local screen = 0x30100
local font   = 0x32500

local maxcols = 32
local maxrows = 12

function blit()
	int[0] = 1
end

function sig()
	if (0x30000)[0] == 0 then
		local pixel = math.random(0, 128*72)
		--pixel = 1
		screen[pixel] = math.random(0xff)
		blit()
	end
end

function printchar(idx, x, y)
	local scraddr = screen + 128*y+x
	local fntaddr = font + idx*4*6
	for y=0,5 do
		print(string.format('%d\t%x\t%x', y, scraddr+y*128, fntaddr+y*4))
		memcpy(scraddr+y*128, fntaddr+y*4, 4)
	end
	blit()
end

local cursor = 0x30006
cursor[0]=1
cursor[1]=1

function printf(s, ...)
	s = string.format(s, ...)

	local col = cursor[0]
	local row = cursor[1]

	for i = 1, #s do

		local char = s:byte(i)

		if char == 0xA then
			col = 1
			row = row + 1
			goto continue
		end

		if char < 32 or char > 126 then
			goto continue
		end
		
		local idx = char - 32
		local x = (col-1) * 4
		local y = (row-1) * 6

		printchar(idx, x, y)

		col = col + 1

		if col > 32 then
			col = 1
			row = row + 1
		end

		::continue::

	end

	cursor[0] = col
	cursor[1] = row
end

--function print(...)
--	local t = table.pack(...)
--	for i=#t-1,2,-1 do
--		table.insert(t, '\t')
--	end
--	printf()
--end

for i=1,13 do
	printf("hello %d\n", i)
end


--[=[

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

	coroutine=coroutine,
	string=string,
	table=table,
	math=math,
	utf8=utf8,

	memcpy=memory.copy,
	memset=memory.fill,
	strndup=memory.tostr,

	exec=asm.exec,
	assemble=asm.assemble,
	disassemble=asm.disassemble,

	drawtext=print,

	lpeg=lpeg,
}

for k,v in pairs(asm.ops) do env[k] = v end
for k,v in pairs(asm.reg) do env[k] = v end
for k,v in pairs(asm.loc) do env[k] = v end

-- welcome screen
print('', 0, 0)

local welcome = [[
 _   _   ____     _     _   _ 
| |_| | | __ \   /,\   | \ / |
|  _  | |    /  / _ \  |  V  |
|_| |_| |__\_\ /_/ \_\ |_| |_|
                                 
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
	if total < 1070 then return end

	_G.sig = function()
		if env.signal then
			xpcall(env.signal, errfn)
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

--]=]
