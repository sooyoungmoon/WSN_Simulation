// 2014.05.21
// 문수영
// Rtn_DIHA.h
// DIHA의 라우팅 클래스 정의

#include "Rtn_DIHA.h"

void Rtn_DIHA::findNbrs()
{

}

void Rtn_DIHA::setCHs()
{
	// 각 cell 별로 CH 선출

	std::map <int, std::set<int>> clusterSet; // 노드들을 클러스터 단위로 분류하기 위한 set	
	std::map <int, int> numNodesInCell; // <cell 번호, cell에 속한 노드 수>	
	std::map <int, int> chs; // <cell 번호, CH's ID>

	std::map < int, pair<double, double> > COA; // <클러스터 ID, <평균 x좌표, 평균 y좌표> >
	
	ndsClstrMap.clear(); // 기존 클러스터 정보 리셋
	clusters.clear();

	for (int c = 0; c < NUM_CELL; c++) // cell 별로 무게 중심 초기화
	{
		COA[c] = make_pair(0, 0); // (0, 0)으로 초기화 -> 모든 경우에 갱신되는지? ->yes, 각 cell 당 최소 1개 이상 노드 존재
		numNodesInCell[c] = 0; // 0으로 초기화
	}



	// 노드를 각 cell 별로 분류, 각 cell 내 노드들의 x 좌표 / y 좌표 합계 구함

	map<int, Node*>::iterator it_node;
	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++) // 노드 목록 순회, cell 정보, x,y 정보 추출 -> 무게 중심 구하기
	{
		Node* pNode = (*it_node).second;
		//Node* pNode = nodes[n];

		int cellID = pNode->cell;

		clusterSet[cellID].insert(pNode->id); // 노드가 위치하는 cell의 노드 집합에 해당 노드 id 추가

		int x = pNode->x;
		int y = pNode->y;

		COA[cellID].first += x;
		COA[cellID].second += y;

		numNodesInCell[cellID]++; // cell의 노드 집합에 포함되는 노드 수 +1증가
	}

	// 각 cell의 무게 중심 구하기 (평균 x, 평균 y)
	for (int c = 0; c < NUM_CELL; c++) // x 좌표 / y 좌표의 총합을 각각 노드 수로 나눔
	{
		COA[c].first /= numNodesInCell[c];
		COA[c].second /= numNodesInCell[c];
	}

	for (int c = 0; c < NUM_CELL; c++) // cell에 속한 각 노드에서 무게중심까지의 거리 구하기
	{
		double minDist=0;
		double curDist=0;

		for ( set<int>::iterator it = clusterSet[c].begin(); it != clusterSet[c].end(); it++)
		{
			int nodeID = *it;
			Node* pNode = nodes[nodeID]; 

			curDist = sqrt ( pow((double)pNode->x - COA[c].first, 2) + pow ((double)pNode->y - COA[c].second, 2) );

			if ( it == clusterSet[c].begin() ) // 첫 번째 원소인 경우, 최소 거리 변수의 값을 해당 노드로부터 무게 중심까지의 거리로 설정
			{
				minDist = curDist;
				chs[c] = nodeID;
			}

			else if ( curDist < minDist)
			{
				minDist = curDist;
				chs[c] = nodeID;
			}
		} // 각 cell을 대표하는 cluster head 결정 완료		

		for ( set<int>::iterator it = clusterSet[c].begin(); it != clusterSet[c].end(); it++) // 클러스터 정보 저장
		{
			int nodeID = *it;
			int clstrhdID = chs[c]; 

			ndsClstrMap[nodeID] = clstrhdID; 

			clusters[clstrhdID].insert (nodeID);
		}
		// 클러스터 정보 저장 완료
	}

	/*
	// CH 선택 (제안 기법과 동일)

	int clsID = 0;

	int mberIdx[NUM_CHS] = {0}; // 클러스터 멤버의 인덱스

	for (int i = 0; i < NUM_CHS; i++)
	{
		int nIDMin = NODES_PER_CLSTR * i; // 해당 클러스터 내의 노드 ID의 최소값과 최대값
		int nIDMax = nIDMin + NODES_PER_CLSTR - 1; 

		int ch = gen1.uniformI(nIDMin, nIDMax);

		clstr_hds[i] = &nodes[ch];
	}
	*/
}

