#pragma once

#include <vector>
#include <iostream>
#include <optional>

#include "PushableBitSequence.h"

using namespace std;

class BitStream{
    private:
        vector<unsigned char> bytes = vector<unsigned char>();
        int index = 0;
        int offset = 0;

        inline void handleEmptyByteTermination();

    public:
        BitStream(){}
        bool push(pushableBitSequence e);
        unsigned char pop(){
            unsigned char c = bytes.at(index);
            bytes.pop_back();
            index--;
            offset = 0;
            return c;
        }
        bool set(int i, int off, pushableBitSequence e);

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
        inline pair<int, int> getPosition(){return pair<int, int>(index, offset);}

        inline bool fillByte(){
            if(offset == 0){return false;}
            offset = 0;
            index++;
            return true;
        }
};