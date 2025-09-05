#include "BitStream.h"

using namespace std;

bool BitStream :: push(pushableBitSequence e){
    if(offset == 0){bytes.push_back(e.data);}
    else{
        bytes.at(index) <<= e.length;
        bytes.at(index) |= e.data;
    }

    if(e.length + offset < 8){offset += e.length; return false;}
    index++;
    offset += e.length; offset &= 0b111;
    return true;
}

inline void BitStream :: handleEmptyByteTermination(){
    fillByte();
    bytes.push_back(0); index++;
}