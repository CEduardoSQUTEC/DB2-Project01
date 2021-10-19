//
// Created by jlr on 10/15/21.
//
<<<<<<< HEAD
//
//#include "menu.h"
//
=======

#include "menu.h"

void clear() {
    for (int i = 0; i < 30; ++i) {
        cout << endl;
    }
}

string chooseFile() {
    int n;
    do {
        clear();
        cout << "  Choose dataset:  \n";
        cout << "-------------------\n";
        cout << "1. netflix\n";
        cout << "2. WorldPopulation\n";
        cout << "3. basket_analysis\n";
        cin >> n;
    } while (n < 1 || n > 3);
    switch (n) {
        case 1:
            return "netflix.csv";
        case 2:
            return "WorldPopulation.csv";
        case 3:
            return "basket_analysis.csv";
        default:
            cerr << "ERROR!!";
            break;
    }
    return "";
}

string chooseAction() {
    int n;
    do {
        clear();
        cout << "  Choose action:  \n";
        cout << "------------------\n";
        cout << "1. Sequential File\n";
        cout << "2. Extendible Hash\n";
        cout << "3. Execution Time \n";
        cin >> n;
    } while (n < 1 || n > 3);
    switch (n) {
        case 1:
            return "sequential";
        case 2:
            return "hash";
        case 3:
            return "Test";
        default:
            cerr << "ERROR!!";
            break;
    }
    return "";
}

template<typename R>
void sequentialTest(string filename) {
    int aux;
    clear();
    cout << "Set auxiliary record factor : ";
    cin >> aux;

    SeqFile<long, R> sf(filename, aux);

    int n;
    do {
        do {
            clear();
            cout << "   Choose action:   \n";
            cout << "--------------------\n";
            cout << "1. Add record\n";
            cout << "2. Remove record\n";
            cout << "3. Search record\n";
            cout << "4. Search records in range\n";
            cin >> n;
        } while (n < 1 || n > 4);

        long key, start, end;
        switch (n) {
            case 1: {
                clear();
                R record;
                record.input();
                cout << endl;
                sf.add(record);
                break;
            }
            case 2: {
                clear();
                cout << "Insert key: ";
                cin >> key;
                sf.remove(key);
                break;
            }
            case 3: {
                clear();
                cout << "Insert key: ";
                cin >> key;
                cout << endl;
                R record2 = sf.search(key);
                record2.print();
                break;
            }
            case 4: {
                clear();
                cout << "Insert start key: ";
                cin >> start;
                cout << "Insert end key: ";
                cin >> end;
                cout << endl;
                vector<R> v = sf.rangeSearch(start, end);
                for (auto i: v) {
                    i.print();
                }
                break;
            }
            default:
                cerr << "ERROR";
                break;
        }

        cout << "Enter 0 to exit or any other number to continue...\n";
        cin >> n;
    } while (n != 0);
}

template<typename R>
void hashTest(string filename) {

}

void testTimeNetflix(string filename, int auxFactor, long start, long end) {

}

void testTimeWorld(string filename, int auxFactor, long start, long end) {

}
>>>>>>> bef8879987f5d39a0913328a4222521348d54831
