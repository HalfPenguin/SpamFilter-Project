#include "ExcelConverter.h"
#include "BayesianModel.h"
#include "SpamFilter.h"
#include <vector>
#include <iostream>

int main() {
    ExcelConverter converter;

    // Convert XLSX → CSV
    converter.convert("data/train/spam_emails_train100.xlsx", "data/csv/train/spam_train.csv");
    converter.convert("data/train/ham_emails_train100.xlsx", "data/csv/train/ham_train.csv");
    converter.convert("data/test/spam_emails_test20.xlsx", "data/csv/test/spam_test.csv");
    converter.convert("data/test/ham_emails_test20.xlsx", "data/csv/test/ham_test.csv");

    // Train Bayesian Model
    BayesianModel model;
    model.trainCSV("data/csv/train/spam_train.csv", true);
    model.trainCSV("data/csv/train/ham_train.csv", false);

    // Load test emails
    SpamFilter filter(model);
    filter.loadCSV("data/csv/test/spam_test.csv", true, "s");
    filter.loadCSV("data/csv/test/ham_test.csv", false, "h");

    // Thresholds
    std::vector<double> thresholds = {0.6, 0.7, 0.8, 0.9, 0.95};

    // Classify and print results
    filter.printResults(thresholds);

    // Top informative words
    auto info = model.topInformativeWords(10);

    std::cout << "\nTop Informative Words:\n";
    for (auto& p : info)
        std::cout << p.first << " (score=" << p.second << ")\n";

    return 0;
}
