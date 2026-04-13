#include "UI.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace {

// Prints a lot of newlines to visually separate screens without platform-specific system calls.
void clearScreen() {
    for (int i = 0; i < 30; ++i) {
        std::cout << '\n';
    }
}

// Reads a double safely using std::getline + stringstream, so it doesn't conflict with getline-based prompts.
double getValidDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);

        std::stringstream ss(line);
        double value = 0.0;
        char extra = '\0';
        if ((ss >> value) && !(ss >> extra)) {
            return value;
        }
        std::cout << "Invalid number. Please try again.\n";
    }
}

// Reads a yes/no response and returns true for yes.
bool getYesNo(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string ans;
        std::getline(std::cin, ans);
        if (ans == "y" || ans == "Y") return true;
        if (ans == "n" || ans == "N") return false;
        std::cout << "Please enter y or n.\n";
    }
}

} // namespace

UI::UI(DataManager& dm, SearchEngine& se, AnalysisEngine& ae)
    : dataManager(dm),
      searchEngine(se),
      analysisEngine(ae),
      csvFilename("data/airports.csv") {
    // Store references to the shared engines and configure the default CSV location.
}

void UI::run() {
    int choice = 0;

    // Main menu loop: show options, read a choice, then dispatch to the right handler.
    do {
        clearScreen();
        showMainMenu();
        choice = getValidInt("Enter your choice: ");

        // Execute one action per selection.
        switch (choice) {
        case 1:
            handleSearchByCountry();
            break;
        case 2:
            handleSearchByName();
            break;
        case 3:
            handleSearchByIATA();
            break;
        case 4:
            handleCountByType();
            break;
        case 5:
            handleAddAirport();
            break;
        case 6:
            handleSaveChanges();
            break;
        case 7:
            handleAdvancedOptions();
            break;
        case 8:
            // Save on exit, then leave the loop.
            handleSaveChanges();
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "Invalid option. Please choose 1-8.\n";
            break;
        }

        // Pause so the user can read output before the next menu redraw.
        if (choice != 8) {
            std::cout << "\nPress Enter to continue...";
            std::string dummy;
            std::getline(std::cin, dummy);
        }
    } while (choice != 8);
}

void UI::showMainMenu() {
    // Print the title and options exactly as specified.
    std::cout << "====== Nottingham SkyRoute Explorer ======\n"
              << "1. Search airports by country\n"
              << "2. Search airports by name\n"
              << "3. Search by IATA code\n"
              << "4. Count airports by type\n"
              << "5. Add a new airport\n"
              << "6. Save changes\n"
              << "7. Advanced options\n"
              << "8. Exit\n"
              << "==========================================\n";
}

void UI::handleSearchByCountry() {
    // Read the country code from the user and run the corresponding search.
    std::cout << "Enter ISO country code (e.g. SG, US): ";
    std::string code;
    std::getline(std::cin, code);

    const std::vector<Airport> results = searchEngine.searchByCountry(code);
    std::cout << "Found " << results.size() << " airports.\n";
    displayAirports(results);
}

void UI::handleSearchByName() {
    // Read a partial name and show all airports whose name contains that substring.
    std::cout << "Enter airport name (partial): ";
    std::string partial;
    std::getline(std::cin, partial);

    const std::vector<Airport> results = searchEngine.searchByName(partial);
    std::cout << "Found " << results.size() << " airports.\n";
    displayAirports(results);
}

void UI::handleSearchByIATA() {
    // Search by the 3-letter IATA code and show the first match if any exist.
    std::cout << "Enter IATA code: ";
    std::string code;
    std::getline(std::cin, code);

    const std::vector<Airport> results = searchEngine.searchByIATA(code);
    if (!results.empty()) {
        displayAirport(results.front());
    } else {
        std::cout << "No airport found with IATA code '" << code << "'.\n";
    }
}

void UI::handleCountByType() {
    // Use the SearchEngine histogram method and print each type with its count.
    const std::map<std::string, int> counts = searchEngine.countByType();

    std::cout << std::left << std::setw(25) << "TYPE" << std::right << std::setw(10) << "COUNT" << '\n';
    std::cout << std::string(35, '-') << '\n';

    for (const auto& [type, count] : counts) {
        std::cout << std::left << std::setw(25) << type << std::right << std::setw(10) << count << '\n';
    }
}