list<Node*> Rtn_DIHA::selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs) // // 제어 메시지를 위한 다음 노드 선택
{
	// BS에서 전송 범위 이내의 노드들을 목록에 추가하여 반환

	list<Node*> nextNdLst;

	map<int, Node*>::iterator it_node;

	if (cntrlMsg->cmType == BS_HELLO)
	{

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
		//for (int nID = Node::numNodesEvctd; nID < Node::totNumNodes; nID++)
		{
			Node* node = (*it_node).second;
			int nID = node->id;
			//Node* node = nodes[nID];
			
			if (node == NULL)
			{
				cout << " (node == NULL)";
				exit(-1);
			}

			if ( node->distance < TRANS_RANGE)
			{
				if ( ndsClstrMap[nID] == nID) // 노드가 CH인 경우 전달 노드 목록에 추가
				{
					nextNdLst.push_back(node);
				}
			}
		}
	}

	// ...
	return nextNdLst;	
}

list<Node*> Rtn_DIHA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)
{
	list<Node*> nextNdLst;

	// 제어 메시지 타입에 따른 처리
	if (cntrlMsg->cmType == BS_HELLO)
	{
		int cNodeID = cNode->id;
		double curDist=0;
		double dist = 0;
		curDist = cNode->distance;

		list<Node*>::iterator it;
		list<Node*> nbrLst = nbrLsts[cNodeID];


		// 자신의 이웃 노드들을 하나씩 검사

		for (it = nbrLst.begin(); it != nbrLst.end(); it++)
		{
			Node* nbr = *it;
			int nbrID = nbr->id;

			dist = nbr->distance;

			if ( dist > curDist) // 각 노드는 자신보다 BS에 먼 이웃 CH들에게 메시지 전달
			{
				if ( ndsClstrMap[nbrID] == nbrID) // 노드가 CH인 경우 전달 노드 목록에 추가
				{
					nextNdLst.push_back(nbr); // 노드 주소 저장
				}
			}
		}
		
	}
	else if (cntrlMsg->cmType == CLSTR_ACK)
	{
		// 각 노드는 자신의 부모 노드 목록에 포함된 노드들에게 메시지 전달

		//list<Node*> parents = this->getParents(cNode);
		list<Node*> parents = this->getParentCHs(cNode); // (2014.08.17 수정) 부모 CH 목록 획득

		list<Node*>::iterator it;
		
		
		for (it= parents.begin(); it!= parents.end(); it++)
		{
			Node* p = *it;

			if (p!= NULL)
			{
				nextNdLst.push_back(p);
			}
		}	
	}
	else
	{
		cout << "type is invalid";
		exit(-1);
	}
	return nextNdLst;			
}

