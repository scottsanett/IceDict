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
        Infinitive, Supine, Impersonal, Question, Other,
        AccSubject, DatSubject, GenSubject, DumSubject,
        Active, Middle, Mediopassive,
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
        Impersonality, Voice, Mood, SubjectCase,
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
    std::array<const char *, 38> InflName {
        {"Participle", "Present Participle", "Past Participle",
        "Infinitive", "Supine", "Impersonal", "Question", "Other",
        "Accusative Subject", "Dative Subject", "Genitive Subject", "Dummy Subject",
        "Active", "Middle", "Mediopassive",
        "Indicative", "Subjunctive", "Imperative",
        "Present", "Past",
        "First", "Second", "Third",
        "Singular", "Plural", "Clipped",
        "Nominative", "Accusative", "Dative", "Genitive",
        "Masculine", "Feminine", "Neuter",
        "Definite", "Indefinite",
        "Positive", "Comparative", "Superlative"}
    };

    std::array<const char *, 38> InflStruct {
        {"participle", "present-participle", "past-participle",
        "infinitive", "supine", "impersonal", "question", "other",
        "accusative-subject", "dative-subject", "genitive-subject", "dummy-subject",
        "active", "middle", "mediopassive",
        "indicative", "subjunctive", "imperative",
        "present-tense", "past-tense",
        "1st", "2nd", "3rd",
        "singular", "plural", "clipped",
        "nominative", "accusative", "dative", "genitive",
        "masculine", "feminine", "neuter",
        " definite", "indefinite",
        "positive", "comparative", "superlative"}
    };

    std::array<const char *, 12> InflCategory {
        {"Impersonality", "Voice", "Mood", "Subject Case",
        "Tense", "Tense", "Person", "Number",
        "Case", "Gender", "Definiteness", "Degree"}
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
