// 2014.07.31
// 문수영
// 키 관리 클래스

#include "Kmr.h"
#include "Sec_IHA.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

void Kmr::setNodes(map<int, Node*> nodes) // (2013.08.28 추가)
{
	this->nodes = nodes; // 배열의 시작 주소 저장
}

void Kmr::setRtn(Rtn* rtn) // (2013.08.28 추가) rtn 객체 주소 저장
{
	this->rtn = rtn;
}

void Kmr::setSec(Sec* sec) // (2013.08.28 추가) sec 객체 주소 저장
{
	this->sec = sec;
}

void Kmr::resetUpdatedKeys() // (2014.07.31) key tree의 각 k-node 갱신 여부 재 초기화
{
	this->resetUpdatedKeys(root);
}

void Kmr::resetUpdatedKeys(struct _tagKNode* kNode) // (2014.07.31) key tree의 각 k-node 갱신 여부 재 초기화 
{
	// 현재 kNode의 updated, updated_addition, updated_eviction 플래그를 거짓 (false)로 리셋
	kNode->updated = false;
	kNode->updated_addition = false;
	kNode->updated_eviction = false;
	kNode->created = false;

	// 재귀 호출 종료 조건
	if (kNode->unode != NULL)
	{
		return;
	}


	// 재귀적 호출 - left child

	if (kNode->leftChild != NULL)
	{
		resetUpdatedKeys(kNode->leftChild); // 해당 kNode의 leftChild에 대해 재귀 호출
	}

	// 재귀적 호출 - right sibling

	if ( kNode != root && kNode->rightSibling != NULL)
	{
		resetUpdatedKeys(kNode->rightSibling); // 해당 kNode의 rightChild에 대해 재귀 호출
	}
}

void Kmr::printReceivedNodes() // (2014.08.07) 키 재분배 메시지 수신 노드 집합 출력
{
	ofstream out("tst_km_received_nodes.txt");

	set<int>::iterator it;

	out << " { ";

	for (it = this->receivedNodes.begin(); it != receivedNodes.end(); it++) // 키 재분배 메시지 수신 노드 목록 출력
	{
		int nodeID = *it;

		out << nodeID << ", ";

	} // for 문 종료 - 키 재분배 메시지 수신 노드 목록 출력

	out << " } ";

	return;
}

void Kmr::resetReceivedNodes() // (2014.08.07) 키 재분배 메시지 수신 노드 집합 리셋
{
	this->receivedNodes.clear();
}

bool Kmr::isSubset (IDSet a, IDSet b) // (2014.07.10 추가) 부분집합 유무 반환
{
	bool ret = true;
	int nodeID;

	IDSet::iterator it;

	for (it = a.begin(); it != a.end(); it++) // 집합 a의 요소들 순회
	{
		nodeID = *it;

		if ( b.find(nodeID) == b.end() ) // 만약 A의 원소들 중 하나라도 B에 포함되지 않는 원소가 존재하면 subset이 아님
		{
			ret = false;
		}
	} // for문 종료 - 집합 A의 모든 요소들에 대해 집합 B에 포함여부 확인

	return ret;
}

void Kmr::initKeyMsg(struct _tagKeyMsg* kMsg) // 키 재분배 메시지 초기화
{
	kMsg->CHID = -1;
}

map<int, struct _tagKeyMsg*> Kmr:: getKeyMsgs( )
{
	return keyMsgs;
}

void Kmr::delKeyMsg() // 키 재분배 메시지 제거
{
	for (unsigned int m = 0; m < keyMsgs.size(); m++) // 키 메시지에 할당된 메모리 공간 반납
	{
		KeyMsg* keyMsg = keyMsgs[m];
		free (keyMsg);
	}

	keyMsgs.clear(); // 키 메시지 목록 삭제
}

void Kmr::delRecvdKMsg(Node* cNode) // 수신된 키 재분배 메시지 제거
{
/*
	cNode->receivedKMsg.newKeys.clear();

	cNode->receivedKMsg.recipents.clear();
	cNode->receivedKMsg.CHID = -1;
	cNode->receivedKMsg.upAssocNds.clear();
	cNode->receivedKMsg.downAssocNds.clear();
	*/

	//free (cNode->receivedKMsg);
}

