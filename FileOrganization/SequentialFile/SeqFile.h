
#ifndef SEQFILE_H
#define SEQFILE_H

#include "../libs.h"
#include "../Record/Netflix.h"
#include "../Record/WorldPopulation.h"
#include "../Record/Basket.h"

// SeqFile : template class in order to be able to work with different types of key
template<typename T, typename Record>
class SeqFile{
private:
    string filename;
    string datfile;
    size_t validRecord;
    size_t auxRecord;
    size_t auxFactor;
public:
    SeqFile(string filename, size_t auxFactor);  //Constructor
    void createBinary();  // Creates a binary file from the data
    size_t countRecords(); // Total of records in binary file
    void add(Record record); // Add aux record to binary file
    void remove(T key); // Remove record (logically) from file
    Record search(T key); // Search by key
    vector<Record> rangeSearch(T beginkey, T endkey); // Search for the records between beginkey and endkey 
    void reconstruct(); // 

    template<typename T1, typename Rec1>
    friend bool binarySearch(T1 key, SeqFile<T1, Rec1>* seq, long &pos);
};

template<typename T, typename Record>
bool binarySearch(T key, SeqFile<T, Record>* seq, long &pos);

#endif //SEQFILE_H