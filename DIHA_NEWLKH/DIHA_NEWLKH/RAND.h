// 2014.08.12
// ������
// RAND.h
// RAND Ŭ���� ����

#ifndef RAND_H
#define RAND_H

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include "params.h"		

class RAND
{
public: 

	void setSeed(unsigned int seed);

	unsigned int genRandInt(int max);

	unsigned int getNextInt();

	void testRand(); // ������ �׽�Ʈ

	RAND();
	~RAND();

private: 
	unsigned int seed;
	unsigned int rInt; // ������ ����

};

#endif