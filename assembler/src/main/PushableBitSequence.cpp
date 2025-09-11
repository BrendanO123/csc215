#include "PushableBitSequence.h"

using namespace std;

pushableBitSequence pushableBitSequenceTemplates :: getIntBool(int value){return pushableBitSequence(1, (unsigned char)value);}
pushableBitSequence pushableBitSequenceTemplates :: getIntRP(int value){return pushableBitSequence(2, (unsigned char)value);}
pushableBitSequence pushableBitSequenceTemplates :: getIntReg(int value){return pushableBitSequence(3, (unsigned char)value);}
pushableBitSequence pushableBitSequenceTemplates :: getIntByte(int value){return pushableBitSequence(8, (unsigned char)value);}
pushableBitSequence pushableBitSequenceTemplates :: getIntBP(int value){
    auto e = pushableBitSequence(8, (unsigned char)value);
    e.second.emplace((unsigned char)(value>>8));
    return e;
}

pushableBitSequence pushableBitSequenceTemplates :: getStringBool(string token){return pushableBitSequence(-1, 0);}
pushableBitSequence pushableBitSequenceTemplates :: getStringRP(string token){return pushableBitSequence(-1, 0);}
pushableBitSequence pushableBitSequenceTemplates :: getStringReg(string token){
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
pushableBitSequence pushableBitSequenceTemplates :: getStringByte(string token){return pushableBitSequence(8, (unsigned char)stoi(token, nullptr));}
pushableBitSequence pushableBitSequenceTemplates :: getStringBP(string token){
    int value = stoi(token.substr(1), nullptr);
    auto e = pushableBitSequence(8, (unsigned char)value);
    e.second.emplace((unsigned char)(value>>8));
    return e;
}