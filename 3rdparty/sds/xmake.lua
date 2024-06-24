target("sds")
	set_kind("static")
	add_files("src/sds.c")
	add_includedirs("include", {public = true})
