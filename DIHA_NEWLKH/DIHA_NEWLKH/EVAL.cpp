// 2014.04.10
// 문수영
// EVAL.cpp
// 제안/기존 기법 성능 평가 클래스 정의

#include "EVAL.h"

#include <iostream>

using std::cout;

void EVAL::setNodes(map<int, Node*> nodes) // 노드 목록 설정
{
	this->nodes = nodes;
}

void EVAL::setEvctdNodes(map<int, Node*> evctdNodes) // 제거된 노드 목록 설정
{
	this->evctdNodes = evctdNodes;
}

void EVAL::setAtk(Atk* atk) // 공격 기법 객체 설정
{
	this->atk = atk;
}

void EVAL::setSec(Sec* sec) // 보안 기법 객체 설정
{
	this->sec = sec;
}

void EVAL::evalSec() // 보안성 평가
{
	// 공격 발생 횟수 획득
	int numAtk = atk->getNumAtk();

	// 공격 탐지 횟수 획득
	int numDtctn = sec->getNumDtctn();

	// MAC 검증 횟수 획득
	int numVeri = sec->getNumVer();

	cout << "num. false reports: " << numAtk << endl;
	cout << "num. detection: " << numDtctn << endl;
	cout << "total num. of MAC verification" << numVeri << endl;

}

void EVAL::evalEnergy()// 에너지 효율성 평가
{
	int i = 0;

	int numExstngNodes = 0;
	int numEvctdNodes = 0;
	int totNumNodes = 0;
	
	numExstngNodes = nodes.size();
	numEvctdNodes = evctdNodes.size();

	totNumNodes = numExstngNodes + numEvctdNodes;

	for (i = 0; i < numExstngNodes; i++) // 현재 네트워크에 존재하는 노드들의 에너지 소모량 누적
	{
		this->avgEnrgCnpmtn += nodes[i]->eCnsmptn;
	}
	
	for (i = 0; i < numEvctdNodes; i++)	// 네트워크에서 제거된 노드들의 에너지 소모량 누적
	{
		this->avgEnrgCnpmtn += evctdNodes[i]->eCnsmptn;
	}

	avgEnrgCnpmtn /= totNumNodes; // 평균 에너지 소모량 계산

	cout << "Avg. Energy Consumption: " << avgEnrgCnpmtn << endl;

}


void EVAL::evalLifetime() // 네트워크 수명 평가
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