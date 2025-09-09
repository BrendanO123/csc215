#include "BitStream.h"

using namespace std;

bool BitStream :: push(pushableBitSequence e){
    if(e.length < 0){return false;}
    if(offset == 0){
        bytes.push_back(e.data << (8 - e.length));
    }
    else{
        bytes.at(index) |= e.data << (8 - offset - e.length);
    }

    offset += e.length;
    if(offset < 8){return false;}
    index++; offset &= 0b111;
    optional<Byte> second = e.second;
    if(second.has_value()){return push(second.value());}
    return true;
}
bool BitStream :: set(int i, int off, pushableBitSequence e){
    if(i < 0 || i > index || off < 0 || off > 8){return false;}
    bytes.at(i) &= ~(((1<<e.length)-1)<<(8 - offset - e.length));
    bytes.at(i) |= e.data << (8 - offset - e.length);
    
    optional<Byte> second = e.second;
    if(second.has_value()){set(i++, off, second.value());}
    return true;
}

inline void BitStream :: handleEmptyByteTermination(){
    fillByte();
    bytes.push_back(0); index++;
}