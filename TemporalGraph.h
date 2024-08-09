// TemporalGraph.h - Header file for the TemporalGraph class
#ifndef TemporalGraph_H
#define TemporalGraph_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <omp.h>
#include <fstream>
#include "utils.h"

using namespace std;

class TemporalGraph {
private:
    // Edge list representation
    std::vector<GraphDataType> edge_idx;
    std::vector<GraphDataType> src_list;
    std::vector<GraphDataType> dst_list;
    std::vector<GraphDataType> time_list;

    // CSR representation
    std::vector<GraphDataType> idx_values;
    std::vector<GraphDataType> time_values;
    std::vector<GraphDataType> indices;
    std::vector<GraphDataType> indptr;

    // Maximum node ID
    GraphDataType max_node_id;
    // Flag to consider reverse edges
    bool reverse;

public:
    // Constructors
    TemporalGraph(bool reverse = false);
    // edge list Constructors
    TemporalGraph(const std::vector<GraphDataType>& edge_idx, const std::vector<GraphDataType>& src_list,
                  const std::vector<GraphDataType>& dst_list, const std::vector<GraphDataType>& time_list,
                  bool reverse = false);
    // csr Constructors
    TemporalGraph(const std::vector<GraphDataType>& idx_values, const std::vector<GraphDataType>& time_values,
                  const std::vector<GraphDataType>& indices, const std::vector<GraphDataType>& indptr);


    // Destructor
    ~TemporalGraph();

    // Method declarations
    void add_edge(GraphDataType edge_idx, GraphDataType src, GraphDataType dst, GraphDataType time);
    GraphDataType get_max_node_id() const;

    void to_csr();
    void save_csr(const string& idx_values_file_path, const string& time_values_file_path, const string& indices_file_path, const string& indptr_file_path) const;
    void read_csr(const string& idx_values_file_path, const string& time_values_file_path, const string& indices_file_path, const string& indptr_file_path);

    void get_neighbors(GraphDataType node, GraphDataType time, vector<GraphDataType>& neighbors, vector<GraphDataType>& neighbor_times, vector<GraphDataType>& neighbor_idx, GraphDataType sample_num, const string& sample_strategy) const;
    double sampling(const vector<GraphDataType>& batch_node_id, const vector<GraphDataType>& batch_node_time,
                            vector<vector<GraphDataType>>& batch_neighbors, vector<vector<GraphDataType>>& batch_neighbor_times,
                            vector<vector<GraphDataType>>& batch_neighbor_idx, GraphDataType sample_num = 32,
                            const string& sample_strategy = "recent");
    void print_csr_sizes() const;

};

#endif // TemporalGraph_H
