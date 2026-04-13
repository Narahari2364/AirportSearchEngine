// Version 1.1 - Airport data structure for Nottingham SkyRoute Explorer
#pragma once

#include <string>

struct Airport {
    std::string ident;        // Site identifier code from the dataset (e.g. FAA LID or internal id).
    std::string type;         // Airport classification (e.g. large_airport, small_airport, heliport).
    std::string name;         // Full name of the airport or landing site.
    float elevation_ft = 0.0f; // Height above mean sea level in feet; 0.0f when not provided in the CSV.
    std::string continent;    // Continent code (e.g. NA, EU) where the airport is located.
    std::string iso_country;  // ISO 3166-1 alpha-2 country code.
    std::string iso_region;   // ISO region code (typically country-subdivision, e.g. US-CA).
    std::string municipality; // City or town associated with the airport.
    std::string icao_code;    // ICAO four-letter location indicator (empty if not assigned).
    std::string iata_code;    // IATA three-letter passenger terminal code (empty if not assigned).
    std::string gps_code;     // GPS or charting code used for navigation references.
    std::string local_code;   // Local or national identifier where applicable.
    std::string coordinates;  // Latitude and longitude as stored in the CSV (often "lat, lon" text).
};
