#include "TokenProcessor.h"

#include <iostream>

#include "InstructionLoader.h"

using namespace std;

const regex TokenProcessor :: variable_regex = regex("[a-zA-Z_][a-zA-Z0-9_\\.]*");
const regex TokenProcessor :: instruction_regex = regex("[a-zA-Z]+");
const regex TokenProcessor :: sudoOp_regex = regex("\\.[a-zA-Z_]+");
const regex TokenProcessor :: parameter_regex = regex("(-?[0-9]+)|([rR][0-9]+)|(-?[0-9]+.[0-9]+)|([a-zA-Z_][a-zA-Z0-9_\\.]*)");
const regex TokenProcessor :: operator_regex = regex("[-+*/&<>][<>/]?");
const pair<pushableBitSequence, pushableBitSequence> TokenProcessor :: SPI = pair<pushableBitSequence, pushableBitSequence>(
    pushableBitSequence(8, 0041),
    pushableBitSequence(8, 0371)
);

TokenProcessor :: TokenProcessor(){
    data = BitStream();
    sudoOpKeywords.emplace(".define", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".DEFINE", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".def", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".DEF", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".var", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".VAR", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".variable", TokenProcessor :: defineStatic);
    sudoOpKeywords.emplace(".VARIABLE", TokenProcessor :: defineStatic);

    sudoOpKeywords.emplace(".pos", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".position", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".POS", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".POSITION", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".func", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".FUNC", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".function", TokenProcessor :: positionDefStatic);
    sudoOpKeywords.emplace(".FUNCTION", TokenProcessor :: positionDefStatic);

    sudoOpKeywords.emplace(".initSP", TokenProcessor :: initSPStatic);
    sudoOpKeywords.emplace(".INIT_SP", TokenProcessor :: initSPStatic);
    sudoOpKeywords.emplace(".initStackPointer", TokenProcessor :: initSPStatic);
    sudoOpKeywords.emplace(".initializeStackPointer", TokenProcessor :: initSPStatic);
    sudoOpKeywords.emplace(".INIT_STACK_POINTER", TokenProcessor :: initSPStatic);
    sudoOpKeywords.emplace(".INITIALIZE_STACK_POINTER", TokenProcessor :: initSPStatic);

    InstructionLoader :: initializeLookups(opCodes, instructionFormats, variables);
}

bool TokenProcessor :: processLine(vector<string> tokens){
    if(tokens.size()<1){return false;}
    string first = tokens.at(0);
    int index = data.getIndex();

    // handle pseudo instructions
    if(regex_match(first, sudoOp_regex)){
        if(sudoOpKeywords.find(first) == sudoOpKeywords.end()){return false;}
        return sudoOpKeywords.at(first)(tokens, this);
    }

    // handle machine code instructions
    if(regex_match(first, instruction_regex)){
        if(opCodes.find(first) == opCodes.end()){return false;}
        if(instructionFormats.find(first) == instructionFormats.end()){return false;}

        int tokenIndex = 1;
        int opCodePieceIndex = 1;

        auto format = instructionFormats.at(first);
        auto opCodePieces = opCodes.at(first);
        data.push(opCodePieces.at(0)); // push opcode

        // loop through tokens in instruction template/format
        for(int formatIndex = 1; formatIndex < format.size(); formatIndex++){
            if(tokenIndex > tokens.size()){return false;} // avoid out of bound token accessing

            // when the next token is NOT a parameter but a hardcoded part of the opcode
            if(format.at(formatIndex) == pushableBitSequenceTemplates :: OP_CODE){
                data.push(opCodePieces.at(opCodePieceIndex)); // push op code piece
                opCodePieceIndex++;
                continue;
            }

            // if the next token IS a parameter and is a variable, push variable casted to right parameter type
            if(variables.find(tokens.at(tokenIndex)) != variables.end()){
                data.push(
                    pushableBitSequenceTemplates :: pushableBitSequenceTemplates[format.at(formatIndex)].intInitializer(
                        variables.at(tokens.at(tokenIndex)).getValue()
                    )
                );
                tokenIndex++;
                continue;
            }

            // if next token IS a parameter and is NOT a known variable, get next token template
            auto e = pushableBitSequenceTemplates :: pushableBitSequenceTemplates
                [format.at(formatIndex)];

            // if the next token matches this template
            if(e.isType(tokens.at(tokenIndex))){
                try{
                    // try to push the literal parameter
                    auto pushable = e.stringInitializer(tokens.at(tokenIndex));
                    if(pushable.length > 0){
                        data.push(pushable); 
                        tokenIndex++; 
                        continue;
                    }
                }
                catch(invalid_argument exp){
                    // and catch unexpected errors (should be graceful with length < 0 if not a real literal so this is unexpected)
                    cerr << "non-numerical token parsing attempted: " << exp.what() << endl;
                }
            }

            // if the next token is still not resolved and looks like a variable, assumed to be a unresolved variable and push
            if(regex_match(tokens.at(tokenIndex), variable_regex)){
                missingVars.emplace(index, tokens, tokens.at(tokenIndex));
                data.push(e.intInitializer(0));
            }
            else{
                // finally, if it doesn't look like a variable, push empty data and return an error on this line
                data.push(e.intInitializer(0));
                return false;
            }
            tokenIndex++;
        }
        return true;
    }

    // handle literal data
    pushableBitSequence e = tryGetParameter(first);
    if(e.length <= 0){return false;}
    data.push(e);
    return true;
}
inline bool TokenProcessor :: processLineWrapper(vector<string> tokens, int index){
    if(index != -1){data.setPosition(index, 0);}
    bool result = processLine(tokens);
    data.fillByte();
    if(index != -1){data.jumpToNewDataPos();}
    return result;
}

