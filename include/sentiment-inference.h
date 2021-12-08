#ifndef SENTIMENTINFERENCE_H
#define SENTIMENTINFERENCE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

#include "cppflow/cppflow.h"

class SentimentInference {
public:
    SentimentInference(const std::string& word_index_file);

    enum SENTIMENT { GOOD = 2, NEUTRAL = 1, BAD = 0 };

    SENTIMENT analyze(const std::string& text);
    
private:
    std::unordered_map<std::string, int> _wordindex_map;
    cppflow::model _model;

    std::vector<std::string> lemmatize(const std::string& text);
};

#endif // SENTIMENTINFERENCE_H