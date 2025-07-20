print(pcall(function()


function gen(spot, ...)
	print(spot, spot, ...)
end

gen(234, 'hi')

local addr = 0x70000

addr[#8] = 65

print('mem', memory.tostr(addr))
print('mem', addr[#8])


local spot = 0x50000

spot = asm.assemble(spot,
	asm.ops.call, {asm.loc.mem, disp=0x60000, bytes=8}
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
