#include "Test/menu.h"
<<<<<<< HEAD
//#include "Test/menu.cpp"
=======
#include "Record/Netflix.h"
#include "Record/Basket.h"
#include "Record/WorldPopulation.h"
>>>>>>> bef8879987f5d39a0913328a4222521348d54831

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
        cout << "Missing method!!\n";
    } else if (action == "Test") {
        if (filename == "netflix.csv") {
            testTimeNetflix(filename, 5, -100, 1500);
        } else if (filename == "WorldPopulation.csv") {
            testTimeWorld(filename, 5, 1920, 2040);
        }
    }

    return 0;
}