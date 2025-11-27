#ifndef SPAMFILTER_H
#define SPAMFILTER_H

#include "BayesianModel.h"
#include <string>
#include <vector>

struct EmailResult {
    std::string id;
    bool trueLabel;
    double prob;
};

class SpamFilter {
private:
    BayesianModel& model;
    std::vector<EmailResult> results;

public:
    SpamFilter(BayesianModel& m) : model(m) {}

    void loadCSV(const std::string& path, bool isSpam, const std::string& prefix);
    void classify(const std::vector<double>& thresholds);
    void printResults(const std::vector<double>& thresholds);
};

#endif
