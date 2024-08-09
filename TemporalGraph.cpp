// TemporalGraph.cpp - Implementation file for the TemporalGraph class
#include "TemporalGraph.h"
#include "utils.h"

// Default constructor
TemporalGraph::TemporalGraph(bool reverse)
    : edge_idx(), src_list(), dst_list(), time_list(), idx_values(), time_values(), indices(), indptr(), max_node_id(-1), reverse(reverse) {
}

// Parameterized constructor
TemporalGraph::TemporalGraph(const std::vector<GraphDataType>& edge_idx, const std::vector<GraphDataType>& src_list,
                             const std::vector<GraphDataType>& dst_list, const std::vector<GraphDataType>& time_list,
                             bool reverse)
    : edge_idx(edge_idx), src_list(src_list), dst_list(dst_list), time_list(time_list),
      idx_values(), time_values(), indices(), indptr(), reverse(reverse) {
    max_node_id = std::max(
        *std::max_element(src_list.begin(), src_list.end()),
        *std::max_element(dst_list.begin(), dst_list.end())
    );
}

// csr Constructor
TemporalGraph::TemporalGraph(const std::vector<GraphDataType>& idx_values, const std::vector<GraphDataType>& time_values,
                             const std::vector<GraphDataType>& indices, const std::vector<GraphDataType>& indptr)
    : edge_idx(), src_list(), dst_list(), time_list(),
      idx_values(idx_values), time_values(time_values), indices(indices), indptr(indptr), max_node_id(-1), reverse(false) {
}

// Destructor
TemporalGraph::~TemporalGraph() {
    // No explicit need to clear vectors here since they will be destroyed automatically
}

// Add edge to the graph
void TemporalGraph::add_edge(GraphDataType edge_idx, GraphDataType src, GraphDataType dst, GraphDataType time) {
    this->edge_idx.push_back(edge_idx);
    this->src_list.push_back(src);
    this->dst_list.push_back(dst);
    this->time_list.push_back(time);
    this->max_node_id = max(this->max_node_id, max(src, dst));
}

// Get the maximum node ID
GraphDataType TemporalGraph::get_max_node_id() const {
    return this->max_node_id;
}

