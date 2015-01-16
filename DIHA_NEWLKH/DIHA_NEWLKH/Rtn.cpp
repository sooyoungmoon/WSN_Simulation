

// 2014.05.23
// ������
// Rtn.cpp
// ����� Ŭ���� ����


#include "Rtn.h"
#include "params.h"
#include <fstream>
#include <exception>

using namespace std;

void Rtn::setNodes( map<int, Node*> nodes)
{
	this->nodes = nodes;
}

void Rtn::setBS( BS* bs)
{
	this->bs = bs;
}

void Rtn::findNbrs()
{
	// �Լ� ���Ǻ� �ۼ� �ʿ�
}

Node* Rtn::getCH (int clstrID)
{
	Node* CH = NULL;

	// ...

	return CH;
}

void Rtn::changeCluster (int nodeID, int clstrID) // �־��� ����� Ŭ�����͸� ���� - nodeID: ��� ID, clstrID: Ŭ������ id
{
	

	this->clstrUpdated[nodeID] = false;

	int oldCluster = getClstrID(nodeID); // (2014.08.04) ���� Ŭ�����Ͱ� ������ -1�� ��ȯ�ǵ��� getClstrID �Լ� ȣ��

	//int oldCluster = ndsClstrMap[nodeID];

	if (oldCluster != clstrID)
	{
		clstrUpdated[nodeID] = true;
	}

	ndsClstrMap[nodeID] = clstrID; // ����� Ŭ������ ID  (=CH's ID) ����

	//if ( nodes[nodeID]->newbee == false) // ���� ������� �����ߴ� ����� ���, �� Ŭ�����Ϳ��� �ش� ��� ID ���� ����
	{
		set<int>::iterator it;
	
		it = clusters[oldCluster].find(nodeID);	

		if (it != clusters[oldCluster].end() )
		{	
			clusters[oldCluster].erase(it); // �� Ŭ�����Ϳ��� ��� ID ����
		}
	}

	
	clusters[clstrID].insert(nodeID); 	// �ű� Ŭ�����Ϳ� ��� id �߰�

}

void Rtn::changeCluster (int nodeID, int clstrID, int CH_X, int CH_Y) // (2014.08.04) �־��� ����� Ŭ�����͸� ���� - nodeID: ��� ID, clstrID: Ŭ������ id, CH_X: �ű� CH�� X ��ǥ, CH_Y: �ű� CH�� Y ��ǥ
{
	double dist;

	this->clstrUpdated[nodeID] = false;

	int oldCluster = getClstrID(nodeID); // (2014.08.04) ���� Ŭ�����Ͱ� ������ -1�� ��ȯ�ǵ��� getClstrID �Լ� ȣ��

	//int oldCluster = ndsClstrMap[nodeID];	

	//if ( nodes[nodeID]->newbee == false) // ���� ������� �����ߴ� ����� ���, �� Ŭ�����Ϳ��� �ش� ��� ID ���� ����
	{
		set<int>::iterator it;
	
		it = clusters[oldCluster].find(nodeID);	

		if (it != clusters[oldCluster].end() )
		{	
			clusters[oldCluster].erase(it); // �� Ŭ�����Ϳ��� ��� ID ����
		}
	}

	dist = sqrt (pow ( (double) nodes[nodeID]->x - CH_X, 2) + pow ( (double) nodes[nodeID]->y - CH_Y, 2));// ���� ���� CH���� �Ÿ� ���

	if (dist < TRANS_RANGE) // (2014.08.04) �ű� Ŭ�������� CH���� ���� ���� �̳��̸� �ش� Ŭ�����Ϳ� ��� id �߰�
	{
		if (oldCluster != clstrID)
		{
			clstrUpdated[nodeID] = true; // �ű� Ŭ�����Ϳ� ������ �߰��Ǵ� ��쿡�� ���� ���θ� ������ ����
		}

		ndsClstrMap[nodeID] = clstrID; // ����� Ŭ������ ID  (=CH's ID) ����
		clusters[clstrID].insert(nodeID); 	// �ű� Ŭ�����Ϳ� ��� id �߰�
	}
}