void UI::handleAddAirport() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Airport ap;

    // Prompt for each CSV field one by one.
    std::cout << "Enter ident: ";
    std::getline(std::cin, ap.ident);

    std::cout << "Enter type: ";
    std::getline(std::cin, ap.type);

    std::cout << "Enter name: ";
    std::getline(std::cin, ap.name);

    std::cout << "Enter elevation_ft (leave blank for 0): ";
    std::string elevationStr;
    std::getline(std::cin, elevationStr);
    if (elevationStr.empty()) {
        ap.elevation_ft = 0.0f;
    } else {
        try {
            ap.elevation_ft = std::stof(elevationStr);
        } catch (const std::exception&) {
            ap.elevation_ft = 0.0f;
        }
    }

    std::cout << "Enter continent: ";
    std::getline(std::cin, ap.continent);

    std::cout << "Enter iso_country: ";
    std::getline(std::cin, ap.iso_country);

    std::cout << "Enter iso_region: ";
    std::getline(std::cin, ap.iso_region);

    std::cout << "Enter municipality: ";
    std::getline(std::cin, ap.municipality);

    std::cout << "Enter icao_code: ";
    std::getline(std::cin, ap.icao_code);

    std::cout << "Enter iata_code: ";
    std::getline(std::cin, ap.iata_code);

    std::cout << "Enter gps_code: ";
    std::getline(std::cin, ap.gps_code);

    std::cout << "Enter local_code: ";
    std::getline(std::cin, ap.local_code);

    std::cout << "Enter coordinates (lat, lon): ";
    std::getline(std::cin, ap.coordinates);

    // Add to the shared dataset via DataManager.
    dataManager.addAirport(ap);
    std::cout << "Airport added successfully.\n";
}

void UI::handleSaveChanges() {
    // Save the current in-memory airport list back to the CSV file.
    if (dataManager.saveToCSV(csvFilename)) {
        std::cout << "Saved changes to '" << csvFilename << "'.\n";
    } else {
        std::cout << "Failed to save changes to '" << csvFilename << "'.\n";
    }
}