void Kmr::storeKMsg(struct _tagKeyMsg* keyMsg, Node* cNode) // (2014.05.15) 수신된 키 재분배 메시지 저장
{
	
/*
	cNode->receivedKMsg.newKeys = keyMsg->newKeys;
	cNode->receivedKMsg.recipents = keyMsg->recipents;
	cNode->receivedKMsg.CHID = keyMsg->CHID;
	cNode->receivedKMsg.upAssocNds = keyMsg->upAssocNds;
	cNode->receivedKMsg.downAssocNds = keyMsg->downAssocNds;
	*/
	
	//cNode->receivedKMsg = new KeyMsg;

	cNode->receivedKMsg->newKeys = keyMsg->newKeys;
	cNode->receivedKMsg->recipents = keyMsg->recipents;
	cNode->receivedKMsg->CHID = keyMsg->CHID;
	cNode->receivedKMsg->upAssocNds = keyMsg->upAssocNds;
	cNode->receivedKMsg->downAssocNds = keyMsg->downAssocNds;
	
}

void Kmr::restoreKMsg(struct _tagKeyMsg* km,  Node* cNode)
{
	// 기존에 저장된 키 재분배 메시지 복원
	
	//KeyMsg* storedMsg = &(cNode->receivedKMsg);

	KeyMsg* storedMsg = cNode->receivedKMsg;

	if (storedMsg == NULL)
	{
		cout << "storedMsg == NULL" << endl;
		exit(-1);
	}

	km->newKeys = storedMsg->newKeys;
	km->recipents = storedMsg->recipents;
	km->CHID = storedMsg->CHID;
	km->upAssocNds = storedMsg->upAssocNds;
	km->downAssocNds = storedMsg->downAssocNds;
	// 수신된 키 재분배 메시지 복원 완료
}

set<int> Kmr::getUserSet( struct _tagKNode* kNode) // (2014.07.08) 주어진 kNode에 저장된 그룹 키를 소유한 노드 목록 반환
{
	set<int> nodeSet;

	// ...

	return nodeSet;
}

int Kmr::renumberKNodes() // (2014.06.12) 키 트리에 속한 모든 kNode들의 level과 index 재설정
{
	int height;
	bool nextLevelExist;
	nextLevelExist = false;
	//list<struct _tagKNode*> cur_level_knodes;	

	cur_level_knodes.clear();

	int cur_level = 0;	
	cur_level_knodes.push_back(root); // 현재 level 노드 목록 구성 (level 0: root)

	
	while ( !cur_level_knodes.empty() )
	{		
		nextLevelExist = renumberKNodes(cur_level); // 현재 level의 kNOde들의 level & index 재설정

		if (nextLevelExist == true)
		{
			cur_level++;
		}
	}

	height = cur_level+1;

	return height;
}

bool Kmr::renumberKNodes(int level) // (2014.06.12) 특정 level에 속한 kNode들의 level과 index 재설정
{
	bool nextLevelExist; // 다음 level 노드가 존재하는 지 여부

	nextLevelExist = false;

	list<struct _tagKNode*> next_level_knodes;	

	list<struct _tagKNode*>::iterator it;

	int index = 0;

	for (it = cur_level_knodes.begin(); it != cur_level_knodes.end(); it++) // 현재 level의 노드들로부터 그 다음 level의 노드들을 구하고 level 및 index 설정
	{
		struct _tagKNode* parent;
		parent = *it;

		struct _tagKNode* kNode;

		kNode = parent->leftChild;

		while (kNode != NULL) // 현재 노드의 자식 노드들에 대해 반복
		{
			nextLevelExist = true; // 다음 level 노드 존재

			kNode->level = level+1;
			kNode->index = index++;
			next_level_knodes.push_back(kNode);
			kNode = kNode->rightSibling;
		}		
	}

	cur_level_knodes = next_level_knodes; // 현재 level의 노드 목록 갱신

	return nextLevelExist;
}

