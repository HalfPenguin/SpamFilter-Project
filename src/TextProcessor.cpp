#include "TextProcessor.h"
#include <cctype>
#include <sstream>

std::string TextProcessor::clean(const std::string& text) {
    std::string cleaned;
    cleaned.reserve(text.size());

    for (char c : text) {
        if (std::isalnum((unsigned char)c))
            cleaned.push_back(std::tolower((unsigned char)c));
        else
            cleaned.push_back(' ');
    }
    return cleaned;
}

std::vector<std::string> TextProcessor::tokenize(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream ss(text);
    std::string w;

    while (ss >> w)
        words.push_back(w);

    return words;
}
