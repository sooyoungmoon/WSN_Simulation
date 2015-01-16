
// 2014.07.31
// 문수영
// Kmr_IHA.cpp
// IHA의 키 매니저 클래스 선언


#include "Kmr_IHA.h"
#include "Sec_IHA.h"
#include <fstream>

using namespace std;

//int Node::numNodesEvctd;

void Kmr_IHA::updateKey(struct _tagKNode* kNode) // 그룹 키 갱신
{
	GroupKey* gk;

	gk = root->groupKey;

	gk->oldVal = gk->newVal;
	gk->newVal = gen1.genRandInt(Max_Key_Value); // 그룹 키 갱신
	
	//... root 키 갱신
}
/*
void Kmr_IHA::resetUpdatedKeys(struct _tagKNode* kNode) // (2014.07.31) key tree의 각 k-node 갱신 여부 재 초기화 
{

	// 현재 kNode의 updated, updated_addition, updated_eviction 플래그를 거짓 (false)로 리셋
	kNode->updated = false;
	kNode->updated_addition = false;
	kNode->updated_eviction = false;


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
*/
void Kmr_IHA::genKeyMsg() // 키 재분배 메시지 생성
{
	/* 1. key tree 상의 root 노드에 저장된 그룹 키 갱신 */
	//GroupKey oldKey;
	//GroupKey newKey;

	if (root == NULL)
	{
		cout << "root == NULL" << endl;
		exit(-1);
	}

	GroupKey* gk = root->groupKey;
	
	if (gk == NULL)
	{
		cout << "gk == NULL" << endl;
		exit(-1);
	}

	gk->oldVal = gk->newVal;
	gk->newVal = gen1.genRandInt(Max_Key_Value); // 그룹 키 갱신
	

	/* 2. 각 노드마다 해당 노드의 고유 키 (individual key)로 새로운 그룹 키를 암호화하여 키 갱신 메시지 구성 */

	KNode* knode = root->leftChild;

	if (knode == NULL)
	{
		cout << "kNode == NULL" << endl;
		exit(-1);
	}

	int cnt = 0;

	while (knode != NULL) // key tree 상의 모든 individual key 순회, 각각의 individual key로 새로운 그룹 키 암호화
	{
		UNode* unode = knode->unode;

		Node* pnode = unode->sensorNode;

		int nodeID = pnode->id;

		KeyMsg* keyMsg = new KeyMsg;	

		this->initKeyMsg(keyMsg);

		keyMsg->newKeys.push_back ( *gk);

		keyMsg->recipents.insert(nodeID);

		this->keyMsgs[cnt] = keyMsg;

		knode = knode->rightSibling;

		cnt++;
	}

	/*
	for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
	{
		KeyMsg* keyMsg = new KeyMsg;

	}
	*/
}

void Kmr_IHA::testKeyMsg() // 키 재분배 메시지 테스트
{
	ofstream out("tst_keyMsg_IHA.txt");

	out << " rekey message " << endl;

	for (int m = 0; m < keyMsgs.size(); m++)
	{
		KeyMsg* keyMsg = keyMsgs[m];

		out << "\nmsg. " << m << " : " << endl;

		list<GroupKey>::iterator it;

		for (it = keyMsg->newKeys.begin(); it != keyMsg->newKeys.end(); it ++)
		{
			int oldVal = (*it).oldVal;
			int newVal = (*it).newVal;

			out << " old key: " << oldVal <<" -> " ;
			out << " new key: " << newVal;
			out << endl;
		}

		out << endl;		
	}

	out.close();
}

int Kmr_IHA::getKeyMsgLen(struct _tagKeyMsg* keyMsg) // 키 메시지 길이 반환
{
	int len = 0;

	len += 2 +2; // sender / receiver ID size;

	// 키 길이 계산
	
	len += keyMsg->newKeys.size() * Group_Key_Size;

	// 목적 노드 ID 크기

	len += keyMsg->recipents.size() * ID_LEN;	
	
	return len;
}

