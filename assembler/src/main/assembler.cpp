#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <filesystem>

#include "FileManager.h"
#include "TokenProcessor.h"

using namespace std;

int main(int argc, char *argv[]) {

    // Check if a file path was provided as an argument
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file_name>" << endl;
        return 1; // Indicate an error
    }

    // The first argument (argv[1]) should be the input file path
    string fileName = argv[1];

    queue<vector<string>> tokens = FileManager :: tokenizeFile(fileName);

    TokenProcessor processor = TokenProcessor();
    pair<char*, size_t> result = processor.processTokens(tokens).toArray();

    FileManager :: writeOut(result.first, result.second, fileName);
    return 0;
}