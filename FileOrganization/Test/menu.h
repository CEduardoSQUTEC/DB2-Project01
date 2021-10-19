#ifndef MENU_H
#define MENU_H

#include "../SequentialFile/SeqFile.h"

template <typename Record>
void hashTest(string filename);

template <typename Record>
void sequentialTest(string filename);

void clear();
string chooseFile();
string chooseAction();
void testTimeNetflix(string filename, int auxFactor, long start, long end);
void testTimeWorld(string filename, int auxFactor, long start, long end);

#endif