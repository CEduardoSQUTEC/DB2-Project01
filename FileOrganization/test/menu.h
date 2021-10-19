#include "../SequentialFile/SeqFile.h"
#include "../SequentialFile/SeqFile.cpp"

template <typename Record>
void hashTest(string filename);

template <typename Record>
void sequentialTest(string filename);

void clear();
string choosefile();
string choosetechnique();
void testTimeNetflix(string filename, int auxFactor, long start, long end);
void testTimeWorld(string filename, int auxFactor, long start, long end);
