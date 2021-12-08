#include "sentiment-inference.h"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "cppflow/cppflow.h"
#include "../dependencies/rapidjson/include/rapidjson/document.h"
#include "../dependencies/rapidjson/include/rapidjson/istreamwrapper.h"
// #include <rapidjson/document.h>
// #include <rapidjson/istreamwrapper.h>
// #include <spacy/spacy>

enum PARAMETERS { MAX_VOCAB = 1500, MAX_LENGTH = 20, DIMENSIONS = 32 };

SentimentInference::SentimentInference(const std::string& word_index_file)
    : _model("saved_model")
{
    if(!std::filesystem::exists(word_index_file)) {
        throw std::runtime_error("Word Index File Does Not Exist");
    }

    std::ifstream ifs(word_index_file.c_str());
    rapidjson::IStreamWrapper isw(ifs);

    rapidjson::Document doc;
    if(doc.ParseStream(isw).HasParseError()) {
        throw std::runtime_error("Error Parsing Word Index JSON");
    }

    for(rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
        _wordindex_map.emplace(itr->value.GetString(), std::atoi(itr->name.GetString()));
    }
}

std::vector<std::string> SentimentInference::lemmatize(const std::string& text) {
    std::string word_only_string = regex_replace(text, std::regex("\b(\\d+)\b"), "");

    std::vector<std::string> tokenized_string;

    size_t pos = text.find(' ');
    size_t initial_pos = 0;

    while (pos != std::string::npos) {
        const std::string& current_substr = text.substr(initial_pos, pos - initial_pos);
        tokenized_string.emplace_back(current_substr);
        initial_pos = pos + 1;

        pos = text.find(' ', initial_pos);
    }
    const std::string& last_substr = text.substr(initial_pos, std::min(pos, text.size()) - initial_pos + 1);
    tokenized_string.emplace_back(last_substr);

    return tokenized_string;
}

SentimentInference::SENTIMENT SentimentInference::analyze(const std::string& text) {
    std::vector<std::string> preprocessed_text = lemmatize(text);

    std::vector<float> indexed_text;
    indexed_text.reserve(PARAMETERS::MAX_LENGTH);

    auto index_assign = [&](const std::string& word) {
        std::unordered_map<std::string, int>::const_iterator itr = _wordindex_map.find(word);
        if(itr == _wordindex_map.end() || itr->second > PARAMETERS::MAX_VOCAB) {
            return 1;
        }
        return itr->second;
    };

    if(preprocessed_text.size() > PARAMETERS::MAX_LENGTH) {
        for(int i = 0; i < PARAMETERS::MAX_LENGTH; ++i) {
            int word_index = index_assign(preprocessed_text[i]);
            indexed_text.emplace_back(word_index);
        }
    }
    else {
        for(int i = 0; i < preprocessed_text.size(); ++i) {
            int word_index = index_assign(preprocessed_text[i]);
            indexed_text.emplace_back(word_index);
        }
        for(int i = 0; i < PARAMETERS::MAX_LENGTH - preprocessed_text.size(); ++i) {
            indexed_text.emplace_back(1);
        }
    }

    cppflow::tensor data_tensor = cppflow::tensor(indexed_text, {20, 1}); 
    auto output = _model({{"serving_default_embedding_input: 0", data_tensor}},{"StatefulPartitionedCall:0"});

    std::vector<int64_t> result = cppflow::arg_max(output[0], 0).get_data<int64_t>();

    int64_t best_result = 0;
    int64_t best_index = 0;

    for(int64_t i = 0; i < result.size(); ++i) {
        int64_t current_result = result[i];
        if(result[i] >= best_result) {
            best_result = result[i];
            best_index = i;
        }
        best_index = !best_result ? 1 : best_index;
        std::cout << current_result;
    }

    std::cout << " " << best_index << " " << text << std::endl;

    return  static_cast<SENTIMENT>(best_index);
}