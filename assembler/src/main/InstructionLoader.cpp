#include "InstructionLoader.h"

using namespace std;

string InstructionLoader :: filePath = "opcodes.txt";

void InstructionLoader :: initializeLookups(
    unordered_map<string, pushableBitSequence>& lookups,
    unordered_map<string, pushableBitSequence>& suffixes
){
    lookups.clear();
    lookups.emplace("true", pushableBitSequence(1, 1));
    lookups.emplace("TRUE", pushableBitSequence(1, 1));
    lookups.emplace("false", pushableBitSequence(1, 0));
    lookups.emplace("FALSE", pushableBitSequence(1, 0));

    lookups.emplace("bc", pushableBitSequence(2, 0));
    lookups.emplace("BC", pushableBitSequence(2, 0));
    lookups.emplace("rp_bc", pushableBitSequence(2, 0));
    lookups.emplace("rp_BC", pushableBitSequence(2, 0));
    lookups.emplace("RP_bc", pushableBitSequence(2, 0));
    lookups.emplace("RP_BC", pushableBitSequence(2, 0));

    lookups.emplace("de", pushableBitSequence(2, 1));
    lookups.emplace("DE", pushableBitSequence(2, 1));
    lookups.emplace("rp_de", pushableBitSequence(2, 1));
    lookups.emplace("rp_DE", pushableBitSequence(2, 1));
    lookups.emplace("RP_de", pushableBitSequence(2, 1));
    lookups.emplace("RP_DE", pushableBitSequence(2, 1));

    lookups.emplace("hl", pushableBitSequence(2, 2));
    lookups.emplace("HL", pushableBitSequence(2, 2));
    lookups.emplace("rp_hl", pushableBitSequence(2, 2));
    lookups.emplace("rp_HL", pushableBitSequence(2, 2));
    lookups.emplace("RP_hl", pushableBitSequence(2, 2));
    lookups.emplace("RP_HL", pushableBitSequence(2, 2));

    lookups.emplace("fa", pushableBitSequence(2, 3));
    lookups.emplace("FA", pushableBitSequence(2, 3));
    lookups.emplace("rp_fa", pushableBitSequence(2, 3));
    lookups.emplace("rp_FA", pushableBitSequence(2, 3));
    lookups.emplace("RP_fa", pushableBitSequence(2, 3));
    lookups.emplace("RP_FA", pushableBitSequence(2, 3));
    lookups.emplace("FlagsA", pushableBitSequence(2, 3));
    lookups.emplace("rp_FlagsA", pushableBitSequence(2, 3));
    lookups.emplace("RP_FlagsA", pushableBitSequence(2, 3));

    ifstream file; file.open(filePath.c_str(), ios::in);

    if (!file.is_open() || ! file.good()) {
        file.close();
        cerr << "Error: Could not open instruction set file."<< endl;
        throw runtime_error("Error: Could not open instruction set file.");
    }

    // Read and process the file content
    string line, name;
    int data, length, suffixData, suffixLength;

    regex pattern = regex("\\s");
    sregex_token_iterator end;
    vector<string> tokens = vector<string>();

    while (getline(file, line)) {
        if(line.empty()){break;}
        for(auto iter = sregex_token_iterator(line.begin(), line.end(), pattern, -1); iter != end; iter++){
            if(!iter->str().empty()){
                tokens.push_back(iter->str());
            }
        }
        if(tokens.size()<2){tokens = vector<string>(); continue;}

        string name = tokens.at(0); 
        length = tokens.at(1).size();
        data = stoi(tokens.at(1), nullptr, 2);
        pushableBitSequence bitSequence = pushableBitSequence(length, data);

        lookups.emplace(name, bitSequence);
        lookups.emplace(toLower(name), bitSequence);

        if(tokens.size()>=3){
            suffixLength = tokens.at(2).size();
            suffixData = stoi(tokens.at(2), nullptr, 2);
            pushableBitSequence footerBitSequence = pushableBitSequence(suffixLength, suffixData);

            suffixes.emplace(name, footerBitSequence);
            suffixes.emplace(toLower(name), footerBitSequence);
        }
        tokens = vector<string>();
    }
    file.close();
}