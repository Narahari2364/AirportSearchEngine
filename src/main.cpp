#include <iostream>

#include "DataManager.h"
#include "SearchEngine.h"
#include "AnalysisEngine.h"
#include "UI.h"

int main() {
    // 1) Welcome message so the user knows the program started.
    std::cout << "Starting Nottingham SkyRoute Explorer...\n";

    // 2) Create the data manager responsible for loading/saving the CSV dataset.
    DataManager dataManager;

    // 3) Load the airports from the CSV file; abort if the file cannot be read.
    if (!dataManager.loadFromCSV("data/airports.csv")) {
        std::cerr << "Error: failed to load data/airports.csv\n";
        return 1;
    }

    // 4) Create the search engine using the shared airports vector (no copying of the dataset).
    SearchEngine searchEngine(dataManager.getAirports());

    // 5) Create the analysis engine using the same shared airports vector.
    AnalysisEngine analysisEngine(dataManager.getAirports());

    // 6) Create the UI and hand it references to the three components.
    UI ui(dataManager, searchEngine, analysisEngine);

    // 7) Start the interactive menu loop.
    ui.run();

    // 8) Exit normally.
    return 0;
}
