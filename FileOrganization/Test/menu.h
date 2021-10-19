#ifndef TEST_MENU_H
#define TEST_MENU_H

#include "../SequentialFile/SeqFile.h"

template <typename R>
void hashTest(string filename);

template <typename R>
void sequentialTest(string filename);

void clear();
string chooseFile();
string chooseAction();
void testTimeNetflix(string filename, int auxFactor, long start, long end);
void testTimeWorld(string filename, int auxFactor, long start, long end);

#endif