void Rtn::removeFromCluster (int nodeID) // (2014.07.14) ��Ʈ��ũ���� ���ŵ� ��带 Ŭ�����Ϳ��� �����ϴ� �Լ�
{
	int oldCluster = ndsClstrMap[nodeID];

	if (nodeID == 1251)
	{
		cout << "nodeID == 1251" << endl;
	}

	ndsClstrMap.erase(nodeID); // node-cluster map���� ����

	set<int>::iterator it;

	it = clusters[oldCluster].find(nodeID);	

	if (it != clusters[oldCluster].end() )
	{	
		clusters[oldCluster].erase(it); // �� Ŭ�����Ϳ��� ��� ID ����
	}

	// Ŭ������ ���� ���θ� �����ϴ� clstrUpdated �ʿ��� �ش� ��� ���� ����

	//std::map<int, bool>::iterator it_nd;	

	clstrUpdated.erase(nodeID);

	/*for (it_nd = this->clstrUpdated.begin(); it_nd != clstrUpdated.end(); it_nd++) // ������ Ŭ������ ���� ���θ� false �� ���� - Ű ��й��� ����
	{
		if (nodeID == (*it_nd).first)
		{
			clstrUpdated.erase(nodeID);
		}
	} // for�� ���� - ������ Ŭ������ ���� ���θ� false �� ����
	*/
}

void Rtn::removeCH (int CHID)
{
	set<int>::iterator it;

	for (it = clusters[CHID].begin(); it != clusters[CHID].end(); it++) // �Ҽ� ������ Ŭ������ ���� ����
	{
		int nID = *it;

		ndsClstrMap[nID] = -1; // ���� Ŭ�����Ϳ� ���� �ִ� ������ Ŭ������ ������ ����
	} // Ŭ������ ���� ���� �Ϸ�


	clusters.erase(CHID); // Ŭ������ map���� �ش� CH�� �Ҽ� ����� ������ ��Ҹ� ����
}

bool Rtn::hasUpdatedCH (int nodeID)// (2014.05.19) �־��� nodeID�� �����Ǵ� ����� Ŭ������ (CH' ID)�� ����Ǿ����� �˻�
{
	if ( this->clstrUpdated[nodeID] == true)
	{
		return true;
	}
	else
		return false;
}

void Rtn::arrangeClusters() // (2014.08.11) Ŭ������ ��� �� ũ�Ⱑ 0�� Ŭ������ ������ ���� 
{
	map<int, set<int>>::iterator it_cluster;
	set<int> cluster;

	set<int> clstr_toBeDltd; // ���ŵ� Ŭ������ ���
	set<int>::iterator it_chID;

	for (it_cluster = this->clusters.begin(); it_cluster != this->clusters.end(); it_cluster++) // ũ�Ⱑ 0�� Ŭ������ ���� ����
	{
		cluster = (*it_cluster).second;

		if (cluster.empty())
		{
			clstr_toBeDltd.insert( (*it_cluster).first);
		}

		// (2014.09.21 ����) CH�� �̹� ���ŵ� ����̸� �ش� Ŭ������ ���� ����
		int CHID = (*it_cluster).first;

		if (nodes[CHID] == NULL)
		{
			clstr_toBeDltd.insert( (*it_cluster).first);
		}

	} // for�� ���� - ���ŵ� Ŭ������ ��� ���� �Ϸ�

	for (it_chID = clstr_toBeDltd.begin(); it_chID != clstr_toBeDltd.end(); it_chID++) // ũ�Ⱑ 0�� Ŭ������ ���� ����
	{
		int CHID = *it_chID;
		this->clusters.erase(CHID);
	} // for�� ���� - Ŭ������ ��� ���� �Ϸ�
}

set<int> Rtn::getClstr (int clstrID) // (2014.05.21) �Է�: Ŭ������ ID (ch's ID), ���: Ŭ������ �Ҽ� ������ iD ����
{
	return clusters[clstrID];
}

map<int, set<int>> Rtn::getClusters()
{
	return this->clusters;
}

int Rtn::getClstrID (int nodeID) // (2014.04.10) �Է�: ��� id, ���: Ŭ������ ID (= CH's ID)
{
	int clusterID = -1;

	std::map<int, int>::iterator it;

	it = ndsClstrMap.find(nodeID);

	if (it != ndsClstrMap.end() ) // �־��� ��� ID�� ���-Ŭ������ �ʿ� �����ϴ� ���,
	{
		clusterID = it->second; // �ش� ��� ID�� �����Ǵ� Ŭ������ id ����
	}

	return clusterID;
}

