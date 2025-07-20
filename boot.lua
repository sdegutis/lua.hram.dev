local ok, err = pcall(function()


local cs = sync.newcritsec()


local t, err = sync.newthread([[
	local a,b,c,cs = ...
	sync.entercritsec(cs)
	print('inside thread', table.pack(...).n, ...)
	print('a='..tostring(a))
	print('b='..tostring(b))
	print('c='..tostring(c))
	sync.leavecritsec(cs)
	sync.delcritsec(cs)
]], 123, nil, 'asdf', cs)

sync.entercritsec(cs)
sync.closehandle(t)
print('outside thread')
print(t, err)
print('sleep')
sync.sleep(1000)
print('wake')
sync.leavecritsec(cs)


end)

if not ok then print(err) end

function int()
end
