#pragma once

#include "DataManager.h"
#include "SearchEngine.h"
#include "AnalysisEngine.h"
#include <vector>
#include <string>

class UI {
public:
    // Wires the UI to the shared data/search/analysis objects that own or operate on airport records.
    UI(DataManager& dataManager, SearchEngine& searchEngine, AnalysisEngine& analysisEngine);

    // Runs the interactive command loop until the user exits from the main menu.
    void run();

private:
    // Prints the numbered main menu actions available to the user.
    void showMainMenu();

    // Prompts for a country code and displays airports in that ISO country (menu option 1).
    void handleSearchByCountry();

    // Prompts for a name substring and lists airports whose names contain that text (option 2).
    void handleSearchByName();

    // Prompts for an IATA code and lists matching airports (option 3).
    void handleSearchByIATA();

    // Shows how many airports exist for each type string using the search engine histogram (option 4).
    void handleCountByType();

    // Collects fields for a new airport from the user and appends it to the dataset (option 5).
    void handleAddAirport();

    // Persists the current in-memory airports back to the configured CSV path (option 6).
    void handleSaveChanges();

    // Opens the nested menu for analysis-style operations (option 7).
    void handleAdvancedOptions();

    // Renders a compact table of many airports for quick scanning.
    void displayAirports(const std::vector<Airport>& list);

    // Prints every field of a single airport in a readable multi-line layout.
    void displayAirport(const Airport& ap);

    // Repeatedly asks until the user enters a valid integer, then returns that value.
    int getValidInt(const std::string& prompt);

    DataManager& dataManager;
    SearchEngine& searchEngine;
    AnalysisEngine& analysisEngine;
    std::string csvFilename;
};
