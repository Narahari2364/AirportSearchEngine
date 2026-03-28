#include "SearchEngine.h"
#include <algorithm>
#include <cctype>

SearchEngine::SearchEngine(std::vector<Airport>& airportList)
    : airports(airportList) {
    // Keep a reference to the live vector owned elsewhere (typically by DataManager).
}

std::vector<Airport> SearchEngine::searchByCountry(const std::string& code) {
    std::vector<Airport> matches;

    // Normalize the query once so every row comparison uses the same uppercase rules.
    std::string codeUpper = code;
    std::transform(codeUpper.begin(), codeUpper.end(), codeUpper.begin(),
        [](unsigned char ch) { return static_cast<char>(::toupper(ch)); });

    // Scan the full dataset and collect rows whose country code matches (ignoring letter case).
    for (const Airport& ap : airports) {
        std::string countryUpper = ap.iso_country;
        std::transform(countryUpper.begin(), countryUpper.end(), countryUpper.begin(),
            [](unsigned char ch) { return static_cast<char>(::toupper(ch)); });
        if (countryUpper == codeUpper) {
            matches.push_back(ap);
        }
    }

    return matches;
}

std::vector<Airport> SearchEngine::searchByName(const std::string& partial) {
    std::vector<Airport> matches;

    // Lowercase the needle once; substring search is then done on two lowercase copies.
    std::string partialLower = partial;
    std::transform(partialLower.begin(), partialLower.end(), partialLower.begin(),
        [](unsigned char ch) { return static_cast<char>(::tolower(ch)); });

    for (const Airport& ap : airports) {
        std::string nameLower = ap.name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
            [](unsigned char ch) { return static_cast<char>(::tolower(ch)); });
        if (nameLower.find(partialLower) != std::string::npos) {
            matches.push_back(ap);
        }
    }

    return matches;
}

std::vector<Airport> SearchEngine::searchByIATA(const std::string& code) {
    std::vector<Airport> matches;

    // IATA codes are short text tags; compare them without caring about capitalization.
    std::string codeUpper = code;
    std::transform(codeUpper.begin(), codeUpper.end(), codeUpper.begin(),
        [](unsigned char ch) { return static_cast<char>(::toupper(ch)); });

    for (const Airport& ap : airports) {
        std::string iataUpper = ap.iata_code;
        std::transform(iataUpper.begin(), iataUpper.end(), iataUpper.begin(),
            [](unsigned char ch) { return static_cast<char>(::toupper(ch)); });
        if (iataUpper == codeUpper) {
            matches.push_back(ap);
        }
    }

    return matches;
}

std::vector<Airport> SearchEngine::searchByType(const std::string& type) {
    std::vector<Airport> matches;

    // Airport "type" strings follow a fixed vocabulary; still compare case-insensitively for robustness.
    std::string typeUpper = type;
    std::transform(typeUpper.begin(), typeUpper.end(), typeUpper.begin(),
        [](unsigned char ch) { return static_cast<char>(::toupper(ch)); });

    for (const Airport& ap : airports) {
        std::string rowTypeUpper = ap.type;
        std::transform(rowTypeUpper.begin(), rowTypeUpper.end(), rowTypeUpper.begin(),
            [](unsigned char ch) { return static_cast<char>(::toupper(ch)); });
        if (rowTypeUpper == typeUpper) {
            matches.push_back(ap);
        }
    }

    return matches;
}

std::map<std::string, int> SearchEngine::countByType() {
    std::map<std::string, int> counts;

    // Tally how many airports appear for each exact type label stored in the CSV.
    for (const Airport& ap : airports) {
        counts[ap.type]++;
    }

    return counts;
}
