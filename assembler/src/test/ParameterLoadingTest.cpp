#include <doctest.h>

#include "TokenProcessor.h"

using namespace std;

TEST_CASE("Test get Boolean"){
    TokenProcessor processor = TokenProcessor();

    // this long check
    CHECK(processor.tryGetParameter("TRUE").length == 1);
    CHECK(processor.tryGetParameter("TRUE").data == 1);
    CHECK_FALSE(processor.tryGetParameter("TRUE").second.has_value());

    // is the same as this short version
    CHECK_EQ(processor.tryGetParameter("TRUE"), pushableBitSequence(1, 1, false));

    CHECK_EQ(processor.tryGetParameter("True"), pushableBitSequence(1, 1, false));
    CHECK_EQ(processor.tryGetParameter("true"), pushableBitSequence(1, 1, false));
    CHECK_EQ(processor.tryGetParameter("T"), pushableBitSequence(1, 1, false));
    CHECK_EQ(processor.tryGetParameter("t"), pushableBitSequence(1, 1, false));

    CHECK_EQ(processor.tryGetParameter("FALSE"), pushableBitSequence(1, 0, false));
    CHECK_EQ(processor.tryGetParameter("False"), pushableBitSequence(1, 0, false));
    CHECK_EQ(processor.tryGetParameter("false"), pushableBitSequence(1, 0, false));
    CHECK_EQ(processor.tryGetParameter("F"), pushableBitSequence(1, 0, false));
    CHECK_EQ(processor.tryGetParameter("f"), pushableBitSequence(1, 0, false));
}