BitStream TokenProcessor :: processTokens(queue<vector<string>> tokens){
    // parse
    vector<string> line;
    while(tokens.size() > 0){
        line = tokens.front();
        tokens.pop();
        if(!processLineWrapper(line)){
            cerr << "Errors detected on line: ";
            for(string token : line){cerr << token << ' ';}
            cerr << endl;
            throw invalid_argument("Program is Invalid");
        };
    }
    cout << "Parsed Program Successfully, Entering Linking" << endl;

    // link
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
            processLineWrapper(e.lineTokens, e.index);
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

pushableBitSequence TokenProcessor :: tryGetParameter(vector<string> tokens, int& i){
    int index = i; i++;
    string main = tokens.at(index);
    if(index + 2 < tokens.size()){
        if(regex_match(tokens.at(index+1), operator_regex) && regex_match(tokens.at(index+2), parameter_regex)){
            auto first = tryGetParameter(main);
            auto second = tryGetParameter(tokens.at(index+2));

            if(first.length <= 0){return pushableBitSequence(-1, 0);}
            if(second.length <= 0){return first;}

            int a = first.getValue();
            int b = second.getValue();
            if(main.at(0) == '-'){a = int(char(a));}
            if(tokens.at(index+2).at(0) == '-'){b = int(char(b));}

            i += 2;
            if(tokens.at(index+1).length() != 1){
                if(tokens.at(index+1) == "//"){
                    return pushableBitSequence(
                        first.length,
                        a / b, 
                        first.second.has_value()
                    );
                }
                if(tokens.at(index+1) == "<<"){
                    return pushableBitSequence(
                        first.length,
                        a << b,
                        first.second.has_value()
                    );
                }
                if(tokens.at(index+1) == ">>"){
                    return pushableBitSequence(
                        first.length,
                        a >> b,
                        first.second.has_value()
                    );
                }
                i -= 2; return first;
            }
            else{
                switch(tokens.at(index+1).at(0)){
                    case '+':
                        return pushableBitSequence(
                            first.length,
                            a + b,
                            first.second.has_value()
                        );
                    case '-':
                        return pushableBitSequence(
                            first.length, 
                            a - b,
                            first.second.has_value()
                        );
                    case '*':
                        return pushableBitSequence(
                            first.length, 
                            a * b,
                            first.second.has_value()
                        );
                    case '&':
                        return pushableBitSequence(
                            min(first.length, second.length), 
                            a & b,
                            first.second.has_value() && second.second.has_value()
                        );
                    case '/':
                        return pushableBitSequence(
                            8,
                            int((a / float(b)) * 256),
                            true
                        );
                    
                    default:
                        i -= 2;
                        return first;
                }
            }
        }
    }

    auto first = tryGetParameter(main);
    return first;
}

pushableBitSequence TokenProcessor :: tryGetParameter(string token){
    auto e = pushableBitSequenceTemplates :: tryGetLiteral(token);
    if(e.length > 0){return e;}
    if(variables.find(token) != variables.end()){
        return variables.at(token);
    }
    return pushableBitSequence(-1, 0);
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

    int index = 2;
    pushableBitSequence e = tryGetParameter(tokens, index);
    if(e.length <= 0){return false;}
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