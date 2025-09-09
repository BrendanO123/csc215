#pragma once

#include <regex>
#include <optional>

using namespace std;

struct pushableBitSequence{
    const unsigned char length;
    unsigned char data;
    optional<unsigned char> second;

    pushableBitSequence(int len, int num) : 
        length((unsigned char)len), data((unsigned char)num){}

    pushableBitSequence(char len, char num) : 
        length((unsigned char)len), data((unsigned char)num){}

    pushableBitSequence(unsigned char len, unsigned char num) : 
        length(len), data(num){}

    pushableBitSequence(int len, unsigned char num) : 
        length((unsigned char)len), data(num){}
};
struct pushableBitSequenceTemplate{
    const regex matcher;
    function<pushableBitSequence(string token)> stringInitializer;
    function<pushableBitSequence(int value)> intInitializer;

    bool isType(string token){return regex_match(token, matcher);}

    pushableBitSequenceTemplate(
        function<pushableBitSequence(int value)> intInit, 
        function<pushableBitSequence(string token)> stringInit,
        regex match
    ) :
        matcher(match), 
        stringInitializer(stringInit), 
        intInitializer(intInit){}
};

namespace pushableBitSequenceTemplates{
    enum pushableBitSequenceTemplateTypes : unsigned char{
        BOOL = 0,
        REGISTER_PAIR = 1,
        REGISTER = 2,
        BYTE = 3,
        BYTE_PAIR = 4,
        OP_CODE = 5
    };

    pushableBitSequence getIntBool(int value);
    pushableBitSequence getIntRP(int value);
    pushableBitSequence getIntReg(int value);
    pushableBitSequence getIntByte(int value);
    pushableBitSequence getIntBP(int value);

    pushableBitSequence getStringBool(string token);
    pushableBitSequence getStringRP(string token);
    pushableBitSequence getStringReg(string token);
    pushableBitSequence getStringByte(string token);
    pushableBitSequence getStringBP(string token);

    const pushableBitSequenceTemplate pushableBitSequenceTemplates[] = {
        pushableBitSequenceTemplate(getIntBool, getStringBool, regex("[tfTF][a-zA-Z]*")),
        pushableBitSequenceTemplate(getIntRP, getStringRP, regex("$a")),
        pushableBitSequenceTemplate(getIntReg, getStringReg, regex("[rR]?[a-eA-EhHlLmM]")),
        pushableBitSequenceTemplate(getIntByte, getStringByte, regex("-?[0-9]+")),
        pushableBitSequenceTemplate(getIntBP, getStringBP, regex("[rR][0-9]+"))
    };
}