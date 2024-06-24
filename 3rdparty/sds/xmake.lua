target("sds")
	set_kind("static")
	add_files("src/sds.c", {sourcekind = "cxx"})
	add_includedirs("include", {public = true})
