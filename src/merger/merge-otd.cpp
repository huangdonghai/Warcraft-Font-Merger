#ifdef _MSC_VER
#pragma comment(lib, "shell32")
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <streambuf>
#include <string>
#include <vector>

#include <clipp/clipp.h>
#include <nlohmann/json.hpp>
#include <nowide/args.hpp>
#include <nowide/cstdio.hpp>
#include <nowide/fstream.hpp>
#include <nowide/iostream.hpp>

#include "invisible.hpp"
#include "merge-name.h"
#include "ps2tt.h"
#include "tt2ps.h"

using json = nlohmann::json;

#if __cpp_char8_t >= 201811L

inline auto &operator<<(decltype(nowide::cerr) &os, const char8_t *u8str) {
	return os << reinterpret_cast<const char *>(u8str);
}

#endif

std::string LoadFile(const std::string &u8filename) {
	nowide::ifstream file(u8filename);
	if (!file) {
		nowide::cerr << u8"读取文件 " << u8filename << u8" 失败\n" << std::endl;
		throw std::runtime_error("failed to load file");
	}
	std::string result{std::istreambuf_iterator<char>(file),
	                   std::istreambuf_iterator<char>()};
	return result;
}

bool IsPostScriptOutline(json &font) {
	return font.find("CFF_") != font.end() || font.find("CFF2") != font.end();
}

// x' = a x + b y + dx
// y' = c x + d y + dy
void Transform(json &glyph, double a, double b, double c, double d, double dx,
               double dy) {
	glyph["advanceWidth"] = round(a * double(glyph["advanceWidth"]));
	if (glyph.find("advanceHeight") != glyph.end()) {
		glyph["advanceHeight"] = round(d * double(glyph["advanceHeight"]));
		glyph["verticalOrigin"] = round(d * double(glyph["verticalOrigin"]));
	}
	if (glyph.find("contours") != glyph.end())
		for (auto &contour : glyph["contours"])
			for (auto &point : contour) {
				double x = point["x"];
				double y = point["y"];
				point["x"] = int(a * x + b * y + dx);
				point["y"] = int(c * x + d * y + dy);
			}
	if (glyph.find("references") != glyph.end())
		for (auto &reference : glyph["references"]) {
			double x = reference["x"];
			double y = reference["y"];
			reference["x"] = int(a * x + b * y + dx);
			reference["y"] = int(c * x + d * y + dy);
		}
}

void FixQuota(json& glyph, double upm, double ascender, double dx, double height, double isAdjustUp)
{
    struct Point
    {
		double x, y;
    };

    double ycenter = ascender - 0.5 * upm;
	double ydiff = upm * 0.7 - height;

	if (glyph.find("contours") != glyph.end())
		for (auto &contour : glyph["contours"]) {
			std::vector<Point> points;
			for (auto &point : contour) {
				double x = point["x"];
				double y = point["y"];
				points.push_back({x, y});
			}

            // inverse contour
			for (auto &point : contour) {
				auto p = points.back();
				p.x += dx;
				p.y = ycenter - (p.y - ycenter);

                if (isAdjustUp) {
					if (p.y > ycenter)
						p.y += ydiff;
				} else {
					if (p.y < ycenter)
						p.y -= ydiff;
				}

                point["x"] = p.x;
				point["y"] = p.y;

                points.pop_back();
			}
		}
}

void GetGlyphExtends(json& glyph, double& xmin, double& xmax, double& ymin,
    double& ymax)
{
	xmin = ymin = std::numeric_limits<double>::max();
	xmax = ymax = std::numeric_limits<double>::min();

	if (glyph.find("contours") != glyph.end())
		for (auto &contour : glyph["contours"])
			for (auto &point : contour) {
				double x = point["x"];
				double y = point["y"];
				if (x < xmin)
					xmin = x;
				if (x > xmax)
					xmax = x;
				if (y < ymin)
					ymin = y;
				if (y > ymax)
					ymax = y;
			}
	if (glyph.find("references") != glyph.end())
		for (auto &reference : glyph["references"]) {
			double x = reference["x"];
			double y = reference["y"];
			if (x < xmin)
				xmin = x;
			if (x > xmax)
				xmax = x;
			if (y < ymin)
				ymin = y;
			if (y > ymax)
				ymax = y;
		}
}

