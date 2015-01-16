// 2014.07.31
// ������
// Ű ���� Ŭ����

#include "Kmr.h"
#include "Sec_IHA.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

void Kmr::setNodes(map<int, Node*> nodes) // (2013.08.28 �߰�)
{
	this->nodes = nodes; // �迭�� ���� �ּ� ����
}

void Kmr::setRtn(Rtn* rtn) // (2013.08.28 �߰�) rtn ��ü �ּ� ����
{
	this->rtn = rtn;
}

void Kmr::setSec(Sec* sec) // (2013.08.28 �߰�) sec ��ü �ּ� ����
{
	this->sec = sec;
}

void Kmr::resetUpdatedKeys() // (2014.07.31) key tree�� �� k-node ���� ���� �� �ʱ�ȭ
{
	this->resetUpdatedKeys(root);
}

void Kmr::resetUpdatedKeys(struct _tagKNode* kNode) // (2014.07.31) key tree�� �� k-node ���� ���� �� �ʱ�ȭ 
{
	// ���� kNode�� updated, updated_addition, updated_eviction �÷��׸� ���� (false)�� ����
	kNode->updated = false;
	kNode->updated_addition = false;
	kNode->updated_eviction = false;
	kNode->created = false;

	// ��� ȣ�� ���� ����
	if (kNode->unode != NULL)
	{
		return;
	}


	// ����� ȣ�� - left child

	if (kNode->leftChild != NULL)
	{
		resetUpdatedKeys(kNode->leftChild); // �ش� kNode�� leftChild�� ���� ��� ȣ��
	}

	// ����� ȣ�� - right sibling

	if ( kNode != root && kNode->rightSibling != NULL)
	{
		resetUpdatedKeys(kNode->rightSibling); // �ش� kNode�� rightChild�� ���� ��� ȣ��
	}
}

void Kmr::printReceivedNodes() // (2014.08.07) Ű ��й� �޽��� ���� ��� ���� ���
{
	ofstream out("tst_km_received_nodes.txt");

	set<int>::iterator it;

	out << " { ";

	for (it = this->receivedNodes.begin(); it != receivedNodes.end(); it++) // Ű ��й� �޽��� ���� ��� ��� ���
	{
		int nodeID = *it;

		out << nodeID << ", ";

	} // for �� ���� - Ű ��й� �޽��� ���� ��� ��� ���

	out << " } ";

	return;
}

void Kmr::resetReceivedNodes() // (2014.08.07) Ű ��й� �޽��� ���� ��� ���� ����
{
	this->receivedNodes.clear();
}

bool Kmr::isSubset (IDSet a, IDSet b) // (2014.07.10 �߰�) �κ����� ���� ��ȯ
{
	bool ret = true;
	int nodeID;

	IDSet::iterator it;

	for (it = a.begin(); it != a.end(); it++) // ���� a�� ��ҵ� ��ȸ
	{
		nodeID = *it;

		if ( b.find(nodeID) == b.end() ) // ���� A�� ���ҵ� �� �ϳ��� B�� ���Ե��� �ʴ� ���Ұ� �����ϸ� subset�� �ƴ�
		{
			ret = false;
		}
	} // for�� ���� - ���� A�� ��� ��ҵ鿡 ���� ���� B�� ���Կ��� Ȯ��

	return ret;
}

void Kmr::initKeyMsg(struct _tagKeyMsg* kMsg) // Ű ��й� �޽��� �ʱ�ȭ
{
	kMsg->CHID = -1;
}

map<int, struct _tagKeyMsg*> Kmr:: getKeyMsgs( )
{
	return keyMsgs;
}

void Kmr::delKeyMsg() // Ű ��й� �޽��� ����
{
	for (unsigned int m = 0; m < keyMsgs.size(); m++) // Ű �޽����� �Ҵ�� �޸� ���� �ݳ�
	{
		KeyMsg* keyMsg = keyMsgs[m];
		free (keyMsg);
	}

	keyMsgs.clear(); // Ű �޽��� ��� ����
}

void Kmr::delRecvdKMsg(Node* cNode) // ���ŵ� Ű ��й� �޽��� ����
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

void Kmr::storeKMsg(struct _tagKeyMsg* keyMsg, Node* cNode) // (2014.05.15) ���ŵ� Ű ��й� �޽��� ����
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
	// ������ ����� Ű ��й� �޽��� ����
	
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
	// ���ŵ� Ű ��й� �޽��� ���� �Ϸ�
}

