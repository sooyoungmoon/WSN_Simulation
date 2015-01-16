
// 2014.04.23
// 문수영
// SrchNode.cpp
//  KEY TREE STRUCTURE 구성 문제에서 A* 탐색을 위한 탐색 노드 정의

#include "SrchNode_KTS.h"
#include "params.h"
#include <math.h>
#include <assert.h>
#include <iostream>

using std::cout;

 int SrchNode_KTS::numNodes;// 현재 센서 노드 수
 int SrchNode_KTS::numClusters; // 요구되는 클러스터 수

void SrchNode_KTS::setNumNodes(int numNodes)
{
	SrchNode_KTS::numNodes = numNodes;

	setNumClusters (ceil ( (double)numNodes / MAX_NNC ) ); // (2014.10.09) 최소 클러스터 개수 설정
	//setNumClusters (ceil ( (double)numNodes / (2*(INTERLEAVING_T+1)) ) ); // 최소 클러스터 개수 설정
	//setNumClusters( numNodes / (2*(INTERLEAVING_T+1)) ); // 최소 클러스터 개수 설정
}

void SrchNode_KTS::setNumClusters(int numClusters)
{
	SrchNode_KTS::numClusters = numClusters;
}

void SrchNode_KTS::setH() // 현재 상태에서의 휴리스틱 함수 값 반환
{
	int min_amsg = 0;
	int min_emsg = 0;

	min_amsg = 1; // 노드 추가 시 최소 키 갱신 메시지 하한
	
	//min_emsg = log2( (double) numClusters / numLeafs) - 1; // 노드 제거 시 최소 키 갱신 메시지 하한

	if ( numLeafs > numNodes)
	{
		min_emsg = -1;
	}
	else
	{	
		min_emsg = log2( (double) numNodes / numLeafs) - 1; // 노드 제거 시 최소 키 갱신 메시지 하한
	}

	this->h = Addition_Ratio * min_amsg + Eviction_Ratio * min_emsg; 
}

int SrchNode_KTS::log2(int n)
{
	assert( n > 0); // n은 양수이어야 함

	int res = 0;	

	while ( n >1)
	{
		n >>= 1;
		res++;
	}

	return res;
}

double SrchNode_KTS::getLinkCost (SrchNode* parent) // 부모 노드와의 연결 비용 계산
{
	double res = 0;

	// 부모 노드와의 연결 비용 (g 함수값의 차이) 계산

	res = Addition_Ratio + Eviction_Ratio * degreeSeq[currentLevel-1] -1; //  AR*hc + ER * (d_c - 1)
	
	return res;
}

// 현재 상태 정보 반환 (key tree 상에서 주어진 level의 degree)
int SrchNode_KTS::getDegree( int level) // (2014.04.18) 
{
	return this->degreeSeq[level]; // 
}

map <int, int> SrchNode_KTS::getDegreeSeq() // 
{
	return this->degreeSeq;
}

