
#include "Atk_FRA.h"
#include <fstream>

using namespace std;

void Atk_FRA::forgeEvntMsg(EvntMsg* msg) // ���� ���ʿ�?
{
	int hop = msg->hop;

	if (hop == 0) // hop count�� 0�̸� ��¥ MAC�� ����
	{
		list<MAC*>::iterator it = msg->macs.begin();

		for (it; it != msg->macs.end(); it++)
		{
			MAC* mac = *it;

			// ���� MAC�� ���� ��尡 �Ѽ� ��尡 �ƴϸ� �߸��� CODE �� ���� (-1)
			if (mac->origin->cmprmsd == false)
				mac->code = -1;
		}
		
		this->numAtk++;  // ���� �߻� Ƚ�� +1 ����

		msg->falseReport = true; // ���� ���� ���� ����
	}


	/*
	int hop = rep->hop;
	
	Node* CoS = rep ->CoS;

	int cmprmsd = CoS->cmprmsd;	

	if (hop == 0) // hop count�� 0�̸� ��¥ MAC�� ����
	{
		int i;
		
		for(i = 0; i < macsPerRep; i++) 
		{
			if( i < compKeys); // �Ѽյ� Ű�� ���ؼ��� �����ڰ� ��Ȯ�� MAC�� ����� ��
			else
				sec->macs[i][1] = 0; // �����ڰ� �𸣴� Ű�� ���ؼ��� Ʋ�� MAC�� ���Ե�
		}		

		this->numAtk++;  // ���� �߻� Ƚ�� +1 ����
	}
	*/

}

void Atk_FRA::initCompNodes (Node nodes[]) 
{
	// �Ѽ� ��� ������ ���� �Ѽ� ��� ����

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