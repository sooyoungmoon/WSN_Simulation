// 2014.03.24
// ������
// RAND.h
// RAND Ŭ���� ����

#include "RAND.h"
#include <fstream>

void RAND::setSeed(unsigned int seed)
{
	this->seed = seed;
	this->rInt = seed;
}

unsigned int RAND::genRandInt(int max) // 0~max-1 ������ ������ ���� ��ȯ
{
	unsigned int result = 0;

	//result = rand()% max;

	result = getNextInt() % max;

	return result;
}

unsigned int RAND::getNextInt() // linear congruential method�� ���� ���� (int��) ����
{
	unsigned int oldVal = rInt;

	unsigned int newVal = LCM_a * oldVal + LCM_c;  // ���ο� ���� ���� - ������� modular ���� ���ʿ� - ���� �ڷ����� ũ�� ���� (32bit)�� ���� �ڵ����� modulo ���� �����

	rInt = newVal;

	return rInt;
}


void RAND::testRand() // (2014.08.12) ������ �׽�Ʈ
{
	std::ofstream out("tst_rand.txt");

	int i = 0;
	int r = 0;
	int cnt[100]= {0}; // �� ���ں� �߻� Ƚ��

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