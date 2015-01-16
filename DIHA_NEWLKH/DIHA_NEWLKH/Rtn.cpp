

// 2014.05.23
// 문수영
// Rtn.cpp
// 라우팅 클래스 구현


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
	// 함수 정의부 작성 필요
}

Node* Rtn::getCH (int clstrID)
{
	Node* CH = NULL;

	// ...

	return CH;
}

void Rtn::changeCluster (int nodeID, int clstrID) // 주어진 노드의 클러스터를 변경 - nodeID: 노드 ID, clstrID: 클러스터 id
{
	

	this->clstrUpdated[nodeID] = false;

	int oldCluster = getClstrID(nodeID); // (2014.08.04) 기존 클러스터가 없으면 -1이 반환되도록 getClstrID 함수 호출

	//int oldCluster = ndsClstrMap[nodeID];

	if (oldCluster != clstrID)
	{
		clstrUpdated[nodeID] = true;
	}

	ndsClstrMap[nodeID] = clstrID; // 노드의 클러스터 ID  (=CH's ID) 변경

	//if ( nodes[nodeID]->newbee == false) // 이전 라운드부터 존재했던 노드의 경우, 구 클러스터에서 해당 노드 ID 정보 제거
	{
		set<int>::iterator it;
	
		it = clusters[oldCluster].find(nodeID);	

		if (it != clusters[oldCluster].end() )
		{	
			clusters[oldCluster].erase(it); // 구 클러스터에서 노드 ID 제거
		}
	}

	
	clusters[clstrID].insert(nodeID); 	// 신규 클러스터에 노드 id 추가

}

void Rtn::changeCluster (int nodeID, int clstrID, int CH_X, int CH_Y) // (2014.08.04) 주어진 노드의 클러스터를 변경 - nodeID: 노드 ID, clstrID: 클러스터 id, CH_X: 신규 CH의 X 좌표, CH_Y: 신규 CH의 Y 좌표
{
	double dist;

	this->clstrUpdated[nodeID] = false;

	int oldCluster = getClstrID(nodeID); // (2014.08.04) 기존 클러스터가 없으면 -1이 반환되도록 getClstrID 함수 호출

	//int oldCluster = ndsClstrMap[nodeID];	

	//if ( nodes[nodeID]->newbee == false) // 이전 라운드부터 존재했던 노드의 경우, 구 클러스터에서 해당 노드 ID 정보 제거
	{
		set<int>::iterator it;
	
		it = clusters[oldCluster].find(nodeID);	

		if (it != clusters[oldCluster].end() )
		{	
			clusters[oldCluster].erase(it); // 구 클러스터에서 노드 ID 제거
		}
	}

	dist = sqrt (pow ( (double) nodes[nodeID]->x - CH_X, 2) + pow ( (double) nodes[nodeID]->y - CH_Y, 2));// 현재 노드와 CH와의 거리 계산

	if (dist < TRANS_RANGE) // (2014.08.04) 신규 클러스터의 CH에서 전송 범위 이내이면 해당 클러스터에 노드 id 추가
	{
		if (oldCluster != clstrID)
		{
			clstrUpdated[nodeID] = true; // 신규 클러스터에 실제로 추가되는 경우에만 갱신 여부를 참으로 설정
		}

		ndsClstrMap[nodeID] = clstrID; // 노드의 클러스터 ID  (=CH's ID) 변경
		clusters[clstrID].insert(nodeID); 	// 신규 클러스터에 노드 id 추가
	}
}

void Rtn::removeFromCluster (int nodeID) // (2014.07.14) 네트워크에서 제거된 노드를 클러스터에서 제거하는 함수
{
	int oldCluster = ndsClstrMap[nodeID];

	if (nodeID == 1251)
	{
		cout << "nodeID == 1251" << endl;
	}

	ndsClstrMap.erase(nodeID); // node-cluster map에서 삭제

	set<int>::iterator it;

	it = clusters[oldCluster].find(nodeID);	

	if (it != clusters[oldCluster].end() )
	{	
		clusters[oldCluster].erase(it); // 구 클러스터에서 노드 ID 제거
	}

	// 클러스터 갱신 여부를 저장하는 clstrUpdated 맵에서 해당 노드 정보 제거

	//std::map<int, bool>::iterator it_nd;	

	clstrUpdated.erase(nodeID);

	/*for (it_nd = this->clstrUpdated.begin(); it_nd != clstrUpdated.end(); it_nd++) // 노드들의 클러스터 갱신 여부를 false 로 리셋 - 키 재분배의 목적
	{
		if (nodeID == (*it_nd).first)
		{
			clstrUpdated.erase(nodeID);
		}
	} // for문 종료 - 노드들의 클러스터 갱신 여부를 false 로 리셋
	*/
}

