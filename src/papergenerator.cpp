#include "papergenerator.h"
#include "questionbank.h"
#include <algorithm>
#include <numeric>
#include <set>
#include <queue>  // for priority_queue

using namespace std;

PaperResult PaperGenerator::generatePaper(
    const string &course,
    const vector<string> &topics,
    int totalMarks,
    const RatioSpec &ratios
) {
    PaperResult result;

    // Compute required marks per category
    int reqEasy   = (totalMarks * ratios.easyPerc) / 100;
    int reqMedium = (totalMarks * ratios.mediumPerc) / 100;
    int reqHard   = (totalMarks * ratios.hardPerc) / 100;
    int reqCLO1 = (totalMarks * ratios.clo1Perc) / 100;
    int reqCLO2 = (totalMarks * ratios.clo2Perc) / 100;
    int reqCLO3 = (totalMarks * ratios.clo3Perc) / 100;

    int allocated = reqEasy + reqMedium + reqHard;
    if (allocated < totalMarks) reqMedium += (totalMarks - allocated);
    allocated = reqCLO1 + reqCLO2 + reqCLO3;
    if (allocated < totalMarks) reqCLO2 += (totalMarks - allocated);

    // Get all candidate questions from QuestionBank
    vector<Question> filtered = qb->getCandidatesForCourseTopics(course, topics);

    // Sort descending by marks for backtracking efficiency
    sort(filtered.begin(), filtered.end(), [](const Question &a, const Question &b) {
        return a.marks > b.marks;
    });

    vector<Question> selected; 
    
    bool success = backtrackSelect(
        filtered, selected, 0, totalMarks, 0,
        reqEasy, reqMedium, reqHard,
        reqCLO1, reqCLO2, reqCLO3
    );

    int usedMarks = accumulate(selected.begin(), selected.end(), 0,
        [](int sum, const Question &q){ return sum + q.marks; });

    int leftoverMarks = totalMarks - usedMarks;

    // -----------------------------
    // Greedy leftover filling (min-heap, optimized)
    // -----------------------------
    if (leftoverMarks > 0) {
        auto cmp = [](const Question &a, const Question &b){ return a.marks > b.marks; };
        priority_queue<Question, vector<Question>, decltype(cmp)> minHeap(cmp);

        // Push only unused questions into minHeap
        for (const auto &q : filtered) {
        // Check if q.id is already in selected vector
        bool alreadySelected = std::find_if(selected.begin(), selected.end(),
        [&](const Question &s){ return s.id == q.id; }) != selected.end();

        // If not selected and not used in QuestionBank, push into minHeap
        if (!alreadySelected && !qb->isUsed(q.id)) {
        minHeap.push(q);
        }
    }

        while (!minHeap.empty() && leftoverMarks > 0) {
            Question q = minHeap.top(); minHeap.pop();
            if (q.marks <= leftoverMarks) {
                selected.push_back(q);
                leftoverMarks -= q.marks;
                qb->markUsed(q.id);  // mark immediately
            }
        }
    }

    // Mark already selected questions (from backtracking) as used
    for (auto &q : selected) qb->markUsed(q.id);

    result.selected = selected;
    result.remainingMarks = leftoverMarks;
    result.success = (leftoverMarks == 0);
    return result;
}

// -----------------------------------
// backtrackSelect definition
// -----------------------------------
bool PaperGenerator::backtrackSelect(
    vector<Question> &candidates,
    vector<Question> &selected,
    int idx,
    int totalMarks,
    int currentMarks,
    int reqEasy, int reqMed, int reqHard,
    int reqCLO1, int reqCLO2, int reqCLO3
) {
    if (currentMarks == totalMarks) return true;
    if (idx >= (int)candidates.size()) return false;

    Question &q = candidates[idx];

    // Skip if question already used in QuestionBank
    if (qb->isUsed(q.id)) return backtrackSelect(candidates, selected, idx + 1, totalMarks, currentMarks,
                                                reqEasy, reqMed, reqHard,
                                                reqCLO1, reqCLO2, reqCLO3);

    // Check difficulty quotas
    bool diffOK = false;
    if (q.difficulty == "Easy" && reqEasy >= q.marks) diffOK = true;
    if (q.difficulty == "Medium" && reqMed >= q.marks) diffOK = true;
    if (q.difficulty == "Hard" && reqHard >= q.marks) diffOK = true;
    if (!diffOK) return backtrackSelect(candidates, selected, idx + 1, totalMarks, currentMarks,
                                        reqEasy, reqMed, reqHard,
                                        reqCLO1, reqCLO2, reqCLO3);

    // Check CLO quotas
    bool cloOK = false;
    if (q.CLO == "CLO1" && reqCLO1 >= q.marks) cloOK = true;
    if (q.CLO == "CLO2" && reqCLO2 >= q.marks) cloOK = true;
    if (q.CLO == "CLO3" && reqCLO3 >= q.marks) cloOK = true;
    if (!cloOK) return backtrackSelect(candidates, selected, idx + 1, totalMarks, currentMarks,
                                       reqEasy, reqMed, reqHard,
                                       reqCLO1, reqCLO2, reqCLO3);

    // Pick this question
    selected.push_back(q);
    int newMarks = currentMarks + q.marks;
    int newEasy = reqEasy, newMed = reqMed, newHard = reqHard;
    int newCLO1 = reqCLO1, newCLO2 = reqCLO2, newCLO3 = reqCLO3;

    // Deduct quotas
    if (q.difficulty == "Easy") newEasy -= q.marks;
    if (q.difficulty == "Medium") newMed -= q.marks;
    if (q.difficulty == "Hard") newHard -= q.marks;

    if (q.CLO == "CLO1") newCLO1 -= q.marks;
    if (q.CLO == "CLO2") newCLO2 -= q.marks;
    if (q.CLO == "CLO3") newCLO3 -= q.marks;

    // Recursive call
    if (newMarks <= totalMarks && backtrackSelect(candidates, selected, idx + 1, totalMarks, newMarks,
                                                  newEasy, newMed, newHard,
                                                  newCLO1, newCLO2, newCLO3))
        return true;

    // Backtrack: remove question
    selected.pop_back();

    // Try skipping this question
    return backtrackSelect(candidates, selected, idx + 1, totalMarks, currentMarks,
                           reqEasy, reqMed, reqHard,
                           reqCLO1, reqCLO2, reqCLO3);
}