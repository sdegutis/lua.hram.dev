local ok, err = pcall(function()


local t, err = thread.spawn([[
	local a,b,c = ...
	print('inside thread', table.pack(...).n, ...)
	print('a='..tostring(a))
	print('b='..tostring(b))
	print('c='..tostring(c))
]], 123, nil, 'asdf')
thread.close(t)

print('outside thread', t, err)



end)

if not ok then print(err) end

function int()
end
