set_version("1.1.0")
set_languages("c17", "cxx20")

add_rules("mode.release", "mode.debug")

if not is_os("windows") then
	add_defines("_POSIX_C_SOURCE=200809L")
end

rule("static_binary")
	on_load(function (target)
		if target:is_plat("linux") then
			target:add("ldflags", "-static-pie")
		elseif target:is_plat("mingw") then
			target:add("ldflags", "-static")
		end
	end)

target("merge-otd")
	set_kind("binary")
	add_rules("static_binary")
	add_deps("clipp", "json", "nowide")
	add_files("src/merger/*.cpp")

target("otfccbuild")
	set_kind("binary")
	add_rules("static_binary")
	add_deps("nowide")
	add_deps("config", "otfcc")
	add_files(
		"src/otfcc-driver/otfccbuild.c",
		"src/otfcc-driver/stopwatch.c",
		{sourcekind = "cxx"})

target("otfccdump")
	set_kind("binary")
	add_rules("static_binary")
	add_deps("nowide")
	add_deps("config", "otfcc")
	add_files(
		"src/otfcc-driver/otfccdump.c",
		"src/otfcc-driver/stopwatch.c",
		{sourcekind = "cxx"})

target("config")
	set_kind("phony")
	add_includedirs("$(buildir)", {public = true})
	add_configfiles("config/config.h.in", {variables = variables})
	add_configfiles("config/config.sh.in", {variables = variables})

includes("3rdparty/clipp")
includes("3rdparty/fmt")
includes("3rdparty/json")
includes("3rdparty/json-builder")
includes("3rdparty/json-parser")
includes("3rdparty/nowide")
includes("3rdparty/sds")
includes("3rdparty/uthash")
includes("lib/otfcc")