// move referenced glyphs recursively
void MoveRef(json &glyph, json &base, json &ext) {
	if (glyph.find("references") != glyph.end())
		for (auto &r : glyph["references"]) {
			std::string name = r["glyph"];
			if (base["glyf"].find(name) == base["glyf"].end()) {
				base["glyf"][name] = std::move(ext["glyf"][name]);
				MoveRef(base["glyf"][name], base, ext);
			}
		}
}

bool IsGidOrCid(const std::string &name) {
	return (name.length() >= 6 && name.substr(0, 5) == "glyph") ||
	       (name.length() >= 4 && name.substr(0, 3) == "cid");
}

void FixGlyphName(json &font, const std::string &prefix) {
	for (auto &[u, n] : font["cmap"].items()) {
		std::string name = n;
		if (IsGidOrCid(name))
			n = prefix + name;
	}
	std::vector<std::string> mod;
	for (auto &[n, g] : font["glyf"].items()) {
		if (IsGidOrCid(n))
			mod.push_back(n);
		if (g.find("references") != g.end()) {
			auto &ref = g["references"];
			for (auto &d : ref)
				if (IsGidOrCid(d["glyph"]))
					d["glyph"] = prefix + std::string(d["glyph"]);
		}
	}
	auto &glyf = font["glyf"];
	for (auto &n : mod) {
		glyf[prefix + n] = glyf[n];
		glyf.erase(n);
	}
}

static std::string jpChars[] = {"20851", "22797", "20011", "183"}; // 关，复, 丫, ·(U+00B7),  force replace

void MergeFont(json &base, json &ext, bool jp=false) {
	double baseUpm = base["head"]["unitsPerEm"];
	double extUpm = ext["head"]["unitsPerEm"];

    // HACK by hdh, alian chinese baseline
	double scale = baseUpm / extUpm;
	double baseDescender = base["OS_2"]["sTypoDescender"];
	double extDescender = ext["OS_2"]["sTypoDescender"];
	double offset = baseDescender - extDescender * scale;
    // END HACK

	if (baseUpm != extUpm || offset != 0) {
		for (auto &glyph : ext["glyf"])
			Transform(glyph, baseUpm / extUpm, 0, 0, baseUpm / extUpm, 0, offset); // HACK by hdh, add offset
	}

	for (json::iterator it = ext["cmap"].begin(); it != ext["cmap"].end();
	     ++it) {
        // HACK by hdh
		bool isForced = false;
		if (jp) {
			for (auto &jpchar : jpChars) {
				if (it.key() == jpchar) {
					isForced = true;
					break;
				}
            }
		}
        // END HACK by hdh
		if (base["cmap"].find(it.key()) == base["cmap"].end() || isForced) {
			std::string name = *it;
			base["cmap"][it.key()] = ext["cmap"][it.key()];
			if (base["glyf"].find(name) == base["glyf"].end() || isForced) {
				base["glyf"][name] = std::move(ext["glyf"][name]);
				MoveRef(base["glyf"][name], base, ext);
			}
		}
	}
}

