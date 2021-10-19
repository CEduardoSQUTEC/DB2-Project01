//
// Created by jlr on 10/12/21.
//
#include "SeqFile.h"

// Constructor
template <typename T, typename Record>
SeqFile<T,Record>::SeqFile(string filename, size_t auxFactor) {
    this->filename = filename;    // Name
    this->auxFactor = auxFactor;  // Max Number of AuxRecords
    createBinary();
    this->validRecord = countRecords();  // Non-auxiliary records (initially the number of records)
    this->auxRecord = 0; // Number of auxiliary records, initially 0
}

// Creating binary file
template <typename T, typename Record>
void SeqFile<T,Record>::createBinary() {
    fstream infile;
    fstream outfile;

    infile.open(filename.c_str(), ios::in);
    if(!infile){
        cerr << "COULD NOT OPEN INFILE \n";
        exit(1);
    }

    long s = 0;
    string str;

    size_t pos = filename.find(".");
    string extractName = (string::npos == pos) ? filename : filename.substr(0,pos);
    string newFile = extractName + "_seq.csv";
    datfile = regex_replace(newFile, regex("csv"), "dat");

    outfile.open(datfile, ios::out | ios::binary);
    if(!outfile) {
        cerr << "COULD NOT OPEN OUTFILE \n";
        exit(1);
    }
    getline(infile, str, '\n');
    Record r;
    while (getline(infile, str, '\n')){
        r.serialization(str);
        r.next = (++s);
        outfile.write((char*) &r, sizeof(Record));
        // r.print();
    }

    r.next = -1;
    outfile.seekp((--s)* sizeof(Record));
    outfile.write((char*) &r, sizeof(Record));

    outfile.close();
    infile.close();

    cout << "CREATED BINARY!!\n";
}

// Count total of records
template <typename T, typename Record>
size_t SeqFile<T,Record>::countRecords() {
    fstream infile;
    infile.open(datfile, ios::in | ios::binary);
    infile.seekg(0, infile.end);
    size_t size = infile.tellg();
    infile.close();

    return size/sizeof(Record);
}

// Add new record as auxiliary
template <typename T, typename Record>
void SeqFile<T,Record>::add(Record record) {

    // If number of aux records reaches auxFactor, reconstruct
    if (auxRecord >= auxFactor) reconstruct();

    fstream file;
    file.open(datfile, ios::binary | ios::out | ios::in);

    // Find previous record position
    long pos;
    binarySearch(record.key, this, pos);

    Record prev;

    // Insert at the beginning
    if(pos < 0){
        file.seekg(0);
        file.read((char*) &prev, sizeof(Record));
        record.next = validRecord + auxRecord;
        file.seekp(0);
        file.write((char*) &prev, sizeof(Record));
        file.seekp(0,file.end);
        file.write((char *) &prev, sizeof(Record));
        auxRecord++;
        cout << "Added record!\n";
        return;
    }

    // Read previous record
    file.seekg(pos* sizeof(Record));
    file.read((char *) &prev, sizeof(Record));

    // New record next points to prev record next
    record.next = prev.next;
    // prev record next points to new record (last record)
    prev.next = validRecord + auxRecord;

    // Write records;
    file.seekp(0, file.end);
    file.write((char*) &record, sizeof(Record));
    file.seekp(pos * sizeof(Record));
    file.write((char *) &prev, sizeof(Record));

    auxRecord++;
    cout << "Added record!\n";
}

// Remove record from file (logically)
template <typename T, typename Record>
void SeqFile<T,Record>::remove(T key) {
    fstream file;
    file.open(datfile, ios::in | ios::out | ios::binary);
    Record record;
    Record prev;
    long prevPos;
    long pos;
    // Find record's position
    bool flag = binarySearch(key, this, pos);
    if (!flag){
        // Record with lowest key is valid, if pos<0, record is not on dataset
        if (pos < 0){
            cout << "ERROR! COULD NOT FOUND RECORD WITH KEY " << key <<".\n";
            return;
        } else {
            // If not found and pos >= 0, read record at position
            prevPos = pos;
            file.seekg(prevPos * sizeof(Record));
            file.read((char *) &prev, sizeof(Record));
            if (prev.next == -1){
                // If read record is the last one
                cout << "ERROR! COULD NOT FIND RECORD WITH KEY " << key << ".\n";
                return;
            }
            pos = prev.next;
            file.seekg(pos * sizeof(Record));
            file.read((char *) &record, sizeof(Record));

            // While next record belongs to auxiliary part, keep searching
            while (pos >= validRecord && pos != -1){
                // If record is in auxiliary part, remove
                if(record.key == key){
                    prev.next = record.next;
                    record.next = -2;
                    file.seekp(prevPos * sizeof(Record));
                    file.write((char *) &prev, sizeof(Record));
                    file.seekp(pos*sizeof(Record));
                    file.write((char*) &record, sizeof(Record));

                    cout << "REMOVED RECORD WITH KEY " << key << ".\n";
                    return;
                }
            }
            // If record was not in auxiliary part
            cout << "ERROR! COULD NOT FIND RECORD WITH KEY " << key << ".\n";
            return;
        }
    } else {
        prevPos = pos - 1;
        // Read record on the physical position before the record to remove
        file.seekg(prevPos * sizeof(Record));
        file.read((char *) &prev, sizeof(Record));
        // Read next record to the previously read
        file.seekg(prev.next * sizeof(Record));
        file.read((char *) &prev, sizeof(Record));
        // If previous record is in auxiliary part, iterate through positions
        // of next while keeping the last read record
        while (record.key != key){
            prevPos = prev.next;
            prev = record;
            file.seekg(prev.next * sizeof(Record));
            file.read((char *) &record, sizeof(Record));
        }
        // Removed record points -2 while previous record points to its next
        prev.next = record.next;
        record.next = -2;
        // Write changes to file
        file.seekp(prevPos * sizeof(Record));
        file.write((char *) &prev, sizeof(Record));
        file.seekp(pos * sizeof(Record));
        file.write((char *) &record, sizeof(Record));
    }
    cout << "RECORD WITH KEY " << key << "HAS BEEN REMOVED!\n";
}

