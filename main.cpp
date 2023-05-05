#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "kd_tree.h"

int main() {
    // Load data from file
    std::string filename = "large_data10k.txt";
    std::vector<ReferencePoint> data = load_data(filename);

    // Build kd tree
    KdTree tree = build_kd_tree(data, 0);

    double query_lat, query_lon;
    int k;
    while (true) {
        std::cout << "Enter latitude, longitude, and the number of nearest points (k): ";
        std::cin >> query_lat >> query_lon >> k;

        std::priority_queue<std::pair<double, ReferencePoint>> nearest_points;

        // Measure time for find_nearest_k_points (kd tree method)
        auto start1 = std::chrono::high_resolution_clock::now();
        find_nearest_k_points(tree, query_lat, query_lon, k, nearest_points);
        auto end1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed1 = end1 - start1;

        // Measure time for find_nearest_k_points_heap (linear scan method)
        auto start2 = std::chrono::high_resolution_clock::now();
        find_nearest_k_points_heap(data, query_lat, query_lon, k);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed2 = end2 - start2;

        // Output the time taken by each method
        std::cout << "The time taken by find_nearest_k_points (kd tree): " << elapsed1.count() << " seconds" << std::endl;
        std::cout << "The time taken by find_nearest_k_points_heap (linear scan): " << elapsed2.count() << " seconds" << std::endl;
    }
}
