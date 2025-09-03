#include "FileManager.h"
using namespace std;

bool FileManager :: writeOut(char* data, int size){
    return false;
}

queue<vector<string>> FileManager :: tokenizeFile(string filePath){
    regex tokenPattern = regex("[^\\s][a-zA-Z0-9-_.]+");
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
    sregex_iterator end;
    while (getline(file, line)) {
        for(auto iter = sregex_iterator(line.begin(), line.end(), tokenPattern); iter != end; iter++){
            if(!iter->str().empty()){
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