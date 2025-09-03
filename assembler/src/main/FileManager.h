#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <queue>

using namespace std;

class FileManager{
    public:
        static queue<vector<string>> tokenizeFile(string filePath);
        static bool writeOut(char* data, int size);
};