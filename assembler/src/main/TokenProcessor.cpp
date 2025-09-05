#include "TokenProcessor.h"

#include <iostream>

#include "InstructionLoader.h"

using namespace std;

const regex TokenProcessor :: RAM_regex = regex("[0-9]+");
const regex TokenProcessor :: register_regex = regex("[rR]?[a-hA-H]");
const regex TokenProcessor :: lookup_regex = regex("[a-zA-Z_][a-zA-Z0-9-_\\.]*");
const regex TokenProcessor :: sudoOp_regex = regex("\\.[a-zA-Z-_]+");

TokenProcessor :: TokenProcessor(){
    data = BitStream();
    definitionKeywords.emplace(".define", TokenProcessor ::  defineStatic);
    definitionKeywords.emplace(".DEFINE", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".def", TokenProcessor ::  defineStatic);
    definitionKeywords.emplace(".var", TokenProcessor ::  defineStatic);
    definitionKeywords.emplace(".VAR", TokenProcessor ::  defineStatic);
    definitionKeywords.emplace(".variable", TokenProcessor ::  defineStatic);
    definitionKeywords.emplace(".VARIABLE", TokenProcessor ::  defineStatic);

    definitionKeywords.emplace(".pos", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".position", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".POS", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".POSITION", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".func", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".FUNC", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".function", TokenProcessor :: positionDefStatic);
    definitionKeywords.emplace(".FUNCTION", TokenProcessor :: positionDefStatic);

    InstructionLoader :: initializeLookups(lookups, suffixes);

    //TODO:
        // add README for assembler
            // how the assembly language works
                // no hoisting
                // comments
                // position definitions
                // var definitions
                // case sensitivity
            // how the assembler works
                // how to run
                // npm and makefile scripts
                // where to put program and where output will be
                // how to use output
            //dependencies
                // how to install node and npm
                // how to set up a .zshrc file with the correct node config
        // add program from class to program folder and remove placeholder
        // test compilation and loading to simulator and run program
}

bool TokenProcessor :: processLine(vector<string> tokens){
    pushableBitSequence* footer = nullptr;
    for(string token : tokens){
        if(regex_match(token, RAM_regex)){
            data.push(pushableBitSequence(8, parseInt(token), true));
        }
        else if(regex_match(token, register_regex)){
            data.push(pushableBitSequence(3, parseReg(token)));
        }
        else if(regex_match(token, lookup_regex)){
            if(lookups.find(token) == lookups.end()){return false;}
            data.push(lookups.at(token));
            if(suffixes.find(token) != suffixes.end() && footer == nullptr){
                footer = &suffixes.at(token);
            }
        }
        else if (regex_match(token, sudoOp_regex)){
            if(definitionKeywords.find(token) == definitionKeywords.end()){return false;}
            pair<string, pushableBitSequence> result = definitionKeywords.at(token)(tokens, this);
            if(result.first == "" || result.second.length == 0){return false;}
            if(lookups.find(result.first) != lookups.end()){return false;}
            lookups.emplace(result.first, result.second);
            return true;
        }
        else{return false;}
    }
    if(footer != nullptr){data.push(*footer);}
    data.fillByte();
    return true;
}

BitStream TokenProcessor :: processTokens(queue<vector<string>> tokens){
    vector<string> line;
    bool valid = true;
    while(tokens.size() > 0){
        line = tokens.front();
        tokens.pop();
        if(!processLine(line)){
            cerr << "Errors decected on line: ";
            for(string token : line){cerr << token << ' ';}
            cerr << endl;
            valid = false;
        };
    }
    if(valid){cout << "Program is valid" << endl;}
    return data;
}

pair<string, pushableBitSequence> TokenProcessor :: define(vector<string> tokens){
    if(tokens.size()>=3){ // must have all the peices of the def
        // must be a valid var name
        if(!regex_match(tokens.at(1), lookup_regex)){return pair<string, pushableBitSequence>("", pushableBitSequence(0, 0));}
        
        // must be a valid token to replace the name with
        string token = tokens.at(2);
        if(regex_match(token, RAM_regex)){
            return pair<string, pushableBitSequence>(tokens.at(1), pushableBitSequence(8, parseInt(token), true));
        }
        else if(regex_match(token, register_regex)){
            return pair<string, pushableBitSequence>(tokens.at(1), pushableBitSequence(3, parseReg(token)));
        }
        else if(regex_match(token, lookup_regex)){
            if(lookups.find(token) != lookups.end()){
                return pair<string, pushableBitSequence>(tokens.at(1), lookups.at(token));
            }
        }
    }
    return pair<string, pushableBitSequence>("", pushableBitSequence(0, 0));
}

pair<string, pushableBitSequence> TokenProcessor :: positionDef(vector<string> tokens){
    if(tokens.size()>=2){ // must have all the peices of the def
        // must be a valid var name
        if(!regex_match(tokens.at(1), lookup_regex)){return pair<string, pushableBitSequence>("", pushableBitSequence(0, 0));}
        
        // get current position in RAM to give a value to named location and add as a memory adress
        int index = data.getIndex();
        if(index == -1){return pair<string, pushableBitSequence>("", pushableBitSequence(0, 0));}
        return pair<string, pushableBitSequence>(tokens.at(1), pushableBitSequence(8, index, true));
    }
    return pair<string, pushableBitSequence>("", pushableBitSequence(0, 0));
}