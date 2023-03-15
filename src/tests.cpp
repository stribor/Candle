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
