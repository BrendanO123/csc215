#include "FileManager.h"

#include "BitStream.h"

using namespace std;

bool FileManager :: writeOut(char* data, int size){
    return false;
}

queue<vector<string>> FileManager :: tokenizeFile(string filePath){
    regex pattern = regex("\\s");
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
        if(!line.empty() && line.at(0) == '#'){continue;}
        for(auto iter = sregex_token_iterator(line.begin(), line.end(), pattern, -1); iter != end; iter++){
            if(!iter->str().empty()){
                tokens.push_back(iter->str());
            }
        }
        if(!tokens.empty() && !tokens.at(0).empty() && tokens.at(0).at(0) == '#'){continue;}
        if(!tokens.empty()){lines.emplace(tokens);}
        tokens = vector<string>();
    }

    // Close the file
    file.close();
    return lines;
}