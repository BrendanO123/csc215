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
    string varName;
    vector<string> lineTokens;

    missingVar(int index, vector<string> tokens, string name) :
        index(index), varName(name), lineTokens(tokens){}
};
class TokenProcessor{
    private:
        static const int estimatedRAMSize = 8192;
        static const pair<pushableBitSequence, pushableBitSequence> SPI;
        static const regex variable_regex;
        static const regex sudoOp_regex;
        static const regex instruction_regex;
        static const regex operator_regex;
        static const regex parameter_regex;

        BitStream data;
        queue<missingVar> missingVars = queue<missingVar>();
        unordered_map<string, pushableBitSequence> variables = unordered_map<string, pushableBitSequence>();
        
        unordered_map<string, vector<pushableBitSequence>> opCodes = unordered_map<string, vector<pushableBitSequence>>();
        unordered_map<string, vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>> instructionFormats = 
            unordered_map<string, vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>>();

        unordered_map<string, function<bool(vector<string>, TokenProcessor*)>> sudoOpKeywords =
            unordered_map<string, function<bool(vector<string>, TokenProcessor*)>>(); 

        inline bool processLineWrapper(vector<string> tokens, int index = -1);
        bool processLine(vector<string> tokens);

        bool define(vector<string> tokens);
        bool positionDef(vector<string> tokens);
        bool relJump(vector<string> tokens);
        bool initSP();

    public:
        TokenProcessor();
        BitStream processTokens(queue<vector<string>> tokens);

        static bool defineStatic(vector<string> tokens, TokenProcessor* obj){return obj->define(tokens);}
        static bool positionDefStatic(vector<string> tokens, TokenProcessor* obj){return obj->positionDef(tokens);}
        static bool relJumpStatic(vector<string> tokens, TokenProcessor* obj){return obj->relJump(tokens);}
        static bool initSPStatic(vector<string> tokens, TokenProcessor* obj){return obj->initSP();}

        pushableBitSequence tryGetParameter(vector<string> tokens, int& index);
        pushableBitSequence tryGetParameter(string token);
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