print(pcall(function()


local spot = 0x50000

spot = asm.assemble(spot,
	asm.ops.call,
	{asm.loc.mem, 0, 0, 0, 0x60000, 8}
)
spot = asm.assemble(spot,
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
