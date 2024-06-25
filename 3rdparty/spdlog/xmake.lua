target("spdlog")
	set_kind("static")
	add_deps("fmt")
	add_files("src/*.cpp|bundled_fmtlib_format.cpp")
	add_defines(
		"SPDLOG_COMPILED_LIB",
		"SPDLOG_FMT_EXTERNAL",
		{public = true})
	add_includedirs("include", {public = true})