void Rtn::removeCH (int CHID)
{
	set<int>::iterator it;

	for (it = clusters[CHID].begin(); it != clusters[CHID].end(); it++) // 소속 노드들의 클러스터 정보 리셋
	{
		int nID = *it;

		ndsClstrMap[nID] = -1; // 기존 클러스터에 속해 있던 노드들의 클러스터 정보를 리셋
	} // 클러스터 정보 리셋 완료


	clusters.erase(CHID); // 클러스터 map에서 해당 CH와 소속 노드들로 구성된 요소를 제거
}

bool Rtn::hasUpdatedCH (int nodeID)// (2014.05.19) 주어진 nodeID에 대응되는 노드의 클러스터 (CH' ID)가 변경되었는지 검사
{
	if ( this->clstrUpdated[nodeID] == true)
	{
		return true;
	}
	else
		return false;
}

void Rtn::arrangeClusters() // (2014.08.11) 클러스터 목록 중 크기가 0인 클러스터 정보를 제거 
{
	map<int, set<int>>::iterator it_cluster;
	set<int> cluster;

	set<int> clstr_toBeDltd; // 제거될 클러스터 목록
	set<int>::iterator it_chID;

	for (it_cluster = this->clusters.begin(); it_cluster != this->clusters.end(); it_cluster++) // 크기가 0인 클러스터 정보 저장
	{
		cluster = (*it_cluster).second;

		if (cluster.empty())
		{
			clstr_toBeDltd.insert( (*it_cluster).first);
		}

		// (2014.09.21 수정) CH가 이미 제거된 노드이면 해당 클러스터 정보 제거
		int CHID = (*it_cluster).first;

		if (nodes[CHID] == NULL)
		{
			clstr_toBeDltd.insert( (*it_cluster).first);
		}

	} // for문 종료 - 제거될 클러스터 목록 저장 완료

	for (it_chID = clstr_toBeDltd.begin(); it_chID != clstr_toBeDltd.end(); it_chID++) // 크기가 0인 클러스터 정보 제거
	{
		int CHID = *it_chID;
		this->clusters.erase(CHID);
	} // for문 종료 - 클러스터 목록 정리 완료
}

set<int> Rtn::getClstr (int clstrID) // (2014.05.21) 입력: 클러스터 ID (ch's ID), 출력: 클러스터 소속 노드들의 iD 집합
{
	return clusters[clstrID];
}

map<int, set<int>> Rtn::getClusters()
{
	return this->clusters;
}

int Rtn::getClstrID (int nodeID) // (2014.04.10) 입력: 노드 id, 출력: 클러스터 ID (= CH's ID)
{
	int clusterID = -1;

	std::map<int, int>::iterator it;

	it = ndsClstrMap.find(nodeID);

	if (it != ndsClstrMap.end() ) // 주어진 노드 ID가 노드-클러스터 맵에 존재하는 경우,
	{
		clusterID = it->second; // 해당 노드 ID에 대응되는 클러스터 id 저장
	}

	return clusterID;
}

void Rtn::addParentNode (Node* cNode, Node* prevNode) // (2013.10.11 추가) 부모 노드 추가 (한 노드가 여러 개의 부모 노드를 가질 수 있음)	
{
	int cNodeID = cNode->id;
	
	//parentsLsts[cNodeID].push_back(prevNode);
	parentsNodes[cNodeID].push_back(prevNode);
}

void Rtn::addParentCH (Node* cNode, Node* parentCH) // // (2014.08.14) 부모 CH 노드 추가
{
	int cNodeID = cNode->id;
	
	parentCHs[cNodeID].push_back(parentCH);
}

