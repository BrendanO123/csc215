#include "BitStream.h"

using namespace std;

bool BitStream :: push(pushableBitSequence e){
    if(e.length < 0){return false;}
    if(offset == 0 && !correctingOldData){
        bytes.push_back(e.data << (8 - e.length));
    }
    else{
        bytes.at(index) |= e.data << (8 - offset - e.length);
    }

    offset += e.length;
    if(offset < 8){return false;}
    index++; offset &= 0b111;
    optional<unsigned char> second = e.second;
    if(second.has_value()){return push(pushableBitSequence(8, second.value()));}
    return true;
}
bool BitStream :: set(int i, int off, pushableBitSequence e){
    if(!setPosition(i, off)){return false;}
    push(e);
    return jumpToNewDataPos();
}

inline void BitStream :: handleEmptyByteTermination(){
    fillByte();
    bytes.push_back(0); index++;
}