void Rtn::addParentNode (Node* cNode, Node* prevNode) // (2013.10.11 �߰�) �θ� ��� �߰� (�� ��尡 ���� ���� �θ� ��带 ���� �� ����)	
{
	int cNodeID = cNode->id;
	
	//parentsLsts[cNodeID].push_back(prevNode);
	parentsNodes[cNodeID].push_back(prevNode);
}

void Rtn::addParentCH (Node* cNode, Node* parentCH) // // (2014.08.14) �θ� CH ��� �߰�
{
	int cNodeID = cNode->id;
	
	parentCHs[cNodeID].push_back(parentCH);
}

std::list<Node*> Rtn::getParentNodes(Node* cNode) // // (2013.10.11 �߰�) �θ� ��� ����Ʈ ��ȯ
{
	int cNodeID = cNode->id;

	return parentsNodes.at(cNodeID);
}

std::list<Node*> Rtn::getParentCHs(Node* cNode) // (2014.08.14 �߰�) �θ� CH ��� ����Ʈ ��ȯ
{
	int cNodeID = cNode->id;

	return parentCHs.at(cNodeID);
}

Node* Rtn::getSPParentNode (Node* cNode) // (2013.09.09) �θ� ��� ��ȯ
{
	Node* spParent = NULL; // ���� ����� BS���� �ִ� ��� ���� �θ� ���

	// .. �� ��带 ���� �θ� ��� ��Ͽ� ����� �ּ� �� ù ��°�� �ִ� ��� ���� �θ� ��忡 �ش��Ѵٰ� ����

	list<Node*> lst = parentsNodes[cNode->id]; // ���� ����� �θ� ��� ��� ��ȯ

	if (!lst.empty() )
	{
		spParent = lst.front(); // �θ� ��� ����� ù ��° ��� ��ȯ (Node*)
	}

	// ...

	return spParent;
}	

Node* Rtn::getSPParentCH (Node* cNode) // (2014.08.16) �θ� CH ��� ��ȯ
{
	Node* spParent = NULL; // ���� ����� BS���� �ִ� ��� ���� �θ� ���

	// .. �� ��带 ���� �θ� ��� ��Ͽ� ����� �ּ� �� ù ��°�� �ִ� ��� ���� �θ� ��忡 �ش��Ѵٰ� ����

	list<Node*> lst = parentCHs[cNode->id]; // ���� ����� �θ� CH ��� ��� ��ȯ

	if (!lst.empty() )
	{
		spParent = lst.front(); // �θ� CH ����� ù ��° ��� ��ȯ (Node*)
	}

	// ...

	return spParent;
}	

void Rtn::setKmr( Kmr* kmr)
{
	this->kmr = kmr;
}

void Rtn::setFuzzy (Fuzzy* fuzzy) // fuzzy ��ü ������� ����
{
	this->fuzzy = fuzzy;
}

void Rtn::setRecipent (struct _tagKeyMsg* keyMsg, Node* next) // ���õ� ���� ��带 ���� ���޵� ���� ��� ����� �޽����� ���
{
	// ���� ��� ��� ȹ��

	int nextNodeID = next->id;
	IDSet recipents = next_recipent_Map[nextNodeID];
	keyMsg->recipents = recipents;
}

void Rtn::resetUpdatdClstr() // (2014.07.15) ������ Ŭ������ ������Ʈ ���θ� �ǹ��ϴ� �÷��׸� false�� ����
{
	std::map<int, bool>::iterator it;

	int nodeID;	

	for (it = this->clstrUpdated.begin(); it != clstrUpdated.end(); it++) // ������ Ŭ������ ���� ���θ� false �� ���� - Ű ��й��� ����
	{
		nodeID = (*it).first;
		clstrUpdated[nodeID] = false;
	} // for�� ���� - ������ Ŭ������ ���� ���θ� false �� ����
}

/*
double Rtn::eval(Node* recipent, Node* candi)
{
	double res = 0;

	res = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	

	return res;
}
*/


void Rtn::resetTopology()
{
	bs->resetNbrLst();
	//ndsClstrMap.clear(); // node-cluster map
	//this->clusters.clear(); // cluster - node map
	nbrLsts.clear(); // node - neighbor nodes map
	//parentsLsts.clear(); // node - parent nodes map

	parentsNodes.clear(); // node - parent nodes map
	parentCHs.clear(); //node - parent CHs map

	map<int, Node*>::iterator it_node;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
	{
		int nodeID = (*it_node).first;

		list<Node*> lst;

		nbrLsts[nodeID] = lst;
		parentsNodes[nodeID] = lst;
		parentCHs[nodeID] = lst;

		/*
		nbrLsts[n] = lst;
		parentsNodes[n] = lst;
		parentCHs[n] = lst;
		*/
		set<int> nodeIDSet;		
		//clusters[n] = nodeIDSet;
	}
}

