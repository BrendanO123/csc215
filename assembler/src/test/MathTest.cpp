#include <doctest.h>

#include "TokenProcessor.h"

using namespace std;

TEST_CASE("Test Addition"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "1", "+", "r128", 
        "r512", "+", "3",
        "7.5", "+", "0.75", 
        "r4", "+", "r1", 
        "255", "+", "1"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 129, false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 515, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 8 * 256 + 64, true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 5, true)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 0, false)); CHECK_EQ(index, 15);
}

TEST_CASE("Test Subtraction"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "12", "-", "r14", 
        "3.0", "-", "0.5", 
        "r4", "-", "r1", 
        "0", "-", "1",
        "r258", "-", "1"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, (unsigned char)(12-14), false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 512 + 128, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 3, true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 255, false)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 257, true)); CHECK_EQ(index, 15);
}

TEST_CASE("Test Multiplication"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "7", "*", "r128", 
        "r512", "*", "3",
        "7.5", "*", "3", 
        "r4", "*", "r9", 
        "64", "*", "5",
        "3", "*", "-5"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, (unsigned char)(7 * 128), false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 512 * 3, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 3 * (7 * 256 + 128), true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 36, true)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 64, false)); CHECK_EQ(index, 15);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, -15, false)); CHECK_EQ(index, 18);
}

TEST_CASE("Test Logical AND"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "5", "&", "r3", 
        "r15", "&", "9",
        "7.5", "&", "255", 
        "r4", "&", "r6", 
        "15", "&", "RA"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 1, false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 9, false)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 128, false)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 4, true)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(3, 7, false)); CHECK_EQ(index, 15);
}

TEST_CASE("Test Left Shift"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "5", "<<", "r3", 
        "r15", "<<", "4",
        "7.5", "<<", "4", 
        "r4", "<<", "r6", 
        "1", "<<", "RA"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 40, false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 15 * 16, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 16 * (7 * 256 + 128), true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 4 * 64, true)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 128, false)); CHECK_EQ(index, 15);
}

TEST_CASE("Test Right Shift"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "5", ">>", "r1", 
        "r63", ">>", "4",
        "7.5", ">>", "4", 
        "r9", ">>", "r2", 
        "128", ">>", "RA"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 2, false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 3, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, (7 * 16 + 8), true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 2, true)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 1, false)); CHECK_EQ(index, 15);
}

TEST_CASE("Test Integer Division"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "9", "//", "r5", 
        "r512", "//", "5",
        "7.5", "//", "8",
        "r9", "//", "r2", 
        "128", "//", "-2"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 1, false)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 102, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, (7 * 32 + 16), true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 4, true)); CHECK_EQ(index, 12);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, -64, false)); CHECK_EQ(index, 15);
}

TEST_CASE("Test Double Division"){
    TokenProcessor processor = TokenProcessor();
    vector<string> line = vector<string>({
        "9", "/", "r4", 
        "r512", "/", "8",
        "r9", "/", "r2", 
        "128", "/", "-2"
    });

    int index = 0;
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 2 * 256 + 64, true)); CHECK_EQ(index, 3);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 64 * 256, true)); CHECK_EQ(index, 6);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, 4 * 256 + 128, true)); CHECK_EQ(index, 9);
    CHECK_EQ(processor.tryGetParameter(line, index), pushableBitSequence(8, -64 * 256, true)); CHECK_EQ(index, 12);
}