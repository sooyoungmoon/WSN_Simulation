
#include "Atk_FRA.h"
#include <fstream>

using namespace std;

void Atk_FRA::forgeEvntMsg(EvntMsg* msg) // 수정 불필요?
{
	int hop = msg->hop;

	if (hop == 0) // hop count가 0이면 가짜 MAC을 설정
	{
		list<MAC*>::iterator it = msg->macs.begin();

		for (it; it != msg->macs.end(); it++)
		{
			MAC* mac = *it;

			// 현재 MAC의 생성 노드가 훼손 노드가 아니면 잘못된 CODE 값 저장 (-1)
			if (mac->origin->cmprmsd == false)
				mac->code = -1;
		}
		
		this->numAtk++;  // 공격 발생 횟수 +1 증가

		msg->falseReport = true; // 위조 보고서 여부 저장
	}


	/*
	int hop = rep->hop;
	
	Node* CoS = rep ->CoS;

	int cmprmsd = CoS->cmprmsd;	

	if (hop == 0) // hop count가 0이면 가짜 MAC을 설정
	{
		int i;
		
		for(i = 0; i < macsPerRep; i++) 
		{
			if( i < compKeys); // 훼손된 키에 대해서는 공격자가 정확한 MAC을 만들어 냄
			else
				sec->macs[i][1] = 0; // 공격자가 모르는 키에 대해서는 틀린 MAC이 포함됨
		}		

		this->numAtk++;  // 공격 발생 횟수 +1 증가
	}
	*/

}

void Atk_FRA::initCompNodes (Node nodes[]) 
{
	// 훼손 노드 비율에 따라 훼손 노드 지정

	for (int i =0; i < NUM_NODES; i++)
	{
		int r = gen1.genRandInt(100);
		//int r = gen1.uniformI(0, 100);

		if ( r < CNR)
			nodes[i].cmprmsd = true;
	}
}

void Atk_FRA::testCompNodes (Node nodes[])
{
	ofstream out("tst_Atk.txt");

	int numCmprmsd = 0;

	for (int i =0; i < NUM_NODES; i++)
	{

		if ( nodes[i].cmprmsd == true)
		{
			out << "Compromised node " << ++numCmprmsd << ":";
			out << "node " << i << endl;
		}
	}

	out.close();
}

Atk_FRA::Atk_FRA() : Atk()
{
}

Atk_FRA::~Atk_FRA()
{
}