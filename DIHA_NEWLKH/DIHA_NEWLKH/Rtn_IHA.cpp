// 2014.05.21
// 문수영


#include "Rtn_IHA.h"
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

void Rtn_IHA::findNbrs()
{
	/*
	// 이웃 노드 탐색 과정

	int nbrIdx = 0; // 이웃 노드 인덱스	

	int i = 0, j = 0;

	for(i=0; i < NUM_NODES;i++) 
	{
		for(j=0; j<NUM_CHS;j++)
		{
			if ( nodes[i].id != clstr_hds[j]->id)
			{
				//이웃 노드가 현재 노드와 통신 가능한 거리에 존재하는 경우 이웃 노드 목록에 추가 
				if(  pow((double)((clstr_hds[j])->x-(nodes+i)->x),2) + pow((double)((clstr_hds[j])->y-(nodes+i)->y),2) < pow((double)TRANS_RANGE,2))
				{
					if (nbrIdx < NBRS_PER_NODE)
					{
						nbrs[i][nbrIdx++] = (clstr_hds[j]); // 이웃 노드 정보 저장

					}
				}
			}
		}

		numNbrs[i] = nbrIdx; // (2013.08.13) 각 노드의 이웃 노드 수 저장
		nbrIdx = 0;
	}

	setCandis(); // 후보 부모 노드 설정
	this->testCandis(); // (2012.11.23 추가) 후보 노드 테스트 함수 호출
	*/
}

void Rtn_IHA::setCHs()
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
		double minDist;
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

list<Node*> Rtn_IHA::selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs) // // 제어 메시지를 위한 다음 노드 선택
{
	// BS에서 전송 범위 이내의 노드들을 목록에 추가하여 반환

	list<Node*> nextNdLst;

	if (cntrlMsg->cmType == BS_HELLO)
	{
		map<int, Node*>::iterator it_node;
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

	this->nxtNds = nextNdLst;

	return this->nxtNds;	
}

list<Node*> Rtn_IHA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)  // 제어 메시지를 위한 다음 노드 선택
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
		// 각 노드는 자신의 최단 경로 CH 노드에게 메시지 전달

		Node* spParent = NULL;

		spParent = getSPParentCH(cNode);
		//spParent = getSPParent(cNode);

		if ( spParent != NULL )
		{
			nextNdLst.push_back(spParent);
		}
	}
	else
	{
		cout << "type is invalid";
		exit(-1);
	}
	return nextNdLst;	
}

Node* Rtn_IHA::selNext(struct _tagEvntMsg* evntMsg, Node* cNode) // (2014.08.13) 이벤트 보고서를 위한 다음 전달 노드 결정
{
	// 최단 경로 기반 라우팅

	Node* next = NULL;

	Node* spParent = this->getSPParentCH(cNode); // 현재 노드의 최단 경로 CH 노드 반환
	//Node* spParent = this->getSPParent(cNode); // 현재 노드의 최단 경로 부모 노드 반환

	if (spParent !=NULL) // 최단 경로 부모 노드를 다음 노드로 지정
	{
		next = spParent;
	}
		
	return next; // 다음 노드 반환
}

list<Node*> Rtn_IHA::selNext (struct _tagKeyMsg* keyMsg, BS* bs)
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
			
			//this->eval_result[cnt] = eval(recipent, candi);
			//this->eval_result[candiID] = eval(recipent, candi);
			cnt++;
		}
		
		/* 정렬 */
		quicksort (0, candis.size()-1);
		//this->quicksort (candis, 0, candis.size()-1);

		/* 다음 노드 집합 추가 */
		nextNdSet.insert(candis[0]);

		// (다음 노드, {목적 노드}) map 구성
		this->next_recipent_Map[ candis[0]->id].insert (recipent->id);
	}

	set<Node*>::iterator it3;

	for (it3 = nextNdSet.begin(); it3 != nextNdSet.end(); it3++) // 구성된 다음 노드 집합의 내용을 다음 노드 목록에 저장
	{
		Node* candi = (*it3);
		nextNdLst.push_back(candi);
	}

	return nextNdLst; // 다음 노드 목록 반환
}

list<Node*> Rtn_IHA::selNext (struct _tagKeyMsg* keyMsg, Node* cNode)
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
		//candis.clear();
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

}

list<Node*> Rtn_IHA::selNext (struct _tagTREQ* treq, Node* cNode) // (2014.05.10) TREQ 메시지 전달을 위한 다음 노드 선택
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

