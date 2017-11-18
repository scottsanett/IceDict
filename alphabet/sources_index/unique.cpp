#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

int main() {
    std::cout << ">> ";
    std::string filename;
    std::cin >> filename;
    std::string output = filename + "output";
    std::ifstream fin(filename);
    std::ofstream fout(output);
    std::string line;
    std::set<std::string> strset;
    std::string currentKey = "";
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string temp;
        iss >> temp;
        if (temp != currentKey) {
            strset.insert(line);
            currentKey = temp;
        }
    }
    for (auto && i : strset) {
        fout << i << '\n';
    }
}
