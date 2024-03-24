
#include <random>
#include <string>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "hashmap.h"
#include "gtest/gtest.h"
#include "test_settings.h"

using clock_type = std::chrono::high_resolution_clock;
using ns = std::chrono::nanoseconds;

std::string print_with_commas(long long int n) {
    std::string ans = "";
    // Convert the given integer
    // to equivalent string
    std::string num = std::to_string(n);

    // Initialise count
    int count = 0;

    // Traverse the string in reverse
    for (int i = num.size() - 1; i >= 0; i--) {
        count++;
        ans.push_back(num[i]);

        // If three characters
        // are traversed
        if (count == 3) {
            ans.push_back(',');
            count = 0;
        }
    }

    // Reverse the string to get
    // the desired output
    reverse(ans.begin(), ans.end());

    // If the given string is
    // less than 1000
    if (ans.size() % 4 == 0) {
        // Remove ','
        ans.erase(ans.begin());
    }
    return ans;
}

#if RUN_TEST_PERF
void benchmark_insert_erase() {
    std::cout << "Task: insert then erase N elements, measured in ns." << '\n';
    auto good_hash_function = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    std::vector<size_t> my_map_timing;
    std::vector<int> sizes{10, 100, 1000, 10000, 100000, 1000000};
    for (size_t size : sizes) {
        std::vector<int> million;
        for (size_t i = 0; i < size; i++) {
            million.push_back(i);
        }
        auto rng = std::default_random_engine {};
        std::shuffle(million.begin(), million.end(), rng);
        size_t my_map_result, std_map_result;
        {
            auto my_start = clock_type::now();

            HashMap<int, int, decltype(good_hash_function)> my_map(size, good_hash_function);
            for (int element : million) {
                my_map.insert({element, element});
            }

            for (int element : million) {
                my_map.erase(element);
            }

            auto my_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(my_end - my_start);

            my_map_result = end.count();
        }

        {
            auto std_start = clock_type::now();

            std::unordered_map<int, int, decltype(good_hash_function)> std_map(size, good_hash_function);
            for (int element : million) {
                std_map.insert({element, element});
            }

            for (int element : million) {
                std_map.erase(element);
            }

            auto std_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(std_end - std_start);

            std_map_result = end.count();
        }

        std::cout << "size "  << std::setw(10) << size;
        std::cout << " | HashMap: " <<  std::setw(13) << print_with_commas(my_map_result);
        std::cout << " | std:unordered_map: "  << std::setw(13) << print_with_commas(std_map_result) << '\n';
        my_map_timing.push_back(my_map_result);
    }
    EXPECT_TRUE(10*my_map_timing[0] < my_map_timing[3]); // Ensure runtime of N = 10 is much faster than N = 10000
}

void benchmark_find() {
    std::cout << "Task: find N elements (random hit/miss), measured in ns." << '\n';
    auto good_hash_function = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    std::vector<size_t> my_map_timing;
    std::vector<int> sizes{10, 100, 1000, 10000, 100000, 1000000};
    for (size_t size : sizes) {
        std::vector<int> million;
        std::vector<int> lookup;
        for (size_t i = 0; i < 2*size; i++) {
            million.push_back(i);
            lookup.push_back(i);
        }
        auto rng = std::default_random_engine {};
        std::shuffle(million.begin(), million.end(), rng);
        std::shuffle(lookup.begin(), lookup.end(), rng);
        size_t my_map_result, std_map_result;
        {

            HashMap<int, int, decltype(good_hash_function)> my_map(size, good_hash_function);
            for (size_t i = 0; i < million.size(); i += 2) {
                int element = million[i];
                my_map.insert({element, element});
            }
            auto my_start = clock_type::now();
            int count = 0;
            for (size_t i = 0; i < lookup.size(); i += 2) {
                int element = lookup[i];
                auto found = my_map.find(element);
                count += (found == my_map.end());
            }

            auto my_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(my_end - my_start);

            my_map_result = end.count();
        }

        {
            std::unordered_map<int, int, decltype(good_hash_function)> std_map(size, good_hash_function);
            for (size_t i = 0; i < million.size(); i += 2) {
                int element = million[i];
                std_map.insert({element, element});
            }
            auto std_start = clock_type::now();
            int count = 0;
            for (size_t i = 0; i < lookup.size(); i += 2) {
                int element = lookup[i];
                auto found = std_map.find(element);
                count += (found == std_map.end());
            }

            auto std_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(std_end - std_start);

            std_map_result = end.count();
        }

        std::cout << "size "  << std::setw(10) << size;
        std::cout << " | HashMap: " <<  std::setw(13) << print_with_commas(my_map_result);
        std::cout << " | std:unordered_map: "  << std::setw(13) << print_with_commas(std_map_result) << '\n';
        my_map_timing.push_back(my_map_result);
    }
    EXPECT_TRUE(10*my_map_timing[0] < my_map_timing[3]); // Ensure runtime of N = 10 is much faster than N = 10000
}

void benchmark_iterate() {
    std::cout << "Task: iterate over all N elements, measured in ns." << '\n';
    auto good_hash_function = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    std::vector<size_t> my_map_timing;
    std::vector<size_t> std_map_timing;
    std::vector<int> sizes{10, 100, 1000, 10000, 100000, 1000000};
    for (size_t size : sizes) {
        std::vector<int> million;
        for (size_t i = 0; i < size; i++) {
            million.push_back(i);
        }
        auto rng = std::default_random_engine {};
        std::shuffle(million.begin(), million.end(), rng);
        size_t my_map_result, std_map_result;
        {
            HashMap<int, int, decltype(good_hash_function)> my_map(size, good_hash_function);
            for (int element : million) {
                my_map.insert({element, element});
            }

            auto my_start = clock_type::now();
            size_t count = 0;
            for (const auto& [key, value] : my_map) {
                count += key;
            }
            auto my_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(my_end - my_start);

            my_map_result = end.count();
        }

        {
            std::unordered_map<int, int, decltype(good_hash_function)> std_map(size, good_hash_function);
            for (int element : million) {
                std_map.insert({element, element});
            }

            auto std_start = clock_type::now();
            size_t count = 0;
            for (const auto& [key, value] : std_map) {
                count += key;
            }
            auto std_end = clock_type::now();
            auto end = std::chrono::duration_cast<ns>(std_end - std_start);

            std_map_result = end.count();
        }

        std::cout << "size "  << std::setw(10) << size;
        std::cout << " | HashMap: " <<  std::setw(13) << print_with_commas(my_map_result);
        std::cout << " | std:unordered_map: "  << std::setw(13) << print_with_commas(std_map_result) << '\n';
        my_map_timing.push_back(my_map_result);
    }
    EXPECT_TRUE(10*my_map_timing[0] < my_map_timing[3]); // Ensure runtime of N = 10 is much faster than N = 10000
}
#endif

int main() {
    std::cout << "Performance Test: " << std::endl;
#if RUN_TEST_PERF
    benchmark_find();
    benchmark_insert_erase();
    benchmark_iterate();
#endif
    return 0;
}