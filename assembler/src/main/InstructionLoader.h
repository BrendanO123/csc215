#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <unordered_map>

#include "BitStream.h"

using namespace std;

class InstructionLoader{
    private:
        static string filePath;
        static inline string toLower(string in){
            transform(in.begin(), in.end(), in.begin(), ::tolower);
            return in;
        }
    public:
        static void initializeLookups(
            unordered_map<string, pushableBitSequence>& lookups,
            unordered_map<string, pushableBitSequence>& suffixes
        );
};