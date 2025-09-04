#pragma once

#include <queue>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <sstream>

#include "BitStream.h"

using namespace std;

class TokenProcessor{
    private:
        stringstream ss = stringstream();
        static const regex RAM_regex;
        static const regex register_regex;
        static const regex lookup_regex;
        static const regex sudoOp_regex;

        BitStream data;
        unordered_map<string, pushableBitSequence> lookups = unordered_map<string, pushableBitSequence>();
        unordered_map<string, function<pair<string, pushableBitSequence>(vector<string>, TokenProcessor*)>> definitionKeywords =
            unordered_map<string, function<pair<string, pushableBitSequence>(vector<string>, TokenProcessor*)>>(); 

        bool processLine(vector<string> tokens);

        inline int parseInt(string num){
            int out;
            ss.clear();
            ss << num;
            ss >> out;
            return out;
        }
        inline unsigned char parseReg(string reg){
            char c = reg.at(reg.length()-1);
            c = tolower(c);
            return (unsigned char)(c - 'a');
        }

        pair<string, pushableBitSequence> define(vector<string> tokens);
        pair<string, pushableBitSequence> positionDef(vector<string> tokens);

    public:
        TokenProcessor();
        BitStream processTokens(queue<vector<string>> tokens);

        static pair<string, pushableBitSequence> defineStatic(vector<string> tokens, TokenProcessor* obj){
            return obj->define(tokens);
        }

        static pair<string, pushableBitSequence> positionDefStatic(vector<string> tokens, TokenProcessor* obj){
            return obj->positionDef(tokens);
        }
};