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
        unordered_map<string, pushableBitSequence> suffixes = unordered_map<string, pushableBitSequence>();
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
            c = toupper(c);
            switch(c){
                case 'B':
                    return 0;
                case 'C':
                    return 1;
                case 'D':
                    return 2;
                case 'E':
                    return 3;
                case 'H':
                    return 4;
                case 'L':
                    return 5;
                case 'M':
                    return 6;
                case 'A':
                    return 7;
                default:
                    return 0;
            }
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