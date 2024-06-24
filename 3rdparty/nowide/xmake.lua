target("nowide")
	if is_os("windows") then
		set_kind("static")
		add_files("src/iostream.cpp")
	else
		set_kind("headeronly")
	end
	add_includedirs("include", {public = true})
