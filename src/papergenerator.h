#ifndef PAPERGENERATOR_H
#define PAPERGENERATOR_H

#include "questionbank.h"
#include <vector>
#include <string>

struct PaperResult {
    std::vector<Question> selected;
    int remainingMarks = 0;
    bool success = false;
};

struct RatioSpec {
    int easyPerc;
    int mediumPerc;
    int hardPerc;
    int clo1Perc;
    int clo2Perc;
    int clo3Perc;
};

class PaperGenerator {
    QuestionBank* qb = nullptr;

public:
    // Add this constructor
    PaperGenerator(QuestionBank* instance) : qb(instance) {}
    PaperResult generatePaper(const std::string &course,
                              const std::vector<std::string> &topics,
                              int totalMarks,
                              const RatioSpec &ratios);


    bool backtrackSelect(
        std::vector<Question> &candidates,
        std::vector<Question> &selected,
        int idx,
        int totalMarks,
        int currentMarks,
        int reqEasy, int reqMed, int reqHard,
        int reqCLO1, int reqCLO2, int reqCLO3
    );
};

#endif // PAPERGENERATOR_H