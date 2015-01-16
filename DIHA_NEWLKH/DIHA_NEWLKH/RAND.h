// 2014.08.12
// 문수영
// RAND.h
// RAND 클래스 선언

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

	void testRand(); // 난수열 테스트

	RAND();
	~RAND();

private: 
	unsigned int seed;
	unsigned int rInt; // 정수형 난수

};

#endif