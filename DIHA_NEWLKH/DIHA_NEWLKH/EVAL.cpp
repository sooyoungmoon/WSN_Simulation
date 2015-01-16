// 2014.04.10
// ������
// EVAL.cpp
// ����/���� ��� ���� �� Ŭ���� ����

#include "EVAL.h"

#include <iostream>

using std::cout;

void EVAL::setNodes(map<int, Node*> nodes) // ��� ��� ����
{
	this->nodes = nodes;
}

void EVAL::setEvctdNodes(map<int, Node*> evctdNodes) // ���ŵ� ��� ��� ����
{
	this->evctdNodes = evctdNodes;
}

void EVAL::setAtk(Atk* atk) // ���� ��� ��ü ����
{
	this->atk = atk;
}

void EVAL::setSec(Sec* sec) // ���� ��� ��ü ����
{
	this->sec = sec;
}

void EVAL::evalSec() // ���ȼ� ��
{
	// ���� �߻� Ƚ�� ȹ��
	int numAtk = atk->getNumAtk();

	// ���� Ž�� Ƚ�� ȹ��
	int numDtctn = sec->getNumDtctn();

	// MAC ���� Ƚ�� ȹ��
	int numVeri = sec->getNumVer();

	cout << "num. false reports: " << numAtk << endl;
	cout << "num. detection: " << numDtctn << endl;
	cout << "total num. of MAC verification" << numVeri << endl;

}

void EVAL::evalEnergy()// ������ ȿ���� ��
{
	int i = 0;

	int numExstngNodes = 0;
	int numEvctdNodes = 0;
	int totNumNodes = 0;
	
	numExstngNodes = nodes.size();
	numEvctdNodes = evctdNodes.size();

	totNumNodes = numExstngNodes + numEvctdNodes;

	for (i = 0; i < numExstngNodes; i++) // ���� ��Ʈ��ũ�� �����ϴ� ������ ������ �Ҹ� ����
	{
		this->avgEnrgCnpmtn += nodes[i]->eCnsmptn;
	}
	
	for (i = 0; i < numEvctdNodes; i++)	// ��Ʈ��ũ���� ���ŵ� ������ ������ �Ҹ� ����
	{
		this->avgEnrgCnpmtn += evctdNodes[i]->eCnsmptn;
	}

	avgEnrgCnpmtn /= totNumNodes; // ��� ������ �Ҹ� ���

	cout << "Avg. Energy Consumption: " << avgEnrgCnpmtn << endl;

}


void EVAL::evalLifetime() // ��Ʈ��ũ ���� ��
{

}

EVAL::EVAL()
{
	atk = NULL;
	sec = NULL;

	avgEnrgCnpmtn = 0;
}

EVAL::~EVAL()
{

}