bool Kmr_IHA::handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode)
{	
	bool res = true; // 전달 계속 여부 (true: 계속 전달, false: 전달 완료)
	bool isRecipent = false; // 현재 수신 노드가 키 재분배 메시지의 목적 노드 목록에 있는지 여부
	// 수신 노드의 ID가 목적 노드 목록에 있는 지 검사
	

	IDSet::iterator it;

	for ( it = keyMsg->recipents.begin(); it != keyMsg->recipents.end(); it++)
	{
		int recipentID = (*it);

		if ( cNode->id == recipentID)
		{
			isRecipent = true;
		}
	}

	if (isRecipent == true)
	{
		keyMsg->recipents.erase(cNode->id); // 현재 노드가 목적 노드 목록에 포함되어 있으면 해당 목록에서 현재 노드 ID 제거

		this->receivedNodes.insert(cNode->id); // (2014.08.07) 키 재분배 메시지 수신 노드 정보 저장 (debugging 용)

		if ( keyMsg->recipents.empty())
		{
			res = false;
		}
	} // IF문 종료 - 현재 노드가 목적 노드인 경우의 처리 완료

	//NodeIDLst recipents = keyMsg->recipents;
	/*
	for ( it = keyMsg->recipents.begin(); it != keyMsg->recipents.end(); it++)
	{
		int recipentID = (*it);

		if ( cNode->id == recipentID)
		{
			keyMsg->recipents.erase(it);
			//recipents.erase(it);

			if ( keyMsg->recipents.empty())
			{
				res = false;
				break;
			}
		}
	}
	
	if (res == false)
	{
		return res;
	}
	*/

	// 그룹 키 갱신 (키 트리에 있는 그룹 키의 내용대로 변경되었다고 가정)


	/// 클러스터 정보 갱신 (rtn)
	/*
	//NodeIDLst::iterator it_node;
	IDSet::iterator it_node;

	int cNodeID = cNode->id;
	int clusterID = keyMsg->CHID;

	rtn->changeCluster(cNodeID, clusterID); 	

	std::set<int> upperAssocnodeset;

	for (it_node = keyMsg->upAssocNds.begin(); it_node != keyMsg->upAssocNds.end(); it_node++)
	{
		int nodeID = (*it_node);

		upperAssocnodeset.insert(nodeID);
	}

	sec->updateAssocNodes(cNodeID, 0, INTERLEAVING_T+1, upperAssocnodeset); // 상위 연관 노드 정보 갱신 (iha, diha)
	//sec->updateAssocNodes(cNodeID, 0, upperAssocnodeset); // 상위 연관 노드 정보 갱신 (iha, diha)

	std::set<int> lowerAssocnodeset;

	for (it_node = keyMsg->downAssocNds.begin(); it_node != keyMsg->upAssocNds.end(); it_node++)
	{
		int nodeID = (*it_node);

		lowerAssocnodeset.insert(nodeID);
	}

	sec->updateAssocNodes(cNodeID, 1, INTERLEAVING_T+1, lowerAssocnodeset); // 하위 연관 노드 정보 갱신 (iha, diha)
	*/
	return res;
}
/*
void Kmr_IHA::genKeyMsg()
{

}
*/
/*
KeyMsg* Kmr_IHA::getKeyMsg() // 키 재분배 메시지 반환
{

}
*/
/*
void Kmr_IHA::addPairKey (int nID, int nID2)
{

}
*/
int Kmr_IHA::cnstrctKTree() // key tree  초기 구축
{
	int height = 2; // key tree의 높이	

	this->root = new KNode;

	// root 노드 초기화
	this->initKNode (root);
	root->isCluster = true; // 그룹 내의 노드들의 iD 목록을 구하기 위해 전체 네트워크를 하나의 클러스터로 간주함 -  getNdsInGrp() 함수

	//root->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

	numNodesAtLevel[0] = 1; // level 0의 노드 수 저장

	/*
	root->groupKey = new GroupKey;
	root->parent = NULL;
	root->leftChild = NULL;
	root->leftSibling = NULL;
	root->rightSibling = NULL;
	root->unode = NULL;
	root->level = 0;
	root->index = 0;
	*/
	// 현재 센서 노드 수 획득
	int numCurNodes = 0;
	numCurNodes = this->nodes.size();

	numNodesAtLevel[1] = numCurNodes; // level 1의 노드 수 저장
	numNodesAtLevel[2] = numCurNodes; // level 2의 노드 수 저장

	KNode* prev = NULL;
	KNode* current = NULL;

	int kNodeIdx = 0;

	map< int, Node*>::iterator it_node;
	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int i = Node::numNodesEvctd; i < Node::totNumNodes; i++) // 새롭게 추가되는 노드들에 대해 반복
	//for (int i = 0; i< numCurNodes; i++)
	{
		KNode* kNode = new KNode; // k-node 생성
		this->initKNode(kNode);
		kNode->parent = root;
		kNode->level = 1;
		kNode->index = kNodeIdx;
		
		//kNode->index = i;
	/*
		kNode->groupKey = new GroupKey;
		kNode->groupKey->newVal = gen1.genRandInt(Max_Key_Value); // (2014.05.08) 난수를 생성하여 그룹 키 값으로 대입
		kNode->parent = root;
		kNode->leftChild = NULL;
		kNode->rightSibling = NULL;
		kNode->unode = NULL;
		kNode->level = 1;
		kNode->index = i;
		*/
		UNode* uNode = new UNode; // u-node 생성	  
		
		uNode->sensorNode = (*it_node).second;
		//uNode->sensorNode = nodes[i] ;
		uNode->isCH = false;
		
		  current = kNode; // 현재 k-node

		  kNode->unode = uNode; // k-node, u-node 간 포인터 설정
		  uNode->parent = kNode;

		  if (prev != NULL)// left/right sibling 설정
		  {
			  prev->rightSibling = current;
			  current->leftSibling = prev;
		  }

		  kNode->parent = root; // k-node, root 간 포인터 설정

		  if (kNodeIdx ==0)
		  {
			  root->leftChild = kNode; // left child 설정
		  }

		  kNodeIdx++;

		  prev = kNode; // 이전 k-node 설정
	}  	
	// k-node, u-node 객체 생성

	//root->updated = true; // (2014.07.31) update 플래그를 참으로 설정
	//root->updated_addition = true;

	this->kTreeHeight = height;

	return height;
}

