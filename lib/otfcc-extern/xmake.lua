target("otfcc-extern")
	set_kind("static")
	add_files("src/**.c")
	add_includedirs("include/dep")
	add_includedirs("include", {public = true})
