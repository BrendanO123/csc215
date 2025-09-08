#pragma once

#include <vector>
#include <iostream>

using namespace std;

struct pushableBitSequence{
    const unsigned char length;
    const unsigned char data;
    const bool terminateWithEmptyByte;

    pushableBitSequence(int len, int num, bool addEmptyByte = false) : 
        length((unsigned char)len), data((unsigned char)num), terminateWithEmptyByte(addEmptyByte){}

    pushableBitSequence(char len, char num, bool addEmptyByte = false) : 
        length((unsigned char)len), data((unsigned char)num), terminateWithEmptyByte(addEmptyByte){}

    pushableBitSequence(unsigned char len, unsigned char num, bool addEmptyByte = false) : 
        length(len), data(num), terminateWithEmptyByte(addEmptyByte){}

    pushableBitSequence(int len, unsigned char num, bool addEmptyByte = false) : 
        length((unsigned char)len), data(num), terminateWithEmptyByte(addEmptyByte){}
};

class BitStream{
    private:
        vector<unsigned char> bytes = vector<unsigned char>();
        int index = 0;
        int offset = 0;

        inline void handleEmptyByteTermination();

    public:
        BitStream(){}
        bool push(pushableBitSequence e);

        inline pair<char*, size_t> toArray(){
            char* arr = new char[bytes.size()];
            copy(begin(bytes), end(bytes), arr);
            return pair<char*, size_t>(arr, bytes.size());
        }

        inline int getIndex(){
            if(offset != 0){return -1;}
            return index;
        }
        inline int getOffset(){return offset;}

        inline bool fillByte(){
            if(offset == 0){return false;}
            offset = 0;
            index++;
            return true;
        }
};