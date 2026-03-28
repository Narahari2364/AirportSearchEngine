#include "AnalysisEngine.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <limits>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

// Removes leading/trailing whitespace from a substring before feeding it to std::stod.
void trimWhitespace(std::string& s) {
    static const char* kWs = " \t\n\r\v\f";
    const std::size_t start = s.find_first_not_of(kWs);
    if (start == std::string::npos) {
        s.clear();
        return;
    }
    const std::size_t end = s.find_last_not_of(kWs);
    s = s.substr(start, end - start + 1);
}

// Splits the CSV "coordinates" text into latitude and longitude doubles.
bool tryParseLatLon(const std::string& coords, double& lat, double& lon) {
    try {
        const std::size_t comma = coords.find(',');
        if (comma == std::string::npos) {
            return false;
        }
        std::string latStr = coords.substr(0, comma);
        std::string lonStr = coords.substr(comma + 1);
        trimWhitespace(latStr);
        trimWhitespace(lonStr);
        lat = std::stod(latStr);
        lon = std::stod(lonStr);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace

AnalysisEngine::AnalysisEngine(std::vector<Airport>& airportList)
    : airports(airportList) {
    // Reference member: always reflects the current contents of the shared vector.
}

double AnalysisEngine::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Earth mean radius for a simple spherical model (kilometres), as used by the Haversine formula.
    constexpr double kEarthRadiusKm = 6371.0;

    // Multiply degrees by (pi / 180) to obtain radians, which is what the trig functions expect.
    const double degToRad = M_PI / 180.0;
    const double phi1 = lat1 * degToRad;
    const double phi2 = lat2 * degToRad;
    const double deltaPhi = (lat2 - lat1) * degToRad;
    const double deltaLambda = (lon2 - lon1) * degToRad;

    // Half-angle chord length on the unit sphere: sin^2(dphi/2) + cos(phi1)cos(phi2)sin^2(dlambda/2).
    const double sinHalfDphi = std::sin(deltaPhi / 2.0);
    const double sinHalfLambda = std::sin(deltaLambda / 2.0);
    double h = sinHalfDphi * sinHalfDphi
        + std::cos(phi1) * std::cos(phi2) * sinHalfLambda * sinHalfLambda;

    // Guard against tiny floating-point drift outside [0, 1] before calling asin/sqrt.
    h = std::min(1.0, std::max(0.0, h));

    // Central angle (in radians) between the two surface points: c = 2 * arcsin(sqrt(h)).
    const double c = 2.0 * std::asin(std::sqrt(h));

    // Arc length along the sphere: radius times angle gives the great-circle distance in km.
    return kEarthRadiusKm * c;
}

Airport AnalysisEngine::findNearestAirport(double lat, double lon) {
    bool found = false;
    double bestDistanceKm = 0.0;
    Airport best{};

    // Brute-force scan: every valid coordinate pair competes for the smallest Haversine distance.
    for (const Airport& ap : airports) {
        try {
            double apLat = 0.0;
            double apLon = 0.0;
            if (!tryParseLatLon(ap.coordinates, apLat, apLon)) {
                continue;
            }
            const double d = calculateDistance(lat, lon, apLat, apLon);
            if (!found || d < bestDistanceKm) {
                found = true;
                bestDistanceKm = d;
                best = ap;
            }
        } catch (const std::exception&) {
            // std::stod (or related parsing) failed for this row — ignore and continue searching.
            continue;
        }
    }

    if (!found) {
        throw std::runtime_error("findNearestAirport: no airport had parseable coordinates");
    }
    return best;
}

std::vector<Airport> AnalysisEngine::findAirportsWithinRadius(double lat, double lon, double radiusKm) {
    std::vector<Airport> inside;

    // Same parsing strategy as findNearestAirport, but keep every airport within the threshold distance.
    for (const Airport& ap : airports) {
        try {
            double apLat = 0.0;
            double apLon = 0.0;
            if (!tryParseLatLon(ap.coordinates, apLat, apLon)) {
                continue;
            }
            const double d = calculateDistance(lat, lon, apLat, apLon);
            if (d <= radiusKm) {
                inside.push_back(ap);
            }
        } catch (const std::exception&) {
            continue;
        }
    }

    return inside;
}

Airport AnalysisEngine::getHighestElevation() {
    if (airports.empty()) {
        throw std::runtime_error("getHighestElevation: airport list is empty");
    }

    // std::max_element walks the range and keeps the iterator to the largest elevation_ft value.
    const auto it = std::max_element(airports.begin(), airports.end(),
        [](const Airport& a, const Airport& b) { return a.elevation_ft < b.elevation_ft; });
    return *it;
}

Airport AnalysisEngine::getLowestElevation() {
    if (airports.empty()) {
        throw std::runtime_error("getLowestElevation: airport list is empty");
    }

    // std::min_element uses the same comparator style but returns the smallest element instead.
    const auto it = std::min_element(airports.begin(), airports.end(),
        [](const Airport& a, const Airport& b) { return a.elevation_ft < b.elevation_ft; });
    return *it;
}

std::vector<Airport> AnalysisEngine::getMissingIATA() {
    std::vector<Airport> missing;

    // Empty string means "no code"; some exports use a literal backslash-N sequence for SQL NULL-style gaps.
    for (const Airport& ap : airports) {
        if (ap.iata_code.empty() || ap.iata_code == "\\N") {
            missing.push_back(ap);
        }
    }

    return missing;
}

std::map<std::string, std::vector<Airport>> AnalysisEngine::groupByMunicipality() {
    std::map<std::string, std::vector<Airport>> buckets;

    // Each municipality string becomes a key; airports sharing that city accumulate in the same vector.
    for (const Airport& ap : airports) {
        buckets[ap.municipality].push_back(ap);
    }

    return buckets;
}

std::vector<Airport> AnalysisEngine::sortAirports(std::vector<Airport> list, const std::string& field,
                                                   bool ascending) {
    // Comparator captures the field name and direction, then dispatches to the right Airport member.
    const auto lessThan = [&](const Airport& a, const Airport& b) -> bool {
        if (field == "name") {
            return a.name < b.name;
        }
        if (field == "country") {
            return a.iso_country < b.iso_country;
        }
        if (field == "type") {
            return a.type < b.type;
        }
        if (field == "elevation") {
            return a.elevation_ft < b.elevation_ft;
        }
        if (field == "latitude") {
            double la = 0.0;
            double lb = 0.0;
            double dummyA = 0.0;
            double dummyB = 0.0;
            const bool okA = tryParseLatLon(a.coordinates, la, dummyA);
            const bool okB = tryParseLatLon(b.coordinates, lb, dummyB);
            const double ka = okA ? la : std::numeric_limits<double>::lowest();
            const double kb = okB ? lb : std::numeric_limits<double>::lowest();
            return ka < kb;
        }
        if (field == "longitude") {
            double loA = 0.0;
            double loB = 0.0;
            double dummyA = 0.0;
            double dummyB = 0.0;
            const bool okA = tryParseLatLon(a.coordinates, dummyA, loA);
            const bool okB = tryParseLatLon(b.coordinates, dummyB, loB);
            const double ka = okA ? loA : std::numeric_limits<double>::lowest();
            const double kb = okB ? loB : std::numeric_limits<double>::lowest();
            return ka < kb;
        }
        throw std::invalid_argument("sortAirports: unknown field '" + field + "'");
    };

    if (ascending) {
        std::sort(list.begin(), list.end(), lessThan);
    } else {
        // Descending order is implemented by reversing the strict weak ordering from the same keys.
        std::sort(list.begin(), list.end(),
            [&](const Airport& a, const Airport& b) { return lessThan(b, a); });
    }

    return list;
}
