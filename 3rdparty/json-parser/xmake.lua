target("json-parser")
	set_kind("static")
	add_files("src/json.c")
	add_includedirs("include", {public = true})
