#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

namespace {

// Parses the coordinates column: removes a leading comma from the stream remainder, trims spaces,
// and strips a pair of surrounding double quotes if the CSV used quoted coordinates.
void normalizeCoordinatesToken(std::string& s) {
    if (!s.empty() && s.front() == ',') {
        s.erase(0, 1);
    }
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
        s.erase(0, 1);
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        s = s.substr(1, s.size() - 2);
    }
}

// Wraps a field in CSV double quotes when it contains a comma, quote, or line break, doubling any inner quotes.
std::string escapeCsvField(const std::string& value) {
    if (value.find_first_of(",\"\n\r") != std::string::npos) {
        std::string out;
        out.push_back('"');
        for (char c : value) {
            if (c == '"') {
                out += "\"\"";
            } else {
                out.push_back(c);
            }
        }
        out.push_back('"');
        return out;
    }
    return value;
}

} // namespace

bool DataManager::loadFromCSV(const std::string& filename) {
    // Open the CSV and fail fast if the path is unreadable.
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error: could not open file for reading: " << filename << '\n';
        return false;
    }

    // Start from a clean slate so repeated loads do not duplicate records.
    airports.clear();

    // Consume the header row so data lines only contain airport records.
    std::string header;
    if (!std::getline(in, header)) {
        std::cout << "0 airports loaded\n";
        return true;
    }

    std::size_t loaded = 0;

    // Read one logical row at a time from the file.
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        // Split the first twelve fields on commas; everything after the twelfth comma is coordinates.
        std::stringstream ss(line);
        std::string cols[12];
        bool rowOk = true;
        for (int i = 0; i < 12; ++i) {
            if (!std::getline(ss, cols[i], ',')) {
                rowOk = false;
                break;
            }
        }
        if (!rowOk) {
            continue;
        }

        std::string coordinates;
        std::getline(ss, coordinates);
        normalizeCoordinatesToken(coordinates);

        Airport ap;
        ap.ident = cols[0];
        ap.type = cols[1];
        ap.name = cols[2];

        // Elevation may be blank in the CSV; invalid text is treated as missing (0.0f).
        if (cols[3].empty()) {
            ap.elevation_ft = 0.0f;
        } else {
            try {
                ap.elevation_ft = std::stof(cols[3]);
            } catch (const std::exception&) {
                ap.elevation_ft = 0.0f;
            }
        }

        ap.continent = cols[4];
        ap.iso_country = cols[5];
        ap.iso_region = cols[6];
        ap.municipality = cols[7];
        ap.icao_code = cols[8];
        ap.iata_code = cols[9];
        ap.gps_code = cols[10];
        ap.local_code = cols[11];
        ap.coordinates = coordinates;

        airports.push_back(ap);
        ++loaded;
    }

    std::cout << loaded << " airports loaded\n";
    return true;
}

bool DataManager::saveToCSV(const std::string& filename) {
    // Create or overwrite the output file for writing.
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error: could not open file for writing: " << filename << '\n';
        return false;
    }

    // Write the schema row exactly as required so other tools can read the file.
    out << "ident,type,name,elevation_ft,continent,iso_country,iso_region,municipality,icao_code,"
           "iata_code,gps_code,local_code,coordinates\n";

    // Serialize each in-memory airport as one comma-separated line (with escaping when needed).
    for (const Airport& a : airports) {
        out << escapeCsvField(a.ident) << ','
            << escapeCsvField(a.type) << ','
            << escapeCsvField(a.name) << ',';

        if (std::floor(a.elevation_ft) == a.elevation_ft) {
            out << static_cast<int>(a.elevation_ft);
        } else {
            out << a.elevation_ft;
        }

        out << ','
            << escapeCsvField(a.continent) << ','
            << escapeCsvField(a.iso_country) << ','
            << escapeCsvField(a.iso_region) << ','
            << escapeCsvField(a.municipality) << ','
            << escapeCsvField(a.icao_code) << ','
            << escapeCsvField(a.iata_code) << ','
            << escapeCsvField(a.gps_code) << ','
            << escapeCsvField(a.local_code) << ','
            << escapeCsvField(a.coordinates) << '\n';
    }

    return static_cast<bool>(out);
}

std::vector<Airport>& DataManager::getAirports() {
    // Expose the internal storage so callers can inspect or modify the dataset.
    return airports;
}

void DataManager::addAirport(const Airport& a) {
    // Grow the dataset by one record at the end of the vector.
    airports.push_back(a);
}