int Kmr_IHA::cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes) // (2014.06.05)
{
	int height;

	// ...

	return height;
}

int Kmr_IHA::cnstrctKTree(struct _tagKNode* root, set<int> nodes)
{
	int height;

	// ...

	return height;
}

int Kmr_IHA::updateKTree(IDSet addedNodes, IDSet deletedNodes) // (2014.05.29)
{
	// root 노드 생성 및 초기화 -> Kmr 객체의 생성자에서 처리

	int height = 2;

	numNodesAtLevel[0] = 1; // level 0의 노드 수 저장
	
	this->kTreeHeight = height;	

	/* 키 트리 상에서 노드 제거 */
	
	struct _tagKNode* cKNode; // 현재 탐색중인 knode
	struct _tagKNode* oKNode; // 구 knode
	cKNode = root->leftChild;

	 // root 노드의 leftChild에서부터 하나씩 검사, 삭제 노드 목록에 있으면 해당 k-node와 u-node 제거, root 노드의 update_eviction 플래그 = true로 설정

	while ( cKNode != NULL) // 모든 individual key에 대해 반복
	{
		// ...

		struct _tagUNode* uNode = NULL;
		uNode = cKNode->unode; // u-Node 획득
		Node* node = uNode->sensorNode;

		if (node ==NULL)
		{
			cKNode= cKNode->rightSibling; // 다음 노드로 이동
			continue;
		}

		IDSet::iterator it_find;
		it_find = deletedNodes.find( node->id);

		/* 제거 노드 목록에 있으면 k-node와 u-node 제거 */
		if (it_find != deletedNodes.end() )
		{
			oKNode = cKNode;

			struct _tagKNode* tKNode; // eviction-point에서 root까지의 경로에 위치하는 k-node의 update_eviction 플래그 설정

			tKNode = oKNode->parent; // kNode 포인터를 eviction-point로 초기화

			while (tKNode != NULL) // root 노드까지의 경로에 존재하는 kNode들에 대해 반복
			{
				tKNode->updated = true;
				tKNode->updated_eviction = true; // update_eviction 플래그 설정
				tKNode = tKNode->parent;
			}

			delete (uNode); // u-node 메모리 반환

			if (cKNode->leftSibling == NULL)
			{
				cKNode->parent->leftChild = cKNode->rightSibling; // 부모 노드의 left child 설정
			}

			if (cKNode->leftSibling != NULL)
			{
				cKNode->leftSibling->rightSibling = cKNode->rightSibling; // left sibling -> right sibling 연결 설정
			}

			if (cKNode->rightSibling != NULL)
			{
				cKNode->rightSibling->leftSibling = cKNode->leftSibling; // left sibling <- right sibling 연결 설정
			}

			cKNode = cKNode->rightSibling;

			delete (oKNode); // k-node 반환

			continue;
		} // k-node와 u-node 제거

		cKNode= cKNode->rightSibling; // 다음 노드로 이동
	
	} // 노드 제거 완료

	numNodesAtLevel[1] -= deletedNodes.size(); // level 1의 노드 수 감소
	numNodesAtLevel[2] -= deletedNodes.size(); // level 2의 노드 수 감소

	/* 키 트리 상에서 노드 추가 */

	IDSet::iterator it_add;

	for (it_add = addedNodes.begin(); it_add != addedNodes.end(); it_add++) // 추가된 노드들에 대해 반복
	{
		int nodeID;

		nodeID = *it_add; // 노드 ID 획득

		struct _tagKNode* kNode = new struct _tagKNode; // k-node 생성
		this->initKNode(kNode); // k-node 초기화


		struct _tagUNode* uNode = new struct _tagUNode; // u-node 생성
		this->initUNode(uNode);

		kNode->unode = uNode;
		kNode->parent = root;

		uNode->parent = kNode;
		uNode->sensorNode = nodes[nodeID];

		struct _tagKNode* addPoint;

		addPoint = root; // addition_point 결정		
		kNode->level = addPoint->level+1;

		/* 포인터 연결 */

		if (addPoint->leftChild != NULL)
		{
			kNode->rightSibling = addPoint->leftChild;	// left - right sibling 노드 간 연결 설정
			kNode->rightSibling->leftSibling = kNode; 
		}

		addPoint->leftChild = kNode; // addition point의 left child 연결 설정

		struct _tagKNode* tKNode; // addition-point에서 root까지의 경로에 위치하는 k-node의 update_addition 플래그 설정

		tKNode = addPoint;

		while (tKNode != NULL) // root 노드까지의 경로에 존재하는 kNode들에 대해 반복
		{
			tKNode->updated = true;
			tKNode->updated_addition = true; // updated_addition 플래그 설정
			tKNode = tKNode->parent; // 상위 k-node로 이동
		}
	}

	numNodesAtLevel[1] += addedNodes.size(); // level 1의 노드 수 증가
	numNodesAtLevel[2] += addedNodes.size(); // level 2의 노드 수 증가


	/* 재 구조화 (restructuring) point 결정 */	

	this->restruct_pts.clear(); // 재 구조화 point 목록 리셋

	set<int> nodesInGrp;

	nodesInGrp = this->getNdsInGrp(root);

	restruct_pts[root] = nodesInGrp;

	//restruct_pts.insert(root); // 재 구조화 point는 root 노드
	/*
	struct _tagKNode* restruct_pt = NULL;
	struct _tagKNode* clstrKey = NULL; // 클러스터 키
	struct _tagKNode* groupKey = NULL; // 그룹 키

	// (h-2) level의 각각의 knode들에 대해 반복 (클러스터에 대응되는 kNode)

	int clusterLevel = height-2; // cluster level 계산

	int numCluster;

	numCluster = this->getNumNodesAtLevel(clusterLevel); // 클러스터 개수 반환

	for (int cl = 0; cl < numCluster; cl++) // 각각의 클러스터에 대해 반복
	{
		clstrKey = getKNode(clusterLevel, cl); // 클러스터 키 획득

		groupKey = clstrKey;

		int numNodesinGrp; // 그룹에 속한 노드 수
		int numClstrsinGrp; // 그룹에 속한 클러스터 수
		numClstrsinGrp = 1; 

		bool reconstruct_needed = false; // 재구조화 필요성 여부


		while ( groupKey != root) // 적정 노드 수를 만족하는 최하위 그룹을 탐색 - groupKey가 root에 도달하면 반복문 탈출
		{
			bool condition = false; // 적정 노드 수 만족 여부 (T: 만족, F: 불만족)
			int numNodesInGrp = this->getNumNdsInGrp(groupKey); // 해당 그룹에 속하는 노드 수 획득
			int numClstrsInGrp = this->getNumClstrsInGrp(groupKey); // 해당 그룹에 속하는 클러스터 수 획득

			int nodesPerCluster;
			nodesPerCluster = (numNodesInGrp / numClstrsInGrp); // 클러스터 당 평균 노드 수 계산

			if ( (nodesPerCluster >= INTERLEAVING_T+1) && (nodesPerCluster <= 2* (INTERLEAVING_T+1) ) )
			{				
				restruct_pt = groupKey;
				condition = true;
				break; // 적정 평균 노드 수가 만족되면 loop 탈출
			}
			else
			{
				reconstruct_needed = true;
			}
		}

		if (reconstruct_needed == true)
		{
			restruct_pts.push_back(restruct_pt);
		}

		// 해당 클러스터가 적정 노드 수를 만족하면 재구조화 포인트 설정 불필요
		// 해당 클러스터가 적정 노드 수를 만족하지 않을 경우 재구조화 포인트 설정

	}
	*/
	/* 각각의 재 구조화 point에 대해 키 트리 부분적 갱신 */ 

	/* level, index 재설정 */

	int cnt_kNode;
	cnt_kNode = 0;

	cKNode = root->leftChild;

	while (cKNode != NULL)
	{
		cKNode->index = cnt_kNode++;
		cKNode = cKNode->rightSibling;
	}



	return kTreeHeight;
}

