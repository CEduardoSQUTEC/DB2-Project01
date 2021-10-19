//
// Created by jlr on 10/12/21.
//
#include "SeqFile.h"

// Constructor
template<typename T, typename R>
SeqFile<T, R>::SeqFile(string filename, size_t auxFactor) {
    this->filename = filename;    // Name
    this->auxFactor = auxFactor;  // Max Number of AuxRecords
    createBinary();
    this->validRecord = countRecords();  // Non-auxiliary records (initially the number of records)
    this->auxRecord = 0; // Number of auxiliary records, initially 0
}

// Creating binary file
template<typename T, typename R>
void SeqFile<T, R>::createBinary() {
    fstream infile;
    fstream outfile;
    infile.open(filename.c_str(), ios::in);
    if (!infile) {
        cerr << "COULD NOT OPEN INFILE \n";
        exit(1);
    }

    long s = 0;
    string str;

    size_t pos = filename.find(".");
    string extractName = (string::npos == pos) ? filename : filename.substr(0, pos);
    string newFile = extractName + "_seq.csv";
    datfile = regex_replace(newFile, regex("csv"), "dat");

    outfile.open(datfile, ios::out | ios::binary);
    if (!outfile) {
        cerr << "COULD NOT OPEN OUTFILE \n";
        exit(1);
    }
    getline(infile, str, '\n');
    R r;
    while (getline(infile, str, '\n')) {
        r.serialization(str);
        r.next = (++s);
        outfile.write((char *) &r, sizeof(R));
        // r.print();
    }

    r.next = -1;
    outfile.seekp((--s) * sizeof(R));
    outfile.write((char *) &r, sizeof(R));

    outfile.close();

//    outfile.open(datfile, ios::in | ios::binary);
//    outfile.seekg(3 *sizeof(R));
//    outfile.read((char*) &r, sizeof(R));
//    r.print();

    infile.close();

    cout << "CREATED BINARY!!\n";
}

// Count total of records
template<typename T, typename R>
size_t SeqFile<T, R>::countRecords() {
    fstream infile;
    infile.open(datfile, ios::in | ios::binary);
    infile.seekg(0, infile.end);
    size_t size = infile.tellg();
    infile.close();

    return size / sizeof(R);
}

// Add new record as auxiliary
template<typename T, typename R>
void SeqFile<T, R>::add(R record) {

    // If number of aux records reaches auxFactor, reconstruct
    if (auxRecord >= auxFactor) reconstruct();

    fstream file;
    file.open(datfile, ios::binary | ios::out | ios::in);

    // Find previous record position
    long pos;
    binarySearch(record.key, this, pos);

    R prev;

    // Insert at the beginning
    if (pos < 0) {
        file.seekg(0);
        file.read((char *) &prev, sizeof(R));
        record.next = validRecord + auxRecord;
        file.seekp(0);
        file.write((char *) &prev, sizeof(R));
        file.seekp(0, file.end);
        file.write((char *) &prev, sizeof(R));
        auxRecord++;
        cout << "Added record!\n";
        return;
    }

    // Read previous record
    file.seekg(pos * sizeof(R));
    file.read((char *) &prev, sizeof(R));

    // New record next points to prev record next
    record.next = prev.next;
    // prev record next points to new record (last record)
    prev.next = validRecord + auxRecord;

    // Write records;
    file.seekp(0, file.end);
    file.write((char *) &record, sizeof(R));
    file.seekp(pos * sizeof(R));
    file.write((char *) &prev, sizeof(R));

    auxRecord++;
    cout << "Added record!\n";
}