set<int> Kmr::getUserSet( struct _tagKNode* kNode) // (2014.07.08) �־��� kNode�� ����� �׷� Ű�� ������ ��� ��� ��ȯ
{
	set<int> nodeSet;

	// ...

	return nodeSet;
}

int Kmr::renumberKNodes() // (2014.06.12) Ű Ʈ���� ���� ��� kNode���� level�� index �缳��
{
	int height;
	bool nextLevelExist;
	nextLevelExist = false;
	//list<struct _tagKNode*> cur_level_knodes;	

	cur_level_knodes.clear();

	int cur_level = 0;	
	cur_level_knodes.push_back(root); // ���� level ��� ��� ���� (level 0: root)

	
	while ( !cur_level_knodes.empty() )
	{		
		nextLevelExist = renumberKNodes(cur_level); // ���� level�� kNOde���� level & index �缳��

		if (nextLevelExist == true)
		{
			cur_level++;
		}
	}

	height = cur_level+1;

	return height;
}

bool Kmr::renumberKNodes(int level) // (2014.06.12) Ư�� level�� ���� kNode���� level�� index �缳��
{
	bool nextLevelExist; // ���� level ��尡 �����ϴ� �� ����

	nextLevelExist = false;

	list<struct _tagKNode*> next_level_knodes;	

	list<struct _tagKNode*>::iterator it;

	int index = 0;

	for (it = cur_level_knodes.begin(); it != cur_level_knodes.end(); it++) // ���� level�� ����κ��� �� ���� level�� ������ ���ϰ� level �� index ����
	{
		struct _tagKNode* parent;
		parent = *it;

		struct _tagKNode* kNode;

		kNode = parent->leftChild;

		while (kNode != NULL) // ���� ����� �ڽ� ���鿡 ���� �ݺ�
		{
			nextLevelExist = true; // ���� level ��� ����

			kNode->level = level+1;
			kNode->index = index++;
			next_level_knodes.push_back(kNode);
			kNode = kNode->rightSibling;
		}		
	}

	cur_level_knodes = next_level_knodes; // ���� level�� ��� ��� ����

	return nextLevelExist;
}

