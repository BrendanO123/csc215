#include "InstructionLoader.h"

using namespace std;

const regex InstructionLoader :: opCode_regex = regex("[01]+");
string InstructionLoader :: filePath = "opcodes.txt";

void InstructionLoader :: initializeLookups(
    unordered_map<string, vector<pushableBitSequence>>& opCodes,
    unordered_map<string, vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>>& instructionFormats, 
    unordered_map<string, pushableBitSequence>& variables
){

    variables.emplace("true", pushableBitSequence(1, 1));
    variables.emplace("True", pushableBitSequence(1, 1));
    variables.emplace("TRUE", pushableBitSequence(1, 1));

    variables.emplace("false", pushableBitSequence(1, 0));
    variables.emplace("False", pushableBitSequence(1, 0));
    variables.emplace("FALSE", pushableBitSequence(1, 0));

    variables.emplace("bc", pushableBitSequence(2, 0));
    variables.emplace("BC", pushableBitSequence(2, 0));
    variables.emplace("rp_bc", pushableBitSequence(2, 0));
    variables.emplace("rp_BC", pushableBitSequence(2, 0));
    variables.emplace("RP_bc", pushableBitSequence(2, 0));
    variables.emplace("RP_BC", pushableBitSequence(2, 0));

    variables.emplace("de", pushableBitSequence(2, 1));
    variables.emplace("DE", pushableBitSequence(2, 1));
    variables.emplace("rp_de", pushableBitSequence(2, 1));
    variables.emplace("rp_DE", pushableBitSequence(2, 1));
    variables.emplace("RP_de", pushableBitSequence(2, 1));
    variables.emplace("RP_DE", pushableBitSequence(2, 1));

    variables.emplace("hl", pushableBitSequence(2, 2));
    variables.emplace("HL", pushableBitSequence(2, 2));
    variables.emplace("rp_hl", pushableBitSequence(2, 2));
    variables.emplace("rp_HL", pushableBitSequence(2, 2));
    variables.emplace("RP_hl", pushableBitSequence(2, 2));
    variables.emplace("RP_HL", pushableBitSequence(2, 2));

    variables.emplace("fa", pushableBitSequence(2, 3));
    variables.emplace("FA", pushableBitSequence(2, 3));
    variables.emplace("rp_fa", pushableBitSequence(2, 3));
    variables.emplace("rp_FA", pushableBitSequence(2, 3));
    variables.emplace("RP_fa", pushableBitSequence(2, 3));
    variables.emplace("RP_FA", pushableBitSequence(2, 3));
    variables.emplace("FlagsA", pushableBitSequence(2, 3));
    variables.emplace("rp_FlagsA", pushableBitSequence(2, 3));
    variables.emplace("RP_FlagsA", pushableBitSequence(2, 3));

    ifstream file; file.open(filePath.c_str(), ios::in);

    if (!file.is_open() || ! file.good()) {
        file.close();
        cerr << "Error: Could not open instruction set file."<< endl;
        throw runtime_error("Error: Could not open instruction set file.");
    }

    // Read and process the file content
    string line, name;
    regex pattern = regex("\\s");
    sregex_token_iterator end;

    auto tokens = vector<string>();
    auto format = vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>();
    auto opCodePieces = vector<pushableBitSequence>();

    while (getline(file, line)) {
        if(line.empty()){break;}
        for(auto iter = sregex_token_iterator(line.begin(), line.end(), pattern, -1); iter != end; iter++){
            if(!iter->str().empty()){
                tokens.push_back(iter->str());
            }
        }
        if(tokens.size()<2){tokens = vector<string>(); continue;}

        name = tokens.at(0);
        opCodes.emplace(name, vector<pushableBitSequence>());
        opCodes.at(name).push_back(pushableBitSequence(int(tokens.at(1).size()), stoi(tokens.at(1), nullptr, 2))); // add in initial opcode start
        format.push_back(pushableBitSequenceTemplates :: OP_CODE);

        for(int i = 2; i < tokens.size(); i++){
            if(regex_match(tokens.at(i), opCode_regex)){
                opCodes.at(name).push_back(pushableBitSequence(int(tokens.at(i).size()), stoi(tokens.at(i), nullptr, 2)));
                format.push_back(pushableBitSequenceTemplates :: OP_CODE);
            }
            else if(tokens.at(i) == "bool"){format.push_back(pushableBitSequenceTemplates :: BOOL);}
            else if(tokens.at(i) == "rp"){format.push_back(pushableBitSequenceTemplates :: REGISTER_PAIR);}
            else if(tokens.at(i) == "reg"){format.push_back(pushableBitSequenceTemplates :: REGISTER);}
            else if(tokens.at(i) == "byte"){format.push_back(pushableBitSequenceTemplates :: BYTE);}
            else if(tokens.at(i) == "ram"){format.push_back(pushableBitSequenceTemplates :: BYTE_PAIR);}
            else{cerr << "Incorrect Token in OpCodes File: " << tokens.at(i);}
        }
        instructionFormats.emplace(tokens.at(0), format);

        tokens = vector<string>();
        opCodePieces = vector<pushableBitSequence>();
        format = vector<pushableBitSequenceTemplates :: pushableBitSequenceTemplateTypes>();
    }
    file.close();
}