static std::wstring verticalPunctuations = L"︵︶﹇﹈︷︸︹︺︽︾︿﹀﹁﹂﹃﹄︻︼︗︘";
static std::wstring horizonPunctuations = L"（）［］｛｝〔〕《》〈〉【】〖〗「」『』"; // 
static std::wstring qzw = L"天地玄黃宇宙洪荒日月盈昃辰宿列張寒來暑往秋收冬藏閏餘成歲律呂調陽雲騰致雨露結為霜金生麗水玉出崑崗劍號巨闕珠稱夜光果珍李柰菜重芥薑海鹹河淡鱗潛羽翔龍師火帝鳥官人皇始制文字乃服衣裳推位讓國有虞陶唐弔民伐罪周發殷湯坐朝問道垂拱平章愛育黎首臣伏戎羌遐邇壹體率賓歸王鳴鳳在樹白駒食場化被草木賴及萬方蓋此身髮四大五常恭惟鞠養豈敢毀傷女慕貞絜男效才良知過必改得能莫忘罔談彼短靡恃己長信使可覆器欲難量墨悲絲染詩讚羔羊景行維賢剋念作聖德建名立形端表正空谷傳聲虛堂習聽禍因惡積福緣善慶尺璧非寶寸陰是競資父事君曰嚴與敬孝當竭力忠則盡命臨深履薄夙興溫凊似蘭斯馨如松之盛川流不息淵澄取映容止若思言辭安定篤初誠美慎終宜令榮業所基藉甚無竟學優登仕攝職從政存以甘棠去而益詠樂殊貴賤禮別尊卑上和下睦夫唱婦隨外受傅訓入奉母儀諸姑伯叔猶子比兒孔懷兄弟同氣連枝交友投分切磨箴規仁慈隱惻造次弗離節義廉退顛沛匪虧性靜情逸心動神疲守真志滿逐物意移堅持雅操好爵自縻都邑華夏東西二京背邙面洛浮渭據涇宮殿盤鬱樓觀飛驚圖寫禽獸畫綵仙靈丙舍傍啟甲帳對楹肆筵設席鼓瑟吹笙升階納陛弁轉疑星右通廣內左達承明既集墳典亦聚群英杜稿鍾隸漆書壁經府羅將相路俠槐卿戶封八縣家給千兵高冠陪輦驅轂振纓世祿侈富車駕肥輕策功茂實勒碑刻銘磻溪伊尹佐時阿衡奄宅曲阜微旦孰營桓公匡合濟弱扶傾綺迴漢惠說感武丁俊乂密勿多士寔寧晉楚更霸趙魏困橫假途滅虢踐土會盟何遵約法韓弊煩刑起翦頗牧用軍最精宣威沙漠馳譽丹青九州禹跡百郡秦并嶽宗恒岱禪主云亭雁門紫塞雞田赤城昆池碣石鉅野洞庭曠遠綿邈巖岫杳冥治本於農務茲稼穡俶載南畝我藝黍稷稅熟貢新勸賞黜陟孟軻敦素史魚秉直庶幾中庸勞謙謹敕聆音察理鑒貌辨色貽厥嘉猷勉其祗植省躬譏誡寵增抗極殆辱近恥林皋幸即兩疏見機解組誰逼索居閒處沉默寂寥求古尋論散慮逍遙欣奏累遣慼謝歡招渠荷的歷園莽抽條枇杷晚翠梧桐早凋陳根委翳落葉飄颻遊鵾獨運凌摩絳霄耽讀翫市寓目囊箱易輶攸畏屬耳垣牆具膳餐飯適口充腸飽飫烹宰飢厭糟糠親戚故舊老少異糧妾御績紡侍巾帷房紈扇圓潔銀燭煒煌晝眠夕寐藍筍象床弦歌酒宴接杯舉觴矯手頓足悅豫且康嫡後嗣續祭祀烝嘗稽顙再拜悚懼恐惶牋牒簡要顧答審詳骸垢想浴執熱願涼驢騾犢特駭躍超驤誅斬賊盜捕獲叛亡布射遼丸嵇琴阮嘯恬筆倫紙鈞巧任釣釋紛利俗並皆佳妙毛施淑姿工顰妍笑年矢每催曦暉朗曜璇璣懸斡晦魄環照指薪修祜永綏吉劭矩步引領俯仰廊廟束帶矜莊徘徊瞻眺孤陋寡聞愚蒙等誚謂語助者焉哉乎也";


