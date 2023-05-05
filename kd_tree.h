#pragma once
#ifndef KD_TREE_H
#define KD_TREE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <chrono>

class ReferencePoint {
public:
    std::string state_alpha;
    std::string county_name;
    double lat;
    double lon;
    int axis; // 0 for latitude, 1 for longitude

    ReferencePoint() : axis(0) {}

    ReferencePoint(std::string state_alpha, std::string county_name, double lat, double lon, int axis)
        : state_alpha(state_alpha), county_name(county_name), lat(lat), lon(lon), axis(axis) {}

    bool operator<(const ReferencePoint& other) const {
        if (axis == 0) {
            return lat < other.lat;
        }
        else {
            return lon < other.lon;
        }
    }
};
class KdNode {
public:
    ReferencePoint point;
    std::shared_ptr<KdNode> left;
    std::shared_ptr<KdNode> right;

    KdNode(const ReferencePoint& point) : point(point), left(nullptr), right(nullptr) {}
};
// Define the QueryResult class, which contains query result information: state abbreviation, county name, and distance
class QueryResult {
public:
    QueryResult(std::string state_alpha,
        std::string county_name,
        double distance) : state_alpha(state_alpha), county_name(county_name), distance(distance) {}
    std::string state_alpha;
    std::string county_name;
    double distance;
};

using KdTree = std::shared_ptr<KdNode>;

KdTree build_kd_tree(std::vector<ReferencePoint>& points, int depth);
double equirectangular_distance(double lat1, double lon1, double lat2, double lon2);
void find_nearest_k_points(KdTree tree, double query_lat, double query_lon, int k, std::priority_queue<std::pair<double, ReferencePoint>>& nearest_points);
std::vector<ReferencePoint> load_data(const std::string& filename);
std::string vote_majority(const std::vector<QueryResult>& nearest_points, int k);
std::vector<QueryResult> find_nearest_k_points_heap(const std::vector<ReferencePoint>& data, double query_lat, double query_lon, int k);

#endif // KD_TREE_H
