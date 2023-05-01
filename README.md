# EC504_2023_NP

Nearest Province Finder



This program is designed to find the k nearest geographical points from a dataset, given a set of input coordinates (latitude and longitude). It then uses a voting mechanism to determine the majority-voted state and county from the k nearest points.

## Data Structures

- `ReferencePoint`: A class that represents a geographical point. It contains the state abbreviation (state_alpha), county name (county_name), latitude (lat), and longitude (lon).
- `QueryResult`: A class that stores the result of the k nearest points search. It contains the state abbreviation (state_alpha), county name (county_name), and distance from the input coordinates.

## Solution - Priority Queue Approach

1. Define a custom comparison function (cmp) for the priority queue that compares two ReferencePoint objects based on their distances from the input coordinates (query_lat, query_lon). This function will be used to maintain the max-heap property in the priority queue.

2. Create an empty priority queue (max-heap) using the custom comparison function.

3. Iterate through the dataset:

   a. For each ReferencePoint in the dataset, calculate its distance from the input coordinates using the equirectangular_distance function.

   b. If the priority queue contains fewer than k elements, push the current ReferencePoint into the priority queue.

   c. If the priority queue has k elements, compare the current ReferencePoint's distance to the distance of the farthest ReferencePoint (top of the max-heap). If the current distance is smaller, remove the top element from the priority queue and push the current ReferencePoint.

4. When the iteration is complete, the priority queue will contain the k nearest ReferencePoint objects. Extract the elements from the priority queue and store them in a vector of QueryResult objects, which contains the state abbreviation, county name, and distance of each nearest point.

The advantage of using a priority queue is that it maintains the k nearest points during the iteration without the need to sort the entire dataset. This results in a time complexity of O(n log k), where n is the number of points in the dataset, and k is the number of nearest points to find. The space complexity is O(k), as the priority queue only needs to store the k nearest points in memory.

## Majority Voting

The majority voting method is used to determine the most common state and county among the k nearest points. Note that the number of nearest points (k) must be at least 5 to ensure a more accurate and reliable result.

1. Create an unordered_map to store the count of each unique state-county combination.
2. Iterate through the k nearest points and increment the count of the corresponding state-county combination in the unordered_map.
3. Find the state-county combination with the maximum count in the unordered_map.
4. The majority-voted state and county are the combination with the highest count.

## Trade Off

Our current implementation, which uses a priority queue, has several advantages:

- It has a time complexity of O(n log k), where n is the number of points in the dataset, and k is the number of nearest points to find. This is more efficient than the brute force approach.
- It has a space complexity of O(k), as it only needs to maintain the k nearest points in memory.

However, there are some potential improvements:

- Using a k-d tree could further improve the time complexity to O(n log n) for building the tree and O(log n) for each nearest neighbor search. This would be more efficient for large datasets and multiple queries.
- Parallelizing the distance calculations and processing could also lead to performance improvements, especially on multi-core processors.

## Conclusion

This program efficiently finds the k nearest geographical points by maintaining a max-heap data structure while iterating through the dataset using the priority queue approach. The solution has a reduced time complexity compared to the brute force approach and a relatively low space complexity, making it suitable for this problem. The majority voting mechanism is then employed to determine the most common state and county among the k nearest points, ensuring a more accurate and reliable result.
