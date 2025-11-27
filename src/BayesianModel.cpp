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

    // Skip header: id,body,label
    std::getline(in, line);

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;

        // Split by comma
        while (std::getline(ss, token, ',')) {
            parts.push_back(token);
        }

        // We need at least: id , body... , label
        if (parts.size() < 3) {
            continue;
        }

        std::string id = parts[0];
        std::string label = parts.back();

        // Rebuild full body from parts[1] ... parts[n-2]
        std::string body;
        for (size_t i = 1; i < parts.size() - 1; i++) {
            if (i > 1) body += " ";
            body += parts[i];
        }

        // CLEAN AND TOKENIZE ONLY THE BODY
        std::string cleaned = TextProcessor::clean(body);
        auto words = TextProcessor::tokenize(cleaned);

        // Use the provided isSpam parameter (from main)
        if (isSpam) spamEmails++;
        else        hamEmails++;

        for (auto& w : words) {
            vocab.insert(w);

            if (isSpam) {
                spamCount[w]++;
                totalSpamWords++;
            } else {
                hamCount[w]++;
                totalHamWords++;
            }
        }
    }
}

double BayesianModel::wordProb(const std::string& w, bool isSpam) {
    long long count = isSpam ? spamCount[w] : hamCount[w];
    long long total = isSpam ? totalSpamWords : totalHamWords;

    long long V = vocab.size();
    return (count + 1.0) / (total + V);
}

double BayesianModel::computeProbability(const std::string& text) {
    // Clean & tokenize
    std::string cleaned = TextProcessor::clean(text);
    std::vector<std::string> words = TextProcessor::tokenize(cleaned);

    // Log priors with smoothing
    double logPriorSpam = std::log(spamEmails + 1) - std::log(spamEmails + hamEmails + 2);
    double logPriorHam  = std::log(hamEmails + 1) - std::log(spamEmails + hamEmails + 2);

    double logLS = logPriorSpam;
    double logLH = logPriorHam;

    const double alpha = 1.0;
    long long V = vocab.size();

    if (words.empty()) {
        // No words -> return prior probability
        double maxLog = std::max(logLS, logLH);
        double sumExp = std::exp(logLS - maxLog) + std::exp(logLH - maxLog);
        return std::exp(logLS - maxLog) / sumExp;
    }

    for (const auto& w : words) {
        long long ws = spamCount.count(w) ? spamCount.at(w) : 0;
        long long wh = hamCount.count(w) ? hamCount.at(w) : 0;

        double pw_spam = (ws + alpha) / (totalSpamWords + alpha * V);
        double pw_ham  = (wh + alpha) / (totalHamWords  + alpha * V);

        double denom = static_cast<double>(words.size());
        logLS += std::log(pw_spam) / denom;
        logLH += std::log(pw_ham) / denom;
    }

    // Normalize using log-sum-exp
    double maxLog = std::max(logLS, logLH);
    double sumExp = std::exp(logLS - maxLog) + std::exp(logLH - maxLog);

    return std::exp(logLS - maxLog) / sumExp; // → value in [0,1]
}

std::vector<std::pair<std::string,double>> BayesianModel::topInformativeWords(int k) {
    std::vector<std::pair<std::string,double>> result;

    for (auto& w : vocab) {
        double ps = wordProb(w, true);
        double ph = wordProb(w, false);
        double score = std::fabs(std::log(ps) - std::log(ph));
        result.push_back({w, score});
    }

    std::sort(result.begin(), result.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if (result.size() > k)
        result.resize(k);

    return result;
}