void Kmr_IHA::dstrctKTree() // 키 트리 제거
{
	KNode* current = root->leftChild;
	KNode* prev = current;

	while(current !=NULL)
	{		
		current = current->rightSibling;
		
		delete (prev->groupKey);
		delete (prev->unode);
		delete prev;

		prev = current;
	}

	delete root;

}

void Kmr_IHA::cnstrctLBHG() // (2014.04.25) 계층적 그룹 형성 - LEAF 노드에 실제 센서 노드 정보 저장
{
	// 구현 중 
	// key tree의 leaf node에 실제 센서 노드 정보 저장

	map<int, Node*>::iterator it_node;
	it_node = nodes.begin();

	//int i = Node::numNodesEvctd;

	KNode* current = NULL;

	current = root->leftChild; // 현재 탐색 중인 k-node (u-node의 부모 노드)

	while (current != NULL) // key tree의 leaf node (u-node) 마다 그에 대응되는 센서 노드 정보 저장
	{
		UNode* unode = current->unode;

		if (unode == NULL)
		{
			cout << " unode is NULL " ; // unode is NULL, 에러 출력 후 종료
			exit(-1);
		}

		unode->sensorNode = (*it_node).second; // nodes[i]; // 센서 노드 정보 저장
		it_node++;
		
		// 센서 노드 정보 출력 (테스트용)
		/* 
		cout << "unode " << i << "- sensor node info. " << endl;

		// ID 출력
		int nodeID = unode->sensorNode->id;
		cout << "\t node ID " << nodeID << endl;

		// 좌표 출력
		cout << "\t (x, y) = ( " << unode->sensorNode->x << ", " << unode->sensorNode->y << " )" << endl;

		// 클러스터 정보 출력 (CH 여부 포함)
		cout << "\t cluster ID (CH's ID) = " << rtn->getClstrID(nodeID) << endl;


		// 연관 노드 정보 출력
		try 
		{
			Sec_IHA* sec_IHA = dynamic_cast<Sec_IHA*>(this->sec);

			//std::map <int, std::set<int> > upperNodes = sec_IHA->upperNodes;
			//std::map <int, std::set<int> > lowerNodes = sec_IHA->lowerNodes;

			std::set<int> upperNodes = sec_IHA->upperNodes[nodeID][INTERLEAVING_T+1]; // 해당 노드의 (T+1) 홉 상위 노드 목록
			std::set<int> lowerNodes = sec_IHA->lowerNodes[nodeID][INTERLEAVING_T+1]; // 해당 노드의 (T+1) 홉 하위 노드 목록

			std::set<int>::iterator it;

			cout << "\t uppper assoc nodes: { " ;

			for (it = upperNodes.begin(); it != upperNodes.end(); it++)
			{
				int nodeID = *it;
				cout << nodeID << ", ";
			}

			cout << " } " << endl;

			cout << "\t lower nodes: { " ;

			for (it = lowerNodes.begin(); it != lowerNodes.end(); it++)
			{
				int nodeID = *it;
				cout << nodeID << ", ";
			}

			cout << " } " << endl;

		} catch (exception& e) {cout << "Exception: " << e.what();}
		
		// 부모 노드 출력
		Node* parent = rtn->getSPParentNode(unode->sensorNode);
		
		if (parent != NULL)
		{
			cout << " parent: " << parent->id << endl;
		}
		//*/

		current = current->rightSibling; // 다음 k-node로 이동
		//i++; // 노드 인덱스 +1 증가
	}	

}