void Rtn::genTREQMsg (int round) // bs���� TREQ �޽��� ����
{
	this->treqMsg->round = round;
	this->treqMsg->senderID = -1; // BS�� �ǹ��ϴ� senderID
}

struct _tagTREQ* Rtn::getTREQMsg() // TREQ �޽��� ȹ��	
{
	return this->treqMsg;
}


void Rtn::updatTREQMsg (Node* cNode) // �߰� ��忡�� TREQ �޽��� �ʵ� ������Ʈ
{
	treqMsg->senderID = cNode->id;
}

void Rtn::genTRESMsg (Node* cNode)
{
	int cNodeID = cNode->id;
	tresMsg->srcID = cNodeID;

	list<Node*> nbrLst = this->nbrLsts[cNodeID];

	tresMsg->nbrLst = nbrLst;
}

struct _tagTRES* Rtn::getTRESMsg() // TREQ �޽��� ȹ��	
{
	return this->tresMsg;
}


void Rtn::handleTREQ(struct _tagTREQ* treq, Node* cNode) // TREQ �޽��� ���� �� �ִ� ��� �θ� ��� ����, �̿� ��� ��� �߰�
{
	//  �ִ� ��� �θ� ��� ����
	int currentRound = treq->round;

	if ( currentRound > cNode->round) // ���� ���忡�� ���ʷ� ���ŵ� TREQ�� ���
	{
		cNode->round = currentRound;

		if (cNode->distance > TRANS_RANGE) // BS���� ���� ���� �ۿ� �ִ� ����� ��� �θ� ��� ���� 
		{
			int senderID = treq->senderID;
			//addParent(cNode, nodes[senderID]);
			addParentNode(cNode, nodes[senderID]); // (2014.08.17) �θ� ��� �߰�
		}
	}

	// �̿� ��� ��Ͽ� �۽� ��� ID �߰�

	int senderID = treq->senderID;
	Node* sender = nodes[senderID];
	int cNodeID= cNode->id;

	this->nbrLsts[cNodeID].push_back( sender);
}


int Rtn::getTRESMsgLen(struct _tagTRES* tres)
{
	int msgLen = 2 * ID_LEN; // source node ID size (2byte) + receiver node ID size (2bytes)

	list<Node*> nbrLst = tres->nbrLst;

	int cnt = 0;

	list<Node*>::iterator it;

	for (it = nbrLst.begin(); it != nbrLst.end(); it++)
	{
		Node* pNode = (*it);

		if (pNode != NULL)
		{
			cnt++;
		}
	}

	msgLen += cnt * ID_LEN; // �̿� ��� ��Ͽ� ���Ե� ID ����� ũ�� ���Ͽ� ��ȯ

	return msgLen; 
}


void Rtn::resetNxtNds()
{
	nxtNds.clear(); // ���� ���� ��� ��� ����
}
/*
void Rtn::quicksort(Node* candis[], int start, int finish)
{
	if (start < finish)
	{
		int q = partition( candis, start, finish); // �迭 ������

		quicksort(candis, start, q-1); // ���� ���� ����
		quicksort(candis, q+1, finish); // ���� ���� ����
	}
}
*/
/*
int Rtn::partition(Node* candis[], int start, int finish)
{
	double val1 = 0;  // j��° ����� eval_result
	double val2= 0;	// finish ��ġ�� �ִ� �����  eval_result

	int i = start - 1;

	int j = 0;

	for ( j = start; j < finish; j++)
	{
		
		val1 = eval_result[j];		
		val2 = eval_result[finish];
		

		if (val1< val2)
		{
			i = i +1;

			Node* temp = candis[i];
			candis[i] = candis[j];
			candis[j] = temp;

			double t = eval_result[i];
			eval_result[i] = eval_result[j];
			eval_result[j] = t;

		}		
	}

	Node* temp = candis[i+1];
	candis[i+1] = candis[finish];
	candis[finish] = temp;

	double t = eval_result[i+1];
	eval_result[i+1] = eval_result[finish];
	eval_result[finish] = t;

	return (i+1);
}
*/
void Rtn::quicksort( int start, int finish)
//void Rtn::quicksort( map<int, Node*> candis, int start, int finish)
{
	if (start < finish)
	{
		int q = partition( start, finish); // �迭 ������
		//int q = partition( candis, start, finish); // �迭 ������

		quicksort(start, q-1); // ���� ���� ����
		quicksort(q+1, finish); // ���� ���� ����
		//quicksort(candis, start, q-1); // ���� ���� ����
		//quicksort(candis, q+1, finish); // ���� ���� ����
	}
}

