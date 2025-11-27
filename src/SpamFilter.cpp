#include "SpamFilter.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

void SpamFilter::loadCSV(const std::string& path, bool isSpam, const std::string& prefix) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Cannot open test CSV: " << path << "\n";
        return;
    }

    // Skip header row: id,body,label
    std::string header;
    std::getline(in, header);

    std::string line;
    int idx = 1;

    while (std::getline(in, line)) {

        // Skip empty / whitespace / comma-only rows
        if (line.find_first_not_of(" \t\r\n,") == std::string::npos)
            continue;

        EmailResult r;

        // Assign ID
        std::ostringstream ss;
        ss << prefix << std::setw(2) << std::setfill('0') << idx++;
        r.id = ss.str();
        r.trueLabel = isSpam;
        
        // Compute probability
        r.prob = model.computeProbability(line);

        results.push_back(r);
    }
}


void SpamFilter::classify(const std::vector<double>& thresholds) {
    // Nothing to do — classification printed in printResults()
}

void SpamFilter::printResults(const std::vector<double>& thresholds) {
    std::cout << "ID,TrueLabel,Prob";

    for (auto t : thresholds)
        std::cout << ",T=" << t;

    std::cout << "\n";

    std::vector<int> correct(thresholds.size(), 0);

    for (auto& r : results) {
        std::cout << r.id << ","
                  << (r.trueLabel ? "Spam" : "Ham") << ","
                  << r.prob;

        for (int i = 0; i < thresholds.size(); i++) {
            bool pred = (r.prob >= thresholds[i]);
            if (pred == r.trueLabel) correct[i]++;

            std::cout << "," << (pred ? "Spam" : "Ham");
        }
        std::cout << "\n";
    }

    std::cout << "\nAccuracies:\n";
    for (int i = 0; i < thresholds.size(); i++) {
        double acc = correct[i] / (double)results.size();
        std::cout << "T=" << thresholds[i]
                  << " -> " << acc * 100 << "%\n";
    }
}