list<Node*> Rtn_IHA::selNext (struct _tagTRES* tres, Node* cNode) // (2014.05.10) TRES 메시지 전달을 위한 다음 노드 선택
{
	// 구현 중

	list<Node*> nextNdLst;

	Node* spParent = this->getSPParentNode(cNode); // 현재 노드의 최단 경로 CH 노드 반환

	if (spParent !=NULL)
	{
		nextNdLst.push_back(spParent);
	}

	// ...

	return nextNdLst;
}

/*
double Rtn_IHA::eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi) // 키 재분배 메시지 전달을 위한 후보 노드 평가
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

double Rtn_IHA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi)
{
	double res = 0; // 후보 노드의 평가 결과

	return res;
}

double Rtn_IHA::eval (struct _tagEvntMsg* evntMsg, Node* candi)
{
	double res = 0; // 후보 노드의 평가 결과

	return res;
}


double Rtn_IHA::eval(Node* recipent, Node* candi) // 키 재분배 메시지 전달을 위한 후보 노드 평가
{
	double res = 0;

	res = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	

	return res;
}

double Rtn_IHA::testConect()
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

Rtn_IHA::Rtn_IHA()
{

}

Rtn_IHA::~Rtn_IHA()
{

}


/*
void Rtn_IHA::findNbrs()
{
	// 이웃 노드 탐색 과정

	int nbrIdx = 0; // 이웃 노드 인덱스	

	int i = 0, j = 0;

	for(i=0; i < NUM_NODES;i++) 
	{
		for(j=0; j<NUM_CHS;j++)
		{
			if ( nodes[i].id != clstr_hds[j]->id)
			{
				//이웃 노드가 현재 노드와 통신 가능한 거리에 존재하는 경우 이웃 노드 목록에 추가 
				if(  pow((double)((clstr_hds[j])->x-(nodes+i)->x),2) + pow((double)((clstr_hds[j])->y-(nodes+i)->y),2) < pow((double)TRANS_RANGE,2))
				{
					if (nbrIdx < NBRS_PER_NODE)
					{
						nbrs[i][nbrIdx++] = (clstr_hds[j]); // 이웃 노드 정보 저장

					}
				}
			}
		}

		numNbrs[i] = nbrIdx; // (2013.08.13) 각 노드의 이웃 노드 수 저장
		nbrIdx = 0;
	}

	setCandis(); // 후보 부모 노드 설정
	this->testCandis(); // (2012.11.23 추가) 후보 노드 테스트 함수 호출
}
*/

/*
void Rtn_IHA::setCHs()
{

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
}
*/

