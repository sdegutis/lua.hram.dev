local ok, err = pcall(function()

img = image.create(0x10100+(4*6*4*(97-32)), 4, 6)
image.copy(nil, img, 1, 1)

end)

if not ok then print(err) end

function int()
end