// Convert to CSR representation
void TemporalGraph::to_csr() {
    double start_time = omp_get_wtime();
    GraphDataType num_nodes = max_node_id + 1;
    indptr.assign(num_nodes + 1, 0);

    if (reverse) {
        // Count the number of edges for each source node considering both src->dst and dst->src
        #pragma omp parallel for
        for (GraphDataType i = 0; i < src_list.size(); ++i) {
            #pragma omp atomic
            indptr[src_list[i] + 1]++;
            #pragma omp atomic
            indptr[dst_list[i] + 1]++;
        }
    } else {
        // Count the number of edges for each source node considering only src->dst
        #pragma omp parallel for
        for (GraphDataType i = 0; i < src_list.size(); ++i) {
            #pragma omp atomic
            indptr[src_list[i] + 1]++;
        }
    }

    // Compute the cumulative sum to get indptr
    for (GraphDataType i = 1; i <= num_nodes; ++i) {
        indptr[i] += indptr[i - 1];
    }

    // Resize idx_values, time_values, and indices to the correct size
    GraphDataType total_edges = reverse ? src_list.size() * 2 : src_list.size();
    idx_values.resize(total_edges);
    time_values.resize(total_edges);
    indices.resize(total_edges);

    // Temporary storage to keep track of the current position in idx_values, time_values, and indices
    vector<GraphDataType> temp_indptr = indptr;

    // Fill idx_values, time_values, and indices
    #pragma omp parallel for
    for (GraphDataType i = 0; i < src_list.size(); ++i) {
        GraphDataType src = src_list[i];
        GraphDataType dst = dst_list[i];
        GraphDataType time = time_list[i];
        GraphDataType idx = edge_idx[i];

        GraphDataType pos_src;

        #pragma omp atomic capture
        pos_src = temp_indptr[src]++;

        idx_values[pos_src] = idx;
        time_values[pos_src] = time;
        indices[pos_src] = dst;

        if (reverse) {
            GraphDataType pos_dst;

            #pragma omp atomic capture
            pos_dst = temp_indptr[dst]++;

            idx_values[pos_dst] = idx;
            time_values[pos_dst] = time;
            indices[pos_dst] = src;
        }
    }

    // Sort time of each indptr by time in ascending order, and rearrange idx_values and indices
    #pragma omp parallel for
    for (GraphDataType i = 0; i < num_nodes; ++i) {
        GraphDataType start = indptr[i];
        GraphDataType end = indptr[i + 1];

        // Skip empty ranges and ranges with only one element
        if (start + 1 >= end) {
            continue;
        }

        // Create a vector of indices for sorting
        vector<GraphDataType> indices_to_sort(end - start);
        iota(indices_to_sort.begin(), indices_to_sort.end(), start);

        // Sort indices based on time_values
        sort(indices_to_sort.begin(), indices_to_sort.end(), [&](GraphDataType a, GraphDataType b) {
            return time_values[a] < time_values[b];
        });

        // Create temporary vectors to hold sorted values
        vector<GraphDataType> sorted_idx_values(end - start);
        vector<GraphDataType> sorted_time_values(end - start);
        vector<GraphDataType> sorted_indices(end - start);

        // Fill the sorted vectors
        #pragma omp parallel for
        for (GraphDataType j = 0; j < end - start; ++j) {
            sorted_idx_values[j] = idx_values[indices_to_sort[j]];
            sorted_time_values[j] = time_values[indices_to_sort[j]];
            sorted_indices[j] = indices[indices_to_sort[j]];
        }

        // Copy the sorted values back to the original vectors
        #pragma omp parallel for
        for (GraphDataType j = 0; j < end - start; ++j) {
            idx_values[start + j] = sorted_idx_values[j];
            time_values[start + j] = sorted_time_values[j];
            indices[start + j] = sorted_indices[j];
        }
    }
    double end_time = omp_get_wtime();
    cout << "The elapsed time for converting to T-CSR graph: " << end_time - start_time << " seconds" << endl;
}

void TemporalGraph::save_csr(const string& idx_values_file_path, const string& time_values_file_path, const string& indices_file_path, const string& indptr_file_path) const {
    // Save idx_values
    ofstream idx_values_file(idx_values_file_path);
    for (GraphDataType idx : idx_values) {
        idx_values_file << idx << endl;
    }
    idx_values_file.close();

    // Save time_values
    ofstream time_values_file(time_values_file_path);
    for (GraphDataType time : time_values) {
        time_values_file << time << endl;
    }
    time_values_file.close();

    // Save indices
    ofstream indices_file(indices_file_path);
    for (GraphDataType index : indices) {
        indices_file << index << endl;
    }
    indices_file.close();

    // Save indptr
    ofstream indptr_file(indptr_file_path);
    for (GraphDataType ptr : indptr) {
        indptr_file << ptr << endl;
    }
    indptr_file.close();
}

void TemporalGraph::read_csr(const string& idx_values_file_path, const string& time_values_file_path, const string& indices_file_path, const string& indptr_file_path) {
    // Read idx_values
    ifstream idx_values_file(idx_values_file_path);
    idx_values.clear();
    GraphDataType idx;
    while (idx_values_file >> idx) {
        idx_values.push_back(idx);
    }
    idx_values_file.close();

    // Read time_values
    ifstream time_values_file(time_values_file_path);
    time_values.clear();
    GraphDataType time;
    while (time_values_file >> time) {
        time_values.push_back(time);
    }
    time_values_file.close();

    // Read indices
    ifstream indices_file(indices_file_path);
    indices.clear();
    GraphDataType index;
    while (indices_file >> index) {
        indices.push_back(index);
    }
    indices_file.close();

    // Read indptr
    ifstream indptr_file(indptr_file_path);
    indptr.clear();
    GraphDataType ptr;
    while (indptr_file >> ptr) {
        indptr.push_back(ptr);
    }
    indptr_file.close();
}

