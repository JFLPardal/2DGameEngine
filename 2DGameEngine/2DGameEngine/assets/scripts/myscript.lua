-- lua global var
some_variable = 100

-- lua table with initialization values
config = 
{
	title = "My Game Engine",	
	isFullscreen = false,
	resolution = 
	{ 
		width = 1020, 
		height = 600
	}
}

function factorial(n)
	if n == 1 then
		return 1
	end
	return n * factorial(n - 1)
end

print("factorial of 5 is "..factorial(5))

-- use a function on the cpp side to calculate a result
print("lua: c++ calculated cube of 3 as: "..cube(3))