#ifndef STOPWORDS_H
#define STOPWORDS_H

#include <unordered_set>
#include <string>

static const std::unordered_set<std::string> STOPWORDS = {
    "the","is","are","was","were","at","in","on","of","for","and","to","a","an",
    "that","it","this","with","as","by","from","be","have","has","had","but",
    "or","not","your","you","we","our","us","hi","hello","dear","regards","thank",
    "thanks","please","kind"
};

#endif