set<int> Kmr::getNdsInGrp(struct _tagKNode* root) // (2014.07.08) 주어진 그룹에 포함된 센서 노드 ID 집합 반환 - root: 그룹의 최상위 kNode (키 그래프 상의 root와는 다를 수 있음)
{
	set<int> nodesInGrp;

	struct _tagKNode* tKNode = NULL;

	if ( root->unode != NULL) // 주어진 kNode가 individual key에 대응될 경우
	{
		struct _tagUNode* uNode = root->unode;
		
		if (uNode->sensorNode != NULL)
		{
			nodesInGrp.insert(uNode->sensorNode->id); // (2014.06.08) 그룹에 속한 센서 노드 ID 집합 저장				
		}

		return nodesInGrp; // leaf node와 연관된 센서 노드 ID 반환
	}


	if ( root->isCluster == true) // 주어진 그룹이 클러스터인 경우
	{
		tKNode = root->leftChild;

		while (tKNode != NULL) // 클러스터에 속한 각 노드에 대해 반복
		{
			struct _tagUNode* uNode = tKNode->unode;

			if (uNode->sensorNode != NULL)
			{
				nodesInGrp.insert(uNode->sensorNode->id); // (2014.06.08) 그룹에 속한 센서 노드 ID 집합 저장
				
			}
			tKNode = tKNode->rightSibling;
		} // while - 클러스터 내 노드 ID 집합 구성

		return nodesInGrp; // 클러스터 내 노드 수 반환
	}	

	tKNode = root->leftChild;

	// 각각의 subtree 들에 대해 재귀적으로 함수 호출, 하위 그룹 노드 ID 집합의 요소들을 그룹 노드 id 집합에 추가

	while (tKNode != NULL) 
	{
		set<int> nodesInSubGrp;
		nodesInSubGrp = getNdsInGrp(tKNode);

		set<int>::iterator it;

		for (it = nodesInSubGrp.begin(); it != nodesInSubGrp.end(); it++)
		{
			int nodeID = *it;
			nodesInGrp.insert(nodeID);
		}

		tKNode = tKNode->rightSibling;
	}

	return nodesInGrp;

}

int Kmr::getNumNdsInGrp(struct _tagKNode* root) // (2014.05.30) 주어진 그룹에 포함된 센서 노드 수 반환 - root: 그룹의 최상위 kNode (키 그래프 상의 root와는 다를 수 있음)
{
	int numNodes = 0;
	//nodeSetInGrp.clear();

	struct _tagKNode* tKNode = NULL;


	if (root->isCluster == true) // (2014.06.10) 주어진 그룹이 클러스터인 경우			
	{
		tKNode = root->leftChild;

		while (tKNode != NULL) // 클러스터에 속한 각 노드에 대해 반복
		{
			struct _tagUNode* uNode = tKNode->unode;

			if (uNode->sensorNode != NULL)
			{
				numNodes++;
				//nodeSetInGrp.insert ( uNode->sensorNode->id); // (2014.06.05) 그룹에 속한 센서 노드 ID 집합 저장
			}

			tKNode = tKNode->rightSibling;

		} // while - 클러스터 내 노드 수 계산
			
		return numNodes; // 클러스터 내 노드 수 반환
	}

	int degree = this->getNumNodesAtLevel(root->level+1) / this->getNumNodesAtLevel(root->level);

	int minChildIdx; 
	minChildIdx = root->index * degree;

	int maxChildIdx; 
	maxChildIdx = (root->index+1) * degree - 1;	
	
	for (int childIdx = minChildIdx; childIdx <= maxChildIdx; childIdx++)
	{
		tKNode = this->getKNode(root->level+1, childIdx);
		numNodes += getNumNdsInGrp(tKNode);
	}

	return numNodes;
}

int Kmr::getNumClstrsInGrp(struct _tagKNode* root) // (2014.05.30) 주어진 그룹에 포함된 클러스터 수 반환
{
	int numClstrs;

	numClstrs = 0;

	// 종료 조건

	if (root->isCluster == true)// kNode가 클러스터 키인 경우
	{
		numClstrs =1;
		return numClstrs;
	}
	else if (root->unode != NULL)  // kNOde가 individual 키인 경우
	{
		numClstrs = 0;
		return numClstrs;
	}

	struct _tagKNode* tKNode;
	tKNode = root->leftChild;

	while (tKNode != NULL)
	{
		int numClstrsInSubgrp;

		numClstrsInSubgrp = 0;

		numClstrsInSubgrp = getNumClstrsInGrp(tKNode); // 자식 kNode를 root로 하는 하위 트리의 클러스터 개수 반환

		numClstrs += numClstrsInSubgrp; // 현재 그룹의 클러스터 개수에 누적

		tKNode = tKNode->rightSibling; // 다음 자식 kNode로 이동
	}
	

	return numClstrs;
	
}

