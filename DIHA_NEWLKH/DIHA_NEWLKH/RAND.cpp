// 2014.03.24
// 문수영
// RAND.h
// RAND 클래스 선언

#include "RAND.h"
#include <fstream>

void RAND::setSeed(unsigned int seed)
{
	this->seed = seed;
	this->rInt = seed;
}

unsigned int RAND::genRandInt(int max) // 0~max-1 사이의 정수형 난수 반환
{
	unsigned int result = 0;

	//result = rand()% max;

	result = getNextInt() % max;

	return result;
}

unsigned int RAND::getNextInt() // linear congruential method에 의한 난수 (int형) 생성
{
	unsigned int oldVal = rInt;

	unsigned int newVal = LCM_a * oldVal + LCM_c;  // 새로운 난수 생성 - 명시적인 modular 연산 불필요 - 정수 자료형의 크기 제한 (32bit)로 인해 자동으로 modulo 연산 수행됨

	rInt = newVal;

	return rInt;
}


void RAND::testRand() // (2014.08.12) 난수열 테스트
{
	std::ofstream out("tst_rand.txt");

	int i = 0;
	int r = 0;
	int cnt[100]= {0}; // 각 숫자별 발생 횟수

	for (i=0; i < 1000; i++)
	{
		r = this->genRandInt(100);
		cnt[r]++; 

		out << r << ", ";
	}

	for (i=0; i < 100; i++)
	{
		out << "[" << i << "]: " << cnt[i] << ", ";
	}

	out.close();
}

RAND::RAND()
{
	seed =0;
	rInt = 0;
}

RAND::~RAND()
{

}