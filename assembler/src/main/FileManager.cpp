#include "FileManager.h"

#include "BitStream.h"

using namespace std;

filesystem :: path FileManager :: programFolder = "programs";
filesystem :: path FileManager :: outputFolder = "outputs";

bool FileManager :: writeOut(char* data, size_t size, string fileName){

    filesystem :: path filePath = outputFolder / filesystem :: path(fileName);
    filePath.replace_extension(".bin");

    filesystem::create_directories(outputFolder);
    ofstream file; file.open(filePath.c_str(), ios::out|ios::binary);

    // Check if the file was opened successfully
    if (!file.is_open() || !file.good()) {
        file.close();
        cerr << "Error: Could not open file " << filePath << endl;
        return false;
    }

    file.write(data, size);
    file.close();

    filePath.replace_extension(".txt");
    file.open(filePath.c_str(), ios::out);
    if (!file.is_open() || ! file.good()) {
        file.close();
        cerr << "Error: Could not open file " << filePath << endl;
        cerr << "Make sure the file name is the relative path to the file from the programs folder" << endl;
        return false;
    }

    string line;
    stringstream ss = stringstream();
    for(int i = 0; i < size; i++){
        file << uint((data[i] & (3 << 6)) >> 6) << uint((data[i] & (7 << 3)) >> 3) << uint(data[i] & 7);
        if(i < size -1){file << '\n';}
    }
    file.close();
    return false;
}

queue<vector<string>> FileManager :: tokenizeFile(string fileName){
    regex pattern = regex("\\s");
    filesystem :: path filePath = programFolder / filesystem :: path(fileName);
    ifstream file; file.open(filePath.c_str(), ios::in);
    queue<vector<string>> lines = queue<vector<string>>();

    // Check if the file was opened successfully
    if (!file.is_open() || ! file.good()) {
        file.close();
        cerr << "Error: Could not open file " << filePath << endl;
        cerr << "Make sure the file name is the relative path to the file from the programs folder" << endl;
        return queue<vector<string>>();
    }

    // Read and process the file content
    string line;
    string token;
    vector<string> tokens = vector<string>();
    sregex_token_iterator end;
    while (getline(file, line)) {
        for(auto iter = sregex_token_iterator(line.begin(), line.end(), pattern, -1); iter != end; iter++){
            if(!iter->str().empty()){
                if(iter->str().at(0) == '#'){break;}
                tokens.push_back(iter->str());
            }
        }
        if(!tokens.empty()){lines.emplace(tokens);}
        tokens = vector<string>();
    }

    // Close the file
    file.close();
    return lines;
}