set<int> Kmr::getNdsInGrp(struct _tagKNode* root) // (2014.07.08) �־��� �׷쿡 ���Ե� ���� ��� ID ���� ��ȯ - root: �׷��� �ֻ��� kNode (Ű �׷��� ���� root�ʹ� �ٸ� �� ����)
{
	set<int> nodesInGrp;

	struct _tagKNode* tKNode = NULL;

	if ( root->unode != NULL) // �־��� kNode�� individual key�� ������ ���
	{
		struct _tagUNode* uNode = root->unode;
		
		if (uNode->sensorNode != NULL)
		{
			nodesInGrp.insert(uNode->sensorNode->id); // (2014.06.08) �׷쿡 ���� ���� ��� ID ���� ����				
		}

		return nodesInGrp; // leaf node�� ������ ���� ��� ID ��ȯ
	}


	if ( root->isCluster == true) // �־��� �׷��� Ŭ�������� ���
	{
		tKNode = root->leftChild;

		while (tKNode != NULL) // Ŭ�����Ϳ� ���� �� ��忡 ���� �ݺ�
		{
			struct _tagUNode* uNode = tKNode->unode;

			if (uNode->sensorNode != NULL)
			{
				nodesInGrp.insert(uNode->sensorNode->id); // (2014.06.08) �׷쿡 ���� ���� ��� ID ���� ����
				
			}
			tKNode = tKNode->rightSibling;
		} // while - Ŭ������ �� ��� ID ���� ����

		return nodesInGrp; // Ŭ������ �� ��� �� ��ȯ
	}	

	tKNode = root->leftChild;

	// ������ subtree �鿡 ���� ��������� �Լ� ȣ��, ���� �׷� ��� ID ������ ��ҵ��� �׷� ��� id ���տ� �߰�

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

int Kmr::getNumNdsInGrp(struct _tagKNode* root) // (2014.05.30) �־��� �׷쿡 ���Ե� ���� ��� �� ��ȯ - root: �׷��� �ֻ��� kNode (Ű �׷��� ���� root�ʹ� �ٸ� �� ����)
{
	int numNodes = 0;
	//nodeSetInGrp.clear();

	struct _tagKNode* tKNode = NULL;


	if (root->isCluster == true) // (2014.06.10) �־��� �׷��� Ŭ�������� ���			
	{
		tKNode = root->leftChild;

		while (tKNode != NULL) // Ŭ�����Ϳ� ���� �� ��忡 ���� �ݺ�
		{
			struct _tagUNode* uNode = tKNode->unode;

			if (uNode->sensorNode != NULL)
			{
				numNodes++;
				//nodeSetInGrp.insert ( uNode->sensorNode->id); // (2014.06.05) �׷쿡 ���� ���� ��� ID ���� ����
			}

			tKNode = tKNode->rightSibling;

		} // while - Ŭ������ �� ��� �� ���
			
		return numNodes; // Ŭ������ �� ��� �� ��ȯ
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

int Kmr::getNumClstrsInGrp(struct _tagKNode* root) // (2014.05.30) �־��� �׷쿡 ���Ե� Ŭ������ �� ��ȯ
{
	int numClstrs;

	numClstrs = 0;

	// ���� ����

	if (root->isCluster == true)// kNode�� Ŭ������ Ű�� ���
	{
		numClstrs =1;
		return numClstrs;
	}
	else if (root->unode != NULL)  // kNOde�� individual Ű�� ���
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

		numClstrsInSubgrp = getNumClstrsInGrp(tKNode); // �ڽ� kNode�� root�� �ϴ� ���� Ʈ���� Ŭ������ ���� ��ȯ

		numClstrs += numClstrsInSubgrp; // ���� �׷��� Ŭ������ ������ ����

		tKNode = tKNode->rightSibling; // ���� �ڽ� kNode�� �̵�
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
	kNode->isCluster = false; // (2014.06.08) Ŭ������ ����
	kNode->COG_X = -1; // (2014.06.19) Ŭ������ �� ������ �����߽�
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

void Kmr::initUNode (struct _tagUNode* uNode) // (2014.06.02) U-NODE �ʱ�ȭ
{
	uNode->isCH = false;
	uNode->parent = NULL;
	uNode->sensorNode = NULL;
}

int Kmr::getKTreeHeight() // key tree height ��ȯ
{
	return this->kTreeHeight;
}

int Kmr::getNumNodesAtLevel (int level) // (2014.06.17 ����) // �־��� level�� kNode ���� ��ȯ
{
	int numKnodes;

	set<struct _tagKNode*> kNodesAtLevel; // �־��� level�� kNOde ��� ȹ��

	kNodesAtLevel = getKNodesAtLevel(level);

	numKnodes = kNodesAtLevel.size(); // �־��� level�� kNode ���� ���

	return numKnodes;
	
}

bool Kmr::isCluster (struct _tagKNode* kNode) // (2014.06.08) �־��� �׷� Ű �� Ŭ�����Ϳ� �����Ǵ� �� Ȯ�� - True: Ŭ������, False: Ŭ������ �ƴ�
{
	if (kNode->isCluster == true)
	{
		return true;
	}

	return false;
}

set<struct _tagKNode*> Kmr::getKNodesAtLevel(int level) // (2014.06.17) �־��� level�� kNode ��� ��ȯ
{
	set<struct _tagKNode*> kNodesAtLevel; // ���� level�� kNode ���
	set<struct _tagKNode*> kNodesAtParentLevel; // �θ� level�� kNOde ���

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

	kNodesAtParentLevel = getKNodesAtLevel(level-1); // �θ� level�� kNode ��� ����

	struct _tagKNode* parentKNode;

	set<struct _tagKNode*>::iterator it;

	for ( it = kNodesAtParentLevel.begin(); it != kNodesAtParentLevel.end(); it++) // �θ� level�� �� kNode�� ����, �ڽ� ��� ����� ���Ͽ� ���� level kNode ��Ͽ� �߰�
	{
		parentKNode = *it;

		tKNode = parentKNode->leftChild;

		while (tKNode != NULL)
		{
			kNodesAtLevel.insert(tKNode);
			tKNode= tKNode->rightSibling;
		}
	} // for�� ���� - ���� level�� kNode ��� ���� �Ϸ�


	return kNodesAtLevel;
}

struct _tagKNode* Kmr::getKNode( int level, int index) // (2014.06.20) key tree�� �־��� level�� index�� �����Ǵ� knode ��ȯ
{
	struct _tagKNode* ret = NULL;
	struct _tagKNode* tKNode = NULL;
	set<struct _tagKNode*> kNodesAtLevel;

	kNodesAtLevel = this->getKNodesAtLevel(level); // �ش� level�� ���� kNode ��� ȹ��

	set<struct _tagKNode*>::iterator it_kNode;

	for (it_kNode = kNodesAtLevel.begin(); it_kNode !=kNodesAtLevel.end(); it_kNode++) // �ش� level�� ���� kNOde ��� ��ȸ, index�� �ش��ϴ� kNode ȹ��
	{
		tKNode = *it_kNode;	

		if ( tKNode->level == level && tKNode->index == index) // �־��� level�� index�� �����Ǵ� kNode ��ȯ
		{
			ret = tKNode; 
		}//

	}// �ش� level��kNode���� ��ȸ�Ͽ� �ش� index�� kNode �߰� �� �ּ� ��ȯ

	/*
	int indexINLevel;

	indexINLevel = 0;

	for (it_kNode = kNodesAtLevel.begin(); it_kNode !=kNodesAtLevel.end(); it_kNode++) // �ش� level�� ���� kNOde ��� ��ȸ, index�� �ش��ϴ� kNode ȹ��
	{
		tKNode = *it_kNode;	

		if (indexINLevel == index)
		{
			ret = tKNode;
			break;
		}

		indexINLevel++;
	}

	if (indexINLevel !=index) // kNOde ����� ���̰� index ���� ���� ���, NULL ��ȯ
	{
		return NULL;
	}
	*/
	return ret;
	
}

struct _tagUNode* Kmr::getUNode (KNode* clstrKey, int snID)
{
	UNode* res = NULL;// ��ȯ��
	bool found = false; // ���� ��忡 �����Ǵ� unode �߰� ���� (T/F)

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


int Kmr::getNumKNodes(struct _tagKNode* root) // (2014.06.13) (����) Ű Ʈ���� ���� k-node�� ���� ��ȯ
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
	// pairwise key ��ü ����

	PairKey* pk = NULL;
	PairKey* pk2 = NULL;

	pk = new PairKey;
	pk2 = new PairKey;

	if (nID <= nID2) // �� ����� iD �� ���� ���� nID��, ū ���� nID2�� ����
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

	// ���� ������ Ű ��ü�� �� ����
	pk->kVal = nID + nID2; // �� ����� ID�� ���� ������ ����
	pk2->kVal = nID + nID2; // �� ����� ID�� ���� ������ ����

	// �� ����� pairwise key ��Ͽ� ���� �߰�
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

	for (it = pKeySet.begin(); it != pKeySet.end(); it++) // pairkey ������ ��ҵ��� �ϳ��� �˻�
	{
		temp_pk = *it;

		if ( temp_pk->nID == smallerID && temp_pk->nID2 == biggerID) // pairkey�� �����ϴ� �� ����� ID �� ���� id�� nID, ū id�� nID2�� ����ȴٰ� ����
		{
			pk = temp_pk;
		}
	}

	return pk;
}

void Kmr::testPairKey() // (2014.08.19) pairwise key ������ ���Ͽ� ���
{
	ofstream out("tst_pairKey.txt");

	set<struct _tagPairKey*> pKSet;
	set<struct _tagPairKey*>::iterator;

	map<int, std::set<int> > clusters = rtn->getClusters(); 
	map<int, std::set<int> >::iterator it_cluster;

	for (it_cluster = clusters.begin(); it_cluster != clusters.end(); it_cluster++) // Ŭ������ ��ȸ - �� Ŭ�����Ϳ� ���� ������ pair key ���� ���
	{
		if ( !(*it_cluster).second.empty())
		{
			int CHID = (*it_cluster).first;

			// (2014.09.21) ���� - 			

			if ( nodes[CHID] == NULL) // CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
			{
				continue;
			}
			// (2014.09.21 ���� �Ϸ�) CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�

			set<int> clusternodes = (*it_cluster).second;

			out << " CH's ID: " << CHID << endl;			

			set<int>::iterator it2;

			for (it2 = clusternodes.begin(); it2!=clusternodes.end(); it2++) // �� Ŭ������ �� �Ҽ� ����� pair key ���
			{
				int nodeID = *it2;
				out << "node: " << nodeID << endl;

				set<struct _tagPairKey*> pkSet = getPKSet(nodeID);

				set<struct _tagPairKey*>::iterator it_pk;

				for (it_pk = pkSet.begin(); it_pk != pkSet.end(); it_pk++) // ���� ����� pair key ��� ���
				{
					PairKey* pk = *it_pk;

					out << "nID1: " << pk->nID << ", ";
					out << "nID2: " << pk->nID2 << ", ";
					out << "key value: " << pk->kVal << endl;					


				} // for�� - ���� ����� pair key ��� ��� �Ϸ�

				out << endl;

			} // for�� ���� - �� Ŭ�����Ϳ� ���� �Ҽ� ��� ID ��� ���

			out << endl << endl;

		} // if�� - ���� Ŭ�����Ͱ� ��� ���� ������ ������ pair key ���� ���
	} // for�� �Ϸ� - �� Ŭ�����Ϳ� ���� ������ pair key ���� ���	

	out.close();
}

set<struct _tagPairKey*> Kmr::getPKSet (int nID) // �־��� ����� pair key set ��ȯ
{
	return this->pKeys[nID];
}

int Kmr::getNumPairKeys(Node* candi) // (2014.08.13) �ĺ� ����� pairwirse key ���� ��ȯ
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

void Kmr::printKTree(struct _tagKNode* kNode) // key tree ���
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

	// u- node ��� ���� �߰��� ��!

	cout << " } ";	

	if (kNode->rightSibling != NULL)
	{
		cout << ", ";
		printKTree(kNode->rightSibling);
	}

	
}

void Kmr::printKTree(ofstream& out, struct _tagKNode* kNode) // (2014.08.31) ���Ͽ� key tree ���
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

	// u- node ��� ���� �߰��� ��!

	out << " } ";	

	if (kNode->rightSibling != NULL)
	{
		out << ", ";
		printKTree(out, kNode->rightSibling);
	}
}

void Kmr::addKTreeNode( KNode* joinPoint, KNode* newNode) // key tree node �߰�
{

}

void Kmr::deleteKTreeNode( KNode* leavePoint, KNode* delNode)  // key tree node ����
{

}

Kmr::Kmr()
{
	astar = new AStar();
	ga = new GA();

	gen1.setSeed(1);


	/* 2014.06.02 ���� */
	this->root = new KNode; 

	// root ��� �ʱ�ȭ
	this->initKNode (root);

	// root ����� �ʱ� Ű �� ����
	root->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

	/* 2014.06.02 ���� �Ϸ�*/
}

Kmr::~Kmr()
{
	delete astar;
	delete ga;
}

/*
int Kmr::getNumPairwiseKeys(Node* candi) // (2013.08.29) �ĺ� ��尡 ���� pairwise key ���� ��ȯ
{
	int numPairKeys = 0;

	int nID = candi->id;

	numPairKeys = this->pKeys[nID].size(); // �ĺ� ��尡 ���� pairwise key ���� ȹ��

	return numPairKeys;
}

PairKey* Kmr::getPairKey (int nID1, int nID2) // (2013.08.19) �� ��尡 �����ϴ� pairwise key ��� �� ��ȯ
{
	// ù ��° ����� pairwise key ��� ȹ��

	PairKey* ret = NULL; 

	PKLIST lst = pKeys[nID1];
	
	PKLIST::iterator it = lst.begin();

	for (it; it != lst.end(); it++)
	{
		PairKey* pk = *it;

		if ( (pk->nID == nID1 && pk->nID2 == nID2) || (pk->nID2 == nID1 && pk->nID == nID2) ) // Ű�� �����ϴ� �� ����� ID�� �Է� �Ķ���Ϳ� ��ġ�ϸ� pair key �߰�
			ret = pk;
	}

	return ret;

	// 
}

Kmr::Kmr()
{

}

Kmr::~Kmr() // (2013.08.29) pairwise key �޸� �ݳ�
{
	for (int i = 0; i < NUM_NODES; i++) // ��� ��忡 ���� �ݺ�
	{
		PKLIST::iterator it = this->pKeys[i].begin();

		for ( it; it != pKeys[i].end(); it++)
		{
			PairKey* pk = *it; // ���� ��� �� ��� pair key ��ü�� �޸� �ݳ�
			delete pk;
		}

		pKeys[i].clear(); // ��� �� ��� ��� ����
	}// ��� pairwise key ��� ����
}
*/
/* ���� �ҽ�
void Kmr::setNodes( Node nodes[])
{
	this->nodes = nodes; // �迭�� ���� �ּ� ����
}

void Kmr::genKeyMsg(int clstrID) // Ű ���� �޽��� ����
{
	//  rtn ��ü�� ��ȣ�ۿ�

	int minID = clstrID * NODES_PER_CLSTR; // Ŭ�������� ��� �� �ּ� ID
	int maxID = minID + NODES_PER_CLSTR - 1; // Ŭ�������� ��� �� �ִ� ID

	Node* CH = rtn->getCH(clstrID); // CH ȹ��
	
	for (int i = minID; i <= maxID; i++)
	{
		// ����� ���� Ű ����

		Node* cNode = nodes+i;

		int newAK =  getNextAK(this->aKeys[i].kVal); // ����� ���ο� ���� Ű ���

		aKeys[i].kVal = newAK; // ���ο� ���� Ű ���� ����

		// Ű ���� �޽����� ���� Ű ����
		keyMsg.keys[i-minID] = aKeys[i]; 		

		// ���� Ű�� ��ȣȭ�� ��� Ű ����
		int sKeyID = gen.uniformI(0, SKeysPerNode-1);

		// Ű ���� �޽����� ��� Ű�� �ε��� ����
		keyMsg.sKeyID[i-minID] = sKeys[i][sKeyID].kID; 
		
	}

	keyMsg.sender = CH; 
	keyMsg.receiver = NULL; 

	keyMsg.numDsm = 0; // Ű ���� Ƚ�� ����

}

KeyMsg* Kmr::getKeyMsg() // Ű ���� �޽��� ��ü�� �ּ� ��ȯ
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
	// ����� ���� Ű ���� �״�� ��ȯ (2012.11.03)

	// ���� ���� Ű ���� ���� ���� ����

	return kVal;
}

AuthKey* Kmr::getAKey (int cNodeID) // ���� ����� ���� Ű ��ȯ
{
	return &aKeys[cNodeID]; 
}

AuthKey* Kmr::getVKeys (int cNodeID) // ���� ����� ���� Ű ��ȯ
{
	return vKeys[cNodeID];
}

int Kmr::getNumObtKeys(int cNodeID) // (2013.04.22) �߰� -  ���� ��尡 ȹ���� ���� Ű�� ���� ��ȯ
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

		// ���� Ű ���� ���
		out << "\nkeyMsg.keys[" << i << "].kVal: " << keyMsg.keys[i].kVal << endl;
		out << "keyMsg.keys[" << i << "].nID: " <<  keyMsg.keys[i].nID << endl;
		out << "keyMsg.keys[" << i << "].seq: " << keyMsg.keys[i].seq<< endl;

		// ��� Ű ���� ���
		out << "keyMsg.sKeyID[" << i << "] = " << keyMsg.sKeyID[i] << endl;	

	}

	// sender ���� ���

	out << "keyMsg.sender " << keyMsg.sender->id << endl;

	// Ű ���� Ƚ�� ���

	out << "keyMsg.numDsm " << keyMsg.numDsm << endl;

		

	out.close();
}

Kmr::Kmr()
{
	cout << " Kmr::Kmr() " << endl;

	this->rtn = NULL;


	// ���� Ű ���� �ʱ�ȭ

	int i = 0;

	for( i=0; i< GKP_SIZE; i++)
	{
		this->gkp[i].kID = i; // ����μ��� kID�� kVal�� ������ ������ ����
		this->gkp[i].kVal = i; 
	}

	// ������ ���� Ű ���� �ʱ�ȭ

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

	// key message �ʱ�ȭ

	for (int k = 0; k < NODES_PER_CLUSTER; k++)
	{
		keyMsg.keys[k].kVal = 0;
		keyMsg.keys[k].nID = 0;
		keyMsg.keys[k].seq = 0;
		keyMsg.sKeyID[k] = 0;
	}

	keyMsg.numDsm = 0;

	// ���� �߻��� ��ü seed ����

	this->seed1 = 1;
	this->gen1.randomize(&seed1);

	this->seed2 = 1;
	this->gen2.randomize(&seed2);

}

Kmr::~Kmr()
{
	// pairwise key ��Ͽ� ����� ��ü���� ��Ͽ��� �����ϰ� �޸� ��ȯ

	for (int i = 0; i < NUM_NODES; i++)
	{
		PKLIST lst = pKeys[i]; // PAIRWISE key ��� ȹ��

		PKLIST::iterator it = lst.begin();

		for (it; it != lst.end(); it++)
		{			
			PairKey* tmp_ptr = *it;		
			delete (tmp_ptr); // �޸� ��ȯ			
		}

		lst.clear(); // ����� ��� ��� ����
	}


}
*/