// HACK hhea for DuoKan Vertical-rl writing mode, simulate 方正新书宋 metrics
void DuokanFixHHEA(json &base)
{
	double baseUpm = base["head"]["unitsPerEm"];
	auto& cmap = base["cmap"];
	double center = 0;
	int count = 0;

	for (auto& ch : qzw)
	{
		auto chname = std::to_string(ch);
		auto it = cmap.find(chname);
		if (it == cmap.end())
			continue;

		auto& glyphName = *it;
		auto glyphIt = base["glyf"].find(glyphName);
		if (glyphIt == base["glyf"].end())
			continue;

		auto& glyph = *glyphIt;
		double xmin, xmax, ymin, ymax;
		GetGlyphExtends(glyph, xmin, xmax, ymin, ymax);
		center += (ymax + ymin) * 0.5;
		count++;
		//printf("ymin: %f, ymax: %f, center: %f\n", ymin, ymax, ((ymax + ymin) * 0.5));
	}
	//printf("count: %d\n", count);
	//printf("center: %f\n", center);
	if (count > 0)
	{
		center /= count;

		const double refLineHeight = 276.0 / 256.0;
		const double refLineGap = 23.0 / 256.0;

		double ascender = center + 0.5 * baseUpm;
		double lineHeight = refLineHeight * baseUpm;
		double lineGap = refLineGap * baseUpm;

		base["hhea"]["ascender"] = ascender;
		base["hhea"]["descender"] = ascender - lineHeight;
		base["hhea"]["lineGap"] = lineGap;
	}
}

void DuokanFixPunc(json &base) {
    double baseUpm = base["head"]["unitsPerEm"];
	double haflUpm = 0.5 * baseUpm;
	double space = round(baseUpm * 60.0 / 1000.0);
	auto& cmap = base["cmap"];
	double Ascender = base["OS_2"]["sTypoAscender"];

#if 1
    for (auto &ch : verticalPunctuations) {
		auto chname = std::to_string(ch);
		auto it = cmap.find(chname);
		if (it == cmap.end())
			continue;

        auto &glyphName = *it;
		auto glyphIt = base["glyf"].find(glyphName);
		if (glyphIt == base["glyf"].end())
			continue;

        auto &glyph = *glyphIt;
		if (glyph.find("advanceHeight") != glyph.end()) {
			if (glyph["advanceHeight"] < baseUpm)
				continue;

			double xmin, xmax, ymin, ymax;
			GetGlyphExtends(glyph, xmin, xmax, ymin, ymax);

            if (ymax - ymin > 0.5 * baseUpm)
				continue;

			glyph["advanceHeight"] = 0.5 * baseUpm;
			double vOrigin = Ascender;
			if (ymax < Ascender - baseUpm * 0.25)
				vOrigin = Ascender - baseUpm * 0.25;
			if (ymax < Ascender - baseUpm * 0.5)
				vOrigin = Ascender - baseUpm * 0.5;

			glyph["verticalOrigin"] = vOrigin;
		}
	}
#endif

    bool isLeft = false;
	for (auto &ch : horizonPunctuations) {
		isLeft = !isLeft;
		auto chname = std::to_string(ch);
		auto it = cmap.find(chname);
		if (it == cmap.end())
			continue;

		auto &glyphName = *it;
		auto glyphIt = base["glyf"].find(glyphName);
		if (glyphIt == base["glyf"].end())
			continue;

		auto &glyph = *glyphIt;
		if (glyph.find("advanceWidth") != glyph.end()) {
            // already fixed
			if (glyph["advanceWidth"] < baseUpm)
                continue;

			double xmin, xmax, ymin, ymax;
			GetGlyphExtends(glyph, xmin, xmax, ymin, ymax);

			double width = xmax - xmin;
            double offset = 0;
#if 0
			if (xmin > 0.25 * baseUpm)
				offset = 0.25 * baseUpm;
			if (xmin > 0.5 * baseUpm)
				offset = 0.5 * baseUpm;
#else
			double curSpace = space;
			double curWidth = 0;
			if (isLeft) {
				if (baseUpm - xmax < space)
					curSpace = baseUpm - xmax;
			} else {
				if (xmin < space)
					curSpace = xmin;
            }
#endif
			curWidth = curSpace * 2 + width;
			if (curWidth > baseUpm)
				continue; // too large, don't fix

            if (curWidth < haflUpm)
				curWidth = haflUpm;

            if (isLeft) {
				offset = xmin - (curWidth - width - curSpace);
			} else {
				offset = xmin - curSpace;
			}


			glyph["advanceWidth"] = curWidth;

            if (ch == L'「' || ch == L'『') {
				FixQuota(glyph, baseUpm, Ascender, -offset, ymax - ymin, true);
				continue;
            }

            if (ch == L'」' || ch == L'』') {
				FixQuota(glyph, baseUpm, Ascender, -offset, ymax - ymin, false);
				continue;
			}

            if (offset != 0) {
			    Transform(glyph, 1, 0, 0, 1, -offset, 0); // HACK by hdh, add offset
            }
		}
	}
}

