#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <vector>

class TextProcessor {
public:
    static std::string clean(const std::string& text);
    static std::vector<std::string> tokenize(const std::string& text);
};

#endif
