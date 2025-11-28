#include "TextProcessor.h"
#include "Stopwords.h"
#include <algorithm>
#include <cctype>
#include <sstream>

std::string TextProcessor::clean(const std::string& s) {
    std::string r;
    for (char c : s) {
        if (std::isalnum(c) || std::isspace(c))
            r += std::tolower(c);
        else
            r += ' ';
    }
    return r;
}

std::vector<std::string> TextProcessor::tokenize(const std::string& s) {
    std::stringstream ss(s);
    std::string w;
    std::vector<std::string> result;

    while (ss >> w) {
        if (!STOPWORDS.count(w))
            result.push_back(w);
    }
    return result;
}