int Rtn::partition( int start, int finish)
//int Rtn::partition( map<int, Node*> candis, int start, int finish)
{
	double val1 = 0;  // j��° ����� eval_result
	double val2= 0;	// finish ��ġ�� �ִ� �����  eval_result

	int i = start - 1;

	int j = 0;

	for ( j = start; j < finish; j++)
	{
		
		val1 = eval_result[j].second;		
		val2 = eval_result[finish].second;
		

		if (val1< val2)
		{
			i = i +1;

			Node* temp = candis[i];
			candis[i] = candis[j];
			candis[j] = temp;

			pair<int, double> t = eval_result[i];
			//double t = eval_result[i].second;
			eval_result[i] = eval_result[j];
			eval_result[j] = t;

		}		
	}

	Node* temp = candis[i+1];
	candis[i+1] = candis[finish];
	candis[finish] = temp;

	pair<int, double> t = eval_result[i+1];
	//double t = eval_result[i+1];
	eval_result[i+1] = eval_result[finish];
	eval_result[finish] = t;

	return (i+1);
}


void Rtn::testNbrs() // �� ����� �̿� ��� ���� ���Ͽ� ���
{

}
	
void Rtn::testCHs() // �� Ŭ������ CH ���� ���Ͽ� ���
{
	ofstream out ("tst_CHS.txt");

	out << " clusters: " << endl;

	map<int, set<int>>::iterator it;

	int numNodesInClstr = 0;

	for (it = clusters.begin(); it != clusters.end(); it++)
	{
		if ( !(*it).second.empty())
		{
			numNodesInClstr += (*it).second.size();

			int CHID = (*it).first;

			if (nodes[CHID] == NULL) // (2014.09.21) ���� - CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
			{
				continue;
			}

			set<int> clusternodes = (*it).second;

			out << " CH's ID: " << CHID;

			out << "(" << nodes[CHID]->x << ", " << nodes[CHID]->y << ") " ;

			out << " { ";

			set<int>::iterator it2;

			for (it2 = clusternodes.begin(); it2!=clusternodes.end(); it2++) // �� Ŭ������ �� �Ҽ� ��� ���
			{
				int nodeID = *it2;
				out << nodeID << ", ";
			} // for�� ���� - �� Ŭ�����Ϳ� ���� �Ҽ� ��� ID ��� ���

			out << " } " ;

			// �� CH�� BS������ �Ÿ� ���
			out << ", DIST: " << nodes[CHID]->distance << ", ";
		

			// �� CH�� �ĺ� �θ� ��� ��� ���

			out << " parents: " ;

			list<Node*>::iterator it_parent;

			for ( it_parent = parentCHs[CHID].begin(); it_parent != parentCHs[CHID].end(); it_parent++) // �� CH�� �ĺ� �θ� ��� ��� ���
			{
				Node* parent = *it_parent;
				
				if (parent !=NULL)
				{
					out << parent->id << ", ";
				}
			} // for�� ���� - �� CH�� �ĺ� �θ� ��� ��� ��� �Ϸ�

			out << endl;
		}		
	}

	out << "Ŭ������ �Ҽ� ��ü ��� ��: " << numNodesInClstr << endl;

	out.close();
}	

void Rtn::testParentNodes() // �ĺ� �θ� ��� ���� �׽�Ʈ
{
	ofstream out("tst_parentNode.txt");

	map<int, Node*>::iterator it_node;
	list<Node*>::iterator it_parent;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // ��� ��� ��ȸ - �θ� ��� ���� ���
	{
		Node* cNode = (*it_node).second;

		if (cNode == NULL) // (2014.09.22) ��尡 NULL�̸� ���� ���� �̵�
		{
			continue;
		}

		list<Node*> parentNodes = this->getParentNodes(cNode); // �θ� ��� ��� ȹ��

		if (cNode != NULL)
		{
			out << "node " << cNode->id << ": ";
		}
		for(it_parent = parentNodes.begin(); it_parent != parentNodes.end(); it_parent++ ) // �θ� ��� ��� ��ȸ
		{
			Node* parent = *it_parent;

			if (parent != NULL)
			{
				out << ", " << parent->id;
			}
		} // for�� - ���� ����� �θ� ��� ���� ���
		
		out << endl;
	} // for�� - ��� ����� �θ� ��� ���� ���


}

