#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_set>

#include "sutil/Hashing.h"

int main() {

    size_t count = 0;
    size_t collisions = 0;

    std::ifstream input("input.res");

    if (!input.is_open()) return 0;

    std::unordered_set<size_t> seen;
    seen.max_load_factor(0.7f);

    size_t hash = 0;

    std::string x;

    while (!input.eof()) {
        input >> x;
        hash = shash::rotl(hash, std::numeric_limits<size_t>::digits / 3) ^ shash::distribute(getHash(x));

        if (seen.find(hash) != seen.end()) {
            collisions++;
        } else {
            seen.insert(hash);
        }

        count++;
    }
    std::cout << "Total: " << count << std::endl;
    std::cout << "Collision # is " << collisions << std::endl;

    return 0;
}
