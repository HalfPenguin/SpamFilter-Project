#include "BayesianModel.h"
#include "TextProcessor.h"
#include <fstream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iostream>

void BayesianModel::trainCSV(const std::string& csvPath, bool isSpam) {
    std::ifstream in(csvPath);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open CSV file: " << csvPath << "\n";
        return;
    }

    std::string line;
    std::getline(in, line); // skip header

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // Find last comma (label comes last)
        size_t lastComma = line.rfind(',');
        if (lastComma == std::string::npos) continue;

        std::string label = line.substr(lastComma + 1);
        std::string id_body = line.substr(0, lastComma);

        // Find first comma (id comes first)
        size_t firstComma = id_body.find(',');
        if (firstComma == std::string::npos) continue;

        std::string id   = id_body.substr(0, firstComma);
        std::string body = id_body.substr(firstComma + 1);

        // Clean body
        std::string cleaned = TextProcessor::clean(body);
        auto words = TextProcessor::tokenize(cleaned);

        std::unordered_set<std::string> uniqueWords;

        if (isSpam) spamEmails++;
        else        hamEmails++;

        for (auto& w : words) {
            vocab.insert(w);
            uniqueWords.insert(w);

            if (isSpam) {
                spamCount[w]++;
                totalSpamWords++;
            } else {
                hamCount[w]++;
                totalHamWords++;
            }
        }

        // Track DF
        for (auto& w : uniqueWords)
            dfCount[w]++;
    }
}


void BayesianModel::computeIDF() {
    int N = spamEmails + hamEmails;
    for (auto& w : vocab) {
        if (!dfCount.count(w)) dfCount[w] = 1;
    }
}

double BayesianModel::idf(const std::string& w) {
    int N = spamEmails + hamEmails;
    int df = dfCount.count(w) ? dfCount.at(w) : 1;
    return std::log(1.0 + double(N) / df); // log-IDF
}

double BayesianModel::computeProbability(const std::string& text) {
    auto cleaned = TextProcessor::clean(text);
    auto words = TextProcessor::tokenize(cleaned);

    double logPriorSpam = std::log(spamEmails + 1) - std::log(spamEmails + hamEmails + 2);
    double logPriorHam  = std::log(hamEmails + 1) - std::log(spamEmails + hamEmails + 2);

    double logLS = logPriorSpam;
    double logLH = logPriorHam;

    const double alpha = 1.0;
    long long V = vocab.size();

    // Compute term-frequency for this email
    std::unordered_map<std::string,int> tf;
    for (auto& w : words) tf[w]++;

    int totalWords = words.size();

    for (auto& p : tf) {
        const std::string& w = p.first;
        int t = p.second;

        // log-TF
        double logtf = 1.0 + std::log(t);

        // log-IDF
        double logidf = idf(w);

        // TF-IDF weight
        double weight = logtf * logidf;

        long long ws = spamCount.count(w) ? spamCount.at(w) : 0;
        long long wh = hamCount.count(w) ? hamCount.at(w) : 0;

        double ps = (ws + alpha) / (totalSpamWords + alpha * V);
        double ph = (wh + alpha) / (totalHamWords  + alpha * V);

        logLS += weight * std::log(ps);
        logLH += weight * std::log(ph);
    }

    // Likelihood normalization
    logLS /= totalWords;
    logLH /= totalWords;

    // log-sum-exp normalization
    double maxLog = std::max(logLS, logLH);
    double sumExp = std::exp(logLS - maxLog) + std::exp(logLH - maxLog);

    return std::exp(logLS - maxLog) / sumExp;
}

std::vector<std::pair<std::string,double>> BayesianModel::topInformativeWords(int k) {
    std::vector<std::pair<std::string,double>> result;

    for (auto& w : vocab) {
        double ps = (spamCount[w] + 1.0) / (totalSpamWords + vocab.size());
        double ph = (hamCount[w]  + 1.0) / (totalHamWords  + vocab.size());
        double score = std::fabs(std::log(ps) - std::log(ph));
        result.push_back({w, score});
    }

    std::sort(result.begin(), result.end(),
              [](auto& a, auto& b) { return a.second > b.second; });

    if (result.size() > k) result.resize(k);
    return result;
}
