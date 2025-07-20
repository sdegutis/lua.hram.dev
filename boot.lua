print(pcall(function()


local spot = 0x50000

spot = asm.assembleall(spot,
	asm.ops.sub, {asm.loc.reg, asm.reg.rsp}, {asm.loc.imm, 24},
	asm.ops.call, {asm.loc.mem, disp=0x60000, bytes=8},
	asm.ops.add, {asm.loc.reg, asm.reg.rsp}, {asm.loc.imm, 24},
	asm.ops.ret
)


print(asm.disassemble(0x50000, spot-0x50000))

asm.exec(0x50000)
asm.exec(0x50000)
asm.exec(0x50000)
asm.exec(0x50000)



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
