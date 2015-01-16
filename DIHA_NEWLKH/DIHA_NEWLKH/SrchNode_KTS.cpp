
// 2014.04.23
// ������
// SrchNode.cpp
//  KEY TREE STRUCTURE ���� �������� A* Ž���� ���� Ž�� ��� ����

#include "SrchNode_KTS.h"
#include "params.h"
#include <math.h>
#include <assert.h>
#include <iostream>

using std::cout;

 int SrchNode_KTS::numNodes;// ���� ���� ��� ��
 int SrchNode_KTS::numClusters; // �䱸�Ǵ� Ŭ������ ��

void SrchNode_KTS::setNumNodes(int numNodes)
{
	SrchNode_KTS::numNodes = numNodes;

	setNumClusters (ceil ( (double)numNodes / MAX_NNC ) ); // (2014.10.09) �ּ� Ŭ������ ���� ����
	//setNumClusters (ceil ( (double)numNodes / (2*(INTERLEAVING_T+1)) ) ); // �ּ� Ŭ������ ���� ����
	//setNumClusters( numNodes / (2*(INTERLEAVING_T+1)) ); // �ּ� Ŭ������ ���� ����
}

void SrchNode_KTS::setNumClusters(int numClusters)
{
	SrchNode_KTS::numClusters = numClusters;
}

void SrchNode_KTS::setH() // ���� ���¿����� �޸���ƽ �Լ� �� ��ȯ
{
	int min_amsg = 0;
	int min_emsg = 0;

	min_amsg = 1; // ��� �߰� �� �ּ� Ű ���� �޽��� ����
	
	//min_emsg = log2( (double) numClusters / numLeafs) - 1; // ��� ���� �� �ּ� Ű ���� �޽��� ����

	if ( numLeafs > numNodes)
	{
		min_emsg = -1;
	}
	else
	{	
		min_emsg = log2( (double) numNodes / numLeafs) - 1; // ��� ���� �� �ּ� Ű ���� �޽��� ����
	}

	this->h = Addition_Ratio * min_amsg + Eviction_Ratio * min_emsg; 
}

int SrchNode_KTS::log2(int n)
{
	assert( n > 0); // n�� ����̾�� ��

	int res = 0;	

	while ( n >1)
	{
		n >>= 1;
		res++;
	}

	return res;
}

double SrchNode_KTS::getLinkCost (SrchNode* parent) // �θ� ������ ���� ��� ���
{
	double res = 0;

	// �θ� ������ ���� ��� (g �Լ����� ����) ���

	res = Addition_Ratio + Eviction_Ratio * degreeSeq[currentLevel-1] -1; //  AR*hc + ER * (d_c - 1)
	
	return res;
}

// ���� ���� ���� ��ȯ (key tree �󿡼� �־��� level�� degree)
int SrchNode_KTS::getDegree( int level) // (2014.04.18) 
{
	return this->degreeSeq[level]; // 
}

map <int, int> SrchNode_KTS::getDegreeSeq() // 
{
	return this->degreeSeq;
}

