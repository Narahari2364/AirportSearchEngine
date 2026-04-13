// Version 1.1 - Geographic calculations using Haversine formula
#pragma once

#include "Airport.h"
#include <vector>
#include <string>
#include <map>

class AnalysisEngine {
public:
    // Binds this analyzer to an existing airport list stored elsewhere (no copy of the vector).
    AnalysisEngine(std::vector<Airport>& airportList);

    // Returns the airport record with the greatest elevation_ft value in the dataset.
    Airport getHighestElevation();

    // Returns the airport record with the smallest elevation_ft value in the dataset.
    Airport getLowestElevation();

    // Collects every airport whose IATA passenger code field is empty.
    std::vector<Airport> getMissingIATA();

    // Organizes airports into buckets keyed by municipality string (one vector per city name).
    std::map<std::string, std::vector<Airport>> groupByMunicipality();

    // Finds the airport closest to the given latitude/longitude using great-circle distance in km.
    Airport findNearestAirport(double lat, double lon);

    // Lists all airports whose distance from the given point is less than or equal to radiusKm.
    std::vector<Airport> findAirportsWithinRadius(double lat, double lon, double radiusKm);

    // Computes the great-circle distance between two WGS84-style latitude/longitude points, in kilometres.
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);

    // Returns a new vector containing a copy of list sorted by the named field (ascending or descending).
    std::vector<Airport> sortAirports(std::vector<Airport> list, const std::string& field, bool ascending);

private:
    std::vector<Airport>& airports;
};
