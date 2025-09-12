#include "TokenProcessor.h"

#include <iostream>

#include "InstructionLoader.h"

using namespace std;

const regex TokenProcessor :: variable_regex = regex("[a-zA-Z_][a-zA-Z0-9-_\\.]*");
const regex TokenProcessor :: instruction_regex = regex("[a-zA-Z]+");
const regex TokenProcessor :: sudoOp_regex = regex("\\.[a-zA-Z-_]+");
const regex TokenProcessor :: offset_regex = regex("-?[0-9]+");
const pair<pushableBitSequence, pushableBitSequence> TokenProcessor :: SPI = pair<pushableBitSequence, pushableBitSequence>(
    pushableBitSequence(8, 0041),
    pushableBitSequence(8, 0371)
);

TokenProcessor :: TokenProcessor(){
    data = BitStream();
    definitionKeywords.emplace(".define", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".DEFINE", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".def", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".DEF", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".var", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".VAR", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".variable", TokenProcessor :: defineStatic);
    definitionKeywords.emplace(".VARIABLE", TokenProcessor :: defineStatic);

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

    definitionKeywords.emplace(".initSP", TokenProcessor :: initSPStatic);
    definitionKeywords.emplace(".INIT_SP", TokenProcessor :: initSPStatic);
    definitionKeywords.emplace(".initStackPointer", TokenProcessor :: initSPStatic);
    definitionKeywords.emplace(".initializeStackPointer", TokenProcessor :: initSPStatic);
    definitionKeywords.emplace(".INIT_STACK_POINTER", TokenProcessor :: initSPStatic);
    definitionKeywords.emplace(".INITIALIZE_STACK_POINTER", TokenProcessor :: initSPStatic);

    InstructionLoader :: initializeLookups(opCodes, instructionFormats, variables);
}

bool TokenProcessor :: processLine(vector<string> tokens){
    if(tokens.size()<1){return false;}
    string first = tokens.at(0);
    if(regex_match(first, sudoOp_regex)){
        if(definitionKeywords.find(first) == definitionKeywords.end()){return false;}
        return definitionKeywords.at(first)(tokens, this);
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
                    cerr << "non-numerical token parsing attempted: " << exp.what() << endl;
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
    pushableBitSequence e = pushableBitSequenceTemplates :: tryGetLiteral(first);
    if(e.length < 0){return false;}
    data.push(e);
    return true;
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
            data.set(
                e.index, 
                e.offset, 
                e.element.intInitializer(
                    variables.at(e.varName).data + (e.memoryAddressOffset.has_value() ? e.memoryAddressOffset.value() : 0)
                )
            );
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

bool TokenProcessor :: define(vector<string> tokens){
    if(tokens.size() < 3){return false;} // must have all the pieces of the def
    if(!regex_match(tokens.at(1), variable_regex)){return false;} // must be a valid var name
    
    // must be a valid token to replace the name with
    string token = tokens.at(2);
    if(variables.find(token) != variables.end()){
        variables.emplace(tokens.at(1), variables.at(token));
        return true;
    }

    pushableBitSequence e = pushableBitSequenceTemplates :: tryGetLiteral(token);
    if(e.length < 0){return false;}
    variables.emplace(tokens.at(1), e);
    return true;
}

bool TokenProcessor :: positionDef(vector<string> tokens){
    if(tokens.size() < 2){return false;} // must have all the pieces of the def
    if(!regex_match(tokens.at(1), variable_regex)){return false;} // must be a valid var name
    
    // get current position in RAM to give a value to named location and add as a memory address
    int index = data.getIndex();
    if(index == -1){return false;}
    variables.emplace(
        tokens.at(1), 
        pushableBitSequenceTemplates :: getIntBP(index)
    );
    return true;
}

bool TokenProcessor :: relJump(vector<string> tokens){
    if(tokens.size() < 4){return false;}
    // first non-pseudo token must be a valid jmp or call instruction
    if(
        !regex_match(tokens.at(1), instruction_regex) || 
        opCodes.find(tokens.at(1)) == opCodes.end() ||
        instructionFormats.find(tokens.at(1)) == instructionFormats.end()
    ){return false;}

    // get opcode
    vector<pushableBitSequence> opCodPieces = opCodes.at(tokens.at(1));
    if(opCodPieces.size()>1){return false;}
    data.push(opCodPieces.at(0));

    auto format = instructionFormats.at(tokens.at(1));
    if(format.at(1) != pushableBitSequenceTemplates :: BYTE_PAIR){return false;}
    if(!regex_match(tokens.at(3), offset_regex)){return false;}
    int offset = parseInt(tokens.at(3));

    // get memory address and offset and push full BytePair
    if(variables.find(tokens.at(2)) != variables.end()){
        data.push(
            pushableBitSequenceTemplates :: getIntBP(variables.at(tokens.at(2)).data + offset)
        );
        return true;
    }
    pushableBitSequenceTemplate BP = pushableBitSequenceTemplates :: pushableBitSequenceTemplates
        [pushableBitSequenceTemplates :: BYTE_PAIR];
    if(BP.isType(tokens.at(2))){
        try{
            data.push(
                pushableBitSequenceTemplates :: getIntBP(parseInt(tokens.at(2).substr(1)) + offset)
            );
            return true;
        }
        catch(invalid_argument e){
            cerr << "non-numerical token parsing attempted: " << e.what() << endl;
        }
    }
    if(regex_match(tokens.at(2), variable_regex)){
        missingVars.emplace(
            data.getPosition(),
            BP,
            tokens.at(2),
            offset
        );
        data.push(BP.intInitializer(0));
        return true;
    }
    data.push(BP.intInitializer(0));
    return false;
}

bool TokenProcessor :: initSP(){
    if(variables.find("LAST_ADDRESS") == variables.end()){
        variables.emplace(
            "LAST_ADDRESS",
            pushableBitSequenceTemplates :: pushableBitSequenceTemplates[
                pushableBitSequenceTemplates :: BYTE_PAIR
            ].intInitializer(estimatedRAMSize)
        );
    }
    data.push(SPI.first);
    data.push(variables.at("LAST_ADDRESS"));
    data.push(SPI.second);

    return true;
}