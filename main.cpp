#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <unordered_map>

// Define the ReferencePoint class, which contains geographical data information: state abbreviation, county name, latitude, and longitude
class ReferencePoint {
public:
    std::string state_alpha;
    std::string county_name;
    double lat;
    double lon;
};

// Define the QueryResult class, which contains query result information: state abbreviation, county name, and distance
class QueryResult {
public:
    std::string state_alpha;
    std::string county_name;
    double distance;
};

// Calculate the distance between two coordinates
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

// Load geographical data from a file
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


// Find k nearest geographical data points based on the input coordinates
std::vector<QueryResult> find_nearest_k_points(const std::vector<ReferencePoint>& data, double query_lat, double query_lon, int k) {
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

// Vote for the most frequent state and county based on the k nearest geographical data points
std::string vote_majority(const std::vector<QueryResult>& nearest_points, int k) {
    std::unordered_map<std::string, int> state_county_count;

    for (int i = 0; i < k; ++i) {
        std::string state_county = nearest_points[i].state_alpha ;
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

int main() {
    std::string filename = "data.txt";
    std::vector<ReferencePoint> data = load_data(filename);
    double query_lat, query_lon;
    int k;
    while (true) {
        std::cout << "Enter latitude, longitude, and the number of nearest points (k): ";
        std::cin >> query_lat >> query_lon >> k;
        std::vector<QueryResult> nearest_points = find_nearest_k_points(data, query_lat, query_lon, k);
        std::cout << "The nearest " << k << " reference points are:" << std::endl;
        for (int i = 0; i < k; ++i) {
            QueryResult result = nearest_points[i];
            std::cout << result.state_alpha << " " << result.county_name << " " << result.distance << "Km" << std::endl;
        }

        std::string majority_state_county = vote_majority(nearest_points, k);
        std::cout << "The majority-voted state for the given coordinates are: " << majority_state_county << std::endl;
    } 
}