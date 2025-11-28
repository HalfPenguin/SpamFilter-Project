#include "SpamFilter.h"
#include "TextProcessor.h"
#include <iostream>
#include <iomanip>

void SpamFilter::loadTestCSV(const std::string& csvPath) {
    std::ifstream in(csvPath);
    if (!in.is_open()) {
        std::cerr << "Error: Cannot open test CSV: " << csvPath << "\n";
        return;
    }

    std::string line;
    std::getline(in, line); // skip header

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(ss, token, ',')) parts.push_back(token);
        if (parts.size() < 3) continue;

        std::string id = parts[0];
        std::string label = parts.back();

        // Normalize label: trim whitespace/quotes/CR and normalize case to "Spam"/"Ham"
        auto trim_label = [](std::string s) {
            while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\r' || s.front() == '\n' || s.front() == '"' || s.front() == '\'')) s.erase(s.begin());
            while (!s.empty() && (s.back()  == ' ' || s.back()  == '\t' || s.back()  == '\r' || s.back()  == '\n' || s.back()  == '"' || s.back()  == '\'')) s.pop_back();
            return s;
        };

        label = trim_label(label);
        std::string lower = label;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "spam") label = "Spam";
        else if (lower == "ham") label = "Ham";

        std::string body;
        for (size_t i = 1; i < parts.size() - 1; i++) {
            if (i > 1) body += " ";
            body += parts[i];
        }

        testEmails.push_back({id, body, label});
    }
}

void SpamFilter::evaluate(const std::vector<double>& thresholds) {
    std::vector<double> probs;
    std::vector<std::string> trueLabels;

    for (auto& e : testEmails) {
        double p = model->computeProbability(e.body);
        probs.push_back(p);
        trueLabels.push_back(e.label);
    }

    // Print main predictions table
    std::cout << "ID,TrueLabel,Prob";
    for (double t : thresholds) std::cout << ",T=" << t;
    std::cout << "\n";

    for (size_t i = 0; i < probs.size(); i++) {
        std::cout << testEmails[i].id << ","
                  << testEmails[i].label << ","
                  << probs[i];

        for (double t : thresholds) {
            std::cout << ",";

            if (probs[i] >= t) std::cout << "Spam";
            else               std::cout << "Ham";
        }
        std::cout << "\n";
    }

    std::cout << "\n";

    // Evaluate for each threshold
    double bestF1 = 0.0;
    double bestThreshold = thresholds[0];

    for (double t : thresholds) {
        int TP=0, TN=0, FP=0, FN=0;

        for (size_t i = 0; i < probs.size(); i++) {
            bool predSpam = probs[i] >= t;
            bool actualSpam = (trueLabels[i] == "Spam");

            if (predSpam && actualSpam) TP++;
            else if (!predSpam && !actualSpam) TN++;
            else if (predSpam && !actualSpam) FP++;
            else if (!predSpam && actualSpam) FN++;
        }

        double precision = TP + FP == 0 ? 0 : double(TP) / (TP + FP);
        double recall    = TP + FN == 0 ? 0 : double(TP) / (TP + FN);
        double f1        = (precision + recall == 0) ? 0 : 2 * precision * recall / (precision + recall);

        std::cout << "Threshold " << t << ":\n";
        std::cout << "  TP=" << TP << " TN=" << TN << " FP=" << FP << " FN=" << FN << "\n";
        std::cout << "  Precision=" << precision
                  << " Recall=" << recall
                  << " F1=" << f1 << "\n\n";

        if (f1 > bestF1) {
            bestF1 = f1;
            bestThreshold = t;
        }
    }

    std::cout << "=====================================\n";
    std::cout << "Best Threshold = " << bestThreshold << "\n";
    std::cout << "Best F1 Score  = " << bestF1 << "\n";
    std::cout << "=====================================\n\n";
}

void SpamFilter::run() {
    double thresholdsArr[] = {0.5, 0.6, 0.7, 0.8, 0.9};
    std::vector<double> thresholds(thresholdsArr, thresholdsArr + 5);

    evaluate(thresholds);
}
