#define _USE_MATH_DEFINES
#include "kd_tree.h"

// Add implementation of functions from the header file
KdTree build_kd_tree(std::vector<ReferencePoint>& points, int depth = 0) {
    if (points.empty()) {
        return nullptr;
    }

    size_t n = points.size();
    int axis = depth % 2;

    // 根据轴对点进行排序
    for (auto& point : points) {
        point.axis = axis;
    }
    std::sort(points.begin(), points.end());

    KdTree node = std::make_shared<KdNode>(points[n / 2]);
    std::vector<ReferencePoint> left_points(points.begin(), points.begin() + n / 2);
    std::vector<ReferencePoint> right_points(points.begin() + n / 2 + 1, points.end());
    node->left = build_kd_tree(left_points, depth + 1);
    node->right = build_kd_tree(right_points, depth + 1);

    return node;
}

double equirectangular_distance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371e3; // meters
    const double deg_to_rad = M_PI / 180.0;

    double phi1 = lat1 * deg_to_rad;
    double phi2 = lat2 * deg_to_rad;
    double delta_phi = (lat2 - lat1) * deg_to_rad;
    double delta_lambda = (lon2 - lon1) * deg_to_rad;

    double a = std::sin(delta_phi / 2) * std::sin(delta_phi / 2) +
        std::cos(phi1) * std::cos(phi2) *
        std::sin(delta_lambda / 2) * std::sin(delta_lambda / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    double d = R * c; // in meters
    return d / 1000; // return distance in kilometers
}

void find_nearest_k_points(KdTree tree, double query_lat, double query_lon, int k, std::priority_queue<std::pair<double, ReferencePoint>>& nearest_points) {
    if (!tree) {
        return;
    }

    ReferencePoint& point = tree->point;
    double distance = equirectangular_distance(query_lat, query_lon, point.lat, point.lon);
    nearest_points.push({ distance, point });

    if (nearest_points.size() > k) {
        nearest_points.pop();
    }

    double delta;
    if (point.axis == 0) {
        delta = query_lat - point.lat;
    }
    else {
        delta = query_lon - point.lon;
    }

    KdTree near_subtree, far_subtree;
    if (delta <= 0) {
        near_subtree = tree->left;
        far_subtree = tree->right;
    }
    else {
        near_subtree = tree->right;
        far_subtree = tree->left;
    }

    find_nearest_k_points(near_subtree, query_lat, query_lon, k, nearest_points);

    if (nearest_points.top().first >= std::abs(delta)) {
        find_nearest_k_points(far_subtree, query_lat, query_lon, k, nearest_points);
    }
}

std::vector<ReferencePoint> load_data(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<ReferencePoint> data;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        ReferencePoint point;
        iss >> point.state_alpha;

        // Read county name, which may contain spaces
        std::string word;
        std::stringstream county_name_ss;
        while (iss >> word) {
            try {
                point.lat = std::stod(word);
                break;
            }
            catch (const std::invalid_argument&) {
                if (!county_name_ss.str().empty()) {
                    county_name_ss << " ";
                }
                county_name_ss << word;
            }
        }

        point.county_name = county_name_ss.str();
        iss >> point.lon;
        data.push_back(point);
    }
    return data;
}

std::string vote_majority(const std::vector<QueryResult>& nearest_points, int k) {
    std::unordered_map<std::string, int> state_county_count;

    for (int i = 0; i < k; ++i) {
        std::string state_county = nearest_points[i].state_alpha;
        // + " " + nearest_points[i].county_name
        state_county_count[state_county]++;
    }

    int max_count = 0;
    std::string majority_state_county;
    for (const auto& entry : state_county_count) {
        if (entry.second > max_count) {
            max_count = entry.second;
            majority_state_county = entry.first;
        }
    }

    return majority_state_county;
}

std::vector<QueryResult> find_nearest_k_points_heap(const std::vector<ReferencePoint>& data, double query_lat, double query_lon, int k) {
    k = std::max(k, 5);
    auto cmp = [query_lat, query_lon](const ReferencePoint& a, const ReferencePoint& b) {
        return equirectangular_distance(query_lat, query_lon, a.lat, a.lon) < equirectangular_distance(query_lat, query_lon, b.lat, b.lon);
    };

    std::priority_queue<ReferencePoint, std::vector<ReferencePoint>, decltype(cmp)> maxHeap(cmp);

    for (const ReferencePoint& point : data) {
        double dist = equirectangular_distance(query_lat, query_lon, point.lat, point.lon);
        if (maxHeap.size() < k) {
            maxHeap.push(point);
        }
        else if (dist < equirectangular_distance(query_lat, query_lon, maxHeap.top().lat, maxHeap.top().lon)) {
            maxHeap.pop();
            maxHeap.push(point);
        }
    }
    std::vector<QueryResult> nearestNeighbors;
    while (!maxHeap.empty()) {
        const ReferencePoint& point = maxHeap.top();
        double dist = equirectangular_distance(query_lat, query_lon, point.lat, point.lon);
        nearestNeighbors.push_back({ point.state_alpha, point.county_name, dist });
        maxHeap.pop();
    }

    std::reverse(nearestNeighbors.begin(), nearestNeighbors.end());

    return nearestNeighbors;
}