std::list<Node*> Rtn::getParentNodes(Node* cNode) // // (2013.10.11 추가) 부모 노드 리스트 반환
{
	int cNodeID = cNode->id;

	return parentsNodes.at(cNodeID);
}

std::list<Node*> Rtn::getParentCHs(Node* cNode) // (2014.08.14 추가) 부모 CH 노드 리스트 반환
{
	int cNodeID = cNode->id;

	return parentCHs.at(cNodeID);
}

Node* Rtn::getSPParentNode (Node* cNode) // (2013.09.09) 부모 노드 반환
{
	Node* spParent = NULL; // 현재 노드의 BS까지 최단 경로 상의 부모 노드

	// .. 각 노드를 위한 부모 노드 목록에 저장된 주소 중 첫 번째가 최단 경로 상의 부모 노드에 해당한다고 가정

	list<Node*> lst = parentsNodes[cNode->id]; // 현재 노드의 부모 노드 목록 반환

	if (!lst.empty() )
	{
		spParent = lst.front(); // 부모 노드 목록의 첫 번째 요소 반환 (Node*)
	}

	// ...

	return spParent;
}	

Node* Rtn::getSPParentCH (Node* cNode) // (2014.08.16) 부모 CH 노드 반환
{
	Node* spParent = NULL; // 현재 노드의 BS까지 최단 경로 상의 부모 노드

	// .. 각 노드를 위한 부모 노드 목록에 저장된 주소 중 첫 번째가 최단 경로 상의 부모 노드에 해당한다고 가정

	list<Node*> lst = parentCHs[cNode->id]; // 현재 노드의 부모 CH 노드 목록 반환

	if (!lst.empty() )
	{
		spParent = lst.front(); // 부모 CH 목록의 첫 번째 요소 반환 (Node*)
	}

	// ...

	return spParent;
}	

void Rtn::setKmr( Kmr* kmr)
{
	this->kmr = kmr;
}

void Rtn::setFuzzy (Fuzzy* fuzzy) // fuzzy 객체 멤버변수 설정
{
	this->fuzzy = fuzzy;
}

void Rtn::setRecipent (struct _tagKeyMsg* keyMsg, Node* next) // 선택된 다음 노드를 통해 전달될 목적 노드 목록을 메시지에 기록
{
	// 목적 노드 목록 획득

	int nextNodeID = next->id;
	IDSet recipents = next_recipent_Map[nextNodeID];
	keyMsg->recipents = recipents;
}