/*
struct _tagKeyMsg* Kmr::getKeyMsg()
{
	return this->rekeyMsg;
}
*/
void Kmr::initKNode (struct _tagKNode* kNode)
{
	GroupKey* gKey = new GroupKey;
	gKey->oldVal = -1; 
	gKey->newVal = gen1.genRandInt(Max_Key_Value);	
	kNode->groupKey = gKey;
	kNode->parent = NULL;
	kNode->leftChild = NULL;
	kNode->leftSibling = NULL;
	kNode->rightSibling = NULL;
	kNode->isCluster = false; // (2014.06.08) 클러스터 여부
	kNode->COG_X = -1; // (2014.06.19) 클러스터 내 노드들의 무게중심
	kNode->COG_Y = -1;
	kNode->CHID = -1;
	kNode->unode = NULL;
	kNode->level = 0;
	kNode->index = 0;

	kNode->updated = false;
	kNode->updated_addition = false; // (2014.05.08)
	kNode->updated_eviction = false; 

	kNode->created = true; // (2014.06.08) 

	kNode->numSensorNodes = -1; 
	kNode->outDegree = 0; 
}

void Kmr::initUNode (struct _tagUNode* uNode) // (2014.06.02) U-NODE 초기화
{
	uNode->isCH = false;
	uNode->parent = NULL;
	uNode->sensorNode = NULL;
}

int Kmr::getKTreeHeight() // key tree height 반환
{
	return this->kTreeHeight;
}

int Kmr::getNumNodesAtLevel (int level) // (2014.06.17 수정) // 주어진 level의 kNode 개수 반환
{
	int numKnodes;

	set<struct _tagKNode*> kNodesAtLevel; // 주어진 level의 kNOde 목록 획득

	kNodesAtLevel = getKNodesAtLevel(level);

	numKnodes = kNodesAtLevel.size(); // 주어진 level의 kNode 개수 계산

	return numKnodes;
	
}

bool Kmr::isCluster (struct _tagKNode* kNode) // (2014.06.08) 주어진 그룹 키 가 클러스터에 대응되는 지 확인 - True: 클러스터, False: 클러스터 아님
{
	if (kNode->isCluster == true)
	{
		return true;
	}

	return false;
}

set<struct _tagKNode*> Kmr::getKNodesAtLevel(int level) // (2014.06.17) 주어진 level의 kNode 목록 반환
{
	set<struct _tagKNode*> kNodesAtLevel; // 현재 level의 kNode 목록
	set<struct _tagKNode*> kNodesAtParentLevel; // 부모 level의 kNOde 목록

	if ( level < 0 || level >= this->kTreeHeight)
	{
		cout << " level is not (0~h-1)" <<endl;
		exit(-1);
	}

	struct _tagKNode* tKNode;

	if (level == 0)
	{
		kNodesAtLevel.insert(root);
		return kNodesAtLevel;
	}

	if (level == 1)
	{
		tKNode = root->leftChild;

		while (tKNode != NULL)
		{
			kNodesAtLevel.insert(tKNode);
			tKNode= tKNode->rightSibling;
		}

		return kNodesAtLevel;
	}

	kNodesAtParentLevel = getKNodesAtLevel(level-1); // 부모 level의 kNode 목록 구함

	struct _tagKNode* parentKNode;

	set<struct _tagKNode*>::iterator it;

	for ( it = kNodesAtParentLevel.begin(); it != kNodesAtParentLevel.end(); it++) // 부모 level의 각 kNode에 대해, 자식 노드 목록을 구하여 현재 level kNode 목록에 추가
	{
		parentKNode = *it;

		tKNode = parentKNode->leftChild;

		while (tKNode != NULL)
		{
			kNodesAtLevel.insert(tKNode);
			tKNode= tKNode->rightSibling;
		}
	} // for문 종료 - 현재 level의 kNode 목록 구성 완료


	return kNodesAtLevel;
}

struct _tagKNode* Kmr::getKNode( int level, int index) // (2014.06.20) key tree의 주어진 level과 index에 대응되는 knode 반환
{
	struct _tagKNode* ret = NULL;
	struct _tagKNode* tKNode = NULL;
	set<struct _tagKNode*> kNodesAtLevel;

	kNodesAtLevel = this->getKNodesAtLevel(level); // 해당 level에 속한 kNode 목록 획득

	set<struct _tagKNode*>::iterator it_kNode;

	for (it_kNode = kNodesAtLevel.begin(); it_kNode !=kNodesAtLevel.end(); it_kNode++) // 해당 level에 속한 kNOde 목록 순회, index에 해당하는 kNode 획득
	{
		tKNode = *it_kNode;	

		if ( tKNode->level == level && tKNode->index == index) // 주어진 level과 index에 대응되는 kNode 반환
		{
			ret = tKNode; 
		}//

	}// 해당 level의kNode들을 순회하여 해당 index의 kNode 발견 시 주소 반환