Node* Rtn_DIHA::selNext (struct _tagEvntMsg* evntMsg, Node* cNode) // (2014.08.13) 이벤트 보고서 다음 전달 노드 결정 함수
{
	// 최단 경로 기반 라우팅
	/*
	Node* next = NULL;

	Node* spParent = this->getSPParentCH(cNode); // 현재 노드의 최단 경로 CH 노드 반환
	//Node* spParent = this->getSPParent(cNode); // 현재 노드의 최단 경로 부모 노드 반환

	if (spParent !=NULL) // 최단 경로 부모 노드를 다음 노드로 지정
	{
		next = spParent;
	}
		
	return next; // 다음 노드 반환
	*/

	
	
	Node* next = NULL;
	int cNodeID = cNode->id; 

	this->resetNxtNds(); // 다음 전달 노드 배열 reset
	this->eval_result.clear(); // 기존 평가 결과 제거
	candis.clear(); // 기존 후보 노드 목록 제거


	kmr->testPairKey(); // (debugging) pair key 정보 테스트

	// 후보 노드에 대한 평가

	// 후보 노드의 DI, EC, NK를 구하고 0~1 사이의 값으로 정규화

	double maxDI = 0; // 후보 노드 중 싱크 노드까지의 최대 거리
	double maxEC = 0; // 후보 노드 중 최대 에너지 소모량
	int maxNK = 0; // 후보 노드가 가진 pairwise 키의 개수의 최대값
	

	/// normalizing factor 계산	

	// 후보 노드를 하나씩 검사

	list<Node*>::iterator it;

	for ( it =  parentCHs[cNodeID].begin(); it != parentCHs[cNodeID].end(); it++) // 후보 노드를 하나씩 검사
	//for ( it =  parentsLsts[cNodeID].begin(); it != parentsLsts[cNodeID].end(); it++) // 후보 노드를 하나씩 검사
	{
		Node* candi = *it;

		if (candi != NULL)
		{	
			int numPairKeys = kmr->getNumPairKeys(candi); // 후보 노드가 가진 pairwise 키의 개수 반환			

			double dis = candi->distance; // 후보 노드의 싱크 노드까지의 거리
			double eCnsmptn = candi->eCnsmptn; // 후보 노드의 에너지 소모량

			if ( numPairKeys > maxNK) maxNK = numPairKeys;

			if ( dis > maxDI) maxDI = dis;

			if ( eCnsmptn > maxEC) maxEC = eCnsmptn;
		}

	} // for문 종료 - 후보 노드를 검사하여 normalizing factor 계산
		
		
	// 각 입력 요소 값 정규화 (0~1)

	int cnt = 0;

	for ( it =  parentCHs[cNodeID].begin(); it != parentCHs[cNodeID].end(); it++) // 후보 노드를 하나씩 검사
	//for ( it =  parentsLsts[cNodeID].begin(); it != parentsLsts[cNodeID].end(); it++) // 후보 노드를 하나씩 검사
	{
		Node* candi = *it;

		if (candi != NULL)
		{		
			double eval_res;
			double normNK = kmr->getNumPairKeys(candi); // 후보 노드가 src cluster 로부터 이미 배포 받은 인증 키의 개수 (정규화)
			double normDI = (candi->distance) / maxDI; // 후보 노드의 싱크 노드까지의 거리 (정규화)
			double normEC = candi->eCnsmptn; // // 후보 노드의 에너지 소모량 (정규화)

			if (maxNK == 0) normNK = 1;
			else
				normNK /= maxNK;			

			if (maxEC == 0) normEC = 1;
			else
				normEC /= maxEC;
			
			
			eval_res = normEC + normDI - normNK; // (2014.12.30) 수식 모델

			//eval_res = eval_fuzzy (normDI, normEC, normNK); // 평가 결과 도출	(PCM)		

			//eval_res = normDI; // (2014.10.01) 평가 결과 도출		(Greedy routing)

			//eval_res = normDI + normEC; // (2014.10.01) 평가 결과 도출		(GEAR)


			//eval_result[cnt] = make_pair (candi->id, eval_res); // 평가 결과 저장 
			//eval_result[cnt] = make_pair (cNodeID, eval_res); // 평가 결과 저장 

			candis[cnt] = candi; // 후보 노드 목록에 저장

			cnt++;
			
		}// if문 종료- 후보 노드 평가 결과 도출 및 저장

	} // for문 종료 - 후보 노드들의 1) 인증 키 개수, 2) BS까지의 거리, 3) 에너지 소모량을 (0~1) 사이의 값으로 정규화하고 Fuzzy logic에 의한 평가 결과 도출
	

	// 후보 노드 목록 정렬

	quicksort (0, candis.size()-1); // 평가 결과에 따라 후보 노드 목록 정렬	

	// 다음 전달 노드 배열 값 갱신

	if ( candis.size() < 1 ) // 후보 노드가 1개도 없는 경우 NULL 반환, 그렇지 않다면 다음 노드 반환
		return NULL;

	else
	{
		next = candis[0];
	}	

	return next;
	
}

