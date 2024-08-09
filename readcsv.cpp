#include "readcsv.h"
#include "utils.h"

void read_csv_file_tgn_format(const string& file_path, vector<GraphDataType>& edge_idx, vector<GraphDataType>& src_list, vector<GraphDataType>& dst_list, vector<GraphDataType>& time_list, bool save) {
    auto start_time = chrono::high_resolution_clock::now();

    ifstream file(file_path);
    string line;

    // Assuming the first line is a header and skipping it
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string cell;

        getline(ss, cell, ',');
        edge_idx.push_back(stoi(cell)+1);

        getline(ss, cell, ',');
        src_list.push_back(stoi(cell));

        getline(ss, cell, ',');
        dst_list.push_back(stoi(cell));

        getline(ss, cell, ',');
        time_list.push_back(stoi(cell));
    }

    cout << "edge_nums: " << edge_idx.size() << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // cout << "Elapsed time: " << elapsed_time.count() / 1000.0 << " s" << endl;

    // The function modifies the input vectors directly, so there's no explicit "return" statement needed.
}

void read_csv_file_tgl_format(const string& file_path, vector<GraphDataType>& edge_idx, vector<GraphDataType>& src_list, vector<GraphDataType>& dst_list, vector<GraphDataType>& time_list, bool save) {
    auto start_time = chrono::high_resolution_clock::now();

    ifstream file(file_path);
    string line;

    // Assuming the first line is a header and skipping it
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string cell;

        getline(ss, cell, ',');
        edge_idx.push_back(stol(cell)+1);

        getline(ss, cell, ',');
        src_list.push_back(stol(cell));

        getline(ss, cell, ',');
        dst_list.push_back(stol(cell));

        getline(ss, cell, ',');
        time_list.push_back(stol(cell));
    }

    cout << "edge_nums: " << edge_idx.size() << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // cout << "Elapsed time: " << elapsed_time.count() / 1000.0 << " s" << endl;

    // The function modifies the input vectors directly, so there's no explicit "return" statement needed.
}