void Rtn::testParentCHs() // �ĺ� �θ� ��� ���� �׽�Ʈ
{
	ofstream out("tst_parentCH.txt");

	map<int, Node*>::iterator it_node;
	list<Node*>::iterator it_parent;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // ��� ��� ��ȸ - �θ� ��� ���� ���
	{
		Node* cNode = (*it_node).second;

		if (cNode ==NULL) // (2014.09.22) ��� �����Ͱ� NULL�̸� ���� ���� �̵�
		{
			continue; //
		}

		list<Node*> parentCHs = this->getParentCHs(cNode); // �θ� ��� ��� ȹ��

		if (cNode != NULL)
		{
			out << "node " << cNode->id << ": ";
		}
		for(it_parent = parentCHs.begin(); it_parent != parentCHs.end(); it_parent++ ) // �θ� ��� ��� ��ȸ
		{
			Node* parentCH = *it_parent;

			if (parentCH != NULL)
			{
				out << ", " << parentCH->id;
			}
		} // for�� - ���� ����� �θ� CH��� ���� ���
		
		out << endl;
	} // for�� - ��� ����� �θ� ��� ���� ���

}
/*
double Rtn::testConect()
{
	
	map<int, set<int>>::iterator it;

 	int numNodesInClstr = 0;
	int numIsolatedCHs = 0; // BS���� ������� ���� ch

	int CHID;

	Node* tempNode;

	double res; // �� CH�� �� BS�κ��� ������ CH���� ����


	for (it = clusters.begin(); it != clusters.end(); it++) // Ŭ������ ��ȸ - �� CH�κ��� BS������ ��ΰ� ����Ǿ� �ִ��� �˻�
	{
		tempNode = NULL;

		if ( !(*it).second.empty())
		{
			numNodesInClstr += (*it).second.size();

			if (numNodesInClstr ==0)
			{
				continue;
			}

			CHID = (*it).first;

			if ( nodes[CHID] == NULL) // (2014.09.21) ���� - CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
			{
				continue; 
			}

			// CH-BS ������ ��� ����

			tempNode = nodes[CHID];

			while (tempNode->distance >= TRANS_RANGE) // �ִ� ��θ� ���� BS �������� �̵�
			{
				tempNode = this->getSPParentNode(tempNode); // SEF ������� ���
				//tempNode = this->getSPParentCH(tempNode);	// ������ ����� ���������� ���			

				if (tempNode == NULL)
				{
					break;
				}

			} // �ִ� ��θ� ���� BS �������� �̵�

			if ( tempNode == NULL) // ���� ��尡 bs�κ��� ���� ���� ����getSPParentCH ���, �ش� CH�� BS�κ��� ����
			{
				numIsolatedCHs++;
			} // �� ��� �� ����
		}

	} // for�� ��� CH�� ���� ���� �˻�

	res =  ((double) numIsolatedCHs)/ clusters.size();

	return res;
}
*/
Rtn::Rtn()
{
	// TREQ, TRES �޽��� ��ü ����

	this->treqMsg = new TREQ;
	this->tresMsg = new TRES;

	treqMsg->round = 0;
	treqMsg->senderID = -1;

	tresMsg->srcID = -1;

	numEvntRcvd = 0;
}

Rtn::~Rtn()
{
	free (treqMsg);
	free (tresMsg);
}


