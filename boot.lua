print(pcall(function()

local spot = 0x50000
FUNC=memory.read(0x40000, 64)

spot = assembly.assemble(spot,
	assembly.ops.mov,
	{assembly.types.reg, assembly.regs.rax},
	{assembly.types.imm, FUNC}
)
--[[
spot = assembly.assemble(spot,
	assembly.ops.mov,
	{assembly.types.reg, assembly.regs.rcx},
	{assembly.types.imm, 34}
)
spot = assembly.assemble(spot,
	assembly.ops.mov,
	{assembly.types.reg, assembly.regs.rdx},
	{assembly.types.imm, 15}
)
---]]
spot = assembly.assemble(spot,
	assembly.ops.call,
	{assembly.types.reg, assembly.regs.rax}
)
--[[
spot = assembly.assemble(spot,
	assembly.ops.add,
	{assembly.types.reg, assembly.regs.rsp},
	{assembly.types.imm, 12}
)
]]
spot = assembly.assemble(spot,
	assembly.ops.ret
)

-- local res = assembly.exec(0x50000)
-- print("res = "..res)


print()
print('DISASM')
print('len', spot-0x50000)
local got = assembly.disassemble(0x50000, spot-0x50000)
print('START')
print(got)
print('END')
print()





function int()
--	local ok, err = pcall(function()
--		print("time", memory.read(0x10008, 32))
--		local event = memory.read(0x10000, 8)
--		if event == 1 then
--			local x = memory.read(0x10004, 8)
--			local y = memory.read(0x10006, 8)
--			print("mousemove!", x, y)
--		end
--	end)
--	if not ok then print(err) end
end

end))
