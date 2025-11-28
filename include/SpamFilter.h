#ifndef SPAMFILTER_H
#define SPAMFILTER_H

#include "BayesianModel.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class SpamFilter {
private:
    struct Email {
        std::string id;
        std::string body;
        std::string label;
    };

    BayesianModel* model;
    std::vector<Email> testEmails;

public:
    void setModel(BayesianModel* m) { model = m; }

    void loadTestCSV(const std::string& csvPath);
    void evaluate(const std::vector<double>& thresholds);
    void run();
};

#endif
