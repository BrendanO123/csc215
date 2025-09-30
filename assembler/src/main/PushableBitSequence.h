#pragma once

#include <regex>
#include <optional>
#include <sstream>

using namespace std;

struct pushableBitSequence{
    const char length;
    unsigned char data;
    optional<unsigned char> second;

    pushableBitSequence(int len, int num, bool hasSecond = false) : 
        length((char)len), data((unsigned char)num){if(hasSecond){second.emplace(num >> 8);}}
    
    pushableBitSequence(char len, int num, bool hasSecond = false) : 
        length(len), data((char)num){if(hasSecond){second.emplace(num >> 8);}}

    pushableBitSequence(char len, unsigned char num, bool hasSecond = false) : 
        length(len), data(num){if(hasSecond){second.emplace(0);}}

    pushableBitSequence(int len, unsigned char num, bool hasSecond = false) : 
        length((char)len), data(num){if(hasSecond){second.emplace(0);}}

    int getValue() const{return int(data) + (second.has_value() ? int(second.value()) << 8 : 0);}

    bool operator==(const pushableBitSequence other) const{
        return other.length == length &&
            other.data == data &&
            other.second.has_value() == second.has_value() &&
                (second.has_value() ? other.second.value() == second.value() : true);
    }
    string to_string(){
        stringstream ss;
        ss << "Length: " << int(length) << "\nData: " << (unsigned int)(data) << "\nSecond: ";
        if(second.has_value()){ss << (unsigned int)(second.value()) << endl;}
        else{ss << "EMPTY" << endl;}
        return ss.str();
    }
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
        pushableBitSequenceTemplate(getIntBool, getStringBool, regex("[tfTF]")),
        pushableBitSequenceTemplate(getIntRP, getStringRP, regex("$a")),
        pushableBitSequenceTemplate(getIntReg, getStringReg, regex("[rR]?[a-eA-EhHlLmM]")),
        pushableBitSequenceTemplate(getIntByte, getStringByte, regex("-?[0-9]+")),
        pushableBitSequenceTemplate(getIntBP, getStringBP, regex("(([rR][0-9]+)|(-?[0-9]+.[0-9]+))"))
    };

    pushableBitSequence tryGetLiteral(string token);
}