#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_set>

#include "sutil/Hashing.h"

enum e1 {};
enum class e2 {};
enum class e3 : unsigned {};
enum class e4 : int {};

int main() {

    size_t size = 100;
    std::string str("Hey Man");
    size_t size2 = 5000;

    {
        CHashArchive hashArchive;
        hashArchive << size << str << size2;

        const size_t hash = hashArchive.get();

        std::cout << hash << std::endl;
    }

    /*size_t count = 0;
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
    std::cout << "Total: " << count << " With " << (count - seenStrings.size()) << " Duplicates" << std::endl;
    std::cout << "Collision # is " << collisions << " of " << seenStrings.size() << std::endl;*/

    return 0;
}