void Rtn::resetUpdatdClstr() // (2014.07.15) 노드들의 클러스터 업데이트 여부를 의미하는 플래그를 false로 리셋
{
	std::map<int, bool>::iterator it;

	int nodeID;	

	for (it = this->clstrUpdated.begin(); it != clstrUpdated.end(); it++) // 노드들의 클러스터 갱신 여부를 false 로 리셋 - 키 재분배의 목적
	{
		nodeID = (*it).first;
		clstrUpdated[nodeID] = false;
	} // for문 종료 - 노드들의 클러스터 갱신 여부를 false 로 리셋
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

void Rtn::genTREQMsg (int round) // bs에서 TREQ 메시지 생성
{
	this->treqMsg->round = round;
	this->treqMsg->senderID = -1; // BS를 의미하는 senderID
}

struct _tagTREQ* Rtn::getTREQMsg() // TREQ 메시지 획득	
{
	return this->treqMsg;
}


void Rtn::updatTREQMsg (Node* cNode) // 중간 노드에서 TREQ 메시지 필드 업데이트
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

struct _tagTRES* Rtn::getTRESMsg() // TREQ 메시지 획득	
{
	return this->tresMsg;
}


void Rtn::handleTREQ(struct _tagTREQ* treq, Node* cNode) // TREQ 메시지 수신 시 최단 경로 부모 노드 설정, 이웃 노드 목록 추가
{
	//  최단 경로 부모 노드 설정
	int currentRound = treq->round;

	if ( currentRound > cNode->round) // 현재 라운드에서 최초로 수신된 TREQ인 경우
	{
		cNode->round = currentRound;

		if (cNode->distance > TRANS_RANGE) // BS에서 전송 범위 밖에 있는 노드의 경우 부모 노드 설정 
		{
			int senderID = treq->senderID;
			//addParent(cNode, nodes[senderID]);
			addParentNode(cNode, nodes[senderID]); // (2014.08.17) 부모 노드 추가
		}
	}

	// 이웃 노드 목록에 송신 노드 ID 추가

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

	msgLen += cnt * ID_LEN; // 이웃 노드 목록에 포함된 ID 목록의 크기 더하여 반환

	return msgLen; 
}


void Rtn::resetNxtNds()
{
	nxtNds.clear(); // 다음 전달 노드 목록 제거
}
/*
void Rtn::quicksort(Node* candis[], int start, int finish)
{
	if (start < finish)
	{
		int q = partition( candis, start, finish); // 배열 나누기

		quicksort(candis, start, q-1); // 좌측 분할 정렬
		quicksort(candis, q+1, finish); // 우측 분할 정렬
	}
}
*/
/*
int Rtn::partition(Node* candis[], int start, int finish)
{
	double val1 = 0;  // j번째 노드의 eval_result
	double val2= 0;	// finish 위치에 있는 노드의  eval_result

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
		int q = partition( start, finish); // 배열 나누기
		//int q = partition( candis, start, finish); // 배열 나누기

		quicksort(start, q-1); // 좌측 분할 정렬
		quicksort(q+1, finish); // 우측 분할 정렬
		//quicksort(candis, start, q-1); // 좌측 분할 정렬
		//quicksort(candis, q+1, finish); // 우측 분할 정렬
	}
}

int Rtn::partition( int start, int finish)
//int Rtn::partition( map<int, Node*> candis, int start, int finish)
{
	double val1 = 0;  // j번째 노드의 eval_result
	double val2= 0;	// finish 위치에 있는 노드의  eval_result

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


void Rtn::testNbrs() // 각 노드의 이웃 노드 정보 파일에 출력
{

}
	
void Rtn::testCHs() // 각 클러스터 CH 정보 파일에 출력
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

			if (nodes[CHID] == NULL) // (2014.09.21) 수정 - CH가 이미 제거된 노드이면 다음 클러스터로 이동
			{
				continue;
			}

			set<int> clusternodes = (*it).second;

			out << " CH's ID: " << CHID;

			out << "(" << nodes[CHID]->x << ", " << nodes[CHID]->y << ") " ;

			out << " { ";

			set<int>::iterator it2;

			for (it2 = clusternodes.begin(); it2!=clusternodes.end(); it2++) // 각 클러스터 내 소속 노드 출력
			{
				int nodeID = *it2;
				out << nodeID << ", ";
			} // for문 종료 - 각 클러스터에 속한 소속 노드 ID 목록 출력

			out << " } " ;

			// 각 CH의 BS까지의 거리 출력
			out << ", DIST: " << nodes[CHID]->distance << ", ";
		

			// 각 CH의 후보 부모 노드 목록 출력

			out << " parents: " ;

			list<Node*>::iterator it_parent;

			for ( it_parent = parentCHs[CHID].begin(); it_parent != parentCHs[CHID].end(); it_parent++) // 각 CH의 후보 부모 노드 목록 출력
			{
				Node* parent = *it_parent;
				
				if (parent !=NULL)
				{
					out << parent->id << ", ";
				}
			} // for문 종료 - 각 CH의 후보 부모 노드 목록 출력 완료

			out << endl;
		}		
	}

	out << "클러스터 소속 전체 노드 수: " << numNodesInClstr << endl;

	out.close();
}	

void Rtn::testParentNodes() // 후보 부모 노드 정보 테스트
{
	ofstream out("tst_parentNode.txt");

	map<int, Node*>::iterator it_node;
	list<Node*>::iterator it_parent;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // 노드 목록 순회 - 부모 노드 정보 출력
	{
		Node* cNode = (*it_node).second;

		if (cNode == NULL) // (2014.09.22) 노드가 NULL이면 다음 노드로 이동
		{
			continue;
		}

		list<Node*> parentNodes = this->getParentNodes(cNode); // 부모 노드 목록 획득

		if (cNode != NULL)
		{
			out << "node " << cNode->id << ": ";
		}
		for(it_parent = parentNodes.begin(); it_parent != parentNodes.end(); it_parent++ ) // 부모 노드 목록 순회
		{
			Node* parent = *it_parent;

			if (parent != NULL)
			{
				out << ", " << parent->id;
			}
		} // for문 - 현재 노드의 부모 노드 정보 출력
		
		out << endl;
	} // for문 - 모든 노드의 부모 노드 정보 출력


}

void Rtn::testParentCHs() // 후보 부모 노드 정보 테스트
{
	ofstream out("tst_parentCH.txt");

	map<int, Node*>::iterator it_node;
	list<Node*>::iterator it_parent;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // 노드 목록 순회 - 부모 노드 정보 출력
	{
		Node* cNode = (*it_node).second;

		if (cNode ==NULL) // (2014.09.22) 노드 포인터가 NULL이면 다음 노드로 이동
		{
			continue; //
		}

		list<Node*> parentCHs = this->getParentCHs(cNode); // 부모 노드 목록 획득

		if (cNode != NULL)
		{
			out << "node " << cNode->id << ": ";
		}
		for(it_parent = parentCHs.begin(); it_parent != parentCHs.end(); it_parent++ ) // 부모 노드 목록 순회
		{
			Node* parentCH = *it_parent;

			if (parentCH != NULL)
			{
				out << ", " << parentCH->id;
			}
		} // for문 - 현재 노드의 부모 CH노드 정보 출력
		
		out << endl;
	} // for문 - 모든 노드의 부모 노드 정보 출력

}
/*
double Rtn::testConect()
{
	
	map<int, set<int>>::iterator it;

 	int numNodesInClstr = 0;
	int numIsolatedCHs = 0; // BS까지 연결되지 못한 ch

	int CHID;

	Node* tempNode;

	double res; // 총 CH들 중 BS로부터 단절된 CH들의 비율


	for (it = clusters.begin(); it != clusters.end(); it++) // 클러스터 순회 - 각 CH로부터 BS까지의 경로가 연결되어 있는지 검사
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

			if ( nodes[CHID] == NULL) // (2014.09.21) 수정 - CH가 이미 제거된 노드이면 다음 클러스터로 이동
			{
				continue; 
			}

			// CH-BS 까지의 경로 조사

			tempNode = nodes[CHID];

			while (tempNode->distance >= TRANS_RANGE) // 최단 경로를 따라 BS 방향으로 이동
			{
				tempNode = this->getSPParentNode(tempNode); // SEF 라우팅의 경우
				//tempNode = this->getSPParentCH(tempNode);	// 나머지 라우팅 프로토콜의 경우			

				if (tempNode == NULL)
				{
					break;
				}

			} // 최단 경로를 따라 BS 방향으로 이동

			if ( tempNode == NULL) // 최종 노드가 bs로부터 전송 범위 밖인getSPParentCH 경우, 해당 CH는 BS로부터 고립됨
			{
				numIsolatedCHs++;
			} // 고립 노드 수 누적
		}

	} // for문 모든 CH의 연결 여부 검사

	res =  ((double) numIsolatedCHs)/ clusters.size();

	return res;
}
*/
Rtn::Rtn()
{
	// TREQ, TRES 메시지 객체 생성

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
	

	int nbrIdx = 0; // 이웃 노드 인덱스	

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
						nbrs[CH1->id][nbrIdx++] = CH2; // 이웃 노드 정보 저장

					}
				}

			}
		}

		numNbrs[CH1->id] = nbrIdx; // (2013.08.13) 각 노드의 이웃 노드 수 저장
		nbrIdx = 0;

	}

	
}