	/*
	int indexINLevel;

	indexINLevel = 0;

	for (it_kNode = kNodesAtLevel.begin(); it_kNode !=kNodesAtLevel.end(); it_kNode++) // 해당 level에 속한 kNOde 목록 순회, index에 해당하는 kNode 획득
	{
		tKNode = *it_kNode;	

		if (indexINLevel == index)
		{
			ret = tKNode;
			break;
		}

		indexINLevel++;
	}

	if (indexINLevel !=index) // kNOde 목록의 길이가 index 보다 작은 경우, NULL 반환
	{
		return NULL;
	}
	*/
	return ret;
	
}

struct _tagUNode* Kmr::getUNode (KNode* clstrKey, int snID)
{
	UNode* res = NULL;// 반환값
	bool found = false; // 센서 노드에 대응되는 unode 발견 여부 (T/F)

	if (clstrKey == NULL)
	{
		cout << " (clstrKey == NULL)" << endl;
		exit(-1);
	}

	KNode* temp = clstrKey->leftChild;

	while (temp != NULL)
	{
		UNode* uNode = temp->unode;

		if (uNode == NULL)
		{
			cout << "(uNode == NULL)" << endl;
			exit(-1);
		}

		if ( uNode->sensorNode->id == snID)
		{
			res = uNode;
			found = true;
			break;
		}
	}
}


int Kmr::getNumKNodes(struct _tagKNode* root) // (2014.06.13) (하위) 키 트리에 속한 k-node의 개수 반환
{
	int numKNodes;

	numKNodes = 1;

	struct _tagKNode* tKNode;

	tKNode = root->leftChild;

	while (tKNode != NULL)
	{
		int numKNodesInSubtree; 

		numKNodesInSubtree = getNumKNodes(tKNode);

		numKNodes += numKNodesInSubtree;

		tKNode = tKNode->rightSibling;
	}

	return numKNodes;
}


void Kmr::addPairKey (int nID, int nID2)
{
	// pairwise key 객체 생성

	PairKey* pk = NULL;
	PairKey* pk2 = NULL;

	pk = new PairKey;
	pk2 = new PairKey;

	if (nID <= nID2) // 두 노드의 iD 중 작은 것을 nID에, 큰 것을 nID2에 저장
	{
		pk->nID = nID;
		pk->nID2 = nID2;

		pk2->nID = nID;
		pk2->nID2 = nID2;
	}

	else
	{
		pk->nID = nID2;
		pk->nID2 = nID;

		pk2->nID = nID2;
		pk2->nID2 = nID;
	}

	// 새로 생성된 키 객체의 값 설정
	pk->kVal = nID + nID2; // 두 노드의 ID를 더한 값으로 설정
	pk2->kVal = nID + nID2; // 두 노드의 ID를 더한 값으로 설정

	// 두 노드의 pairwise key 목록에 각각 추가
	pKeys[nID].insert(pk);
	pKeys[nID2].insert(pk2);	

}


PairKey* Kmr::getPairKey (int nID1, int nID2)
{
	PairKey* pk = NULL;
	PairKey* temp_pk = NULL;

	set<PairKey*> pKeySet;
	pKeySet = pKeys[nID1];

	set<PairKey*>::iterator it;

	int smallerID;
	int biggerID;

	if (nID1 < nID2)
	{
		smallerID = nID1;
		biggerID = nID2;
	}
	else
	{
		smallerID = nID2;
		biggerID = nID1;
	}

	for (it = pKeySet.begin(); it != pKeySet.end(); it++) // pairkey 집합의 요소들을 하나씩 검사
	{
		temp_pk = *it;

		if ( temp_pk->nID == smallerID && temp_pk->nID2 == biggerID) // pairkey를 공유하는 두 노드의 ID 중 작은 id가 nID, 큰 id가 nID2에 저장된다고 가정
		{
			pk = temp_pk;
		}
	}

	return pk;
}

