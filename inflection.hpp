#ifndef INFLECTION_HPP
#define INFLECTION_HPP

#include <array>
#include <utility>
#include <algorithm>
#include <QString>
#include <QVector>

namespace Infl {
    enum Policy { Full, Short };

    enum Forms {
        Participle, PresentParticiple, PastParticiple,
        Infinitive, Supine, Impersonal, Other,
        Active, Middle,
        Indicative, Subjunctive, Imperative,
        Present, Past,
        FirstPerson, SecondPerson, ThirdPerson,
        Singular, Plural, Clipped,
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

    template <typename T>
    T enumAt(long i) {
        return static_cast<T>(i);
    }
};

class Inflection
{
private:
    std::array<const char *, 32> InflName {
        {"Participle", "Present Participle", "Past Participle",
        "Infinitive", "Supine", "Impersonal", "Other",
        "Active", "Middle",
        "Indicative", "Subjunctive", "Imperative",
        "Present", "Past",
        "First", "Second", "Third",
        "Singular", "Plural", "Clipped",
        "Nominative", "Accusative", "Dative", "Genitive",
        "Masculine", "Feminine", "Neuter",
        "Definite", "Indefinite",
        "Positive", "Comparative", "Superlative"}
    };

    std::array<const char *, 32> InflStruct {
        {"part.", "pres. part.", "past part.",
        "infin.", " supine", "impers.", "other",
        "act.", "mid.",
        "indic.", "subj.", "imperat.",
        " pres. ", " past ",
        "1st", "2nd", "3rd",
        "sg.", "pl.", "clipped",
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

    QVector<std::pair<QString, QString>> InflVec;


public:
    Inflection();

    bool find(QString const &, enum Infl::Policy, enum Infl::Forms);
    QString nameOf(enum Infl::Forms);
    QString structOf(enum Infl::Forms);
    QString categoryOf(enum Infl::Category);

    Infl::Forms enumOfForms(QString const &);
    Infl::Category enumOfCategory(QString const &);
};

#endif // INFLECTION_HPP
