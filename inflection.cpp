#include "inflection.hpp"

Inflection::Inflection() {
    for (unsigned i = 0; i < InflName.size(); ++i) {
        InflVec.push_back(std::make_pair(InflName.at(i), InflStruct.at(i)));
    }
}

bool Inflection::find(std::string const & str, enum Infl::Policy p, enum Infl::Forms f) {
    bool result = false;
    if (p == Infl::Full) {
        result = str.find(InflName.at(f)) == std::string::npos;
    }
    else if (p == Infl::Short) {
        result = str.find(InflStruct.at(f)) == std::string::npos;
    }
    return result;
}

std::string Inflection::nameOf(enum Infl::Forms f) {
    return InflName.at(f);
}

std::string Inflection::structOf(enum Infl::Forms f) {
    return InflStruct.at(f);
}

std::string Inflection::categoryOf(enum Infl::Category c) {
    return InflCategory.at(c);
}