void DuokanFix(json& base)
{
	DuokanFixHHEA(base);
	DuokanFixPunc(base);
}

void RemoveBlankGlyph(json &font) {
	static UnicodeInvisible invisible;
	std::vector<std::string> eraseList;

	for (json::iterator it = font["cmap"].begin(); it != font["cmap"].end();
	     ++it) {
		if (!invisible.CanBeInvisible(std::stoi(it.key()))) {
			std::string name = it.value();
			auto &glyph = font["glyf"][name];
			if (glyph.find("contours") == glyph.end() &&
			    glyph.find("references") == glyph.end())
				eraseList.push_back(it.key());
		}
	}

	for (auto g : eraseList) {
		std::string name = font["cmap"][g];
		font["cmap"].erase(g);
		if (std::find_if(font["cmap"].begin(), font["cmap"].end(),
		                 [name](auto v) { return v == name; }) ==
		    font["cmap"].end())
			font["glyf"].erase(name);
	}
}

json MergeCodePage(std::vector<json> cpranges) {
	json result = json::object();

	for (auto &cprange : cpranges)
		for (auto &[k, v] : cprange.items()) {
			// std::cerr << cprange << ' ' << k << std::endl;
			if (result.find(k) != result.end())
				result[k] = result[k] || v;
			else
				result[k] = v;
		}

	return result;
}