// Remove record from file (logically)
template<typename T, typename R>
void SeqFile<T, R>::remove(T key) {
    fstream file;
    file.open(datfile, ios::in | ios::out | ios::binary);
    R record;
    R prev;
    long prevPos;
    long pos;
    // Find record's position
    bool flag = binarySearch(key, this, pos);
    if (!flag) {
        // R with lowest key is valid, if pos<0, record is not on dataset
        if (pos < 0) {
            cout << "ERROR! COULD NOT FOUND RECORD WITH KEY " << key << ".\n";
            return;
        } else {
            // If not found and pos >= 0, read record at position
            prevPos = pos;
            file.seekg(prevPos * sizeof(R));
            file.read((char *) &prev, sizeof(R));
            if (prev.next == -1) {
                // If read record is the last one
                cout << "ERROR! COULD NOT FIND RECORD WITH KEY " << key << ".\n";
                return;
            }
            pos = prev.next;
            file.seekg(pos * sizeof(R));
            file.read((char *) &record, sizeof(R));

            // While next record belongs to auxiliary part, keep searching
            while (pos >= validRecord && pos != -1) {
                // If record is in auxiliary part, remove
                if (record.key == key) {
                    prev.next = record.next;
                    record.next = -2;
                    file.seekp(prevPos * sizeof(R));
                    file.write((char *) &prev, sizeof(R));
                    file.seekp(pos * sizeof(R));
                    file.write((char *) &record, sizeof(R));

                    cout << "REMOVED RECORD WITH KEY " << key << ".\n";
                    return;
                }
                prevPos = pos;
                prev = record;
                file.seekg(prev.next * sizeof(R));
                file.read((char*) &record, sizeof(R));
                pos = prev.next;
            }
            // If record was not in auxiliary part
            cout << "ERROR! COULD NOT FIND RECORD WITH KEY " << key << ".\n";
            return;
        }
    } else {
        prevPos = pos - 1;
        // Read record on the physical position before the record to remove
        file.seekg(prevPos * sizeof(R));
        file.read((char *) &prev, sizeof(R));
        // Read next record to the previously read
        file.seekg(prev.next * sizeof(R));
        file.read((char *) &record, sizeof(R));
        // If previous record is in auxiliary part, iterate through positions
        // of next while keeping the last read record
        while (record.key != key) {
            prevPos = prev.next;
            prev = record;
            file.seekg(prev.next * sizeof(R));
            file.read((char *) &record, sizeof(R));
        }
        // Removed record points -2 while previous record points to its next
        prev.next = record.next;
        record.next = -2;
        // Write changes to file
        file.seekp(prevPos * sizeof(R));
        file.write((char *) &prev, sizeof(R));
        file.seekp(pos * sizeof(R));
        file.write((char *) &record, sizeof(R));
    }
    cout << "RECORD WITH KEY " << key << "HAS BEEN REMOVED!\n";
}

template<typename T, typename R>
R SeqFile<T, R>::search(T key) {
    R record;
    fstream infile;
    infile.open(datfile, ios::in | ios::binary);

    // Find record position
    long pos;
    bool flag = binarySearch(key, this, pos);

    if (!flag) {
        // If pos < 0 : the searched record is not in dataset
        if (pos > 0) {
            cout << "ERROR WITH KEY: " << key << endl;
            return R();
        } else {

            // If not found and pos >= 0, read record at pos
            infile.seekg(pos * sizeof(R));
            infile.read((char *) &record, sizeof(R));
            // While next record belongs to the auxiliary part, search for key
            while (record.next >= validRecord && record.next != -1) {
                infile.seekg(record.next * sizeof(R));
                infile.read((char *) &record, sizeof(R));
                if (record.key == key) {
                    cout << "R with key " << key << " found!\n";
                    return record;
                }
            }
        }
        cout << "ERROR. COULD NOT FOUND RECORD WITH KEY " << key << ".\n";
        return R();
    } else {
        // If key is found, return record at position
        infile.seekg(pos * sizeof(R));
        infile.read((char *) &record, sizeof(R));
    }
    cout << "R with key " << key << " found!\n";
    return record;
}