void Kmr::testPairKey() // (2014.08.19) pairwise key 정보를 파일에 출력
{
	ofstream out("tst_pairKey.txt");

	set<struct _tagPairKey*> pKSet;
	set<struct _tagPairKey*>::iterator;

	map<int, std::set<int> > clusters = rtn->getClusters(); 
	map<int, std::set<int> >::iterator it_cluster;

	for (it_cluster = clusters.begin(); it_cluster != clusters.end(); it_cluster++) // 클러스터 순회 - 각 클러스터에 속한 노드들의 pair key 정보 출력
	{
		if ( !(*it_cluster).second.empty())
		{
			int CHID = (*it_cluster).first;

			// (2014.09.21) 수정 - 			

			if ( nodes[CHID] == NULL) // CH가 이미 제거된 노드이면 다음 클러스터로 이동
			{
				continue;
			}
			// (2014.09.21 수정 완료) CH가 이미 제거된 노드이면 다음 클러스터로 이동

			set<int> clusternodes = (*it_cluster).second;

			out << " CH's ID: " << CHID << endl;			

			set<int>::iterator it2;

			for (it2 = clusternodes.begin(); it2!=clusternodes.end(); it2++) // 각 클러스터 내 소속 노드의 pair key 출력
			{
				int nodeID = *it2;
				out << "node: " << nodeID << endl;

				set<struct _tagPairKey*> pkSet = getPKSet(nodeID);

				set<struct _tagPairKey*>::iterator it_pk;

				for (it_pk = pkSet.begin(); it_pk != pkSet.end(); it_pk++) // 현재 노드의 pair key 목록 출력
				{
					PairKey* pk = *it_pk;

					out << "nID1: " << pk->nID << ", ";
					out << "nID2: " << pk->nID2 << ", ";
					out << "key value: " << pk->kVal << endl;					


				} // for문 - 현재 노드의 pair key 목록 출력 완료

				out << endl;

			} // for문 종료 - 각 클러스터에 속한 소속 노드 ID 목록 출력

			out << endl << endl;

		} // if문 - 현재 클러스터가 비어 있지 않으면 노드들의 pair key 정보 출력
	} // for문 완료 - 각 클러스터에 속한 노드들의 pair key 정보 출력	

	out.close();
}

set<struct _tagPairKey*> Kmr::getPKSet (int nID) // 주어진 노드의 pair key set 반환
{
	return this->pKeys[nID];
}

int Kmr::getNumPairKeys(Node* candi) // (2014.08.13) 후보 노드의 pairwirse key 개수 반환
{
	int ret = 0;

	int nodeID = candi->id;

	Sec_IHA* sec_IHA = dynamic_cast<Sec_IHA*> (sec);

	set<int> upperAssociatedNodes = sec_IHA->upperNodes[nodeID][INTERLEAVING_T+1];

	ret = upperAssociatedNodes.size();

	return ret;


	/*
	int ret = 0;

	int nodeID = candi->id;

	set<struct _tagPairKey*> pkSet = getPKSet(nodeID);

	ret = pkSet.size();

	return ret;
	*/
}

void Kmr::printKTree()
{
	ofstream out ("tst_kTree.txt");

	if (root != NULL)
	{
		//this->printKTree(root);
		this->printKTree(out, root);

	}
	//cout << endl;

	out.close();
}

void Kmr::printKTree(struct _tagKNode* kNode) // key tree 출력
{
	if (kNode == NULL)
		return;

	cout << " { " ;

	cout << kNode->level << ": ";

	cout << kNode->index;

	if (kNode->isCluster == true)
	{
		cout << "(" << kNode->numSensorNodes << ") "; 
	}

	if (kNode->unode != NULL)
	{
		Node* node;
		node = kNode->unode->sensorNode;
		if (node != NULL)
		{
			cout << " (SN_" << node->id<< ")";
		}
	}

	if (kNode->leftChild != NULL)
	{
		printKTree(kNode->leftChild);
	}

	// u- node 출력 구문 추가할 것!

	cout << " } ";	

	if (kNode->rightSibling != NULL)
	{
		cout << ", ";
		printKTree(kNode->rightSibling);
	}

	
}

void Kmr::printKTree(ofstream& out, struct _tagKNode* kNode) // (2014.08.31) 파일에 key tree 출력
{
	if (kNode == NULL)
		return;

	out << " { " ;

	out << kNode->level << ": ";

	out << kNode->index;

	if (kNode->isCluster == true)
	{
		out << "(" << kNode->numSensorNodes << ") "; 
	}

	if (kNode->unode != NULL)
	{
		Node* node;
		node = kNode->unode->sensorNode;
		if (node != NULL)
		{
			out << " (SN_" << node->id<< ")";
		}
	}

	if (kNode->leftChild != NULL)
	{
		printKTree(out, kNode->leftChild);
	}

	// u- node 출력 구문 추가할 것!

	out << " } ";	

	if (kNode->rightSibling != NULL)
	{
		out << ", ";
		printKTree(out, kNode->rightSibling);
	}
}

