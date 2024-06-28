#include <chrono>
#include <string>

#include <clipp.h>
#include <nowide/args.hpp>
#include <nowide/cstdio.hpp>
#include <nowide/iostream.hpp>
#include <spdlog/common.h>

#include <intl.hpp>
#include <otfcc/font.h>
#include <otfcc/sfnt-builder.h>
#include <otfcc/sfnt.h>

#include <config.h>

#include "aliases.h"
#include "stopwatch.h"

using std::string;

void printInfo() {
	fmt::println(fmt::runtime(_("This is WFM otfccbuild, version {}.{}.{}.")),
	             WFM_all_VERSION_MAJOR, WFM_all_VERSION_MINOR,
	             WFM_all_VERSION_PATCH);
}
string readEntireFile(const string &inPath, const otfcc::options_t &options) {
	FILE *f = nowide::fopen(inPath.c_str(), "rb");
	if (!f) {
		options.logger.error(_("Cannot read JSON file \"{}\". Exit."), inPath);
		throw std::runtime_error("failed to load file");
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	string result(size, 0);
	fread(result.data(), 1, size, f);
	fclose(f);
	return result;
}

string readEntireStdin() {
#ifdef _WIN32
	freopen(NULL, "rb", stdin);
#endif
	string result;
	int ch;
	while ((ch = getchar()) != EOF)
		result.push_back(ch);
	return result;
}

int main(int argc, char *argv[]) {
	nowide::args args(argc, argv);

	auto begin = std::chrono::system_clock::now();

	bool show_help = false;
	bool show_version = false;
	std::string outputPath;
	std::string inPath;

	otfcc::options_t options("otfccbuild");

	using clipp::option;
	using clipp::required;
	using clipp::value;
	auto cli =
	    (value("input.json", inPath) % _("Path to input file. When absent the "
	                                     "input will be read from the STDIN."),
	     option("-h", "--help").set(show_help) %
	         _("Display this help message and exit."),
	     option("-v", "--version").set(show_version) %
	         _("Display version information and exit."),
	     required("-o") & value("output.(ttf|otf)", outputPath) %
	                          _("Set output file path to <output.(ttf|otf)>."),
	     option("-s", "--dummy-dsig").set(options.dummy_DSIG) %
	         _("Include an empty DSIG table in the font. For some Microsoft "
	           "applications, DSIG is required to enable OpenType features."),
	     option("-O0").call([&options]() { options.optimize_to(0); }) %
	         _("Turn off any optimization."),
	     option("-O1").call([&options]() { options.optimize_to(1); }) %
	         _("Default optimization."),
	     option("-O2").call([&options]() { options.optimize_to(2); }) %
	         _("More aggressive optimizations for web font. In this level, the "
	           "following options will be set:\n"),
	     option("-O3").call([&options]() { options.optimize_to(3); }) %
	         _("Most aggressive opptimization strategy will be used. In this "
	           "level, these options will be set:\n"),
	     option("--verbose").set(options.verbose) %
	         _("Show more information when building."),
	     option("-q", "--quiet").set(options.quiet) %
	         _("Be silent when building."),
	     option("--ignore-hints").set(options.ignore_hints) %
	         _("Ignore the hinting information in the input."),
	     option("--keep-average-char-width")
	             .set(options.keep_average_char_width) %
	         _("Keep the OS/2.xAvgCharWidth value from the input instead of "
	           "stating the average width of glyphs. Useful when creating a "
	           "monospaced font."),
	     option("--keep-unicode-ranges").set(options.keep_unicode_ranges) %
	         _("Keep the OS/2.ulUnicodeRange[1-4] as-is."),
	     option("--keep-modified-time").set(options.keep_modified_time) %
	         _("Keep the head.modified time in the json, instead of using "
	           "current time."),
	     option("--short-post").set(options.short_post) %
	         _("Don't export glyph names in the result font."),
	     option("-i", "--ignore-glyph-order").set(options.ignore_glyph_order) %
	         _("Ignore the glyph order information in the input."),
	     option("-k", "--keep-glyph-order", "--dont-ignore-glyph-order")
	             .set(options.ignore_glyph_order, false) %
	         _("Keep the glyph order information in the input. Use to preserve "
	           "glyph order under -O2 and -O3."),
	     option("--merge-features").set(options.merge_features) %
	         _("Merge duplicate OpenType feature definitions."),
	     option("--dont-merge-features").set(options.merge_features, false) %
	         _("Keep duplicate OpenType feature definitions."),
	     option("--merge-lookups").set(options.merge_lookups) %
	         _("Merge duplicate OpenType lookups."),
	     option("--dont-merge-lookups").set(options.merge_lookups, false) %
	         _("Keep duplicate OpenType lookups."),
	     option("--force-cid").set(options.force_cid) %
	         _("Convert name-keyed CFF OTF into CID-keyed."),
	     option("--subroutinize").set(options.cff_doSubroutinize) %
	         _("Subroutinize CFF table."),
	     option("--stub-cmap4").set(options.stub_cmap4) %
	         _("Create a stub `cmap` format 4 subtable if format 12 subtable "
	           "is present."),
			   option("--ship").call([&options]() {
				options.ignore_glyph_order = true;
				options.short_post = true;
				options.dummy_DSIG = true;
			   }));

	if (!clipp::parse(argc, argv, cli)) {
		nowide::cout << clipp::make_man_page(cli, "merge-otd");
		return EXIT_FAILURE;
	}

	options.logger.set_level(options.quiet     ? spdlog::level::critical
	                         : options.verbose ? spdlog::level::trace
	                                           : spdlog::level::warn);

	if (show_help) {
		printInfo();
		nowide::cout << clipp::make_man_page(cli, "otfccbuild");
		return 0;
	}
	if (show_version) {
		printInfo();
		return 0;
	}

	std::string buffer;
	loggedStep("Load file") {
		if (!inPath.empty()) {
			loggedStep(fmt::format("Load from file {}", inPath)) {
				buffer = readEntireFile(inPath, options);
			}
		} else {
			loggedStep("Load from stdin") { buffer = readEntireStdin(); }
		}
		logStepTime;
	}

	json_value *jsonRoot = NULL;
	loggedStep("Parse into JSON") {
		jsonRoot = json_parse(buffer.data(), buffer.size());
		buffer.clear();
		logStepTime;
		if (!jsonRoot) {
			options.logger.error(_("Cannot parse JSON file \"{}\". Exit."),
			                     inPath);
			exit(EXIT_FAILURE);
		}
	}

	otfcc_Font *font;
	loggedStep("Parse") {
		otfcc_IFontBuilder *parser = otfcc_newJsonReader();
		font = parser->read(jsonRoot, 0, options);
		if (!font) {
			logError(_("Cannot parse JSON file \"{}\" as a font. Exit."),
			         inPath);
			exit(EXIT_FAILURE);
		}
		parser->free(parser);
		json_value_free(jsonRoot);
		logStepTime;
	}
	loggedStep("Consolidate") {
		otfcc_iFont.consolidate(font, options);
		logStepTime;
	}
	loggedStep("Build") {
		otfcc_IFontSerializer *writer = otfcc_newOTFWriter();
		caryll_Buffer *otf = (caryll_Buffer *)writer->serialize(font, options);
		loggedStep("Write to file") {
			FILE *outfile = nowide::fopen(outputPath.c_str(), "wb");
			if (!outfile) {
				logError(_("Cannot write to file \"{}\". Exit."), outputPath);
				exit(EXIT_FAILURE);
			}
			fwrite(otf->data, sizeof(uint8_t), buflen(otf), outfile);
			fclose(outfile);
		}
		logStepTime;
		buffree(otf);
		writer->free(writer);
		otfcc_iFont.free(font);
	}

	return 0;
}