list<Node*> Rtn_DIHA::selNext (struct _tagKeyMsg* keyMsg, BS* bs)
{
	list<Node*> nextNdLst;
	set<Node*> nextNdSet;

	this->eval_result.clear();
	nxtNds.clear();
	next_recipent_Map.clear(); // (다음 노드, {목적 노드}) 맵 리셋
	candis.clear();

	set<int> recipents = keyMsg->recipents;

	set<int>::iterator it;

	for (it = recipents.begin(); it != recipents.end(); it++) // 각 목적 노드에 대해 후보 노드 평가
	{
		int recipentID = (*it);
		Node* recipent = nodes[recipentID];		

		list<Node*> nbrLst = bs->getNbrLst();
		list<Node*>::iterator it2 = nbrLst.begin();
		
		/* 평가 */

		//map<int, Node*> candis;

		int cnt = 0;

		for ( it2 = nbrLst.begin(); it2 != nbrLst.end(); it2++)
		{
			Node* candi = (*it2);
			int candiID = candi->id;
			candis[cnt] = candi;

			double evaluation_res = eval(recipent, candi);

			this->eval_result[cnt] = make_pair(candiID, evaluation_res);

			//this->eval_result[candiID] = eval(recipent, candi);
			cnt++;
		}
		
		/* 정렬 */
		quicksort (0, candis.size()-1);
		//this->quicksort (candis, 0, candis.size()-1);

		/* 다음 노드 집합 추가 */
		nextNdSet.insert(candis[0]);

		// (다음 노드, {목적 노드}) map 구성

		if (candis[0] != NULL)
		{
			this->next_recipent_Map[candis[0]->id].insert (recipent->id);
		}
	}

	set<Node*>::iterator it3;

	for (it3 = nextNdSet.begin(); it3 != nextNdSet.end(); it3++) // 구성된 다음 노드 집합의 내용을 다음 노드 목록에 저장
	{
		Node* candi = (*it3);
		nextNdLst.push_back(candi);
	}

	return nextNdLst; // 다음 노드 목록 반환
}

list<Node*> Rtn_DIHA::selNext (struct _tagKeyMsg* keyMsg, Node* cNode)
{
	list<Node*> nextNdLst;
	set<Node*> nextNdSet;

	this->eval_result.clear();
	nxtNds.clear();
	next_recipent_Map.clear(); // (다음 노드, {목적 노드}) 맵 리셋
	candis.clear();

	set<int> recipents = keyMsg->recipents;

	set<int>::iterator it;

	for (it = recipents.begin(); it != recipents.end(); it++) // 각 목적 노드에 대해 후보 노드 평가
	{
		int recipentID = (*it);
		Node* recipent = nodes[recipentID];

		int cNodeID = cNode->id;

		list<Node*>::iterator it2 = this->nbrLsts[cNodeID].begin();
		
		/* 평가 */

		//map<int, Node*> candis;

		int cnt = 0;

		for ( it2 = nbrLsts[cNodeID].begin(); it2 != nbrLsts[cNodeID].end(); it2++)
		{
			Node* candi = (*it2);
			int candiID = candi->id;
			candis[cnt] = candi;

			double evaluation_res = eval(recipent, candi);
			this->eval_result[cnt] = make_pair(candiID, evaluation_res);

			//this->eval_result[candiID] = eval(recipent, candi);
			cnt++;
		}
		
		/* 정렬 */
		quicksort (0, candis.size()-1);
		//this->quicksort (candis, 0, candis.size()-1);

		/* 다음 노드 집합 추가 */
		nextNdSet.insert(candis[0]);
		
		if (candis[0] != NULL)
		{
			this->next_recipent_Map[ candis[0]->id].insert (recipent->id); // (다음 노드, {목적 노드}) map 구성
		}
	}

	set<Node*>::iterator it3;

	for (it3 = nextNdSet.begin(); it3 != nextNdSet.end(); it3++) // 구성된 다음 노드 집합의 내용을 다음 노드 목록에 저장
	{
		Node* candi = (*it3);
		nextNdLst.push_back(candi);
	}

	return nextNdLst; // 다음 노드 목록 반환

	// 구현 중
	/*
	list<Node*> nextNdLst;


	// 현재 노드의 이웃 노드 목록에 속한 노드들 검사
	list<int> recipents = keyMsg->recipents;
	int recipentID = recipents.front();
	Node* recipent = nodes[recipentID];

	int cNodeID = cNode->id;
	list<Node*>::iterator it = this->nbrLsts[cNodeID].begin();
	*/
	/* 평가 */
	/*
	for (it; it !=  nbrLsts[cNodeID].end(); it++)
	{
		Node* nbr = (*it);
		int nbrID = nbr->id;

		this->eval_result[nbrID] = this->eval(keyMsg, cNode, nbr);
	}
	*/
	/* 정렬 */

	// 키 재분배 메시지 수신 노드 (현재는 1개라고 가정) 에 가장 가까운 노드를 다음 전달 노드 목록에 추가
	// ...

	/* 다음 전달 노드 목록 구성 */

	return nextNdLst;
}

