// Version 1.1 - Added case-insensitive search capability
#pragma once

#include "Airport.h"
#include <vector>
#include <string>
#include <map>

class SearchEngine {
public:
    // Binds this search engine to an existing airport list stored elsewhere (no copy of the vector).
    SearchEngine(std::vector<Airport>& airportList);

    // Returns all airports whose ISO country code equals the given code.
    std::vector<Airport> searchByCountry(const std::string& code);

    // Returns all airports whose name contains the given substring (typically matched as plain text).
    std::vector<Airport> searchByName(const std::string& partial);

    // Returns airports whose IATA code equals the requested code (often zero or one match).
    std::vector<Airport> searchByIATA(const std::string& code);

    // Returns all airports whose type string equals the given type (e.g. large_airport).
    std::vector<Airport> searchByType(const std::string& type);

    // Builds a histogram: how many airports exist for each distinct type string in the dataset.
    std::map<std::string, int> countByType();

private:
    std::vector<Airport>& airports;
};
