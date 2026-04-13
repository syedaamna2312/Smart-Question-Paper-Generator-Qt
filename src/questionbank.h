#ifndef QUESTIONBANK_H
#define QUESTIONBANK_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <stdexcept>

// --- Question struct ---
struct Question {
    int id;
    std::string course;
    std::string topic;
    std::string text;
    std::string difficulty; // "Easy", "Medium", "Hard"
    std::string CLO;        // "CLO1", "CLO2", "CLO3"
    int marks;

    Question() : id(0), marks(0) {}

    bool operator==(const Question &other) const { return id == other.id; }
    bool operator!=(const Question &other) const { return !(*this == other); }
};

// --- Topic Trie ---
class TopicTrie {
    struct Node {
        bool end = false;
        std::map<char, Node*> next;
    };
    Node* root;
    void collect(const Node* node, const std::string &prefix, std::vector<std::string> &out) const;

public:
    TopicTrie();
    ~TopicTrie();
    void insert(const std::string &word);
    std::vector<std::string> suggest(const std::string &prefix) const;
};

// --- QuestionBank class ---
class QuestionBank {
    std::unordered_map<int, Question> questionsById;
    std::unordered_map<std::string, std::set<int>> topicIndex;
    TopicTrie trie;
    int nextId = 1;
    std::unordered_set<int> usedQuestions;

    QuestionBank() = default;

public:
    static QuestionBank& instance();
    QuestionBank(const QuestionBank&) = delete;
    QuestionBank& operator=(const QuestionBank&) = delete;

    int addQuestion(const std::string &course, const std::string &topic,
                    const std::string &text, const std::string &difficulty,
                    const std::string &CLO, int marks);

    void markUsed(int questionId);
    bool isUsed(int questionId) const;
    void resetUsedFlags();

    std::vector<std::string> suggestTopics(const std::string &prefix) const;
    const Question& getQuestionById(int id) const;
    std::vector<Question> getCandidatesForCourseTopics(const std::string &course,
                                                       const std::vector<std::string> &topics = {}) const;
    std::vector<Question> getAllQuestionsFlat() const;

    size_t totalQuestions() const { return questionsById.size(); }
};

#endif // QUESTIONBANK_H