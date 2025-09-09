#pragma once

#include <regex>
#include <optional>

struct pushableBitSequence{
    const unsigned char length;
    unsigned char data;
    optional<Byte> second;

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
    const bool singleByte;

    const unsigned char length;

    const regex matcher;
    function<pushableBitSequence(string token)> stringInitializer;
    function<pushableBitSequence(int value)> intInitializer;

    bool isType(string token){return regex_match(token, matcher);}

    pushableBitSequenceTemplate(
        int len, 
        function<pushableBitSequence(int value)> intInit, 
        function<pushableBitSequence(string token)> stringInit,
        regex match,
        bool multiByte = false
    ) :
        singleByte(!multiByte), 
        length(len), 
        matcher(match), 
        stringInitializer(stringInit), 
        intInitializer(intInit){}

    pushableBitSequenceTemplate(
        int len, 
        function<pushableBitSequence(int value)> intInit, 
        function<pushableBitSequence(string token)> stringInit,
        regex match,
        bool multiByte = false
    ) :
        singleByte(!multiByte), 
        length(len), 
        matcher(match), 
        stringInitializer(stringInit), 
        intInitializer(intInit){}
};

namespace pushableBitSequenceTemplates{
    enum pushableBitSequenceTemplateTypes : unsigned char{
        BOOL = 1,
        REGISTER_PAIR = 2,
        REGISTER = 3,
        BYTE = 4,
        BYTE_PAIR = 5,
        OP_CODE = 6
    };
    const pushableBitSequenceTemplate pushableBitSequenceTemplates[] = {
        pushableBitSequenceTemplate(1, getIntBool, getStringBool, regex("[tfTF][a-zA-Z]*")),
        pushableBitSequenceTemplate(2, getIntRP, getStringRP, regex("([rR][pP]_)?[a-zA-Z]+")),
        pushableBitSequenceTemplate(3, getIntReg, getStringReg, regex("[rR]?[a-eA-EhHlLmM]")),
        pushableBitSequenceTemplate(8, getIntByte, getStringByte, regex("-?[0-9]+")),
        pushableBitSequenceTemplate(8, getIntBP, getStringBP, regex("[rR][0-9]+"), true)
    };

    pushableBitSequence getIntBool(int value){return pushableBitSequence(1, (unsigned char)value);}
    pushableBitSequence getIntRP(int value){return pushableBitSequence(2, (unsigned char)value);}
    pushableBitSequence getIntReg(int value){return pushableBitSequence(3, (unsigned char)value);}
    pushableBitSequence getIntByte(int value){return pushableBitSequence(8, (unsigned char)value);}
    pushableBitSequence getIntBP(int value){
        auto e = pushableBitSequence(8, (unsigned char)value);
        e.second.emplace(Byte(value>>8));
        return e;
    }

    pushableBitSequence getStringBool(string token){return pushableBitSequence(-1, 0);}
    pushableBitSequence getStringRP(string token){return pushableBitSequence(-1, 0);}
    pushableBitSequence getStringReg(string token){
        switch(tolower(token.at(token.size()-1))){
            case 'b':
                return pushableBitSequence(3, 0);
            case 'c':
                return pushableBitSequence(3, 1);
            case 'd':
                return pushableBitSequence(3, 2);
            case 'e':
                return pushableBitSequence(3, 3);
            case 'h':
                return pushableBitSequence(3, 4);
            case 'l':
                return pushableBitSequence(3, 5);
            case 'm':
                return pushableBitSequence(3, 6);
            case 'a':
                return pushableBitSequence(3, 7);
            default:
                return pushableBitSequence(3, 7);
        }
    }
    pushableBitSequence getStringByte(string token){return pushableBitSequence(8, (unsigned char)stoi(token, nullptr));}
    pushableBitSequence getStringBP(string token){
        int value = stoi(token, nullptr);
        auto e = pushableBitSequence(8, (unsigned char)value);
        e.second.emplace(Byte(value>>8));
        return e;
    }
}

struct Byte : pushableBitSequence{
    static const regex matcher;

    Byte(int value) : pushableBitSequence(8, (unsigned char)value){}
    Byte(string token) : pushableBitSequence(8, 0){data = stoi(token, nullptr);}

    pushableBitSequence getInstance(int value){return Byte(value);}
    pushableBitSequence getInstance(string token){return Byte(token);}
};

struct OpCode : pushableBitSequence{
    static const bool standAlone = true;
    static const regex matcher;

    OpCode(int len, int num) : pushableBitSequence(len, num){}
    OpCode(char len, char num) : pushableBitSequence(len, num){}
    OpCode(unsigned char len, unsigned char num) : pushableBitSequence(len, num){}
    OpCode(int len, unsigned char num) : pushableBitSequence(len, num){}
};