/*
void Rtn::findNbrs()
{
	

	int nbrIdx = 0; // �̿� ��� �ε���	

	int i = 0, j = 0;

	for(i=0; i < NUM_CHS;i++) 
	{
		Node* CH1= getCH(i);

		for(j=0; j<NUM_CHS;j++)
		{
			if (j!=i)
			{
				
				Node* CH2 = getCH(j);

				if(  pow((double)(CH1->x-CH2->x),2) + pow((double)(CH1->y-CH2->y),2) < pow((double)TRANS_RANGE,2))
				{
					if (nbrIdx < NBRS_PER_NODE)
					{
						nbrs[CH1->id][nbrIdx++] = CH2; // �̿� ��� ���� ����

					}
				}

			}
		}

		numNbrs[CH1->id] = nbrIdx; // (2013.08.13) �� ����� �̿� ��� �� ����
		nbrIdx = 0;

	}

	
}



void Rtn::setNodes(Node nodes[])
{
	
	this->nodes = nodes; // �迭�� ���� �ּ� ����

}




void Rtn::resetNxtNds()
{
	for (int i = 0; i < MAX_NXT_NODES; i++) // (2013.09.08 ����) index�� ���� ���� (�ִ� ���� ��� ��)
	{
		this->nxtNds[i] = NULL; // ���� ��� ��� �� �ʱ�ȭ
	}
}

void Rtn::addParent (Node* cNode, Node* prevNode)
{
	int cNodeID = cNode->id;

	parents[cNodeID].push_back( prevNode); // �θ� ��� ����

	numParents[cNodeID]++; // (2013.11.27) �ĺ� ��� �� +1 ����
}

std::list<Node*> Rtn::getParents(Node* cNode) // (2013.10.11 �߰�) �θ� ��� ��� ��ȯ
{
	int cNodeID = cNode->id;

	return parents[cNodeID]; 
}

Node* Rtn::getSPParent (Node* cNode) // (2014.03.24 ����) �ִ� ��� ���� �ĺ� �θ� ��� ��ȯ
{
	int cNodeID = cNode->id;

	Node* parent = NULL;

	if (!parents[cNodeID].empty())
	{
		list<Node*>::iterator it= parents[cNodeID].begin();
		parent = *it;		
	}

	return parent;

	//return parents[cNodeID]; // �θ� ��� ��ȯ
}



void Rtn::addCandi (Node* cNode, Node* candi) // ���� ����� �ĺ� ��� ��Ͽ� ���ο� �ĺ� ��� �߰�
{

}

void Rtn::setKmr( Kmr* kmr)
{
	this->kmr = kmr;
}

void Rtn::setSec (Sec *sec)
{
	this->sec = sec;
}

Node* Rtn::getCH (int clstrID)
{
	return clstr_hds[clstrID];	
}

void Rtn::testNbrs()
{
	ofstream out;

	try
	{
		out.open("tst_Rtn_Nbrs.txt");
	}
	catch (exception & e)
	{
		cout << "���� �߻�: Rtn::testNbrs() : " << endl;
		cout << e.what() << endl;

	}

	int i = 0, j = 0;	

	for ( i=0; i < NUM_NODES; i++)
	{
		out << "<node " << i << " > ";
		out << " (x, y) = (" << nodes[i].x << ", " << nodes[i].y << ") ";	
		out << " dist = " << sqrt (nodes[i].distance) << "m,";

		for ( j=0; j < NBRS_PER_NODE; j++) // �̿� ��� ��� ���
		{
			Node* nbr = nbrs[i][j];
			if (nbr != NULL)
			{
				out << "\n nbr " << j << ": ID = " << nbr->id;
				out << " (x, y) = (" << nbr->x << ", " << nbr->y << ") ";	
				out << " dist = " << sqrt (nbr->distance) << "m\n";
				out << endl;
			}
			
		}
	}

	out.close();
}


void Rtn::testCHs()
{
	ofstream out("tst_Rtn_CH.txt");

	for (int i = 0; i < NUM_CHS; i++)
	{
		out << "\nCH " << i << endl;
		out << " ID = " << clstr_hds[i]->id << " ";
		out << " (x, y) = (" << clstr_hds[i]->x << ", " << clstr_hds[i]->y << ") ";	
		out << " dist = " << sqrt (clstr_hds[i]->distance) << "m\n";
		out << endl;
	}

	out.close();
}

void Rtn::testCandis()
{
	ofstream out("tst_Rtn_candis.txt");

	int i = 0, j = 0;	
	int holes = 0; // �ĺ� ��尡 ���� ����� ��

	int CH_ID=0;

	for ( i=0; i < NUM_CHS; i++)
	{
		CH_ID = clstr_hds[i]->id;

		out << "<CH " << i << " >: node " << CH_ID << ", distance: " << sqrt (nodes[CH_ID].distance) << "m" ;

		if (numCandis[CH_ID] == 0)
		{
			holes++;
			out << " <- hole! ";
		}

		out << endl;

		for ( j=0; j < numCandis[CH_ID]; j++) // �̿� ��� ��� ���
		{
			Node* candi = candis[CH_ID][j];
			if (candi != NULL)
			{
				out << "\n candi " << j << ": ID = " << candi->id;
				out << " (x, y) = (" << candi->x << ", " << candi->y << ") ";	
				out << " dist = " << sqrt (candi->distance) << "m\n";
				out << endl;
			}
			
		}
	}

	out << "hole�� ����: " << holes << endl;

	out.close();


	
}

bool Rtn::isInternal( Node* CH, Node* intermediate) // (���� ���� ���� ���) BS- CH ������ ������ �߰� ��� (intermediate)�� ���ԵǴ� �� ���θ� ��ȯ
{
	bool ret = false;

	// (2013.11.13 ����)

	double opt_dist = sqrt(CH->distance); // CH���� BS������ �̻����� ���� ����� ����
	double temp_dist = sqrt(intermediate->distance);  // BS���� �߰� �������� �Ÿ� L ���
	double temp_dist2= sqrt(pow((double) CH->x - (double)  intermediate->x, 2) + pow((double) CH->y - (double) intermediate->y, 2));   // �߰� ��忡�� CH������ �Ÿ� L' ���
						 
	double thr_dist =  sqrt ( pow((double) opt_dist/2  , 2) + pow ((double) CLSTR_HEIGHT ,2) ) * 2; // (2013.11.3 ����)
	//double thr_dist = 2 * opt_dist;
	//double thr_dist = (pow ( opt_dist/2, 2) + pow ( PLM_THRESHOLD, 2)) * 4; // ���� ��忡 ���Խ�ų ���� �����ϱ� ���� �Ӱ谪 (�Ÿ� ����)

	if ( (temp_dist + temp_dist2) <= thr_dist)  // �� �Ÿ��� �� (L +L')�� �Ӱ谪 ��, ���� �ۿ� ���� ���� SKIP, ���� ��� �˻�
	{
		ret = true; // ���� �ۿ� ���� ���� SKIP, ���� ��� �˻�
	}				
	else
		ret = false;
					  
	// (2013.11.13 ���� �Ϸ�)

	return ret;
}

Rtn::Rtn()
{
	// ��� ���� �� �ʱ�ȭ

	nodes = NULL;

	int i = 0, j = 0;

	for ( i = 0; i < NUM_NODES; i++)
	{				
		//parents[i] = NULL;

		for ( j = 0; j < NBRS_PER_NODE; j++)
		{
			nbrs[i][j] = NULL;
		}

		for ( j = 0; j < CANDIS_PER_NODE; j++)
		{
			candis[i][j] = NULL;
		}

		this->numParents[i] = 0; // (2013.11.27 �߰�)
		this->numCandis[i] = 0;

	}

	for ( i = 0; i < CANDIS_PER_NODE; i++)
	{
		eval_result[i] = -1000;
	}

	for ( i = 0; i < NUM_CHS; i++)
	{
		clstr_hds[i] = NULL;		
	}

	for ( i = 0; i < BRANCH_FACTOR; i++)
	{
		nxtNds[i] = NULL;
	}

	seed1 = 1;

	gen1.randomize(&(seed1)); 

	numEvntRcvd = 0;

}

void Rtn::quicksort(Node* candis[], int start, int finish)
{
	if (start < finish)
	{
		int q = partition( candis, start, finish); // �迭 ������

		quicksort(candis, start, q-1); // ���� ���� ����
		quicksort(candis, q+1, finish); // ���� ���� ����
	}
}

int Rtn::partition(Node* candis[], int start, int finish)
{
	double val1 = 0;  // j��° ����� eval_result
	double val2= 0;	// finish ��ġ�� �ִ� �����  eval_result

	int i = start - 1;

	int j = 0;

	for ( j = start; j < finish; j++)
	{
		
		val1 = eval_result[j];		
		val2 = eval_result[finish];
		

		if (val1< val2)
		{
			i = i +1;

			Node* temp = candis[i];
			candis[i] = candis[j];
			candis[j] = temp;

			double t = eval_result[i];
			eval_result[i] = eval_result[j];
			eval_result[j] = t;

		}		
	}

	Node* temp = candis[i+1];
	candis[i+1] = candis[finish];
	candis[finish] = temp;

	double t = eval_result[i+1];
	eval_result[i+1] = eval_result[finish];
	eval_result[finish] = t;

	return (i+1);
}


Rtn::~Rtn()
{

}*/