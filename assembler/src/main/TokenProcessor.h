#pragma once

#include <queue>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <sstream>

#include "BitStream.h"

using namespace std;

struct missingVar{
    int index;
    int offset;
    pushableBitSequenceTemplate element;
    string varName;

    missingVar(pair<int,int> position, pushableBitSequenceTemplate e, string name) : 
        index(position.first), offset(position.second), element(e), varName(name){}
    missingVar(int index, int offset, pushableBitSequenceTemplate e, string name) :
        index(index), offset(offset), element(e), varName(name){}
};
class TokenProcessor{
    private:
        static const regex variable_regex;
        static const regex sudoOp_regex;
        static const regex instruction_regex;
        static const regex offset_regex;

        BitStream data;
        queue<missingVar> missingVars = queue<missingVar>();
        unordered_map<string, pushableBitSequence> variables = unordered_map<string, pushableBitSequence>();
        
        unordered_map<string, vector<pushableBitSequence>> opCodes = unordered_map<string, vector<pushableBitSequence>>();
        unordered_map<string, vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>> instructionFormats = 
            unordered_map<string, vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>>();

        unordered_map<string, function<pair<string, pushableBitSequence>(vector<string>, TokenProcessor*)>> definitionKeywords =
            unordered_map<string, function<pair<string, pushableBitSequence>(vector<string>, TokenProcessor*)>>(); 

        bool processLine(vector<string> tokens);

        pair<string, pushableBitSequence> define(vector<string> tokens);
        pair<string, pushableBitSequence> positionDef(vector<string> tokens);
        pair<string, pushableBitSequence> relJump(vector<string> tokens);

    public:
        TokenProcessor();
        BitStream processTokens(queue<vector<string>> tokens);

        static pair<string, pushableBitSequence> defineStatic(vector<string> tokens, TokenProcessor* obj){
            return obj->define(tokens);
        }

        static pair<string, pushableBitSequence> positionDefStatic(vector<string> tokens, TokenProcessor* obj){
            return obj->positionDef(tokens);
        }

        static pair<string, pushableBitSequence> relJumpStatic(vector<string> tokens, TokenProcessor* obj){
            return obj->relJump(tokens);
        }

        inline int parseInt(string num){return stoi(num, nullptr);}
        unsigned char parseReg(string reg){
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
                    return 7;
            }
        }
};