int main(int argc, char *u8argv[]) {
	nowide::args _{argc, u8argv};

	std::string outputPath;

	std::string overrideNameStyle;

	std::string baseFileName;
	std::vector<std::string> appendFileNames;

    bool isJp = false;
	bool duokanFix = false;

	auto cli = ({
		using namespace clipp;
		((option("-o", "--output") & value("out.otd", outputPath)) %
		     "输出 otd 文件路径。如果不指定，则覆盖第一个输入 otd 文件。",
		 (option("-n", "--name") &
		  value("Font Name;Weight;Width;Slope", overrideNameStyle)) %
		     R"+(指定字体家族名和样式。如果不指定，则自动根据原字体名合成新的字体名。
格式："字体名;字重;宽度;倾斜"
字重的取值范围
　数字：100 至 950 之间的整数（含两端点）。
　下列单词（不区分大小写，忽略连字符；括号内的单词视作左边单词的同义词）：
　　Thin       = 100（UltraLight）
　　ExtraLight = 200
　　Light      = 300
　　SemiLight  = 350（DemiLight）
　　Normal     = 372
　　Regular    = 400（Roman、""）
　　Book       = 450
　　Medium     = 500
　　SemiBold   = 600（Demi、DemiBold）
　　Bold       = 700
　　ExtraBold  = 800
　　Black      = 900（Heavy、UltraBold）
　　ExtraBlack = 950
宽度的取值范围
　数字：1 至 9 之间的整数（含两端点）。
　下列单词（不区分大小写，忽略连字符；括号内的单词视作左边单词的同义词）：
　　UltraCondensed = 1
　　ExtraCondensed = 2
　　Condensed      = 3
　　SemiCondensed  = 4
　　Normal         = 5（""）
　　SemiExtended   = 6（SemiExpanded）
　　Extended       = 7（Expanded）
　　ExtraExtended  = 8（ExtraExpanded）
　　UltraExtended  = 9（UltraExpanded）
倾斜的取值范围
　下列单词（不区分大小写；括号内的单词视作左边单词的同义词）：
　　Upright（Normal、Roman、Unslanted、""）
　　Italic （Italized）
　　Oblique（Slant）)+",
		 option("-jp").set(isJp).doc("target is japanese font"),
		 option("-dk").set(duokanFix).doc("duokan fix"),
		 values("base.otd", appendFileNames));
	});
	if (!clipp::parse(argc, u8argv, cli) || appendFileNames.empty()) {
		nowide::cout << clipp::make_man_page(cli, "merge-otd") << std::endl;
		return EXIT_FAILURE;
	}

    baseFileName = appendFileNames[0];
	appendFileNames.erase(appendFileNames.begin());

	std::vector<json> ulCodePageRanges1, ulCodePageRanges2;
	std::vector<json> nametables;

	json base;
	bool basecff;
	try {
		auto s = LoadFile(baseFileName);
		base = json::parse(s);
	} catch (const std::runtime_error &) {
		return EXIT_FAILURE;
	}
	basecff = IsPostScriptOutline(base);
	RemoveBlankGlyph(base);
	nametables.push_back(base["name"]);

	for (const std::string &name : appendFileNames) {
		json ext;
		try {
			auto s = LoadFile(name);
			ext = json::parse(s);
		} catch (std::runtime_error &) {
			return EXIT_FAILURE;
		}
		bool extcff = IsPostScriptOutline(ext);
		if (basecff && !extcff) {
			ext["glyf"] = Tt2Ps(ext["glyf"]);
		} else if (!basecff && extcff) {
			ext["glyf"] = Ps2Tt(ext["glyf"]);
		}
		RemoveBlankGlyph(ext);
		nametables.push_back(ext["name"]);
		FixGlyphName(ext, name + std::string(":"));
		MergeFont(base, ext, isJp);
		if (ext.find("OS_2") != ext.end()) {
			auto &OS_2 = ext["OS_2"];
			if (OS_2.find("ulCodePageRange1") != OS_2.end())
				ulCodePageRanges1.push_back(OS_2["ulCodePageRange1"]);
			if (OS_2.find("ulCodePageRange2") != OS_2.end())
				ulCodePageRanges2.push_back(OS_2["ulCodePageRange2"]);
		}
	}

    if (duokanFix) {
		DuokanFix(base);
    }

	if (base.find("OS_2") != base.end()) {
		auto &OS_2 = base["OS_2"];
		if (OS_2.find("ulCodePageRange1") != OS_2.end())
			ulCodePageRanges1.push_back(OS_2["ulCodePageRange1"]);
		if (OS_2.find("ulCodePageRange2") != OS_2.end())
			ulCodePageRanges2.push_back(OS_2["ulCodePageRange2"]);

		OS_2["ulCodePageRange1"] = MergeCodePage(ulCodePageRanges1);
		OS_2["ulCodePageRange2"] = MergeCodePage(ulCodePageRanges2);
	}

	MergeNameTable(nametables, base, overrideNameStyle);

	std::string out = base.dump();
	FILE *outfile = nowide::fopen(
	    outputPath.empty() ? baseFileName.c_str() : outputPath.c_str(), "wb");
	fwrite(out.c_str(), 1, out.size(), outfile);
	return 0;
}
