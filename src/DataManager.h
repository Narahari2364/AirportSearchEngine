// Version 1.1 - Handles CSV parsing with quoted field support
#pragma once

#include "Airport.h"
#include <vector>
#include <string>

class DataManager {
public:
    // Loads airport records from a CSV file; returns true on success, false on failure.
    bool loadFromCSV(const std::string& filename);

    // Writes all stored airports to a CSV file; returns true on success, false on failure.
    bool saveToCSV(const std::string& filename);

    // Returns a reference to the in-memory list of airports (allows read/write access by callers).
    std::vector<Airport>& getAirports();

    // Appends one airport record to the end of the stored vector.
    void addAirport(const Airport& a);

private:
    std::vector<Airport> airports;
};
