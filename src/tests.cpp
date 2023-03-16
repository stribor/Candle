#include <catch2/catch_test_macros.hpp>

#include "parser/gcodepreprocessorutils.h"

TEST_CASE("removeComment", "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::removeComment("G00 X0 Y0 Z0\n") == "G00X0Y0Z0");
    REQUIRE(GcodePreprocessorUtils::removeComment("G00 X (comment) 0 Y0 Z0 (comment) (another comment) ;end of line comment \n") == "G00X0Y0Z0");
}

TEST_CASE("truncateDecimals" , "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::truncateDecimals(1, "G0 X123.4456 Y3.12345 Z0.5") == "G0 X123.4 Y3.1 Z0.5");
    REQUIRE(GcodePreprocessorUtils::truncateDecimals(2, "G0 X123.4456 Y3.12345 Z0.5") == "G0 X123.45 Y3.12 Z0.50");
    REQUIRE(GcodePreprocessorUtils::truncateDecimals(3, "G0 X123.4456 Y3.12345 Z0.5") == "G0 X123.446 Y3.123 Z0.500");
}

TEST_CASE("AtoF", "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::AtoF("123.456") == 123.456);
    REQUIRE(GcodePreprocessorUtils::AtoF("  123.456") == 123.456);
    REQUIRE(GcodePreprocessorUtils::AtoF("  123.456    ") == 123.456);
    REQUIRE(GcodePreprocessorUtils::AtoF(" 1 123.456") == 1.0);
    REQUIRE(GcodePreprocessorUtils::AtoF("") == 0.);
    REQUIRE(GcodePreprocessorUtils::AtoF(" ") == 0.);
    REQUIRE(GcodePreprocessorUtils::AtoF("12a123") == 12.);
    REQUIRE(GcodePreprocessorUtils::AtoF("+12a123") == 12.);
    REQUIRE(GcodePreprocessorUtils::AtoF("-12a123") == -12.);
}

TEST_CASE("parseComment", "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::parseComment("G1 X10 (test comment 1)") == "test comment 1");
    REQUIRE(GcodePreprocessorUtils::parseComment("G1 X10 ((test comment 1))") == "(test comment 1");
    REQUIRE(GcodePreprocessorUtils::parseComment("G1 X10 ;test comment 2") == "test comment 2");
    REQUIRE(GcodePreprocessorUtils::parseComment("G1 X10 ;;test comment 2") == ";test comment 2");
    REQUIRE(GcodePreprocessorUtils::parseComment("G0 X123.4456 Y3.12345 Z0.5") == "");
}

TEST_CASE("overrideSpeed", "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::overrideSpeed("G0 X0 Y10 Z200 F123", 50) == "G0 X0 Y10 Z200 F61.50");
    REQUIRE(GcodePreprocessorUtils::overrideSpeed("G0 X0 Y10 Z200 F123 G0 X0 Y10 Z200 F233", 50) == "G0 X0 Y10 Z200 F61.50 G0 X0 Y10 Z200 F116.50");
    REQUIRE(GcodePreprocessorUtils::overrideSpeed("G00 X0 Y0 Z0\n", 10) == "G00 X0 Y0 Z0\n");
}

TEST_CASE("parseGCodeEnum", "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G0") == GCodes::G00);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G00") == GCodes::G00);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G1") == GCodes::G01);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G01") == GCodes::G01);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G2") == GCodes::G02);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G02") == GCodes::G02);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G3") == GCodes::G03);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G03") == GCodes::G03);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G5") == GCodes::G05);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G05") == GCodes::G05);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G5.1") == GCodes::G05_1);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G05.1") == GCodes::G05_1);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G5.2") == GCodes::G05_2);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G05.2") == GCodes::G05_2);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G7") == GCodes::G07);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G07") == GCodes::G07);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G8") == GCodes::G08);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G08") == GCodes::G08);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G90") == GCodes::G90);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G90.1") == GCodes::G90_1);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G91") == GCodes::G91);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G91.1") == GCodes::G91_1);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G17") == GCodes::G17);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G18") == GCodes::G18);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G19") == GCodes::G19);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G20") == GCodes::G20);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G21") == GCodes::G21);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G38.2") == GCodes::G38_2);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G38.3") == GCodes::G38_3);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G38.4") == GCodes::G38_4);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G38.5") == GCodes::G38_5);

    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("") == GCodes::unknown);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G4") == GCodes::unknown);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G04") == GCodes::unknown);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G6") == GCodes::unknown);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G06") == GCodes::unknown);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G90.3") == GCodes::unknown);
    REQUIRE(GcodePreprocessorUtils::parseGCodeEnum("G90.4") == GCodes::unknown);
}

TEST_CASE("splitCommand", "[GcodePreprocessorUtils]")
{
    REQUIRE(GcodePreprocessorUtils::splitCommand("G0 X0 Y0 Z0") == std::vector<std::string>{"G0", "X0", "Y0", "Z0"});
    REQUIRE(GcodePreprocessorUtils::splitCommand("G 0 X1.0 Y0 Z2.0") == std::vector<std::string>{"G0", "X1.0", "Y0", "Z2.0"});
    REQUIRE(GcodePreprocessorUtils::splitCommand("   G01 (comment   )X0Y0Z0") == std::vector<std::string>{"G01", "X0", "Y0", "Z0"});
    REQUIRE(GcodePreprocessorUtils::splitCommand("G0 X0 Y10 Z200 F123 G0 X0 Y10 Z200 F233") == std::vector<std::string>{"G0", "X0", "Y10", "Z200", "F123", "G0", "X0", "Y10", "Z200", "F233"});
    REQUIRE(GcodePreprocessorUtils::splitCommand("GF0") == std::vector<std::string>{"G", "F0"});
    REQUIRE(GcodePreprocessorUtils::splitCommand("[GC:G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 S0.0 F500.0]") == std::vector<std::string> { "GC:", "G0", "G54", "G17", "G21", "G90", "G94", "M0", "M5", "M9", "T0",
                                                                                                                                    "S0.0", "F500.0" });
    REQUIRE(GcodePreprocessorUtils::splitCommand("$G") == std::vector<std::string>{"$G"});
    REQUIRE(GcodePreprocessorUtils::splitCommand("X0.5 Y0. I0. J-0.5") == std::vector<std::string>{"X0.5", "Y0.", "I0.", "J-0.5"});
}
