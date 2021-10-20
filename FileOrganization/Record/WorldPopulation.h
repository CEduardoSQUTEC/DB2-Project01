#ifndef FILEORGANIZATION_WORLD_H
#define FILEORGANIZATION_WORLD_H

#include "Record.h"

class WorldPopulation : public Record {
public:
    long key;   //year
    long long Population;
    float ChangePerc;
    long long NetChange;
    int Density;
    long long Urban;
    int UrbanPerc;
public:
    WorldPopulation() {}

    WorldPopulation(long key, long long Population, float ChangePerc,
                    long long NetChange, int Density, long long Urban, int UrbanPerc) {
        this->key = key;
        this->Population = Population;
        this->ChangePerc = ChangePerc;
        this->NetChange = NetChange;
        this->Density = Density;
        this->Urban = Urban;
        this->UrbanPerc = UrbanPerc;
    }

    // Constructor for testing
    WorldPopulation(long key) {
        this->key = key;
        this->Population = 100;
        this->ChangePerc = 100;
        this->NetChange = 100;
        this->Density = 100;
        this->Urban = 100;
        this->UrbanPerc = 100;
    }

    void serialization(string object) override {
        int i = 0;
        string att;
        for (char letter: object) {
            if (letter != ',') {
                att.push_back(letter);
            } else {
                switch (i) {
                    case 0:
                        key = stol(att);
                        break;
                    case 1:
                        Population = stoll(att);
                        break;
                    case 2:
                        ChangePerc = stof(att);
                        break;
                    case 3:
                        NetChange = stoll(att);
                        break;
                    case 4:
                        Density = stoi(att);
                        break;
                    case 5:
                        Urban = stoll(att);
                        break;
                    default:
                        cerr << "Error in serializing\n";
                        break;
                }
                i++;
                att.clear();
            }
        }
        // case 16:
        // after "for" ends
        UrbanPerc = stoi(att);
    }

    void print() override {
        cout << "Year: " << key << endl;
        cout << "Population: " << Population << endl;
        cout << "ChangePerc: " << ChangePerc << endl;
        cout << "NetChange: " << NetChange << endl;
        cout << "Density: " << Density << endl;
        cout << "Urban: " << Urban << endl;
        cout << "UrbanPerc: " << UrbanPerc << endl;
        cout << "Next record (logic position): " << next << endl;
        cout << "== == == == == == == == == ==\n";
    }

    void input() override {
        cout << "Year (key): ";
        cin >> key;
        cout << "Population: ";
        cin >> Population;
        cout << "ChangePerc: ";
        cin >> ChangePerc;
        cout << "NetChange: ";
        cin >> NetChange;
        cout << "Density: ";
        cin >> Density;
        cout << "Urban: ";
        cin >> Urban;
        cout << "UrbanPerc: ";
        cin >> UrbanPerc;
    }

    long getKey() const {
        return key;
    }

};

#endif