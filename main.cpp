// g++ -fopenmp -std=c++11 main.cpp TemporalGraph.cpp readcsv.cpp  utils.cpp -o main
#include "TemporalGraph.h"
#include "readcsv.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <integer1> <integer2>\n";
        return 1;
    }
    int sample_num_argv = std::stoi(argv[1]);
    int mini_batch_argv = std::stoi(argv[2]);
    cout << "start coverting..." << endl;
    // Initialize vectors
    vector<GraphDataType> edge_idx;
    vector<GraphDataType> src_list;
    vector<GraphDataType> dst_list;
    vector<GraphDataType> time_list;
    string file_path = "edges.csv";

    /*
    read_csv_file_tgn_format - for the tgn graph data type with million-scale edges;
    read_csv_file_tgl_format - for the tgl graph data type with billion-scale edges.
    */
    read_csv_file_tgn_format(file_path, edge_idx, src_list, dst_list, time_list);

    bool reverse = false;
    TemporalGraph tg(edge_idx, src_list, dst_list, time_list, reverse);

    tg.to_csr();

    // cout << "done!" << endl;

    bool save = false;
    if (save) {
        string dataset = extractSecondLastFolder(file_path);
        if (reverse){
            dataset += "_reverse";
        }
        cout << "dataset: " << dataset << endl;
        string root_path = "./csr_data/" + dataset + "/";
        if (createDirectory(root_path)) {
            cout << "Folder created or already exists." << endl;
        }
        tg.save_csr(root_path + "idx_values.csv", root_path + "time_values.csv", root_path + "indices.csv", root_path + "indptr.csv");
        cout << "CSR saved at " << root_path << endl;
    }
    
    exit(0);

    // test sampling
    int batch_size = mini_batch_argv * 3;
    vector<GraphDataType> batch_node_id(edge_idx.begin(), edge_idx.begin() + batch_size);
    vector<GraphDataType> batch_node_time(time_list.begin(), time_list.begin() + batch_size);
    vector<vector<GraphDataType>> batch_neighbors;
    vector<vector<GraphDataType>> batch_neighbor_times;
    vector<vector<GraphDataType>> batch_neighbor_idx;
    GraphDataType sample_num = sample_num_argv;
    string sample_strategy = "recent";

    double elapsed_time = 0;
    for (int i = 0; i < 10; i++) {
        if (i < 5) continue;
        elapsed_time += tg.sampling(batch_node_id, batch_node_time, batch_neighbors, batch_neighbor_times, batch_neighbor_idx, sample_num, sample_strategy);
    }
    cout << "Elapsed sampling time: " << elapsed_time / 5 << " s" << endl;
    exit(0);
    // Print results
    for (size_t i = 0; i < batch_neighbors.size(); ++i) {
        if (batch_neighbors[i].empty()) {
            cout << "Node " << batch_node_id[i] << " at time " << batch_node_time[i] << " has no neighbors." << endl;
            continue;
        }
        cout << "Node " << batch_node_id[i] << " at time " << batch_node_time[i] << " has " << batch_neighbors[i].size() << " neighbors:" << endl;
        for (size_t j = 0; j < batch_neighbors[i].size(); ++j) {
            cout << "  Neighbor: " << batch_neighbors[i][j]
                 << ", Time: " << batch_neighbor_times[i][j]
                 << ", Index: " << batch_neighbor_idx[i][j] << endl;
        }
    }

    return 0;
}