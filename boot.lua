print(pcall(function()


local spot = 0x50000

spot = assembly.assemble(spot,
	assembly.ops.call,
	{assembly.loc.mem, 0, 0, 0, 0x60000, 8}
)
spot = assembly.assemble(spot,
	assembly.ops.ret
)


print(assembly.disassemble(0x50000, spot-0x50000))

print(memory.read(0x1000f, 64))
assembly.exec(0x50000)
--[[
assembly.exec(0x50000)
assembly.exec(0x50000)
assembly.exec(0x50000)
--]]



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
