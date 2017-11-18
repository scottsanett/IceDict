#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    std::cout << ">> ";
    std::string filename;
    std::cin >> filename;
    std::ifstream fin(filename);
    std::ofstream fout(filename + "output");
    std::string line;
    while (std::getline(fin, line)) {
        std::string key;
        std::istringstream iss(line);
        iss >> key;
        fout << key << '\n';
    }
}