void UI::handleAdvancedOptions() {
    // Advanced analysis submenu loop; returns when the user chooses to go back.
    while (true) {
        clearScreen();
        std::cout << "Advanced options\n\n"
                  << "1. Find nearest airport to coordinates\n\n"
                  << "2. List airports within radius\n\n"
                  << "3. Highest elevation airport\n\n"
                  << "4. Lowest elevation airport\n\n"
                  << "5. Show airports with missing IATA codes\n\n"
                  << "6. Sort airports\n\n"
                  << "7. Back to main menu\n\n";

        const int choice = getValidInt("Enter your choice: ");

        if (choice == 7) {
            return;
        }

        switch (choice) {
        case 1: {
            // Prompt for coordinates and display the nearest airport using AnalysisEngine.
            const double lat = getValidDouble("Enter latitude: ");
            const double lon = getValidDouble("Enter longitude: ");
            try {
                const Airport nearest = analysisEngine.findNearestAirport(lat, lon);
                displayAirport(nearest);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << '\n';
            }
            break;
        }
        case 2: {
            // Prompt for a centre point and radius, then list all airports within that distance.
            const double lat = getValidDouble("Enter latitude: ");
            const double lon = getValidDouble("Enter longitude: ");
            const double radius = getValidDouble("Enter radius (km): ");
            const std::vector<Airport> results = analysisEngine.findAirportsWithinRadius(lat, lon, radius);
            std::cout << "Found " << results.size() << " airports within " << radius << " km.\n";
            displayAirports(results);
            break;
        }
        case 3: {
            // Highest elevation airport in the dataset.
            try {
                const Airport ap = analysisEngine.getHighestElevation();
                displayAirport(ap);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << '\n';
            }
            break;
        }
        case 4: {
            // Lowest elevation airport in the dataset.
            try {
                const Airport ap = analysisEngine.getLowestElevation();
                displayAirport(ap);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << '\n';
            }
            break;
        }
        case 5: {
            // Show all airports that do not have an IATA code.
            const std::vector<Airport> results = analysisEngine.getMissingIATA();
            std::cout << "Found " << results.size() << " airports with missing IATA codes.\n";
            displayAirports(results);
            break;
        }
        case 6: {
            // Sort a copy of the current dataset by a selected field and direction.
            std::cout << "Field options: name, country, type, elevation, latitude, longitude\n";
            std::cout << "Enter field: ";
            std::string field;
            std::getline(std::cin, field);

            const int dir = getValidInt("Ascending (1) or descending (0): ");
            const bool ascending = (dir != 0);

            try {
                std::vector<Airport> copy = dataManager.getAirports();
                const std::vector<Airport> sorted = analysisEngine.sortAirports(copy, field, ascending);
                displayAirports(sorted);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << '\n';
            }
            break;
        }
        default:
            std::cout << "Invalid option. Please choose 1-7.\n";
            break;
        }

        // Pause after each advanced action before redrawing the submenu.
        std::cout << "\nPress Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

void UI::displayAirports(const std::vector<Airport>& list) {
    // Handle an empty result set early.
    if (list.empty()) {
        std::cout << "No airports found.\n";
        return;
    }

    // If the list is very large, ask whether to display all results or limit output to 50.
    std::size_t limit = list.size();
    if (list.size() > 50) {
        const bool showAll = getYesNo("More than 50 results. Show all? (y/n): ");
        limit = showAll ? list.size() : 50;
    }

    // Print a fixed-width table header.
    std::cout << std::left
              << std::setw(10) << "IDENT"
              << std::setw(15) << "TYPE"
              << std::setw(35) << "NAME"
              << std::setw(10) << "COUNTRY"
              << std::setw(6) << "IATA"
              << '\n';
    std::cout << std::string(10 + 15 + 35 + 10 + 6, '-') << '\n';

    // Print each airport row, truncating long strings so the table stays aligned.
    for (std::size_t i = 0; i < limit; ++i) {
        auto trunc = [](const std::string& s, std::size_t maxLen) -> std::string {
            if (s.size() <= maxLen) return s;
            return s.substr(0, maxLen - 3) + "...";
        };

        const Airport& ap = list[i];
        std::cout << std::left
                  << std::setw(10) << trunc(ap.ident, 10)
                  << std::setw(15) << trunc(ap.type, 15)
                  << std::setw(35) << trunc(ap.name, 35)
                  << std::setw(10) << trunc(ap.iso_country, 10)
                  << std::setw(6) << trunc(ap.iata_code, 6)
                  << '\n';
    }

    if (limit < list.size()) {
        std::cout << "... showing first " << limit << " of " << list.size() << " results.\n";
    }
}

void UI::displayAirport(const Airport& ap) {
    // Show a detailed view with one labelled field per line.
    std::cout << "Ident: " << ap.ident << '\n'
              << "Type: " << ap.type << '\n'
              << "Name: " << ap.name << '\n'
              << "Elevation (ft): " << ap.elevation_ft << '\n'
              << "Continent: " << ap.continent << '\n'
              << "ISO Country: " << ap.iso_country << '\n'
              << "ISO Region: " << ap.iso_region << '\n'
              << "Municipality: " << ap.municipality << '\n'
              << "ICAO Code: " << ap.icao_code << '\n'
              << "IATA Code: " << ap.iata_code << '\n'
              << "GPS Code: " << ap.gps_code << '\n'
              << "Local Code: " << ap.local_code << '\n'
              << "Coordinates: " << ap.coordinates << '\n';
}

int UI::getValidInt(const std::string& prompt) {
    // Repeatedly prompt until a valid integer is read from std::cin.
    while (true) {
        std::cout << prompt;
        int value = 0;

        if (std::cin >> value) {
            // Consume the rest of the line so future getline calls behave correctly.
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        // Bad input: clear the stream state and discard the rest of the line.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid integer. Please try again.\n";
    }
}
