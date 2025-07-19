print(pcall(function()

memory.copy(0x30000, "\x48\x89\xC8\x48\xFF\xC0\xC3")
memory.copy(0x40000, 0x30000, 7)

print("ops", assembly.ops)
print("mov", assembly.ops.mov)

--local res = assembly.exec(0x40000, 41)
--print("res = "..res)


print()
print('DISASM')
local len = memory.read(0x60000, 64)
print('len', len)
local got = assembly.disassemble(0x50000, len)
print('START')
print(got)
print('END')
print()

--[=[
m = mutex.create()

t = thread.create([[
	local m = ...
	mutex.lock(m)
	print('locked in thread')
	mutex.unlock(m)
	print('done in thread')
]], m)

--thread.sleep(1000)
mutex.lock(m)
print('locked in main')
mutex.unlock(m)
print('done in main')


-- print(t)

-- require 'bar'

--]=]




local m = 0x10000
for i = 0,3*3*4-1 do memory.write(m+i, 8, math.random(0xff)-1) end
local img = image.create(m, 3, 3)

image.copy(nil, img, 0, 0)

local m2 = 0x20000
memory.fill(m2, 0, 9*9*4)
local img2 = image.create(m2, 9, 9)

image.copy(img2, img, 0, 0, 0, 0, 3, 3)
image.copy(img2, img, 6, 0, 0, 0, 3, 3)
image.copy(img2, img, 0, 6, 0, 0, 3, 3)
image.copy(img2, img, 6, 6, 0, 0, 3, 3)

image.delete(img)



function mousemove(x, y)
	--image.copy(nil, img2, x, y)
	image.update(nil, x, y, 2, 2, m+4*4, 3*4)
	print('mouse moved', x, y)
end

function mousewheel(d)
	print('mouse wheel', d)
end

function mouseup(b)
	print('mouse up', b)
end

function mousedown(b)
	print('mouse up', b)
end

function keyup(k)
	print('key up', k)
end

function keydown(k)
	print('key up', k)
end

function keychar(s)
	print('key char', s)
end

end))
