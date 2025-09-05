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
    private:
        static filesystem :: path programFolder;
        static filesystem :: path outputFolder;
    public:
        static queue<vector<string>> tokenizeFile(string fileName);
        static bool writeOut(char* data, size_t size, string fileName);
};