template <typename T, typename Record>
Record SeqFile<T,Record>::search(T key) {
    Record record;
    fstream infile;
    infile.open(datfile, ios::in | ios::binary);

    // Find record position
    long pos;
    /*if (flag)*/ cout << "BINARYY\n";
    bool flag = binarySearch(key, this, pos);
    puts("sal pls/n");
    /*if (flag)*/ cout << "TRUEEE\n";

    if(!flag) {
        // If pos < 0 : the searched record is not in dataset
        if (pos > 0) {
            cout << "ERROR WITH KEY: " << key << endl;
            return Record();
        } else {

            // If not found and pos >= 0, read record at pos
            infile.seekg(pos * sizeof(Record));
            infile.read((char *) &record, sizeof(Record));
            // While next record belongs to the auxiliary part, search for key
            while (record.next >= validRecord && record.next != -1) {
                infile.seekg(record.next * sizeof(Record));
                infile.read((char *) &record, sizeof(Record));
                if (record.key == key) {
                    cout << "Record with key " << key << " found!\n";
                    return record;
                }
            }
        }
        cout << "ERROR. COULD NOT FOUND RECORD WITH KEY " << key << ".\n";
        return Record();
    } else {
        // If key is found, return record at position
        cout << "ELSEE\n";
        infile.seekg(pos * sizeof(Record));
        infile.read((char *) &record, sizeof(Record));
    }
    cout << "Record with key " << key << " found!\n";
    return record;
}

template <typename T, typename Record>
vector<Record> SeqFile<T,Record>::rangeSearch(T beginkey, T endkey) {
    vector<Record> v;
    // if end-key is lower than begin-key
    if(endkey < beginkey){
        cout << "ERROR. KEYs ARE WRONG!\n";
        return v;
    }

    fstream infile;
    infile.open(datfile, ios::in | ios::binary);
    Record record;
    // Find begin position
    long begin;
    bool flag = binarySearch(beginkey, this, begin);

    // If not found
    if(!flag) {
        // If pos is before beginning, start from 1st record
        if (begin < 0) {
            begin = 0;
            infile.seekg(0);
        } else { // Search from the next record of begin
            infile.seekg(begin * sizeof(Record));
            infile.read((char*) &record, sizeof(Record));
            infile.seekg(record.next * sizeof(Record));
        }
    } else { // If found, start search from that record
        infile.seekg(begin * sizeof(Record));
    }
    infile.read((char *) &record, sizeof(record));
    T current = record.key;

    // Add records to v until the current key is grater than endkey
    // or all records are read
    while (current <= endkey && record.next != -1){
        v.push_back(record);
        infile.seekg(record.next * sizeof(Record));
        infile.read((char *) &record, sizeof(Record));
        current = record.key;
    }
    if (record.key <= endkey) v.push_back(record);
    infile.close();
    return v;
}

// Create a new file where all records become valid and stored in physical order
template <typename T, typename Record>
void SeqFile<T,Record>::reconstruct() {
    string reconstruct = "reconstruct.dat";
    fstream infile(datfile, ios::in | ios::binary);
    fstream outfile(reconstruct, ios::out | ios::binary);

    Record record;
    long next;
    long cont = 1;

    // Read first record and write to new file
    infile.seekg(0);
    infile.read((char *) &record, sizeof (Record));
    next = record.next;
    record.next = cont++;
    outfile.seekp(0);
    outfile.write((char*) &record, sizeof(Record));

    // Read file through pointers
    while (next != -1){
        infile.seekg(next * sizeof(Record));
        infile.read((char*) &record, sizeof(Record));
        next = record.next;
        if(next != -1) record.next = cont++;
        outfile.write((char *) &record, sizeof(Record));

        vector<Record> auxRec;
        vector<long> auxNext;
        // If next is in auxiliary part, store all until auxiliary part is left
        // or end is reached
        while(next >= validRecord && next != -1){
            infile.seekg(next * sizeof(Record));
            infile.read((char*) &record, sizeof(Record));
            next = record.next;
            auxRec.push_back(record);
            auxNext.push_back(record.next);
        }

        // Sort vector by key to write them in order
        sort(auxRec.begin(), auxRec.end(), [](Record &r1, Record &r2){return r1.key < r2.key;});
        for(size_t i = 0; i < auxRec.size(); i++){
            auxRec[i].next = auxNext[i];
            if(auxRec[i].next != -1) auxRec[i].next = cont++;
            outfile.write((char *) &record, sizeof(Record));
        }
    }

    infile.close();
    outfile.close();

}

// Binary Search for Sequential File
// Returns true if key is found
template<typename T, typename Record>
bool binarySearch(T key, SeqFile<T, Record>* seqFile, long & pos){
    fstream infile;
    infile.open(seqFile->datfile, ios::in | ios::binary);
    Record record;
    record.print();
    long left = 0;
    long right = seqFile->validRecord -1;
    long mid;
    while (right >= left){
      mid = floor((left+right)/2);

      infile.seekp(mid * sizeof(Record));
      infile.read((char *) &record, sizeof(Record));
    //   cout <<"mid: "<< mid << " -- "<< right << " -- "<< key << " -- " << left <<endl;
    //   cout <<"KEY: "<< record.key << " YEAR: "<< record.year << " TITLE: "<< record.title <<endl;
      if (key == record.key){
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