void Rtn::setNodes(Node nodes[])
{
	
	this->nodes = nodes; // 배열의 시작 주소 저장

}




void Rtn::resetNxtNds()
{
	for (int i = 0; i < MAX_NXT_NODES; i++) // (2013.09.08 수정) index의 범위 수정 (최대 다음 노드 수)
	{
		this->nxtNds[i] = NULL; // 다음 노드 목록 재 초기화
	}
}

void Rtn::addParent (Node* cNode, Node* prevNode)
{
	int cNodeID = cNode->id;

	parents[cNodeID].push_back( prevNode); // 부모 노드 설정

	numParents[cNodeID]++; // (2013.11.27) 후보 노드 수 +1 증가
}

std::list<Node*> Rtn::getParents(Node* cNode) // (2013.10.11 추가) 부모 노드 목록 반환
{
	int cNodeID = cNode->id;

	return parents[cNodeID]; 
}

Node* Rtn::getSPParent (Node* cNode) // (2014.03.24 수정) 최단 경로 상의 후보 부모 노드 반환
{
	int cNodeID = cNode->id;

	Node* parent = NULL;

	if (!parents[cNodeID].empty())
	{
		list<Node*>::iterator it= parents[cNodeID].begin();
		parent = *it;		
	}

	return parent;

	//return parents[cNodeID]; // 부모 노드 반환
}



