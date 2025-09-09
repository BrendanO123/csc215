#include "TokenProcessor.h"

#include <iostream>

#include "InstructionLoader.h"

using namespace std;

const regex TokenProcessor :: variable_regex = regex("[a-zA-Z_][a-zA-Z0-9-_\\.]*");
const regex TokenProcessor :: instruction_regex = regex("[a-zA-Z]+");
const regex TokenProcessor :: sudoOp_regex = regex("\\.[a-zA-Z-_]+");
const regex TokenProcessor :: offset_regex = regex("-?[0-9]+");

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

    definitionKeywords.emplace(".relativeJump", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".relJump", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".relJmp", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".RELATIVE_JUMP", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".REL_JUMP", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".REL_JMP", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".offsetJump", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".offJmp", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".OFFSET_JUMP", TokenProcessor :: relJumpStatic);
    definitionKeywords.emplace(".OFF_JMP", TokenProcessor :: relJumpStatic);

    InstructionLoader :: initializeLookups(opCodes, instructionFormats, variables);
}

bool TokenProcessor :: processLine(vector<string> tokens){
    if(tokens.size()<1){return false;}
    string first = tokens.at(0);
    if(regex_match(first, sudoOp_regex)){
        if(definitionKeywords.find(first) == definitionKeywords.end()){return false;}

        pair<int, int> position = data.getPosition();
        pair<string, pushableBitSequence> result = definitionKeywords.at(first)(tokens, this);

        if(result.first == "" || result.second.length < 0){
            if(position != data.getPosition()){return true;}
            return false;
        }
        if(variables.find(result.first) != variables.end()){
            if(position != data.getPosition()){return true;}
            return false;
        }

        variables.emplace(result.first, result.second);
        return true;
    }
    if(regex_match(first, instruction_regex)){
        if(opCodes.find(first) == opCodes.end()){return false;}
        if(instructionFormats.find(first) == instructionFormats.end()){return false;}

        int tokenIndex = 1;
        int opCodePieceIndex = 1;

        auto format = instructionFormats.at(first);
        auto opCodePieces = opCodes.at(first);
        data.push(opCodePieces.at(0));

        for(int formatIndex = 1; formatIndex < format.size(); formatIndex++){
            if(tokenIndex > tokens.size()){return false;}

            if(format.at(formatIndex) == pushableBitSequenceTemplates :: OP_CODE){
                data.push(opCodePieces.at(opCodePieceIndex));
                opCodePieceIndex++;
                continue;
            }

            if(variables.find(tokens.at(tokenIndex)) != variables.end()){
                data.push(
                    pushableBitSequenceTemplates :: pushableBitSequenceTemplates[format.at(formatIndex)].intInitializer(
                        variables.at(tokens.at(tokenIndex)).data
                    )
                );
                tokenIndex++;
                continue;
            }

            auto e = pushableBitSequenceTemplates :: pushableBitSequenceTemplates
                [format.at(formatIndex)];
            if(e.isType(tokens.at(tokenIndex))){
                try{
                    auto pushable = e.stringInitializer(tokens.at(tokenIndex));
                    if(pushable.length > 0){
                        data.push(pushable); 
                        tokenIndex++; 
                        continue;
                    }
                }
                catch(invalid_argument exp){
                    cerr << "non-numerical token parsing attempted" << endl;
                }
            }
            if(regex_match(tokens.at(tokenIndex), variable_regex)){
                missingVars.emplace(
                    data.getPosition(), 
                    e, 
                    tokens.at(tokenIndex)
                );
                data.push(e.intInitializer(0));
            }
            else{
                data.push(e.intInitializer(0));
                return false;
            }
            tokenIndex++;
        }

        data.fillByte();
        return true;
    }
    return false;
}