template<typename T, typename R>
vector<R> SeqFile<T, R>::rangeSearch(T beginkey, T endkey) {
    vector<R> v;
    // if end-key is lower than begin-key
    if (endkey < beginkey) {
        cout << "ERROR. KEYs ARE WRONG!\n";
        return v;
    }

    fstream infile;
    infile.open(datfile, ios::in | ios::binary);
    R record;
    // Find begin position
    long begin;
    bool flag = binarySearch(beginkey, this, begin);

    // If not found
    if (!flag) {
        // If pos is before beginning, start from 1st record
        if (begin < 0) {
            begin = 0;
            infile.seekg(0);
        } else { // Search from the next record of begin
            infile.seekg(begin * sizeof(R));
            infile.read((char *) &record, sizeof(R));
            infile.seekg(record.next * sizeof(R));
        }
    } else { // If found, start search from that record
        infile.seekg(begin * sizeof(R));
    }
    infile.read((char *) &record, sizeof(record));
    T current = record.key;

    // Add records to v until the current key is grater than endkey
    // or all records are read
    while (current <= endkey && record.next != -1) {
        v.push_back(record);
        infile.seekg(record.next * sizeof(R));
        infile.read((char *) &record, sizeof(R));
        current = record.key;
    }
    if (record.key <= endkey) v.push_back(record);
    infile.close();
    return v;
}

// Create a new file where all records become valid and stored in physical order
template<typename T, typename R>
void SeqFile<T, R>::reconstruct() {
    string reconstruct = "reconstruct.dat";
    fstream infile(datfile, ios::in | ios::binary);
    fstream outfile(reconstruct, ios::out | ios::binary);

    R record;
    long next;
    long cont = 1;

    // Read first record and write to new file
    infile.seekg(0);
    infile.read((char *) &record, sizeof(R));
    next = record.next;
    record.next = cont++;
    outfile.seekp(0);
    outfile.write((char *) &record, sizeof(R));

    // Read file through pointers
    while (next != -1) {
        infile.seekg(next * sizeof(R));
        infile.read((char *) &record, sizeof(R));
        next = record.next;
        if (next != -1) record.next = cont++;
        outfile.write((char *) &record, sizeof(R));

        vector<R> auxRec;
        vector<long> auxNext;
        // If next is in auxiliary part, store all until auxiliary part is left
        // or end is reached
        while (next >= validRecord && next != -1) {
            infile.seekg(next * sizeof(R));
            infile.read((char *) &record, sizeof(R));
            next = record.next;
            auxRec.push_back(record);
            auxNext.push_back(record.next);
        }

        // Sort vector by key to write them in order
        sort(auxRec.begin(), auxRec.end(), [](R &r1, R &r2) { return r1.key < r2.key; });
        for (size_t i = 0; i < auxRec.size(); i++) {
            auxRec[i].next = auxNext[i];
            if (auxRec[i].next != -1) auxRec[i].next = cont++;
            outfile.write((char *) &record, sizeof(R));
        }
    }

    infile.close();
    outfile.close();

}

// Binary Search for Sequential File
// Returns true if key is found
template<typename T, typename R>
bool binarySearch(T key, SeqFile<T, R> *seqFile, long &pos) {
    fstream infile;
    infile.open(seqFile->datfile, ios::in | ios::binary);
    R record;
//    record.print();
    long left = 0;
    long right = seqFile->validRecord - 1;
    long mid;
    while (right >= left) {
        mid = floor((left + right) / 2);

        infile.seekp(mid * sizeof(R));
        infile.read((char *) &record, sizeof(R));
        //   cout <<"mid: "<< mid << " -- "<< right << " -- "<< key << " -- " << left <<endl;
        //   cout <<"KEY: "<< record.key << " YEAR: "<< record.year << " TITLE: "<< record.title <<endl;
        if (key == record.key) {
            pos = mid;
            return true;
        } else if (key < record.key || record.next == -2)
            right = mid - 1;
        else if (key > record.key)
            left = mid + 1;
    }
    infile.close();
    pos = right;
    return false;
}