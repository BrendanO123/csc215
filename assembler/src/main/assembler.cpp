#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <filesystem>

#include "FileManager.h"

typedef unsigned char ubyte;

using namespace std;

const filesystem :: path folder = "programs";

int main(int argc, char *argv[]) {

    // Check if a file path was provided as an argument
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file_name>" << endl;
        return 1; // Indicate an error
    }

    // The first argument (argv[1]) should be the input file path
    filesystem :: path fileName = argv[1];

    queue<vector<string>> tokens = FileManager :: tokenizeFile(folder / fileName);
    vector<string> line;
    while(!tokens.empty()){
        line = tokens.front();
        tokens.pop();
        for(string token : line){
            cout << token << ' ';
        }
        cout << endl;
    }
    return 0;
}