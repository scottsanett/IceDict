#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

int main() {
    std::cout << ">> ";
    std::string filename;
    std::string output;
    std::cin >> filename;
    output = filename + "output";
    std::string line;
    std::ifstream ifs(filename);
    std::ofstream ofs(output);
    size_t i = 0;
    std::string current = "";
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        std::string temp;
        iss >> temp;
        if (key.back() != ';') {
            key += " " + temp;
        }
        if (key != current) {
            current = key;
            key += " " + std::to_string(i);
            ofs << key << '\n';
        }
        ++i;
    }
}
