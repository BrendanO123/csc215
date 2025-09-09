#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <unordered_map>

#include "BitStream.h"
#include "PushableBitSequence.h"

using namespace std;

class InstructionLoader{
    private:
        static const regex opCode_regex;
        static string filePath;
        static inline string toLower(string in){
            transform(in.begin(), in.end(), in.begin(), ::tolower);
            return in;
        }
    public:
        static void initializeLookups(
            unordered_map<string, vector<pushableBitSequence>>& opCodes,
            unordered_map<string, vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>>& instructionFormats, 
            unordered_map<string, pushableBitSequence>& variables
        );
};