void Kmr_IHA::cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp) // (2014.06.13)
{
	// 기존 키 관리 기법의 경우 키 트리가 갱신되는 과정에서 leaf 노드와 실제 센서 노드 간의 대응 관계가 설정되므로 
	// 따로 계층적 그룹 갱신이 필요하지 않음. 상위 클래스에서 가상 함수로 선언되었으므로 구현 부만 정의
}

void Kmr_IHA::updateLBHG() // // (2014.06.13) 계층적 그룹 갱신 (모든 재 구조화 지점에 대해 해당 그룹 갱신)
{
	// 위와 동일한 이유로 처리 내용 없음
}

Kmr_IHA::Kmr_IHA() 
{


}

Kmr_IHA::~Kmr_IHA()
{
	
}

/*
void Kmr_IHA::addPairKey (int nID, int nID2)// (2013.08.29) 두 노드의 iD를 입력으로 pairwise key를 추가
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
	pKeys[nID].push_back(pk);
	pKeys[nID2].push_back(pk2);
}

void Kmr_IHA::setPairKeys() // (2013.08.28 추가) pair key 설정 함수
{
	// 이미 노드들의 연관 관계가 설정되어 있다고 가정

	// 모든 노드들에 대해, 연관 노드 목록 검사

	for (int i = 0; i < NUM_NODES; i++)
	{
		// 현재 노드와 상위 연관 노드들과 공유하는 pair key 생성



		// 생성된 pair key를 해당 노드의 pair key 목록에 추가


		// 현재 노드와 하위 연관 노드들과 공유하는 pair key 생성

		// 생성된 pair key를 해당 노드의 pair key 목록에 추가
	}
}

void Kmr_IHA::testKeyInfo()
{
	ofstream out("tst_Kmr_IHA_Key.txt");

	for (int i = 0; i < NUM_NODES; i++)
	{
		
		out << "node ID: " << i << endl; 

		// 인증 키 정보 출력	
		
		

		// 비밀 키 정보 출력

	}
	out.close();
}

Kmr_IHA::Kmr_IHA() 
{


}

Kmr_IHA::~Kmr_IHA()
{

}
*/