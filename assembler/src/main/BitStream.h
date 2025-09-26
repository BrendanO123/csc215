#pragma once

#include <vector>
#include <iostream>
#include <optional>

#include "PushableBitSequence.h"

using namespace std;

class BitStream{
    private:
        bool correctingOldData = false;
        vector<unsigned char> bytes = vector<unsigned char>();
        int index = 0;
        int offset = 0;

        inline void handleEmptyByteTermination();

    public:
        BitStream(){}
        bool push(pushableBitSequence e);
        unsigned char pop(){
            unsigned char c = bytes.at(index);
            if(correctingOldData){bytes.erase(bytes.begin() + index);}
            else{bytes.pop_back();}
            index--;
            offset = 0;
            return c;
        }
        bool set(int i, int off, pushableBitSequence e);
        bool set(pair<int, int> pos, pushableBitSequence e){return set(pos.first, pos.second, e);}
        bool setPosition(int i, int off){
            if(i>=0 && i<=bytes.size() && off >=0 && off < 8){
                index = i; 
                correctingOldData = (index < bytes.size() || (index == bytes.size() && off < offset));
                offset = off;
                return true;
            }
            return false;
        }
        bool setPosition(pair<int, int> pos){return setPosition(pos.first, pos.second);}
        bool jumpToNewDataPos(){
            bool result = correctingOldData;
            correctingOldData = false;
            index = bytes.size();
            offset = 0;
            return result;
        }

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