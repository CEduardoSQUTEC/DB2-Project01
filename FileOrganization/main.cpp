#include "Test/menu.h"
#include "Record/Netflix.h"
#include "Record/Basket.h"
#include "Record/WorldPopulation.h"

int main() {
    string filename = chooseFile();
    string action = chooseAction();

    if (action == "sequential") {
        if (filename == "netflix.csv") {
            sequentialTest<Netflix>(filename);
        } else if (filename == "WorldPopulation.csv") {
            sequentialTest<WorldPopulation>(filename);
        } else if (filename == "basket_analysis.csv") {
            sequentialTest<Basket>(filename);
        }
    } else if (action == "hash") {
        if (filename == "netflix.csv") {
            hashTest<long, Netflix>("netflix");
        } else if (filename == "WorldPopulation.csv") {
            hashTest<long, WorldPopulation>("WorldPopulation");
        } else if (filename == "basket_analysis.csv") {
            hashTest<long, Basket>("basket_analysis");
        }
    } else if (action == "Test") {
        if (filename == "netflix.csv") {
            testTimeNetflix(filename, 5, -100, 1500);
        } else if (filename == "WorldPopulation.csv") {
            testTimeWorld(filename, 5, 1920, 2040);
        }
    }

    return 0;
}