#pragma once

#include <queue>
#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class FileManager{
    public:
        static queue<vector<string>> tokenizeFile(string filePath);
        static bool writeOut(char* data, int size);
};