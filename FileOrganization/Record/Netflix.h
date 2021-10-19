//
// Created by jlr on 10/12/21.
//
#ifndef P1_NETFLIX_H
#define P1_NETFLIX_H

#include "Record.h"

class Netflix : public Record {
public:
    long key; // movie ID
    int year; // movie year
    string *title; // movie title
public:
    Netflix() {}

    Netflix(long key, long year, string *title) {
        this->key = key;
        this->year = year;
        this->title = new string();
        *(this->title) = *title;
    }

    Netflix(long key) {
        this->key = key;
        this->year = 100;
        this->title = new string();
    }

    long getKey() const {
        return key;
    }

    void serialization(string object) override {
        int i = 0;
        string it;
        for (char letter: object) {
            if (letter != ',' || i == 2)
                it.push_back(letter);
            else {
                switch (i) {
                    case 0:
                        this->key = stol(it);
                        break;
                    case 1:
                        this->year = stoi(it);
                        break;
                    default:
                        cerr << "Could not Serialize\n";
                        break;
                }
                i++;
                it.clear();
            }
        }
        this->title = new string(it);

    }

    void print() override {
        cout << " Id: " << this->key << endl;
        cout << " Year : " << this->year << endl;
        cout << "Title: " << *(this->title) << endl;
        cout << "Next record (logic position): " << next << endl;
        cout << "== == == == == == == == == ==\n";
    }

    void input() override {
        cout << " (KEY) Id: ";
        cin >> this->key;
        cout << "    Year : ";
        cin >> this->year;
        string t;
        cout << "MovieTitle: ";
        cin >> t;
        this->title = &t;
    }

};


#endif //P1_NETFLIX_H
