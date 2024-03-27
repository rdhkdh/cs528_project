#include <iostream>
#include <vector>
#include <algorithm>

struct Task {
    int arrival;
    int execution_time;
    int deadline;
    double failure_probability;
    // Assuming you have a definition for User
    // User* owner;
};

// Custom comparison function for sorting tasks by failure probability in decreasing order
bool compareByFailureProbability(const Task& a, const Task& b) {
    return a.failure_probability > b.failure_probability;
}

int main() {
    // Sample vector of Task objects
    std::vector<Task> tasks = {
        {1, 10, 5, 0.2},
        {2, 8, 4, 0.1},
        {3, 12, 6, 0.3}
    };

    // Sort the vector in decreasing order of failure probability
    std::sort(tasks.begin(), tasks.end(), compareByFailureProbability);

    // Print the sorted tasks
    std::cout << "Sorted tasks in decreasing order of failure probability:" << std::endl;
    for (const auto& task : tasks) {
        std::cout << "Failure Probability: " << task.failure_probability << std::endl;
        // Print other task details if needed
    }

    return 0;
}