list<Node*> Rtn_DIHA::selNext (struct _tagTREQ* treq, Node* cNode) // (2014.05.10) TREQ 메시지 전달을 위한 다음 노드 선택
{
	// 구현 중

	if (cNode == NULL)
	{
		cout << "cNode == NULL";
		exit(-1);
	}

	list<Node*> nextNdLst;

	// 필드 내의 각각의 노드 검사

	map<int, Node*>::iterator it_node;
	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
	{
		Node* pNode = (*it_node).second;
		//Node* pNode = nodes[n];

		if ( pNode != cNode)
		{
			double dist = sqrt( pow((double) pNode->x - cNode->x, 2 ) + pow ( (double) pNode->y-cNode->y, 2));// 현재 노드와 현재 검사 중인 노드 간 거리 계산

			if ( dist < TRANS_RANGE)
			{
				nextNdLst.push_back(pNode);
			}
		}
	}
	// ...


	return nextNdLst;
}

list<Node*> Rtn_DIHA::selNext (struct _tagTRES* tres, Node* cNode) // (2014.05.10) TREQ 메시지 전달을 위한 다음 노드 선택
{
	// 구현 중

	list<Node*> nextNdLst;

	Node* spParent = this->getSPParentNode(cNode); // 현재 노드의 최단 경로 부모 노드 반환

	if (spParent !=NULL)
	{
		nextNdLst.push_back(spParent);
	}

	// ...

	return nextNdLst;
}

double Rtn_DIHA::eval(Node* recipent, Node* candi)
{
	double res = 0;

	res = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	

	return res;
}

double Rtn_DIHA::testConect()
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
				tempNode = this->getSPParentCH(tempNode);		

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

/*
double Rtn_DIHA::eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi) // 키 재분배 메시지 전달을 위한 후보 노드 평가
{
	double res = 0; // 후보 노드의 평가 결과

	list<int> recipents = keyMsg->recipents;
	int recipentID = recipents.front();
	Node* recipent = nodes[recipentID];

	// 키 메시지 수신 노드와 후보 노드 간의 거리를 계산

	double dist = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	
	res = dist;
	return res;
}
*/

double Rtn_DIHA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi)
{
	double res = 0; // 후보 노드의 평가 결과

	return res;
}

double Rtn_DIHA::eval (struct _tagEvntMsg* evntMsg, Node* candi)
{
	double res = 0; // 후보 노드의 평가 결과

	return res;
}

double Rtn_DIHA::eval_fuzzy (double dist, double eCnpmtn, double numKeys) // (2014.08.13) fuzzy logic에 의한 (이벤트 전달을 위한) 후보 노드 평가 
{
	double res = 0;

	res =  (double) fuzzy.think( eCnpmtn*100, dist*100,  numKeys*100); // fuzzy logic에 의한 추론

	return res;
}

Rtn_DIHA::Rtn_DIHA()
{

}

Rtn_DIHA::~Rtn_DIHA()
{

}