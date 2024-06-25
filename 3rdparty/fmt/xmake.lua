target("fmt")
	set_kind("static")
	add_files("src/*.cc|fmt.cc")
	add_includedirs("include", {public = true})
