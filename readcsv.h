// readcsv.h
#ifndef GRAPH_CSV_READER_H
#define GRAPH_CSV_READER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include "utils.h"
using namespace std;
/*
read_csv_file_tgn_format - for the tgn graph data type with million-scale edges;
read_csv_file_tgl_format - for the tgl graph data type with billion-scale edges.
*/
// Function declaration
void read_csv_file_tgn_format(const string& file_path, vector<GraphDataType>& edge_idx, vector<GraphDataType>& src_list, vector<GraphDataType>& dst_list, vector<GraphDataType>& time_list, bool save = false);
void read_csv_file_tgl_format(const string& file_path, vector<GraphDataType>& edge_idx, vector<GraphDataType>& src_list, vector<GraphDataType>& dst_list, vector<GraphDataType>& time_list, bool save = false);

#endif // GRAPH_CSV_READER_H
