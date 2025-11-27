#ifndef BAYESIANMODEL_H
#define BAYESIANMODEL_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

class BayesianModel {
private:
    std::unordered_map<std::string, long long> spamCount;
    std::unordered_map<std::string, long long> hamCount;
    std::unordered_set<std::string> vocab;

    long long totalSpamWords = 0;
    long long totalHamWords = 0;

    int spamEmails = 0;
    int hamEmails = 0;

    double priorSpam = 0.5;
    double priorHam = 0.5;

public:
    void trainCSV(const std::string& csvPath, bool isSpam);
    double computeProbability(const std::string& text);
    std::vector<std::pair<std::string,double>> topInformativeWords(int k);

private:
    double wordProb(const std::string& word, bool isSpam);
};

#endif
