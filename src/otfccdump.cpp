#include <chrono>

#include <json-builder.h>
#include <clipp.h>
#include <nowide/args.hpp>
#include <nowide/cstdio.hpp>
#include <nowide/iostream.hpp>
#include <spdlog/common.h>

#include <intl.hpp>
#include <otfcc/font.h>
#include <otfcc/sfnt.h>

#include <config.h>

#include "aliases.h"
#include "stopwatch.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
inline const bool is_stdout_tty =
    GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) == FILE_TYPE_CHAR;
#else
#include <unistd.h>
inline const bool is_stdout_tty = isatty(STDOUT_FILENO);
#endif

void printInfo() {
	fmt::println(fmt::runtime(_("This is WFM otfccdump, version {}.{}.{}.")),
	             WFM_all_VERSION_MAJOR, WFM_all_VERSION_MINOR,
	             WFM_all_VERSION_PATCH);
}
int main(int argc, char *argv[]) {
	nowide::args args(argc, argv);

	bool show_help = false;
	bool show_version = false;
	bool show_pretty = false;
	bool show_ugly = false;
	uint32_t ttcindex = 0;

	otfcc::options_t options("otfccdump");

	std::string outputPath;
	std::string inPath;

	using clipp::option;
	using clipp::value;
	auto cli =
	    (option("-h", "--help").set(show_help) %
	         _("Display this help message and exit."),
	     option("-v", "--version").set(show_version) %
	         _("Display version information and exit."),
	     option("-o") & value("file", outputPath) %
	                        _("Set output file path to <file>. When absent the "
	                          "dump will be written to STDOUT."),
	     option("-n", "--ttc-index") &
	         value("n", ttcindex) %
	             _("Use the <n>th subfont within the input font."),
	     option("--pretty").set(show_pretty) % _("Prettify the output JSON."),
	     option("--ugly").set(show_ugly) % _("Force uglify the output JSON."),
	     option("--verbose").set(options.verbose) %
	         _("Show more information when building."),
	     option("-q", "--quiet").set(options.quiet) %
	         _("Be silent when building."),
	     option("--ignore-glyph-order").set(options.ignore_glyph_order) %
	         _("Do not export glyph order information."),
	     option("--glyph-name-prefix") &
	         value("prefix", options.glyph_name_prefix) %
	             _("Add a prefix to the glyph names."),
	     option("--ignore-hints").set(options.ignore_hints) %
	         _("Do not export hinting information."),
	     option("--decimal-cmap").set(options.decimal_cmap) %
	         _("Export 'cmap' keys as decimal number."),
	     option("--hex-cmap").set(options.decimal_cmap, false) %
	         _("Export 'cmap' keys as hex number (U+FFFF)."),
	     option("--name-by-hash").set(options.name_glyphs_by_hash) %
	         _("Name glyphs using its hash value."),
	     option("--name-by-gid").set(options.name_glyphs_by_gid) %
	         _("Name glyphs using its glyph id."),
	     option("--instr-as-bytes").set(options.instr_as_bytes),
	     option("--debug-wait-on-start").set(options.debug_wait_on_start),
	     value("input.ttf", inPath));

	if (!clipp::parse(argc, argv, cli)) {
		nowide::cout << clipp::make_man_page(cli, "merge-otd");
		return EXIT_FAILURE;
	}

	if (options.debug_wait_on_start) {
		getchar();
	}

	options.logger.set_level(options.quiet     ? spdlog::level::critical
	                         : options.verbose ? spdlog::level::trace
	                                           : spdlog::level::warn);

	if (show_help) {
		printInfo();
		nowide::cout << clipp::make_man_page(cli, "otfccdump");
		return 0;
	}
	if (show_version) {
		printInfo();
		return 0;
	}

	auto begin = std::chrono::system_clock::now();

	otfcc_SplineFontContainer *sfnt;
	loggedStep("Read SFNT") {
		logProgress(_("From file {}"), inPath);
		FILE *file = nowide::fopen(inPath.c_str(), "rb");
		sfnt = otfcc_readSFNT(file);
		if (!sfnt || sfnt->count == 0) {
			logError(_("Cannot read SFNT file \"{}\". Exit."), inPath);
			exit(EXIT_FAILURE);
		}
		if (ttcindex >= sfnt->count) {
			logError(_("Subfont index {0} out of range for \"{1}\" (0 -- {2}). "
			           "Exit."),
			         ttcindex, inPath, (sfnt->count - 1));
			exit(EXIT_FAILURE);
		}
		logStepTime;
	}

	otfcc_Font *font;
	loggedStep("Read Font") {
		otfcc_IFontBuilder *reader = otfcc_newOTFReader();
		font = reader->read(sfnt, ttcindex, options);
		if (!font) {
			logError(_("Font structure broken or corrupted \"{}\". Exit."),
			         inPath);
			exit(EXIT_FAILURE);
		}
		reader->free(reader);
		if (sfnt)
			otfcc_deleteSFNT(sfnt);
		logStepTime;
	}
	loggedStep("Consolidate") {
		otfcc_iFont.consolidate(font, options);
		logStepTime;
	}
	json_value *root;
	loggedStep("Dump") {
		otfcc_IFontSerializer *dumper = otfcc_newJsonWriter();
		root = (json_value *)dumper->serialize(font, options);
		if (!root) {
			logError(_("Font structure broken or corrupted \"{}\". Exit."),
			         inPath);
			exit(EXIT_FAILURE);
		}
		logStepTime;
		dumper->free(dumper);
	}

	char *buf;
	size_t buflen;
	loggedStep("Serialize to JSON") {
		json_serialize_opts jsonOptions;
		jsonOptions.mode = json_serialize_mode_packed;
		jsonOptions.opts = 0;
		jsonOptions.indent_size = 4;
		if (show_pretty || (outputPath.empty() && is_stdout_tty)) {
			jsonOptions.mode = json_serialize_mode_multiline;
		}
		if (show_ugly)
			jsonOptions.mode = json_serialize_mode_packed;
		buflen = json_measure_ex(root, jsonOptions);
		buf = (char *)calloc(1, buflen);
		json_serialize_ex(buf, root, jsonOptions);
		logStepTime;
	}

	loggedStep("Output") {
		if (!outputPath.empty()) {
			FILE *outputFile = nowide::fopen(outputPath.c_str(), "wb");
			if (!outputFile) {
				logError(_("Cannot write to file \"{}\". Exit."), outputPath);
				exit(EXIT_FAILURE);
			}
			size_t actualLen = buflen - 1;
			while (!buf[actualLen])
				actualLen -= 1;
			fwrite(buf, sizeof(char), actualLen + 1, outputFile);
			fclose(outputFile);
		} else {
			fputs(buf, stdout);
		}
		logStepTime;
	}

	loggedStep("Finalize") {
		free(buf);
		if (font)
			otfcc_iFont.free(font);
		if (root)
			json_builder_free(root);
		logStepTime;
	}

	return 0;
}