BitStream TokenProcessor :: processTokens(queue<vector<string>> tokens){
    // parse
    bool valid = true;
    vector<string> line;
    while(tokens.size() > 0){
        line = tokens.front();
        tokens.pop();
        if(!processLine(line)){
            cerr << "Errors detected on line: ";
            for(string token : line){cerr << token << ' ';}
            cerr << endl;
            throw invalid_argument("Program is Invalid");
        };
    }
    if(valid){cout << "Parsed Program Successfully, Entering Linking" << endl;}

    // link
    valid = true;
    int staleCounter = 0;
    while(!missingVars.empty() && staleCounter < missingVars.size()){
        auto e = missingVars.front();
        missingVars.pop();
        if(variables.find(e.varName) == variables.end()){
            missingVars.push(e); 
            staleCounter++;
            continue;
        }
        else{
            data.set(e.index, e.offset, e.element.intInitializer(variables.at(e.varName).data));
            staleCounter = 0;
        }
    }
    if(missingVars.size()){
        cerr << "Undefined Symbols Referenced:" << endl;
        while(missingVars.size()){
            auto e = missingVars.front();
            missingVars.pop();
            cerr << e.varName << endl;
        }
        throw invalid_argument("Program is Invalid");
    }
    else{cout << "Program is valid" << endl;}
    return data;
}

pair<string, pushableBitSequence> TokenProcessor :: define(vector<string> tokens){
    if(tokens.size()>=3){ // must have all the pieces of the def
        // must be a valid var name
        if(!regex_match(tokens.at(1), variable_regex)){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}
        
        // must be a valid token to replace the name with
        string token = tokens.at(2);
        if(variables.find(token) != variables.end()){
            return pair<string, pushableBitSequence>(tokens.at(1), variables.at(token));
        }
        for(pushableBitSequenceTemplate e : pushableBitSequenceTemplates :: pushableBitSequenceTemplates){
            if(e.isType(token)){
                pushableBitSequence test = e.stringInitializer(token);
                if(test.length < 0){continue;}
                return pair<string, pushableBitSequence>(tokens.at(1),test);
            }
        }
    }
    return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));
}

pair<string, pushableBitSequence> TokenProcessor :: positionDef(vector<string> tokens){
    if(tokens.size()>=2){ // must have all the peices of the def
        // must be a valid var name
        if(!regex_match(tokens.at(1), variable_regex)){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}
        
        // get current position in RAM to give a value to named location and add as a memory adress
        int index = data.getIndex();
        if(index == -1){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}
        return pair<string, pushableBitSequence>(
            tokens.at(1), 
            pushableBitSequenceTemplates :: getIntBP(index)
        );
    }
    return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));
}

pair<string, pushableBitSequence> TokenProcessor :: relJump(vector<string> tokens){
    if(tokens.size()>=4){
        // first non-pseudo token must be a valid jmp or call instruction
        if(
            !regex_match(tokens.at(1), instruction_regex) || 
            opCodes.find(tokens.at(1)) == opCodes.end() ||
            instructionFormats.find(tokens.at(1)) == instructionFormats.end()
        ){
            return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));
        }

        // get opcode
        vector<pushableBitSequence> opCodPieces = opCodes.at(tokens.at(1));
        if(opCodPieces.size()>1){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}
        data.push(opCodPieces.at(0));

        auto format = instructionFormats.at(tokens.at(1));

        if(format.at(1) != pushableBitSequenceTemplates :: BYTE_PAIR){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}

        // get memory address and offset and push full BytePair
        pushableBitSequenceTemplate BP = pushableBitSequenceTemplates :: pushableBitSequenceTemplates
            [pushableBitSequenceTemplates :: BYTE_PAIR];
        if(!BP.isType(tokens.at(2))){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}
        if(!regex_match(tokens.at(3), offset_regex)){return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));}
        data.push(
            pushableBitSequenceTemplates :: getIntBP(parseInt(tokens.at(2).substr(1)) + parseInt(tokens.at(3)))
        );
    }
    return pair<string, pushableBitSequence>("", pushableBitSequence(-1, 0));
}