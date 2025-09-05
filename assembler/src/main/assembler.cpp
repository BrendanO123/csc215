#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <filesystem>

#include "FileManager.h"
#include "TokenProcessor.h"

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

    queue<vector<string>> tokens = FileManager :: tokenizeFile(folder/fileName);

    TokenProcessor processor = TokenProcessor();
    pair<char*, size_t> result = processor.processTokens(tokens).toArray();
    for(int i=0; i<result.second; i++){
        cout << int((unsigned char)(result.first[i])) << ' ';
    }
    cout << endl;
    return 0;
}