void SrchNode_KTS::calNxtStates() // ������ ���� ���� ��� ���
{

	// ������ ���� ������ ���� ���

	if (this->numLeafs < this->numClusters) // // ���� KTS ���� leaf node ���� �ּ� Ŭ������ �������� ���� ���
	{
		if ( numLeafs >= ceil( (double)numClusters/Max_Degree) ) // ���� level���� �ּ� Ŭ������ ������ ������ų �� �ִ� ���
		{
			this->numNxtSts = 1;
			this->nxtStates = new SrchNode*[numNxtSts]; // ���� ���� ������ �迭 ����
			SrchNode_KTS* tNode = new SrchNode_KTS(); // Ž�� ��� ����
			this->nxtStates[0] = tNode; // ���� ���� ������ �迭�� ����

			// Ž�� ����� ���� ���� �� ����
			
			tNode->currentLevel = this->currentLevel;
			

			for (int i =0; i < currentLevel; i++)
			{
				tNode->degreeSeq[i] = this->degreeSeq[i]; // �θ� ����� degree ���� ����
			}

			tNode->degreeSeq[tNode->currentLevel] = ceil ((double) numClusters/ numLeafs); // // �ּ� Ŭ������ ������ ������ �� �ֵ��� degree ����
			tNode->numLeafs = this->numLeafs * tNode->degreeSeq[tNode->currentLevel];
			tNode->currentLevel++;

		}
		else // �� ���� ���
		{
			this->numNxtSts = Max_Degree-1; // 2 ~ Max_Degree ������ degree ����
			this->nxtStates = new SrchNode*[numNxtSts]; // ���� ���� ������ �迭 ����

			for (int i = 0; i < numNxtSts; i++)
			{
				SrchNode_KTS* tNode = new SrchNode_KTS(); // Ž�� ��� ����
				nxtStates[i] = tNode; // ���� ���� ������ �迭�� ����

				// Ž�� ����� ���� ���� �� ����
				tNode->currentLevel = this->currentLevel;

				for (int level_i =0; level_i < currentLevel; level_i++)
				{
					tNode->degreeSeq[level_i] = this->degreeSeq[level_i]; // �θ� ����� degree ���� ����
				}

				/*for (int i =0; i < currentLevel; i++)
				{
					tNode->degreeSeq[i] = this->degreeSeq[i]; // �θ� ����� degree ���� ����
				}*/

				tNode->degreeSeq[tNode->currentLevel] = i+2; // if i=0, degree = 2 , if i=3, degree = 5
				tNode->numLeafs = this->numLeafs * tNode->degreeSeq[tNode->currentLevel];
				tNode->currentLevel++;
			}
		}
	}
	else // // ���� KTS ���� leaf node ���� �ּ� Ŭ������ ������ �����ϴ� ���
	{
		this->numNxtSts = 1; 
		this->nxtStates = new SrchNode*[numNxtSts]; // ���� ���� ������ �迭 ����
		SrchNode_KTS* tNode = new SrchNode_KTS(); // Ž�� ��� ����
		this->nxtStates[0] = tNode; // ���� ���� ������ �迭�� ����

		// Ž�� ����� ���� ���� �� ����
		
		tNode->currentLevel = this->currentLevel;

		for (int i =0; i < currentLevel; i++)
		{
			tNode->degreeSeq[i] = this->degreeSeq[i]; // �θ� ����� degree ���� ����
		}

		tNode->degreeSeq[tNode->currentLevel] = ceil ((double) this->numNodes / numLeafs); // // leaf node ���� ���� ��� �� �̻��� �ǵ��� degree ����
		tNode->numLeafs = this->numLeafs * tNode->degreeSeq[tNode->currentLevel];
		tNode->currentLevel++;
	}


	// ������ ���� ������ ���� ���
	/*
	if (this->numLeafs < this->numClusters) // // ���� KTS ���� leaf node ���� �ּ� Ŭ������ �������� ���� ���
	{
		this->numNxtSts = 2; // degree = 2 or 3

		this->nxtStates = new SrchNode*[numNxtSts]; // ���� ���� ������ �迭 ����

		for (int i = 0; i < numNxtSts; i++)
		{

			SrchNode_KTS* tNode = new SrchNode_KTS(); // Ž�� ��� ����
			nxtStates[i] = tNode; // ���� ���� ������ �迭�� ����

			// Ž�� ����� ���� ���� �� ����
			tNode->currentLevel = this->currentLevel +1;
			tNode->degreeSeq[tNode->currentLevel] = i+1; // if i=0, degree = 1 , if i=1, degree 2
			tNode->numLeafs *= tNode->degreeSeq[tNode->currentLevel];
		}
	}
	else
	{
		this->numNxtSts = 1; // ������ ���� ���� ���� = 1

		this->nxtStates = new SrchNode*[numNxtSts]; // ���� ���� ������ �迭 ����
		SrchNode_KTS* tNode = new SrchNode_KTS(); // Ž�� ��� ����
		this->nxtStates[0] = tNode; // ���� ���� ������ �迭�� ����

		// Ž�� ����� ���� ���� �� ����
		tNode->currentLevel = this->currentLevel +1;
		tNode->degreeSeq[tNode->currentLevel] = (2*this->numNodes) / tNode->numLeafs; // leaf node�� ���� ���� ��� ���� 2�谡 �ǵ��� ����
		tNode->numLeafs *= tNode->degreeSeq[tNode->currentLevel];		
	}	

	// ���� ���� ������ �迭 ����

	// Ž�� ��� ����

	*/


	/*
	// ���� level�� ( ('key tree height' - 2') ���� ���� ���, ������ degree ���� 1~3 ���̶�� ����)

	if ( this->numLeafs < this->numClusters)
		; // ... ���� ���� ������ ��� ����� ������, ���� ���¸� ��� ����� ������ �ذ� ��� ���� ��

	// ���� level�� ( ('key tree height' - 2') �̸� degree ���� 2*(t+1)
	*/

}

bool SrchNode_KTS::isGoalNode()
{
	bool res = false;

	if ( this->numLeafs >= this->numNodes)
	{
		res = true;
	}
	return res;
}

void SrchNode_KTS::prntStat()
{
	cout << "\n current level: " << this->currentLevel << endl; // ���� level ���	
	cout << " degree seq: < ";

	int sizeSeq = degreeSeq.size();

	if (sizeSeq > 0)
	{

		for (int i = 0; i < sizeSeq-1; i++) // degree sequnce ���
		{
			cout << degreeSeq[i] << ", ";
		}
	
		cout << degreeSeq[sizeSeq-1];
	}

	cout << " > " << endl;
	cout << " num. leaf nodes: " << this->numLeafs << endl; // leaf node �� ���
	cout << " f(): " << this->f; // �޸���ƽ �Լ� �� ��� ( f + g)
}

bool SrchNode_KTS::isSameState( SrchNode* n)
{
	bool res = false;

	bool cond1 = false;
	bool cond2 = true;
	bool cond3 = false;

	SrchNode_KTS* temp_n = dynamic_cast <SrchNode_KTS*> (n);


	if (this->currentLevel == temp_n->currentLevel) // ���� level ��
	{
		cond1 = true;
	}

	// degree sequence ��

	if (this->degreeSeq.size() != temp_n->degreeSeq.size() )
	{
		cond2 = false;
	}

	for (int i = 0; i < degreeSeq.size(); i++)
	{
		if (degreeSeq[i] != temp_n->degreeSeq[i])
		{
			cond2 = false;
			break;
		}
	}

	if (this->numLeafs == temp_n->numLeafs)
	{
		cond3 = true; 
	}
	// leaf node �� ��

	if ( cond1 == true && cond2 == true) // �� ���� ������ ��� ���̸� true ��ȯ
	{
		if (cond3 == true)
		{
			res = true;
		}
	}

	return res;
}

SrchNode_KTS::SrchNode_KTS()
{
	//numNodes = 0;

	currentLevel = 0;

	numLeafs = 1;
}

SrchNode_KTS::~SrchNode_KTS()
{

}