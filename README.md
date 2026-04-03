# Nottingham SkyRoute Explorer

**Nottingham SkyRoute Explorer** is a C++17 console application developed as coursework for module **SOFT10101** at Nottingham Trent University. It loads airport records from a CSV file into memory and lets you search, analyse, and extend the dataset through a text-based menu—suitable for exploring how real-world geodata can be managed with the C++ standard library.

## Features

- **CSV loading** — Reads airport records from a CSV file (header row plus comma-separated fields, including quoted coordinates).
- **Search by country** — Filter airports by ISO country code (case-insensitive).
- **Search by name** — Find airports whose names contain a given substring (case-insensitive).
- **Search by IATA** — Look up airports by IATA passenger code (case-insensitive).
- **Count by type** — Show how many airports exist for each type (e.g. `large_airport`, `heliport`).
- **Add new airport** — Enter new records interactively and append them to the in-memory list.
- **Save to CSV** — Write the current dataset back to a CSV file.
- **Advanced features**
  - **Haversine distance** — Great-circle distance in kilometres between two latitude/longitude points.
  - **Nearest airport** — Closest airport to a chosen coordinate (skips rows with unparseable coordinates).
  - **Radius search** — List all airports within a given distance (km) of a point.
  - **Elevation analysis** — Highest and lowest elevation airports in the loaded data.
  - **Sorting** — Sort a copy of the list by fields such as name, country, type, elevation, latitude, or longitude.
  - **Missing IATA detection** — List airports with an empty IATA code or a `\N` placeholder where data is missing.

## How to compile

From the project root directory:

```bash
g++ -std=c++17 -Wall -Wextra -o AirportSearchEngine src/main.cpp src/DataManager.cpp src/SearchEngine.cpp src/AnalysisEngine.cpp src/UI.cpp
```

## How to run

Ensure `data/airports.csv` exists (see [Data source](#data-source) below), then:

```bash
./AirportSearchEngine
```

## Project structure

```
AirportSearchEngine/
├── data/           # Input CSV and related data files
├── report/         # Coursework reports or exports (optional)
└── src/            # C++ source and headers
    ├── Airport.h
    ├── DataManager.h / DataManager.cpp
    ├── SearchEngine.h / SearchEngine.cpp
    ├── AnalysisEngine.h / AnalysisEngine.cpp
    ├── UI.h / UI.cpp
    └── main.cpp
```

## Data source

Airport records can be obtained from the open dataset:

https://raw.githubusercontent.com/datasets/airport-codes/master/data/airport-codes.csv

Save or preprocess the file as needed so it matches the column layout expected by this application (e.g. `ident`, `type`, `name`, `elevation_ft`, `continent`, `iso_country`, `iso_region`, `municipality`, `icao_code`, `iata_code`, `gps_code`, `local_code`, `coordinates`).

## Technologies used

- **C++17**
- **STL containers** (e.g. `std::vector`, `std::map`, `std::string`)
- **Haversine formula** for spherical distance in kilometres
