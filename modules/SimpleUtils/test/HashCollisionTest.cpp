#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_set>

#include "sutil/Hashing.h"

int main() {

    size_t count = 0;
    size_t collisions = 0;

    std::ifstream input("C:\\Repos\\SimpleCPP\\modules\\SimpleUtils\\test\\input.res");

    if (!input.is_open()) return 0;

    std::unordered_set<size_t> seen;
    std::unordered_set<std::string> seenStrings;
    seen.max_load_factor(0.7f);

    size_t hash = 0;

    std::string x;

    while (!input.eof()) {
        input >> x;
        hash = getHash(x);

        if (seenStrings.find(x) == seenStrings.end()) {
            if (seen.find(hash) != seen.end()) {
                collisions++;
            } else {
                seen.insert(hash);
            }
            seenStrings.insert(x);
        }

        count++;
    }
    std::cout << "Total: " << count << std::endl;
    std::cout << "Collision # is " << collisions << std::endl;

    return 0;
}
