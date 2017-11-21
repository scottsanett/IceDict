#ifndef INFLECTION_HPP
#define INFLECTION_HPP

#include <string>
#include <vector>
#include <array>
#include <utility>
#include <map>

namespace Infl {
    enum Policy { Full, Short };

    enum Forms {
        Participle, PresentParticiple, PastParticiple,
        Infinitive, Supine, Impersonal, Other,
        Active, Middle,
        Indicative, Subjunctive, Imperative,
        Present, Past,
        FirstPerson, SecondPerson, ThirdPerson,
        Singular, Plural,
        Nominative, Accusative, Dative, Genitive,
        Masculine, Feminine, Neuter,
        Definite, Indefinite,
        Positive, Comparative, Superlative
    };

    enum Category {
        Impersonality, Voice, Mood,
        Tense, ParticipleTense, Person, Number,
        Case, Gender, Definiteness, Form
    };

};

class Inflection
{
private:
    std::array<const char *, 31> InflName {
        {"Participle", "Present", "Past",
        "Infinitive", "Supine", "Impersonal", "Other",
        "Active", "Middle",
        "Indicative", "Subjunctive", "Imperative",
        "Present", "Past",
        "First", "Second", "Third",
        "Singular", "Plural",
        "Nominative", "Accusative", "Dative", "Genitive",
        "Masculine", "Feminine", "Neuter",
        "Definite", "Indefinite",
        "Positive", "Comparative", "Superlative"}
    };

    std::array<const char *, 31> InflStruct {
        {"part.", "pres. part.", "past part.",
        "infin.", " supine", "impers.", "other",
        "act.", "mid.",
        "indic.", "subj.", "imperat.",
        " pres. ", " past ",
        "1st", "2nd", "3rd",
        "sg.", "pl.",
        "nom.", "acc.", "dat.", "gen.",
        " m.", " f.", " n.",
        " def.", " ind.",
        "pos.", "comp.", "superl."}
    };

    std::array<const char *, 11> InflCategory {
        {"Impersonality", "Voice", "Mood",
        "Tense", "Tense", "Person", "Number",
        "Case", "Gender", "Definiteness", "Form"}
    };


    std::vector<std::pair<std::string, std::string>> InflVec;

public:
    Inflection();

    bool find(std::string const &, enum Infl::Policy, enum Infl::Forms);
    std::string nameOf(enum Infl::Forms);
    std::string structOf(enum Infl::Forms);
    std::string categoryOf(enum Infl::Category);
};

#endif // INFLECTION_HPP
