#include "BayesianModel.h"
#include "SpamFilter.h"
#include "ExcelConverter.h"
#include <iostream>

int main() {

    // ===============================
    //  1. Convert XLSX to CSV
    // ===============================
    std::cout << "Converting XLSX files to CSV...\n";

    ExcelConverter converter;

    converter.convert("data/train/ham_emails_train100.xlsx",
                      "data/csv/train/ham_train.csv");

    converter.convert("data/train/spam_emails_train100.xlsx",
                      "data/csv/train/spam_train.csv");

    converter.convert("data/test/ham_emails_test20.xlsx",
                      "data/csv/test/ham_test.csv");

    converter.convert("data/test/spam_emails_test20.xlsx",
                      "data/csv/test/spam_test.csv");

    std::cout << "Conversion completed.\n\n";

    // ===============================
    //  2. Train Model
    // ===============================
    std::cout << "Training model...\n";

    BayesianModel model;

    model.trainCSV("data/csv/train/ham_train.csv", false);
    model.trainCSV("data/csv/train/spam_train.csv", true);

    model.computeIDF();

    // ===============================
    //  3. Load Test Data
    // ===============================

    SpamFilter filter;
    filter.setModel(&model);

    filter.loadTestCSV("data/csv/test/spam_test.csv");
    filter.loadTestCSV("data/csv/test/ham_test.csv");

    // ===============================
    //  4. Evaluate
    // ===============================
    filter.run();
    
    // Top informative words
    auto info = model.topInformativeWords(10);

    std::cout << "\nTop Informative Words:\n";
    for (auto& p : info)
        std::cout << p.first << " (score=" << p.second << ")\n";
 
    return 0;
}
