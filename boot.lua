local ok, err = pcall(function()

img = image.create(0x10100+(4*6*4*(97-32)), 4, 6)
image.copy(nil, img, 1, 1)



end)

addr = 0x10100+(4*6*4*(97-32))
for i=0,23 do
	local old = addr[i]
	addr[i] = 0x12
	print(old, addr[i], old)
end

if not ok then print(err) end



fullscreen = 0x50000
ok, err = asm.assembleall(fullscreen,
	asm.ops.sub, {asm.loc.reg, asm.reg.rsp}, {asm.loc.imm, 24},
	asm.ops.call, {asm.loc.mem, disp=0x60008, bytes=8},
	asm.ops.add, {asm.loc.reg, asm.reg.rsp}, {asm.loc.imm, 24},
	asm.ops.ret
)
fullscreen()


sysdata = 0x10000

function int()

	local event = sysdata[0]
	local arg = sysdata[4]

	if event == 6 and arg == 122 then
		fullscreen()
	end
	
end
