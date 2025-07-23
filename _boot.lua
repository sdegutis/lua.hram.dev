local isboot = ...
local int    = 0x30004
local screen = 0x30100
local font   = 0x32500

local maxcols = 32
local maxrows = 12

function blit()
	int[0] = 1
end

local lastfore = 0xf0
function setfontcolor(fore, back)
	fore = tonumber(fore)
	back = tonumber(back)
	for i = 0, 4*6*16*6-1 do
		if font[i] == lastfore then
			font[i] = fore
		else
			font[i] = back
		end
	end
	lastfore = fore
end


function printchar(idx, x, y)
	local scraddr = screen + 128*y+x
	local fntaddr = font + idx*4*6
	for y=0,5 do
		memcpy(scraddr+y*128, fntaddr+y*4, 4)
	end
	blit()
end

local cursor = 0x30006
cursor[0]=1
cursor[1]=1

local tabsize = 4

function printf(s, ...)
	s = string.format(s, ...)

	local col = cursor[0]
	local row = cursor[1]

	for i = 1, #s do

		local char = s:byte(i)

		if char == 0x9 then
			while (col-1) % tabsize ~= 0 do
				col = col + 1
				if col > 32 then
					col = 1
					row = row + 1
					goto continue
				end
			end
			goto continue
		end

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

function print(...)
	local t = table.pack(...)
	for i=1,t.n do
		t[i] = tostring(t[i])
	end
	for i=t.n,2,-1 do
		table.insert(t, i, '\t')
	end
	table.insert(t, '\n')
	printf(table.concat(t))
end


local welcome = [[
  _   _   ____     _     _   _  
 | |_| | | __ \   /,\   | \ / | 
 |  _  | |    /  / _ \  |  V  | 
 |_| |_| |__\_\ /_/ \_\ |_| |_| 
                                
THE HAND ROLLED ASSEMBLY MACHINE
================================
                                
///  program like it's 1979! ///
]]

-- welcome screen

setfontcolor(0x0f)
cursor[1] = 2

local co = coroutine.create(function()
	for i = 1, 5 do
		coroutine.yield()
	end
	for s in welcome:gmatch('[^\n]+') do
		printf(s)
		coroutine.yield()
	end
end)

coroutine.resume(co)




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

	memcpy=memcpy,
	memset=memset,
	strndup=strndup,

	exec=asm.exec,
	asm=asm.asm,
	dasm=asm.dasm,

	print=print,
	printf=printf,

	lpeg=lpeg,
}

for k,v in pairs(asm.ops) do env[k] = v end
for k,v in pairs(asm.reg) do env[k] = v end
for k,v in pairs(asm.loc) do env[k] = v end



function errfn(err)
	print(tostring(err))
end

local total = 0
function sig()
	local event = (0x30001)[0]
	if event == 0 then total = total + 1 end
	if total % 2 == 0 then coroutine.resume(co) end
	if total < 70 then return end

	_G.sig = function()
		if env.signal then
			xpcall(env.signal, errfn)
		end
	end

	memset(0x30100, 0, 128*72)
	setfontcolor(0xff)
	cursor[1] = 1

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
