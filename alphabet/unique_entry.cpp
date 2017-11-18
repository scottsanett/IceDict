#include <set>
#include <fstream>
#include <string>
#include <iostream>

int main() {
    std::cout << ">> ";
    std::string filename;
    std::cin >> filename;
    std::ifstream fin(filename);
    std::ofstream fout(filename + "output");
    std::set<std::string> strset;
    std::string line;
    while (std::getline(fin, line)) {
        strset.insert(line);
    }
    for (auto && i : strset) {
        fout << i << '\n';
    }
}
