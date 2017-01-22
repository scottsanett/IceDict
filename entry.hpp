#ifndef __ENTRY_HPP__
#define __ENTRY_HPP__
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

using vec_t = std::vector<std::string>;

class Entry {
	std::string name;
	vec_t definition;
public:
	Entry() = default;
	Entry(std::string str, std::vector<std::string> vec): name(str), definition(vec){}
	std::shared_ptr<vec_t> getDef() const { return std::make_shared<vec_t>(definition); }
};

#endif