// Get the neighbors of a node at a given time with sampling
void TemporalGraph::get_neighbors(GraphDataType node, GraphDataType time, vector<GraphDataType>& neighbors, vector<GraphDataType>& neighbor_times, vector<GraphDataType>& neighbor_idx, GraphDataType sample_num, const string& sample_strategy) const {
    neighbors.clear();
    neighbor_times.clear();
    neighbor_idx.clear();

    GraphDataType start = indptr[node];
    GraphDataType end = indptr[node + 1];

    // Using binary search to find the first edge with time greater than the given time
    GraphDataType left = start;
    GraphDataType right = end - 1;
    GraphDataType mid;
    while (left <= right) {
        mid = left + (right - left) / 2;
        if (time_values[mid] >= time) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    // If the first edge with time greater than the given time is the first edge of the node, then there is no neighbor
    if (left == start) {
        return;
    }

    // Get the neighbors and their times
    vector<GraphDataType> temp_neighbors;
    vector<GraphDataType> temp_neighbor_times;
    vector<GraphDataType> temp_neighbor_idx;
    for (GraphDataType i = start; i < left; ++i) {
        temp_neighbors.push_back(indices[i]);
        temp_neighbor_times.push_back(time_values[i]);
        temp_neighbor_idx.push_back(idx_values[i]);
    }

    // Sample neighbors based on the strategy
    if (sample_strategy == "recent") {
        // Get the most recent neighbors
        GraphDataType count = min(sample_num, static_cast<GraphDataType>(temp_neighbors.size()));
        for (GraphDataType i = temp_neighbors.size() - count; i < temp_neighbors.size(); ++i) {
            neighbors.push_back(temp_neighbors[i]);
            neighbor_times.push_back(temp_neighbor_times[i]);
            neighbor_idx.push_back(temp_neighbor_idx[i]);
        }
    } else if (sample_strategy == "random") {
        // Randomly sample neighbors
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, temp_neighbors.size() - 1);
        unordered_set<GraphDataType> sampled_indices;
        while (sampled_indices.size() < min(sample_num, static_cast<GraphDataType>(temp_neighbors.size()))) {
            sampled_indices.insert(dis(gen));
        }
        for (GraphDataType index : sampled_indices) {
            neighbors.push_back(temp_neighbors[index]);
            neighbor_times.push_back(temp_neighbor_times[index]);
            neighbor_idx.push_back(temp_neighbor_idx[index]);
        }
    }
}

// based on get_neighbors() function, implement sampling(batch_node_id, batch_node_time) function in parallel for batch sampling
double TemporalGraph::sampling(const vector<GraphDataType>& batch_node_id, const vector<GraphDataType>& batch_node_time,
                            vector<vector<GraphDataType>>& batch_neighbors, vector<vector<GraphDataType>>& batch_neighbor_times,
                            vector<vector<GraphDataType>>& batch_neighbor_idx, GraphDataType sample_num,
                            const string& sample_strategy) {
    double start_time = omp_get_wtime();
    size_t batch_size = batch_node_id.size();
    batch_neighbors.resize(batch_size);
    batch_neighbor_times.resize(batch_size);
    batch_neighbor_idx.resize(batch_size);

    #pragma omp parallel for
    for (size_t b = 0; b < batch_size; ++b) {
        GraphDataType node = batch_node_id[b];
        GraphDataType time = batch_node_time[b];
        auto& neighbors = batch_neighbors[b];
        auto& neighbor_times = batch_neighbor_times[b];
        auto& neighbor_idx = batch_neighbor_idx[b];

        get_neighbors(node, time, neighbors, neighbor_times, neighbor_idx, sample_num, sample_strategy);
    }

    double end_time = omp_get_wtime();
    // cout << "The elapsed time for batch sampling: " << end_time - start_time << " seconds" << endl;

    return end_time - start_time;
}

// Function to print CSR representation sizes (for verification)
void TemporalGraph::print_csr_sizes() const {
    cout << "idx_values size: " << idx_values.size() << endl;
    cout << "time_values size: " << time_values.size() << endl;
    cout << "indices size: " << indices.size() << endl;
    cout << "indptr size: " << indptr.size() << endl;
}