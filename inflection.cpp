#include "inflection.hpp"
#include <QDebug>

Inflection::Inflection() {
    for (unsigned i = 0; i < InflName.size(); ++i) {
        InflVec.push_back(std::make_pair(InflName.at(i), InflStruct.at(i)));
    }
}

bool Inflection::find(QString const & str, enum Infl::Policy p, enum Infl::Forms f) {
    bool result = false;
    if (p == Infl::Full) {
        result = str.contains(InflName.at(f));
    }
    else if (p == Infl::Short) {
        result = str.contains(InflStruct.at(f));
    }
    return result;
}

QString Inflection::nameOf(enum Infl::Forms f) {
    return InflName.at(f);
}


QString Inflection::structOf(enum Infl::Forms f) {
    return InflStruct.at(f);
}

QString Inflection::categoryOf(enum Infl::Category c) {
    return InflCategory.at(c);
}

Infl::Forms Inflection::enumOfForms(QString const & str) {
    auto itr = std::find(InflName.cbegin(), InflName.cend(), str);
    auto dis = std::distance(InflName.cbegin(), itr);
    return Infl::enumAt<Infl::Forms>(dis);
}

Infl::Category Inflection::enumOfCategory(QString const & str) {
    auto itr = std::find(InflCategory.cbegin(), InflCategory.cend(), str);
    auto dis = std::distance(InflCategory.cbegin(), itr);
    return Infl::enumAt<Infl::Category>(dis);
}
