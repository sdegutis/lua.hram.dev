print(pcall(function()


local dostuff = 0x50000
local ok, err = asm.assembleall(dostuff,
	asm.ops.mov, {asm.loc.reg, asm.reg.rax}, {asm.loc.reg, asm.reg.rcx},
	asm.ops.sub, {asm.loc.reg, asm.reg.rsp}, {asm.loc.imm, 24},
	asm.ops.call, {asm.loc.mem, disp=0x60008, bytes=8},
	asm.ops.add, {asm.loc.reg, asm.reg.rsp}, {asm.loc.imm, 24},
	asm.ops.inc, {asm.loc.reg, asm.reg.rax},
	asm.ops.ret
)

if not ok then
	print(err)
else
	print(asm.disassemble(dostuff, ok-dostuff))

	print(dostuff(1))
	print(dostuff(2))
	print(dostuff(3))
	print(dostuff(4))
end


end))

function int()
--	local ok, err = pcall(function()
--		print("time", (0x10008)[#32])
--		local event = (0x10000)[#8]
--		if event == 1 then
--			local x = (0x10004)[#8]
--			local y = (0x10006)[#8]
--			print("mousemove!", x, y)
--		end
--	end)
--	if not ok then print(err) end
end
