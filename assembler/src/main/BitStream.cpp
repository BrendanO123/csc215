#include "BitStream.h"

using namespace std;

bool BitStream :: push(pushableBitSequence e){
    if(offset == 0){
        bytes.push_back(e.data << (8 - e.length));
    }
    else{
        bytes.at(index) |= e.data << (8 - offset - e.length);
    }

    offset += e.length;
    if(offset < 8){return false;}
    index++; offset &= 0b111;
    return true;
}

inline void BitStream :: handleEmptyByteTermination(){
    fillByte();
    bytes.push_back(0); index++;
}