void SrchNode_KTS::calNxtStates() // 가능한 다음 상태 목록 계산
{

	// 가능한 다음 상태의 개수 계산

	if (this->numLeafs < this->numClusters) // // 현재 KTS 상의 leaf node 수가 최소 클러스터 개수보다 작은 경우
	{
		if ( numLeafs >= ceil( (double)numClusters/Max_Degree) ) // 다음 level에서 최소 클러스터 개수를 만족시킬 수 있는 경우
		{
			this->numNxtSts = 1;
			this->nxtStates = new SrchNode*[numNxtSts]; // 다음 상태 포인터 배열 생성
			SrchNode_KTS* tNode = new SrchNode_KTS(); // 탐색 노드 생성
			this->nxtStates[0] = tNode; // 다음 상태 포인터 배열에 저장

			// 탐색 노드의 상태 변수 값 설정
			
			tNode->currentLevel = this->currentLevel;
			

			for (int i =0; i < currentLevel; i++)
			{
				tNode->degreeSeq[i] = this->degreeSeq[i]; // 부모 노드의 degree 순열 복사
			}

			tNode->degreeSeq[tNode->currentLevel] = ceil ((double) numClusters/ numLeafs); // // 최소 클러스터 개수를 만족할 수 있도록 degree 설정
			tNode->numLeafs = this->numLeafs * tNode->degreeSeq[tNode->currentLevel];
			tNode->currentLevel++;

		}
		else // 그 밖의 경우
		{
			this->numNxtSts = Max_Degree-1; // 2 ~ Max_Degree 사이의 degree 도출
			this->nxtStates = new SrchNode*[numNxtSts]; // 다음 상태 포인터 배열 생성

			for (int i = 0; i < numNxtSts; i++)
			{
				SrchNode_KTS* tNode = new SrchNode_KTS(); // 탐색 노드 생성
				nxtStates[i] = tNode; // 다음 상태 포인터 배열에 저장

				// 탐색 노드의 상태 변수 값 설정
				tNode->currentLevel = this->currentLevel;

				for (int level_i =0; level_i < currentLevel; level_i++)
				{
					tNode->degreeSeq[level_i] = this->degreeSeq[level_i]; // 부모 노드의 degree 순열 복사
				}

				/*for (int i =0; i < currentLevel; i++)
				{
					tNode->degreeSeq[i] = this->degreeSeq[i]; // 부모 노드의 degree 순열 복사
				}*/

				tNode->degreeSeq[tNode->currentLevel] = i+2; // if i=0, degree = 2 , if i=3, degree = 5
				tNode->numLeafs = this->numLeafs * tNode->degreeSeq[tNode->currentLevel];
				tNode->currentLevel++;
			}
		}
	}
	else // // 현재 KTS 상의 leaf node 수가 최소 클러스터 개수를 만족하는 경우
	{
		this->numNxtSts = 1; 
		this->nxtStates = new SrchNode*[numNxtSts]; // 다음 상태 포인터 배열 생성
		SrchNode_KTS* tNode = new SrchNode_KTS(); // 탐색 노드 생성
		this->nxtStates[0] = tNode; // 다음 상태 포인터 배열에 저장

		// 탐색 노드의 상태 변수 값 설정
		
		tNode->currentLevel = this->currentLevel;

		for (int i =0; i < currentLevel; i++)
		{
			tNode->degreeSeq[i] = this->degreeSeq[i]; // 부모 노드의 degree 순열 복사
		}

		tNode->degreeSeq[tNode->currentLevel] = ceil ((double) this->numNodes / numLeafs); // // leaf node 수가 센서 노드 수 이상이 되도록 degree 설정
		tNode->numLeafs = this->numLeafs * tNode->degreeSeq[tNode->currentLevel];
		tNode->currentLevel++;
	}


	// 가능한 다음 상태의 개수 계산
	/*
	if (this->numLeafs < this->numClusters) // // 현재 KTS 상의 leaf node 수가 최소 클러스터 개수보다 작은 경우
	{
		this->numNxtSts = 2; // degree = 2 or 3

		this->nxtStates = new SrchNode*[numNxtSts]; // 다음 상태 포인터 배열 생성

		for (int i = 0; i < numNxtSts; i++)
		{

			SrchNode_KTS* tNode = new SrchNode_KTS(); // 탐색 노드 생성
			nxtStates[i] = tNode; // 다음 상태 포인터 배열에 저장

			// 탐색 노드의 상태 변수 값 설정
			tNode->currentLevel = this->currentLevel +1;
			tNode->degreeSeq[tNode->currentLevel] = i+1; // if i=0, degree = 1 , if i=1, degree 2
			tNode->numLeafs *= tNode->degreeSeq[tNode->currentLevel];
		}
	}
	else
	{
		this->numNxtSts = 1; // 가능한 다음 상태 개수 = 1

		this->nxtStates = new SrchNode*[numNxtSts]; // 다음 상태 포인터 배열 생성
		SrchNode_KTS* tNode = new SrchNode_KTS(); // 탐색 노드 생성
		this->nxtStates[0] = tNode; // 다음 상태 포인터 배열에 저장

		// 탐색 노드의 상태 변수 값 설정
		tNode->currentLevel = this->currentLevel +1;
		tNode->degreeSeq[tNode->currentLevel] = (2*this->numNodes) / tNode->numLeafs; // leaf node의 수가 센서 노드 수의 2배가 되도록 설정
		tNode->numLeafs *= tNode->degreeSeq[tNode->currentLevel];		
	}	

	// 다음 상태 포인터 배열 생성

	// 탐색 노드 생성

	*/


	/*
	// 현재 level이 ( ('key tree height' - 2') 보다 작은 경우, 가능한 degree 값은 1~3 사이라고 가정)

	if ( this->numLeafs < this->numClusters)
		; // ... 다음 상태 개수를 어떻게 계산할 것인지, 다음 상태를 어떻게 계산할 것인지 해결 방안 도출 중

	// 현재 level이 ( ('key tree height' - 2') 이면 degree 값은 2*(t+1)
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
	cout << "\n current level: " << this->currentLevel << endl; // 현재 level 출력	
	cout << " degree seq: < ";

	int sizeSeq = degreeSeq.size();

	if (sizeSeq > 0)
	{

		for (int i = 0; i < sizeSeq-1; i++) // degree sequnce 출력
		{
			cout << degreeSeq[i] << ", ";
		}
	
		cout << degreeSeq[sizeSeq-1];
	}

	cout << " > " << endl;
	cout << " num. leaf nodes: " << this->numLeafs << endl; // leaf node 수 출력
	cout << " f(): " << this->f; // 휴리스틱 함수 값 출력 ( f + g)
}

bool SrchNode_KTS::isSameState( SrchNode* n)
{
	bool res = false;

	bool cond1 = false;
	bool cond2 = true;
	bool cond3 = false;

	SrchNode_KTS* temp_n = dynamic_cast <SrchNode_KTS*> (n);


	if (this->currentLevel == temp_n->currentLevel) // 현재 level 비교
	{
		cond1 = true;
	}

	// degree sequence 비교

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
	// leaf node 수 비교

	if ( cond1 == true && cond2 == true) // 세 가지 조건이 모두 참이면 true 반환
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