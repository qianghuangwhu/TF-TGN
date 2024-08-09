#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
using namespace std;

/*
int  - for the tgn graph data type with million-scale edges;
long - for the tgl graph data type with billion-scale edges.
*/

typedef int GraphDataType; // Define the data type for graph data

bool createDirectory(const std::string& path);
string extractSecondLastFolder(const string &filePath);

#endif // UTILS_H