/*
void Rtn_IHA::setCandis()
{
	// 아무 처리도 안함
}
*/
/*
Node** Rtn_IHA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)  // 제어 메시지를 위한 다음 노드 선택
{
	// 다음 노드 목록 계산
	this->resetNxtNds(); //  다음 전달 노드 배열 reset
	
	int idx = cNode->id; // 노드의 배열 내 인덱스

	int numNxtNds = 0; // 노드의 다음 전달 노드 수

	int nxtNdsIdx = 0; // 다음 노드 배열 내 인덱스

	// 제어 메시지 타입에 따른 처리

	if (cntrlMsg->cmType == BS_HELLO)
	{
		// 각 노드는 자신보다 BS에 먼 이웃 노드들에게 메시지 전달

		for (int i =0; i < numNbrs[idx]; i++) // 현재 노드의 이웃 노드를 하나씩 검사
		{
			// 현재 이웃 노드가 자신보다 BS로부터 더 먼 경우, 
			// 다음 노드 배열에 해당 이웃 노드 정보 저장

			Node * neighbor = nbrs[idx][i];

			if (cNode->distance < neighbor->distance) // 현재 노드보다 이웃 노드의 BS까지의 거리가 더 먼 경우
			{

				int clstr1 = cNode->id / NODES_PER_CLSTR;
				int clstr2 = neighbor->id / NODES_PER_CLSTR;

				if (clstr1 != clstr2) // 두 노드의 클러스터가 다를 경우 다음 전달 노드로 선택
				{
					nxtNds[nxtNdsIdx++] = neighbor;
					numNxtNds++;

					if (numNxtNds > MAX_NXT_NODES)
					{
						break;
					}
				}
			}
		}
	}
	else if (cntrlMsg->cmType == CLSTR_ACK)
	{
		// 각 노드는 자신의 부모 노드에게 CLSTR_ACK 메시지 전달
		
		if (!parents[idx].empty())
		{
			list<Node*>::iterator it= parents[idx].begin();
			Node* parent = *it;

			nxtNds[0] = parent;	
			numNxtNds = 1;
		}	
		
	}
	else
	{
		cout << "error - Rtn_PRM::selNext()- 잘못된 제어메시지 타입" << endl;
	}	

	// 다음 노드 목록 반환
	
	return this->nxtNds;	

}

Node* Rtn_IHA::selNext (struct _tagEvntMsg* evntMsg, Node* cNode)
{
	Node* next = NULL;

	// 미리 저장된 부모 노드 반환

	int cNodeID = cNode->id; // 현재 노드의 ID

	if (!parents[cNodeID].empty())
	{
		list<Node*>::iterator it= parents[cNodeID].begin();
		Node* parent = *it;

		next = parent;
	}


	//next = parents[cNodeID]; // 현재 노드의 부모 노드 반환

	return next;
}

double Rtn_IHA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) 
{
	// 현재는 사용 안 함
	double res = 0; // 후보 노드의 평가 결과

	return res;
}

double Rtn_IHA::eval (struct _tagEvntMsg* evntMsg, Node* candi)
{
	// 현재는 사용 안 함
	double res = 0; // 후보 노드의 평가 결과

	return res;
}

Rtn_IHA::Rtn_IHA(void)
{
	cout << "Rtn_IHA()::Rtn_IHA() " << endl;
}

Rtn_IHA::~Rtn_IHA(void)
{

}
*/
/*
void Rtn_GEA::setCHs()
{
	// CH 선택 

	int clsID = 0;

	int mberIdx[NUM_CHS] = {0}; // 클러스터 멤버의 인덱스

	for (int i = 0; i < NUM_CHS; i++)
	{
		int nIDMin = NODES_PER_CLSTR * i; // 해당 클러스터 내의 노드 ID의 최소값과 최대값
		int nIDMax = nIDMin + NODES_PER_CLSTR - 1; 

		int ch = gen1.uniformI(nIDMin, nIDMax);

		clstr_hds[i] = (nodes+ch);
	}
}

void Rtn_GEA::setCandis()
{
	// (2012.11.18)제안 기법과 GEAR의 후보 노드 설정 과정은 같다고 가정함
	// 조건 1 - 현재 노드의 이웃 노드
	// 조건 2 - 현재 노드보다 싱크 노드에 가까움
	// 조건 3 - 현재 노드와 다른 클러스터에 속함

	int i = 0, j = 0; 
	int candiIdx = 0; // 후보 부모 노드의 인덱스

	for (i = 0; i < NUM_NODES; i++)
	{
		for ( j = 0; j < NBRS_PER_NODE; j++)
		{
			Node* cNode = this->nbrs[i][j]; // 현재 검사 중인 이웃 노드

			if (cNode == NULL) continue;

			if (cNode->distance < (nodes+i)->distance) // 현재 노드보다 이웃 노드의 싱크 노드까지의 거리가 더 가까운 경우
			{
				int clstr1 = (nodes+i)->id / NODES_PER_CLSTR; 
				int clstr2 = cNode->id / NODES_PER_CLSTR; 

				//if (clstr1 != clstr2) // (2012.11.24 제거) 현재 노드와 이웃 노드의 클러스터가 다른 경우
				{
					this->candis[i][candiIdx++] = cNode; // 후보 노드 정보 저장

					if ( candiIdx >= CANDIS_PER_NODE) // 후보 노드 배열이 가득 찬 경우 현재 for문 탈출
						break; 
				}
			}

		}

		numCandis[i] = candiIdx; // 노드의 후보 부모 노드 수 저장

		candiIdx = 0;
	}

}

Node** Rtn_GEA::selNext (KeyMsg* keyMsg, Node* cNode)
{
	// 제안 기법의 다음 노드 평가 과정과 유사하지만
	// 키 배포 가능 갯수를 고려하지 않고,
	// 다른 평가 함수를 사용 (거리와 에너지 소모량만 고려)

	// (2012.11.24 수정) 다음 전달 노드 배열 reset
	this->resetNxtNds();


	// 후보 노드 평가
	int idx = cNode->id;

	// 후보 노드의 DI, EC 를 구하고 0~1 사이의 값으로 정규화
	
	double maxDI = 0; // 후보 노드 중 싱크 노드까지의 최대 거리
	double maxEC = 0; // 후보 노드 중 최대 에너지 소모량

	for (int i = 0; i < numCandis[idx]; i++) // normalizing factor 계산
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{			
			double dis = candi->distance; // 후보 노드의 싱크 노드까지의 거리
			double eCnsmptn = candi->eCnsmptn; // 후보 노드의 에너지 소모량					

			if ( dis > maxDI) maxDI = dis;

			if ( eCnsmptn > maxEC) maxEC = eCnsmptn;
		}
	}
	
	for (int i = 0; i < numCandis[idx]; i++)
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{		
			double normDI = (candi->distance) / maxDI; // 후보 노드의 싱크 노드까지의 거리 (정규화)
			double normEC = candi->eCnsmptn; // // 후보 노드의 에너지 소모량 (정규화)

			if (maxEC ==0) normEC = 0;
			else
				normEC /= maxEC;

			eval_result[i] = eval ( normDI, normEC); // 평가 결과 도출, 저장
		}
	}
	
	// 후보 노드 목록 정렬
	this->quicksort( candis[idx], 0, numCandis[idx]-1); // 평가 결과에 따라 후보 노드 목록 정렬 (실제 후보 노드 수에 맞게 인자 전달 필요)

	// 다음 노드 목록 저장

	int numNxtNds = 0;

	if (numCandis[idx] < BRANCH_FACTOR)
	{
		numNxtNds = numCandis[idx];
	}
	else 
		numNxtNds = BRANCH_FACTOR;


		for (int i = 0; i < numNxtNds; i++) // 다음 전달 노드 배열에 노드 정보 저장
		{
			nxtNds[i] = candis[idx][numCandis[idx] -1-i];

			// (2012.11.12) 임시 test
			/*
			if (cNode->id == 999)
			{
				cout << "nxtNds[i] " << nxtNds[i]->id << " " ;
				cout << "nxtNds[i+1] " << nxtNds[i+1]->id << " " ;
			}
			
		}	

	// 다음 노드 목록 반환
	
	return this->nxtNds;	
}

Node** Rtn_GEA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)  // 사용 안 함
{
	return NULL; 
}


Node* Rtn_GEA::selNext (EvntMsg* evntMsg, Node* cNode)
{
	// 이벤트 메시지 전달 방법 또한 키 배포 메시지의 다음 전달 노드 결정 방법과 유사
	// 여러 개의 노드가 아닌 단 1개의 다음 노드 주소 반환

	// (2012.11.24 수정) 다음 전달 노드 배열 reset
	this->resetNxtNds();

	// 후보 노드 평가
	int idx = cNode->id;

	// 후보 노드의 DI, EC 를 구하고 0~1 사이의 값으로 정규화
	
	double maxDI = 0; // 후보 노드 중 싱크 노드까지의 최대 거리
	double maxEC = 0; // 후보 노드 중 최대 에너지 소모량

	for (int i = 0; i < numCandis[idx]; i++) // normalizing factor 계산
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{			
			double dis = candi->distance; // 후보 노드의 싱크 노드까지의 거리
			double eCnsmptn = candi->eCnsmptn; // 후보 노드의 에너지 소모량					

			if ( dis > maxDI) maxDI = dis;

			if ( eCnsmptn > maxEC) maxEC = eCnsmptn;
		}
	}
	
	for (int i = 0; i < numCandis[idx]; i++)
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{		
			double normDI = (candi->distance) / maxDI; // 후보 노드의 싱크 노드까지의 거리 (정규화)
			double normEC = candi->eCnsmptn; // // 후보 노드의 에너지 소모량 (정규화)

			if (maxEC ==0) normEC = 0;
			else
				normEC /= maxEC;

			eval_result[i] = eval ( normDI, normEC); // 평가 결과 도출, 저장
		}
	}
	
	// 후보 노드 목록 정렬
	this->quicksort( candis[idx], 0, numCandis[idx]-1); // 평가 결과에 따라 후보 노드 목록 정렬 (실제 후보 노드 수에 맞게 인자 전달 필요)

	// 다음 노드 목록 저장

	int numNxtNds = 0;

	if (numCandis[idx] < BRANCH_FACTOR)
	{
		numNxtNds = numCandis[idx];
	}
	else 
		numNxtNds = BRANCH_FACTOR;


		for (int i = 0; i < numNxtNds; i++) // 다음 전달 노드 배열에 노드 정보 저장
		{
			nxtNds[i] = candis[idx][numCandis[idx] -1-i];			
		}	

	// 다음 노드 목록 반환	
	return this->nxtNds[0];	
}




double Rtn_GEA::eval(KeyMsg* keyMsg, Node* candi)
{
	double res = 0;

	// ...

	return res;
}

double Rtn_GEA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) // 사용 안 함
{
	double res = 0;

	// ...

	return res;
}

double Rtn_GEA::eval (struct _tagEvntMsg* evntMsg, Node* candi) // 사용 안 함
{
	double res = 0;

	// ...

	return res;
}

double Rtn_GEA::eval (double dist, double eCnpmtn ) // 각 factor는 0~1 사이의 값으로 정규화
{
	double res = 0;

	res = (-1) * alpha_GEA * dist + (-1) * beta_GEA * eCnpmtn; // (2012.11.29 수정) 평가 함수 값 도출

	return res;
}
*/