void Kmr::addKTreeNode( KNode* joinPoint, KNode* newNode) // key tree node 추가
{

}

void Kmr::deleteKTreeNode( KNode* leavePoint, KNode* delNode)  // key tree node 제거
{

}

Kmr::Kmr()
{
	astar = new AStar();
	ga = new GA();

	gen1.setSeed(1);


	/* 2014.06.02 수정 */
	this->root = new KNode; 

	// root 노드 초기화
	this->initKNode (root);

	// root 노드의 초기 키 값 설정
	root->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

	/* 2014.06.02 수정 완료*/
}

Kmr::~Kmr()
{
	delete astar;
	delete ga;
}

/*
int Kmr::getNumPairwiseKeys(Node* candi) // (2013.08.29) 후보 노드가 가진 pairwise key 개수 반환
{
	int numPairKeys = 0;

	int nID = candi->id;

	numPairKeys = this->pKeys[nID].size(); // 후보 노드가 가진 pairwise key 개수 획득

	return numPairKeys;
}

PairKey* Kmr::getPairKey (int nID1, int nID2) // (2013.08.19) 두 노드가 공유하는 pairwise key 계산 및 반환
{
	// 첫 번째 노드의 pairwise key 목록 획득

	PairKey* ret = NULL; 

	PKLIST lst = pKeys[nID1];
	
	PKLIST::iterator it = lst.begin();

	for (it; it != lst.end(); it++)
	{
		PairKey* pk = *it;

		if ( (pk->nID == nID1 && pk->nID2 == nID2) || (pk->nID2 == nID1 && pk->nID == nID2) ) // 키를 공유하는 두 노드의 ID가 입력 파라미터와 일치하면 pair key 발견
			ret = pk;
	}

	return ret;

	// 
}

Kmr::Kmr()
{

}

Kmr::~Kmr() // (2013.08.29) pairwise key 메모리 반납
{
	for (int i = 0; i < NUM_NODES; i++) // 모든 노드에 대해 반복
	{
		PKLIST::iterator it = this->pKeys[i].begin();

		for ( it; it != pKeys[i].end(); it++)
		{
			PairKey* pk = *it; // 현재 목록 내 모든 pair key 객체의 메모리 반납
			delete pk;
		}

		pKeys[i].clear(); // 목록 내 모든 요소 제거
	}// 모든 pairwise key 목록 정리
}
*/
/* 기존 소스
void Kmr::setNodes( Node nodes[])
{
	this->nodes = nodes; // 배열의 시작 주소 저장
}

void Kmr::genKeyMsg(int clstrID) // 키 배포 메시지 생성
{
	//  rtn 객체와 상호작용

	int minID = clstrID * NODES_PER_CLSTR; // 클러스터의 멤버 중 최소 ID
	int maxID = minID + NODES_PER_CLSTR - 1; // 클러스터의 멤버 중 최대 ID

	Node* CH = rtn->getCH(clstrID); // CH 획득
	
	for (int i = minID; i <= maxID; i++)
	{
		// 노드의 인증 키 갱신

		Node* cNode = nodes+i;

		int newAK =  getNextAK(this->aKeys[i].kVal); // 노드의 새로운 인증 키 계산

		aKeys[i].kVal = newAK; // 새로운 인증 키 정보 저장

		// 키 배포 메시지에 인증 키 저장
		keyMsg.keys[i-minID] = aKeys[i]; 		

		// 인증 키를 암호화할 비밀 키 선택
		int sKeyID = gen.uniformI(0, SKeysPerNode-1);

		// 키 배포 메시지에 비밀 키의 인덱스 저장
		keyMsg.sKeyID[i-minID] = sKeys[i][sKeyID].kID; 
		
	}

	keyMsg.sender = CH; 
	keyMsg.receiver = NULL; 

	keyMsg.numDsm = 0; // 키 배포 횟수 리셋

}

KeyMsg* Kmr::getKeyMsg() // 키 배포 메시지 객체의 주소 반환
{
	return (&keyMsg);
}

void Kmr::setRtn( Rtn* rtn)
{
	this->rtn = rtn;
}

bool Kmr::isStored(int nodeId, SecKey sKey)
{
	bool res = false;
	
	for (int i =0; i < SKeysPerNode; i++)
	{
		if (sKeys[nodeId][i].kID == sKey.kID)
			res = true;
	}

	return res;
}

bool Kmr::isStored(int nodeId, AuthKey vKey)
{
	bool res = false;
	
	for (int i =0; i < VKeysPerNode; i++)
	{
		if (vKeys[nodeId][i].nID == vKey.nID && vKeys[nodeId][i].seq == vKey.seq)
		{
			res = true;
		}
	}

	return res;
}


int Kmr::getNextAK(int kVal)
{
	// 현재는 기존 키 값을 그대로 반환 (2012.11.03)

	// 추후 인증 키 갱신 과정 구현 예정

	return kVal;
}

AuthKey* Kmr::getAKey (int cNodeID) // 현재 노드의 인증 키 반환
{
	return &aKeys[cNodeID]; 
}

AuthKey* Kmr::getVKeys (int cNodeID) // 현재 노드의 검증 키 반환
{
	return vKeys[cNodeID];
}

int Kmr::getNumObtKeys(int cNodeID) // (2013.04.22) 추가 -  현재 노드가 획득한 인증 키의 개수 반환
{
	return this->numObtKeys[cNodeID];
}

void Kmr::testKeyMsg(int clstrID)
{
	ofstream out;

	if (clstrID == 0)
		out.open("tst_Kmr_KeyMsg.txt");

	else
		out.open("tst_Kmr_KeyMsg.txt", ios::app);

	out << "\ncluster " << clstrID << endl;

	for(int i = 0; i < NODES_PER_CLSTR; i++)
	{

		// 인증 키 정보 출력
		out << "\nkeyMsg.keys[" << i << "].kVal: " << keyMsg.keys[i].kVal << endl;
		out << "keyMsg.keys[" << i << "].nID: " <<  keyMsg.keys[i].nID << endl;
		out << "keyMsg.keys[" << i << "].seq: " << keyMsg.keys[i].seq<< endl;

		// 비밀 키 정보 출력
		out << "keyMsg.sKeyID[" << i << "] = " << keyMsg.sKeyID[i] << endl;	

	}

	// sender 정보 출력

	out << "keyMsg.sender " << keyMsg.sender->id << endl;

	// 키 배포 횟수 출력

	out << "keyMsg.numDsm " << keyMsg.numDsm << endl;

		

	out.close();
}

Kmr::Kmr()
{
	cout << " Kmr::Kmr() " << endl;

	this->rtn = NULL;


	// 전역 키 집합 초기화

	int i = 0;

	for( i=0; i< GKP_SIZE; i++)
	{
		this->gkp[i].kID = i; // 현재로서는 kID와 kVal을 동일한 값으로 설정
		this->gkp[i].kVal = i; 
	}

	// 노드들의 각종 키 정보 초기화

	for ( i = 0; i < NUM_NODES; i++)
	{
		aKeys[i].kVal = 0;
		aKeys[i].nID = i;
		aKeys[i].seq = 1;

		for (int j = 0; j < SKeysPerNode; j++)
		{
			sKeys[i][j].kID = 0;
			sKeys[i][j].kVal = 0;
		}

		for (int j = 0; j < VKeysPerNode; j++)
		{
			vKeys[i][j].kVal = 0;
			vKeys[i][j].nID = -1;
			vKeys[i][j].seq = 0;
		}

		numObtKeys[i] = 0;

	}

	// key message 초기화

	for (int k = 0; k < NODES_PER_CLUSTER; k++)
	{
		keyMsg.keys[k].kVal = 0;
		keyMsg.keys[k].nID = 0;
		keyMsg.keys[k].seq = 0;
		keyMsg.sKeyID[k] = 0;
	}

	keyMsg.numDsm = 0;

	// 난수 발생기 객체 seed 설정

	this->seed1 = 1;
	this->gen1.randomize(&seed1);

	this->seed2 = 1;
	this->gen2.randomize(&seed2);

}

Kmr::~Kmr()
{
	// pairwise key 목록에 저장된 객체들을 목록에서 제거하고 메모리 반환

	for (int i = 0; i < NUM_NODES; i++)
	{
		PKLIST lst = pKeys[i]; // PAIRWISE key 목록 획득

		PKLIST::iterator it = lst.begin();

		for (it; it != lst.end(); it++)
		{			
			PairKey* tmp_ptr = *it;		
			delete (tmp_ptr); // 메모리 반환			
		}

		lst.clear(); // 목록의 모든 요소 제거
	}


}
*/