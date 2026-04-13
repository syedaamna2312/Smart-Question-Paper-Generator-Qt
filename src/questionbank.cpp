#include "questionbank.h"
#include <algorithm>
#include <functional>

// ---------------------------
// TopicTrie Implementation
// ---------------------------
TopicTrie::TopicTrie() { root = new Node(); }

TopicTrie::~TopicTrie() {
    std::function<void(Node*)> deleteNodes = [&](Node* node){
        if (!node) return;
        for(auto &p: node->next) deleteNodes(p.second);
        delete node;
    };
    deleteNodes(root);
}

void TopicTrie::insert(const std::string &word) {
    if (word.empty()) return;
    Node* curr = root;
    for(char c: word){
        if (!curr->next.count(c)) curr->next[c] = new Node();
        curr = curr->next[c];
    }
    curr->end = true;
}

void TopicTrie::collect(const Node* node, const std::string &prefix, std::vector<std::string> &out) const {
    if(!node) return;
    if(node->end) out.push_back(prefix);
    for(auto &p: node->next) collect(p.second, prefix + p.first, out);
}

std::vector<std::string> TopicTrie::suggest(const std::string &prefix) const {
    std::vector<std::string> result;
    Node* curr = root;
    for(char c: prefix){
        if(!curr->next.count(c)) return result;
        curr = curr->next.at(c);
    }
    collect(curr, prefix, result);
    return result;
}

// ---------------------------
// QuestionBank Implementation
// ---------------------------
QuestionBank& QuestionBank::instance() {
    static QuestionBank qb;
    return qb;
}

int QuestionBank::addQuestion(const std::string &course, const std::string &topic,
                              const std::string &text, const std::string &difficulty,
                              const std::string &CLO, int marks) {
    Question q;
    q.id = nextId++;
    q.course = course;
    q.topic = topic;
    q.text = text;
    q.difficulty = difficulty;
    q.CLO = CLO;
    q.marks = marks;

    questionsById[q.id] = q;
    topicIndex[topic].insert(q.id);
    trie.insert(topic);

    return q.id;
}

void QuestionBank::markUsed(int questionId) {
    if (questionsById.find(questionId) != questionsById.end())
        usedQuestions.insert(questionId);
}

bool QuestionBank::isUsed(int questionId) const {
    return usedQuestions.find(questionId) != usedQuestions.end();
}

void QuestionBank::resetUsedFlags() {
    usedQuestions.clear();
}

std::vector<std::string> QuestionBank::suggestTopics(const std::string &prefix) const {
    return trie.suggest(prefix);
}

const Question& QuestionBank::getQuestionById(int id) const {
    auto it = questionsById.find(id);
    if (it == questionsById.end()) throw std::runtime_error("Question ID not found");
    return it->second;
}

std::vector<Question> QuestionBank::getCandidatesForCourseTopics(const std::string &course,
                                                                 const std::vector<std::string> &topics) const {
    std::vector<Question> result;
    std::set<std::string> topicSet(topics.begin(), topics.end());

    for (const auto &p : questionsById) {
        const Question &q = p.second;
        if (q.course != course) continue;
        if (!topics.empty() && topicSet.find(q.topic) == topicSet.end()) continue;
        if (isUsed(q.id)) continue;
        result.push_back(q);
    }
    return result;
}

std::vector<Question> QuestionBank::getAllQuestionsFlat() const {
    std::vector<Question> out;
    out.reserve(questionsById.size());
    for (const auto &p : questionsById) out.push_back(p.second);
    return out;
}