TEST_CASE("Test get Register Pair"){
    TokenProcessor processor = TokenProcessor();
    CHECK_EQ(processor.tryGetParameter("bc"), pushableBitSequence(2, 0, false));
    CHECK_EQ(processor.tryGetParameter("BC"), pushableBitSequence(2, 0, false));
    CHECK_EQ(processor.tryGetParameter("rp_bc"), pushableBitSequence(2, 0, false));
    CHECK_EQ(processor.tryGetParameter("rp_BC"), pushableBitSequence(2, 0, false));
    CHECK_EQ(processor.tryGetParameter("RP_bc"), pushableBitSequence(2, 0, false));
    CHECK_EQ(processor.tryGetParameter("RP_BC"), pushableBitSequence(2, 0, false));

    CHECK_EQ(processor.tryGetParameter("de"), pushableBitSequence(2, 1, false));
    CHECK_EQ(processor.tryGetParameter("DE"), pushableBitSequence(2, 1, false));
    CHECK_EQ(processor.tryGetParameter("rp_de"), pushableBitSequence(2, 1, false));
    CHECK_EQ(processor.tryGetParameter("rp_DE"), pushableBitSequence(2, 1, false));
    CHECK_EQ(processor.tryGetParameter("RP_de"), pushableBitSequence(2, 1, false));
    CHECK_EQ(processor.tryGetParameter("RP_DE"), pushableBitSequence(2, 1, false));

    CHECK_EQ(processor.tryGetParameter("hl"), pushableBitSequence(2, 2, false));
    CHECK_EQ(processor.tryGetParameter("HL"), pushableBitSequence(2, 2, false));
    CHECK_EQ(processor.tryGetParameter("rp_hl"), pushableBitSequence(2, 2, false));
    CHECK_EQ(processor.tryGetParameter("rp_HL"), pushableBitSequence(2, 2, false));
    CHECK_EQ(processor.tryGetParameter("RP_hl"), pushableBitSequence(2, 2, false));
    CHECK_EQ(processor.tryGetParameter("RP_HL"), pushableBitSequence(2, 2, false));

    CHECK_EQ(processor.tryGetParameter("fa"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("FA"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("rp_fa"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("rp_FA"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("RP_fa"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("RP_FA"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("FlagsA"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("rp_FlagsA"), pushableBitSequence(2, 3, false));
    CHECK_EQ(processor.tryGetParameter("RP_FlagsA"), pushableBitSequence(2, 3, false));
}

TEST_CASE("Test get Single Register"){
    TokenProcessor processor = TokenProcessor();
    CHECK_EQ(processor.tryGetParameter("b"), pushableBitSequence(3, 0, false));
    CHECK_EQ(processor.tryGetParameter("B"), pushableBitSequence(3, 0, false));
    CHECK_EQ(processor.tryGetParameter("rb"), pushableBitSequence(3, 0, false));
    CHECK_EQ(processor.tryGetParameter("rB"), pushableBitSequence(3, 0, false));
    CHECK_EQ(processor.tryGetParameter("Rb"), pushableBitSequence(3, 0, false));
    CHECK_EQ(processor.tryGetParameter("RB"), pushableBitSequence(3, 0, false));

    CHECK_EQ(processor.tryGetParameter("c"), pushableBitSequence(3, 1, false));
    CHECK_EQ(processor.tryGetParameter("C"), pushableBitSequence(3, 1, false));
    CHECK_EQ(processor.tryGetParameter("rc"), pushableBitSequence(3, 1, false));
    CHECK_EQ(processor.tryGetParameter("rC"), pushableBitSequence(3, 1, false));
    CHECK_EQ(processor.tryGetParameter("Rc"), pushableBitSequence(3, 1, false));
    CHECK_EQ(processor.tryGetParameter("RC"), pushableBitSequence(3, 1, false));

    CHECK_EQ(processor.tryGetParameter("d"), pushableBitSequence(3, 2, false));
    CHECK_EQ(processor.tryGetParameter("D"), pushableBitSequence(3, 2, false));
    CHECK_EQ(processor.tryGetParameter("rd"), pushableBitSequence(3, 2, false));
    CHECK_EQ(processor.tryGetParameter("rD"), pushableBitSequence(3, 2, false));
    CHECK_EQ(processor.tryGetParameter("Rd"), pushableBitSequence(3, 2, false));
    CHECK_EQ(processor.tryGetParameter("RD"), pushableBitSequence(3, 2, false));

    CHECK_EQ(processor.tryGetParameter("e"), pushableBitSequence(3, 3, false));
    CHECK_EQ(processor.tryGetParameter("E"), pushableBitSequence(3, 3, false));
    CHECK_EQ(processor.tryGetParameter("re"), pushableBitSequence(3, 3, false));
    CHECK_EQ(processor.tryGetParameter("rE"), pushableBitSequence(3, 3, false));
    CHECK_EQ(processor.tryGetParameter("Re"), pushableBitSequence(3, 3, false));
    CHECK_EQ(processor.tryGetParameter("RE"), pushableBitSequence(3, 3, false));

    CHECK_EQ(processor.tryGetParameter("h"), pushableBitSequence(3, 4, false));
    CHECK_EQ(processor.tryGetParameter("H"), pushableBitSequence(3, 4, false));
    CHECK_EQ(processor.tryGetParameter("rh"), pushableBitSequence(3, 4, false));
    CHECK_EQ(processor.tryGetParameter("rH"), pushableBitSequence(3, 4, false));
    CHECK_EQ(processor.tryGetParameter("Rh"), pushableBitSequence(3, 4, false));
    CHECK_EQ(processor.tryGetParameter("RH"), pushableBitSequence(3, 4, false));

    CHECK_EQ(processor.tryGetParameter("l"), pushableBitSequence(3, 5, false));
    CHECK_EQ(processor.tryGetParameter("L"), pushableBitSequence(3, 5, false));
    CHECK_EQ(processor.tryGetParameter("rl"), pushableBitSequence(3, 5, false));
    CHECK_EQ(processor.tryGetParameter("rL"), pushableBitSequence(3, 5, false));
    CHECK_EQ(processor.tryGetParameter("Rl"), pushableBitSequence(3, 5, false));
    CHECK_EQ(processor.tryGetParameter("RL"), pushableBitSequence(3, 5, false));

    CHECK_EQ(processor.tryGetParameter("m"), pushableBitSequence(3, 6, false));
    CHECK_EQ(processor.tryGetParameter("M"), pushableBitSequence(3, 6, false));
    CHECK_EQ(processor.tryGetParameter("rm"), pushableBitSequence(3, 6, false));
    CHECK_EQ(processor.tryGetParameter("rM"), pushableBitSequence(3, 6, false));
    CHECK_EQ(processor.tryGetParameter("Rm"), pushableBitSequence(3, 6, false));
    CHECK_EQ(processor.tryGetParameter("RM"), pushableBitSequence(3, 6, false));

    CHECK_EQ(processor.tryGetParameter("a"), pushableBitSequence(3, 7, false));
    CHECK_EQ(processor.tryGetParameter("A"), pushableBitSequence(3, 7, false));
    CHECK_EQ(processor.tryGetParameter("ra"), pushableBitSequence(3, 7, false));
    CHECK_EQ(processor.tryGetParameter("rA"), pushableBitSequence(3, 7, false));
    CHECK_EQ(processor.tryGetParameter("Ra"), pushableBitSequence(3, 7, false));
    CHECK_EQ(processor.tryGetParameter("RA"), pushableBitSequence(3, 7, false));
}

TEST_CASE("Test get Single Byte"){
    TokenProcessor processor = TokenProcessor();
    CHECK_EQ(processor.tryGetParameter("0"), pushableBitSequence(8, 0, false));
    CHECK_EQ(processor.tryGetParameter("8"), pushableBitSequence(8, 8, false));
    CHECK_EQ(processor.tryGetParameter("-1"), pushableBitSequence(8, 255, false));
    CHECK_EQ(processor.tryGetParameter("-10"), pushableBitSequence(8, 246, false));
}

TEST_CASE("Test get Byte Pair"){
    TokenProcessor processor = TokenProcessor();
    CHECK(processor.tryGetParameter("r8").length == 8);
    CHECK(processor.tryGetParameter("r8").data == 8);
    CHECK(processor.tryGetParameter("r8").second.has_value());
    CHECK(processor.tryGetParameter("r8").second.value() == 0);
    CHECK_EQ(processor.tryGetParameter("r8"), pushableBitSequence(8, 8, true));

    CHECK(processor.tryGetParameter("r257").length == 8);
    CHECK(processor.tryGetParameter("r257").data == 1);
    CHECK(processor.tryGetParameter("r257").second.has_value());
    CHECK(processor.tryGetParameter("r257").second.value() == 1);
    CHECK_EQ(processor.tryGetParameter("r257"), pushableBitSequence(8, 257, true));

    CHECK(processor.tryGetParameter("1.5").length == 8);
    CHECK(processor.tryGetParameter("1.5").data == 128);
    CHECK(processor.tryGetParameter("1.5").second.has_value());
    CHECK(processor.tryGetParameter("1.5").second.value() == 1);
    CHECK_EQ(processor.tryGetParameter("1.5"), pushableBitSequence(8, 128 + 256, true));

    CHECK(processor.tryGetParameter("-0.75").length == 8);
    CHECK(processor.tryGetParameter("-0.75").data == 64);
    CHECK(processor.tryGetParameter("-0.75").second.has_value());
    CHECK(processor.tryGetParameter("-0.75").second.value() == 255);
    CHECK_EQ(processor.tryGetParameter("-0.75"), pushableBitSequence(8, 255 * 256 + 64, true));
}