void Rtn::addCandi (Node* cNode, Node* candi) // 현재 노드의 후보 노드 목록에 새로운 후보 노드 추가
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
		cout << "에러 발생: Rtn::testNbrs() : " << endl;
		cout << e.what() << endl;

	}

	int i = 0, j = 0;	

	for ( i=0; i < NUM_NODES; i++)
	{
		out << "<node " << i << " > ";
		out << " (x, y) = (" << nodes[i].x << ", " << nodes[i].y << ") ";	
		out << " dist = " << sqrt (nodes[i].distance) << "m,";

		for ( j=0; j < NBRS_PER_NODE; j++) // 이웃 노드 목록 출력
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
	int holes = 0; // 후보 노드가 없는 노드의 수

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

		for ( j=0; j < numCandis[CH_ID]; j++) // 이웃 노드 목록 출력
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

	out << "hole의 개수: " << holes << endl;

	out.close();


	
}

bool Rtn::isInternal( Node* CH, Node* intermediate) // (전달 영역 제한 기법) BS- CH 사이의 영역에 중간 노드 (intermediate)가 포함되는 지 여부를 반환
{
	bool ret = false;

	// (2013.11.13 수정)

	double opt_dist = sqrt(CH->distance); // CH에서 BS까지의 이상적인 최적 경로의 길이
	double temp_dist = sqrt(intermediate->distance);  // BS에서 중간 노드까지의 거리 L 계산
	double temp_dist2= sqrt(pow((double) CH->x - (double)  intermediate->x, 2) + pow((double) CH->y - (double) intermediate->y, 2));   // 중간 노드에서 CH까지의 거리 L' 계산
						 
	double thr_dist =  sqrt ( pow((double) opt_dist/2  , 2) + pow ((double) CLSTR_HEIGHT ,2) ) * 2; // (2013.11.3 수정)
	//double thr_dist = 2 * opt_dist;
	//double thr_dist = (pow ( opt_dist/2, 2) + pow ( PLM_THRESHOLD, 2)) * 4; // 연관 노드에 포함시킬 지를 결정하기 위한 임계값 (거리 기준)

	if ( (temp_dist + temp_dist2) <= thr_dist)  // 두 거리의 합 (L +L')과 임계값 비교, 영역 밖에 속한 노드는 SKIP, 다음 요소 검사
	{
		ret = true; // 영역 밖에 속한 노드는 SKIP, 다음 요소 검사
	}				
	else
		ret = false;
					  
	// (2013.11.13 수정 완료)

	return ret;
}

Rtn::Rtn()
{
	// 멤버 변수 값 초기화

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

		this->numParents[i] = 0; // (2013.11.27 추가)
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
		int q = partition( candis, start, finish); // 배열 나누기

		quicksort(candis, start, q-1); // 좌측 분할 정렬
		quicksort(candis, q+1, finish); // 우측 분할 정렬
	}
}

int Rtn::partition(Node* candis[], int start, int finish)
{
	double val1 = 0;  // j번째 노드의 eval_result
	double val2= 0;	// finish 위치에 있는 노드의  eval_result

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