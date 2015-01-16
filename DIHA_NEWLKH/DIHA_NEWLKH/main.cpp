// 2014.08.31
// 문수영
// main.cpp
// main 함수 정의 선언


#include <iostream>
#include <fstream>
#include <map>
#include "params.h"

#include "Node.h"
#include "BS.h"
#include <list>

#include "Atk.h"
#include "Atk_FRA.h"

#include "Kmr.h"
#include "Kmr_IHA.h"
#include "Kmr_NEWLKH.h"

#include "Rtn.h"
#include "Rtn_IHA.h"
#include "Rtn_DIHA.h"
#include "Rtn_SEF.h"

#include "Sec.h"
#include "Sec_IHA.h"
#include "Sec_DIHA.h"
#include "Sec_SEF.h"

#include "RAND.h"
// #include "RAND.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::ofstream;

Fuzzy fuzzy; // fuzzy 추론을 위한 Fuzzy 클래스 객체

RAND gen0; // 노드가 배치될 cell 결정을 위한 난수 생성
int seed0=1;

RAND gen1; // 노드 배치를 위한 난수 생성
int seed1=1;	

RAND gen2; // CoS 선택을 위한 난수 생성
int seed2=1;	

RAND gen3; // 위조 보고서 발생을 위한 난수 생성
int seed3=1;	

RAND gen4; // 노드 훼손을 위한 난수 생성

int seed4=1; 

 // Node 클래스의 정적 변수 선언
unsigned int Node::totNumNodes; // 현재까지 생성된 총 노드 수
unsigned int Node::numNodesEvctd; // 현재까지 제거된 노드 수

void deployNode ( Node* pNode, int xMin, int xMax, int yMin, int yMax); // 신규 노드를 필드에 배치 (위치 및 BS까지의 거리 결정)
void testNode(map<int, Node*> nodes); // 노드 정보 테스트
map<int, Node*> arrangeNodes( map<int, Node*> nodes); // 노드 정보 정리 (value = NULL인 요소 제거)

int main(int argc, char* argv[])
{
	/* 1. 시작 */
	cout << " /* 1. 시작 */ " << endl;

	// 난수 생성기 seed 설정

	gen0.setSeed(seed0);
	gen1.setSeed(seed1);
	gen2.setSeed(seed2);
	gen3.setSeed(seed3);
	gen4.setSeed(seed4);

	
	//gen0.testRand();
	//gen1.testRand();
	//gen2.testRand();
	//gen3.testRand();
	//gen4.testRand();
	

	// 성능 지표 측정을 위해 필요한 변수들 선언

	/// 홉 수 관련
		int totNumEvents = 0; // 현재까지 발생한 이벤트 갯수
		int numEventsInRound = 0; // (2014.09.24) 현재 라운드에서 발생한 이벤트 갯수

		int countedEvents = 0; // 평균 전달 홉 수를 구할 때 반영한 이벤트 보고서의 개수 (bs까지 도달하거나, 중간에 공격이 검출된 보고서의 개수)애

		double avgHopCnt = 0; // 보고서 평균 진행 홉 수
		double avgHopCntDetectedER = 0; // 중간 노드에 의해 검출된 위조 보고서의 평균 진행 홉 수

	/// cutoff 관련

		bool cutoffOccur = false;// 현재 보고서 전달 중 cutoff가 발생하였는지 여부
		bool noCutoff = true; // 시뮬레이션 시작부터 cutoff가 발생하지 않았는지 여부 (true: 한 번도 발생 안 함)

		int numCutoffs = 0; // (2013.11.28) cutoff 발생 횟수
		int numCutoffsCoS = 0; // (2014.09.30) (CoS의 에너지 고갈로 인한) cutoff 발생 횟수
		int numCutoffsFwd = 0; // (2014.09.30) (중간 노드의 에너지 고갈로 인한) cutoff 발생 횟수
		int numCutoffsIsol = 0; // (2014.09.30) (중간 노드의 고립으로 인한) cutoff 발생 횟수
		int numCutoffsInRound = 0; // (2014.09.24) 현재 라운드의 cutoff 발생 횟수
		double cutoffRatio = 0; // (2014.01.02) (cutoff횟수/ 이벤트발생횟수) 발생 비율

		map<int, bool> cutoff; // 각 Src cluster에서 cutoff가 발생하였는지 여부 <CH's ID, T/F>
		int numCutoffClusters = 0; // cutoff가 발생한 클러스터 개수		

		int firstCutoff = -1; // 첫 번째 클러스터에서 cutoff가 발생할 때까지의 시간 (loop 횟수)
		//int midCutoff = -1; // 중간 클러스터에서  ~
		int lastCutoff = -1; // 마지막 클러스터에서 ~

		double depletNodeRatio; // 에너지 고갈 노드의 비율
		
	/// 검증 관련	

		int totalNumVer = 0; // 총 검증 횟수

	// 성능 지표 측정을 위해 필요한 변수 선언 완료


	/* 2. 프로토콜 선택 및 초기화 */

	cout << " /* 2. 프로토콜 선택 및 초기화 */ " << endl;

	Atk* atk;
	Sec* sec;
	Kmr* kmr; // 키 관리 객체 
	Rtn* rtn;

	map<int, Node*> nodes; // 노드 맵 (노드 id, 노드 포인터) (현재 네트워크에 존재하는 노드)
	map<int, Node*> delNodes; // 노드 맵 (필드에서 제거된 노드)

	IDSet addedNodes; // 현재 라운드에서 추가된 노드의 ID 집합
	IDSet deletedNodes; // 현재 라운드에서 제거된 노드의 ID 집합
	IDSet depletedNodes; // (2014.09.17) 기존 라운드에서 에너지가 고갈된 노드의 ID 집합
	IDSet compNodes; // (2014.09.24) 훼손된 노드의 ID 집합

	map<int, set<int>> clusters; // 클러스터 맵

	map<int, set<int>> srcClusters; // (2014.08.31) src 클러스터 맵 - CoS로 선택될 수 있는 CH들의 클러스터 (BS로부터 거리가 T+1홉 이상인 CH)

	BS bs; // BS

	list<Node*> fNodes; // 메시지 전달을 위한 전달 노드 목록

	try
	{	

		if ( strcmp(argv[1], "IHA") == 0) // 키 관리 기법 선택
			kmr = new Kmr_IHA();
		else if ( strcmp(argv[1], "PRM") == 0)
			kmr = new Kmr_NEWLKH();			

		if ( strcmp(argv[2], "IHA") == 0) // 라우팅 프로토콜 선택
			rtn = new Rtn_IHA();
		else if ( strcmp(argv[2], "PRM") == 0) 
			rtn = new Rtn_DIHA();		
		else if ( strcmp(argv[2], "SEF") == 0) 
			rtn = new Rtn_SEF();

		if ( strcmp(argv[3], "IHA") == 0) // 보안 프로토콜 선택
			sec = new Sec_IHA();
		else if ( strcmp(argv[3], "PRM") == 0)
			sec = new Sec_DIHA();
		else if ( strcmp(argv[3], "SEF") == 0)
			sec = new Sec_SEF();

		if ( strcmp(argv[4], "FRA") == 0) // 공격 프로토콜 선택
			atk = new Atk_FRA();
		else 
		{
			cerr << "공격 프로토콜이 잘못된 유형" << endl;
		}
	}

	catch (bad_alloc & ba )
	{
		cout << " error 발생: " << endl;
		cout << ba.what() << endl;
	}
	
	kmr->setRtn(rtn); 
	kmr->setSec(sec);
	sec->setRtn(rtn);
	sec->setKmr(kmr);

	bs.setKmr(kmr);
	bs.setRtn(rtn);

	rtn->setBS(&bs); // (2014.08.07)
	rtn->setKmr(kmr);
	rtn->setFuzzy(&fuzzy);

	Sec_IHA* sec_IHA = dynamic_cast <Sec_IHA*> (sec);

	/* 3. 노드 객체 생성 및 초기화 */

	Node::totNumNodes = 0;

	int round = 0;
	
	ofstream out_event("tst_eventMsg.txt"); // 이벤트 정보 출력 파일 열기

	ofstream out_isolRatio("tst_isolRatio.csv"); // 고립 노드 (또는 CH) 비율 출력 파일 열기

	while (round < NUM_ROUNDS) // 실험이 종료될 때까지 라운드 반복
	{
		/* 3. 노드 추가/제거 */

		cout << "/* 3. 노드 추가/제거 */ " << endl;

		addedNodes.clear(); // 현재 라운드에서 추가된 노드의 ID 목록 리셋
		deletedNodes.clear(); // 현재 라운드에서 제거된 노드의 ID 목록 리셋

		  // 첫 번째 라운드인 경우, 초기 노드 수만큼 노드 객체 생성

		if (round ==0 )
		{
			int cnt = 0; // 생성된 객체의 수
			int numNodesPerCell =  NUM_NODES / NUM_CELL; // virtual grid에 존재하는 cell 하나 당 배치될 노드 수 계산

			//cout << "numNodesPerCell: " << numNodesPerCell << endl;  // 디버깅 용 코드 

			while (cnt < NUM_NODES) // 초기 노드 수 만큼 노드 객체를 생성하고 노드 맵에 추가
			{
				Node* pNode = new Node();				

				nodes.insert( make_pair ( Node::totNumNodes++, pNode));		

				addedNodes.insert ( pNode->id); // (2014.06.02) 현재 라운드에서 새로 추가된 노드 목록에 저장

				// 신규 노드의 위치 결정				
				
				//int cellID = gen0.genRandInt(NUM_CELL); // 노드가 추가될 cell ID 계산

				// (2014.09.17 수정) 최초 라운드에는 각 cell 마다 t+1개의 노드가 배치되도록 함
				int cellID = cnt % NUM_CELL; // 노드가 추가될 cell ID 계산

				//int cellID = cnt / numNodesPerCell; // 노드가 추가될 cell ID 계산
				pNode->cell = cellID;
				
				int xMin = (cellID % CELLS_PER_ROW) * CELL_WIDTH;
				int xMax = xMin + CELL_WIDTH;
				int yMin = (cellID / CELLS_PER_ROW) * CELL_HEIGHT;
				int yMax = yMin + CELL_HEIGHT;

				//cout << "cell iD: " << cellID; // 디버깅 용 코드 
				//cout << " node ID: " << pNode->id; // 디버깅 용 코드			

				deployNode(pNode, xMin, xMax, yMin, yMax); // 해당 cell 영역 내의 임의의 위치에 노드 배치
				/*
				if (gen4.genRandInt(100) < CNR ) // (2014.08.22) 일정한 확률로 훼손 노드로 지정
				{
					pNode->cmprmsd = true;
					compNodes.insert(pNode->id);
				}	
				*/
				pNode->setKmr(kmr);
				pNode->setRtn(rtn);
				pNode->setSec(sec);

				//cout << "\t (" << pNode->x << ", " << pNode->y << ") \n" ; // 디버깅 용 코드 
				cnt++;
			}

			//testNode(nodes); // 노드 정보 테스트
		}
		else // 두 번째 이후 라운드인 경우, 노드 추가/제거 비율을 고려하여 새로 추가 및 제거
		{
			// 노드 제거

			set<int>::iterator it_dpltdNode;

			for ( it_dpltdNode = depletedNodes.begin(); it_dpltdNode != depletedNodes.end(); it_dpltdNode++)
			{
				int nodeID = *it_dpltdNode;
				Node* node = nodes[nodeID];

				deletedNodes.insert( nodeID); // 제거된 노드 ID 목록에 고갈 노드의 id 추가
				delNodes.insert( make_pair( nodeID, node) ); // 제거된 노드 map에 고갈 노드 정보 추가
				nodes.erase (nodeID); // 노드 map에서 고갈 노드 정보 제거

				rtn->removeFromCluster (nodeID); // (2014.10.10)

				Node::numNodesEvctd++; // 제거된 노드 수 +1증가
			} // 고갈 노드 제거

			// (2014.09.24) 훼손된 노드 제거 과정 추가 예정

			set<int>::iterator it_compNode;

			for ( it_compNode = compNodes.begin(); it_compNode != compNodes.end(); it_compNode++) // 훼손 노드 목록 순회 - 제거 노드 목록에 추가
			{
				int nodeID = *it_compNode;
				Node* node = nodes[nodeID];

				deletedNodes.insert( nodeID); // 제거된 노드 ID 목록에 고갈 노드의 id 추가
				delNodes.insert( make_pair( nodeID, node) ); // 제거된 노드 map에 고갈 노드 정보 추가
				nodes.erase (nodeID); // 노드 map에서 고갈 노드 정보 제거
				Node::numNodesEvctd++; // 제거된 노드 수 +1증가
			} // for문 종료 -  훼손 노드 제거

			compNodes.clear(); // 훼손 노드 목록 리셋

			// 노드 추가
			
			int numAddedNodes = static_cast <int> (depletedNodes.size() + NUM_NODES * ((double) FTR/ 100 + 0.1)); // (2014.10.22) 추가되는 노드 수 = 고갈 노드 수 + 초기 노드 수 * 공격 비율

			//int numAddedNodes = NUM_NODES; // (2014.10.02) 초기 노드 수만큼 새로운 노드 추가

			//int numAddedNodes = deletedNodes.size(); // 제거된 노드 수만큼 새로운 노드 추가
			//int numAddedNodes = depletedNodes.size(); // 고갈된 노드 수만큼 새로운 노드 추가

			// (2014.09.24) 훼손된 노드 수만 큼 새로운 노드 추가 예정

			int cnt = 0;

			while (cnt < numAddedNodes)
			{
				Node* pNode = new Node();
				nodes.insert( make_pair ( Node::totNumNodes++, pNode));			

				addedNodes.insert ( pNode->id); // (2014.06.02) 현재 라운드에서 새로 추가된 노드 목록에 저장

				// 신규 노드의 위치 결정

				int numNodesPerCell =  numAddedNodes / NUM_CELL; // virtual grid에 존재하는 cell 하나 당 배치될 노드 수 계산				
				
				int cellID = gen0.genRandInt(NUM_CELL); // 노드가 추가될 cell ID 계산
				
				pNode->cell = cellID;

				int xMin = (cellID % CELLS_PER_ROW) * CELL_WIDTH;
				int xMax = xMin + CELL_WIDTH;
				int yMin = (cellID / CELLS_PER_ROW) * CELL_HEIGHT;
				int yMax = yMin + CELL_HEIGHT;

				deployNode(pNode, xMin, xMax, yMin, yMax); // 해당 cell 영역 내의 임의의 위치에 노드 배치

				/*
				if (gen4.genRandInt(100) < CNR ) // (2014.08.22) 일정한 확률로 훼손 노드로 지정
				{
					pNode->cmprmsd = true;
					compNodes.insert(pNode->id);
				}	
				*/
				pNode->setKmr(kmr);
				pNode->setRtn(rtn);
				pNode->setSec(sec);

				cnt++;
			} // 신규 노드 추가 완료		
			
			depletedNodes.clear(); // 고갈 노드 목록 리셋
		
		} // 노드 추가 및 제거 완료

		map<int, Node*>::iterator it_node;

		// 노드 훼손

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // (2014.10.02) 노드 훼손
		{
			Node* pNode = (*it_node).second;

			if (gen4.genRandInt(100) < CNR ) // 일정한 확률로 훼손 노드로 지정
			{
				pNode->cmprmsd = true;
				compNodes.insert(pNode->id);
			}	

		}// for문 - 노드 훼손 완료


		kmr->setNodes(nodes);// 객체 멤버 변수 설정		
		sec->setNodes(nodes);
		rtn->setNodes(nodes);
		atk->setNodes(nodes);

		// 노드 수 출력
		cout << "라운드: " << round << endl;
		cout << "\t노드 수: " << nodes.size() << endl << endl;

		testNode(nodes); // 노드 정보 테스트

		/* 3-2. Topology Request/Response 시작 */

		rtn->resetTopology(); // topology 관련 정보 리셋

		fNodes.clear(); // 전달 노드 목록 리셋
		map<int, bool> forwarded ;// 메시지 전달 여부 <node ID, T/F> - 메시지를 전달한 노드는 true로 설정				

		rtn->genTREQMsg(round); // TREQ 메시지 생성
		TREQ* treq = rtn->getTREQMsg(); // TREQ 메시지 획득

		//map<int, Node*>::iterator it_node;

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
		//for (unsigned int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
		{
			Node* pNode = (*it_node).second;

			if (pNode == NULL) // (2014.09.22) 
			{
				continue;
			}

			double dist = pNode->distance;

			if (dist < TRANS_RANGE) // BS에서 전송 범위 이내이면 전달 노드 목록에 추가
			{
				bs.addNbr(pNode); // BS의 이웃 노드 목록에 추가
				fNodes.push_back(pNode); // 전달 노드 목록에 추가
				forwarded[pNode->id] = true; // 전달 여부를 참으로 설정
			}	
			else
			{
				forwarded[pNode->id] = false; // 전달 여부를 거짓으로 설정
			}			
		}

		while( !fNodes.empty())
		{
			// 전달 노드 목록의 첫 번째 노드 획득
			Node* cNode = fNodes.front(); 
			fNodes.pop_front(); 

			rtn->updatTREQMsg(cNode); // TREQ 메시지 업데이트			
			
			list<Node*> nxtNds = rtn->selNext(treq, cNode); // 현재 노드의 다음 전달 노드 목록 획득

			cNode->txMsg(treq); // 현재 노드의 송신 함수 호출

			if ( !nxtNds.empty()) // 현재 노드의 다음 전달 노드들의 수신 함수를 호출하고 전달 노드 목록에 추가
			{
				list<Node*>::iterator it;

				for (it = nxtNds.begin(); it != nxtNds.end(); it++)
				{
					Node* nextNode = (*it);
					int nextNodeID = nextNode->id;
					nextNode->rxMsg(treq);

					if ( forwarded[nextNodeID] == false) // 전달 노드 목록에 포함된 적이 없으면 목록에 추가
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;
					}
				}// 현재 전달 노드의 다음 전달 노드들에게 메시지 전달 완료
			}

		} // TREQ 메시지 전달 완료

		// TRES 메시지 

		// 각 노드들에 대해, TREQ 메시지를 생성하고 BS 방향으로 전달 

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
		//for (unsigned int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
		{
			fNodes.clear();
			forwarded.clear();

			Node* sNode = (*it_node).second;

			if (sNode == NULL) // (2014.09.22) 
			{
				continue;
			}

			//Node* sNode = nodes[n];
			rtn->genTRESMsg(sNode); // source node의 TRES 메시지 생성

			TRES* tres = rtn->getTRESMsg(); // TRES 메시지 획득

			fNodes.push_back(sNode);
			forwarded[sNode->id] = true;

			while( !fNodes.empty())
			{
				// 전달 노드 목록의 첫 번째 노드 획득
				Node* cNode = fNodes.front(); 
				fNodes.pop_front(); 

				list<Node*> nxtNds = rtn->selNext(tres, cNode); // 현재 노드의 다음 전달 노드 목록 획득

				cNode->txMsg(tres); // 현재 노드의 송신 함수 호출

				list<Node*>::iterator it;

				for (it = nxtNds.begin(); it!= nxtNds.end(); it++)
				{
					Node* nextNode = (*it);
					int nextNodeID = nextNode->id;
					nextNode->rxMsg(tres);

					if ( forwarded[nextNodeID] == false) // 전달 노드 목록에 포함된 적이 없으면 목록에 추가
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;
					}
				}

			}
		} // TRES 메시지 전달 완료

		/* 3-2. Topology Request/Response 완료 */

		bool fwd = true;

		//rtn->testParentNodes();

		/* 4. 클러스터 재구성 */  //  기존 키 관리 기법의 경우에만 실행
		/*
		cout << "/ 4. 클러스터 재구성 /" << endl; 
		  // ch 선출, 클러스터 정보 저장
		
		rtn->setCHs(); 
		rtn->testCHs(); 
		
		// 5. 연관 노드 설정 // //  기존 키 관리 기법, IHA의 경우에만 실행
		
		cout << "/ 5. 연관 노드 설정 /" << endl;
		
		sec_IHA->resetAssoc(); // 이전 라운드의 연관 노드 정보 제거 (reset)

		/// BS_HELLO 메시지 생성 및 전달 
		
		sec->genCntrlMsg( BS_HELLO, &bs); // BS_HELLO 메시지 생성
		struct _tagCntrlMsg* cntlMsg; // 제어 메시지 주소

		cntlMsg = sec->getCntrlMsg(); // BS_HELLO 메시지 획득		

		/// 전달 노드 정보 리셋
		fNodes.clear();
		forwarded.clear();
		
		list<Node*> nxtNds; // 다음 전달 노드 목록
		
		nxtNds = rtn->selNext(cntlMsg, &bs); // 초기 전달 노드 목록 획득		

		list<Node*>::iterator it;

		//bool fwd = true;

		for (it = nxtNds.begin(); it!= nxtNds.end(); it++) // 초기 전달 노드들에 대해 반복
		{
			// BS -> {초기 전달 노드} 메시지 전달

			Node* nextNode = (*it);
			int nextNodeID = nextNode->id;

			/// BS -> 노드 전달 
			sec->setReceiver(cntlMsg, nextNode);
			bs.txMsg(cntlMsg, nextNode); // BS의 송신 함수 호출			
				
			bool fwd = true; // 계속 전달 여부
			fwd = nextNode->rxMsg(cntlMsg); // BS의 이웃 노드의 수신 함수 호출

			//if (fwd == false)
			{
			//	break;
			}

			if ( fwd == true && forwarded[nextNodeID] == false) // 전달 노드 목록에 포함된 적이 없으면 목록에 추가
			{
				fNodes.push_back(nextNode);
				forwarded[nextNodeID] = true;				
			}
		} // BS 메시지 초기 전달
		
		while ( !fNodes.empty() ) // 전달 노드 목록이 비어 있지 않을 동안 반복
		{
			 // 현재 노드 획득, 전달 노드 목록에서 제거 
			Node* cNode = fNodes.front(); 
			fNodes.pop_front();

			sec->genCntrlMsg(BS_HELLO, cNode); // 현재 노드에서 제어 메시지 재 생성
			cntlMsg = sec->getCntrlMsg();
			
			list<Node*> nxtNds = rtn->selNext(cntlMsg, cNode); // 현재 노드의 다음 전달 노드 목록 결정 및 획득
			
			list<Node*>::iterator it;

			for (it = nxtNds.begin(); it != nxtNds.end(); it++)  // 현재 노드의 다음 전달 노드들에 대해 반복
			{
				Node* nextNode = *it;
				int nextNodeID = nextNode->id;

				/// 메시지 송수신 
				sec->setReceiver(cntlMsg, nextNode);
				cNode->txMsg(cntlMsg); // 송신

				bool fwd = true; // 계속 전달 여부
				fwd = nextNode->rxMsg(cntlMsg); // 수신

				if (forwarded[nextNodeID]==false && fwd == true) // 수신 노드를 전달 노드 목록에 추가 (조건부)
				{
					forwarded[nextNodeID] = true;
					fNodes.push_back(nextNode);
				}		

			}
		}


		/// BS_HELLO 메시지 생성 및 전달 완료		

		/// Clstr_ACK 메시지 생성 및 전달 

		  // 클러스터 마다 반복

		map<int, set<int>>::iterator it_clusters;
		clusters = rtn->getClusters();

		for (it_clusters = clusters.begin(); it_clusters != clusters.end(); it_clusters++) // 클러스터마다 반복
		{
			
			if ( !(*it_clusters).second.empty()) // 해당 클러스터에 속한 노드가 1개 이상 존재할 경우
			{
				int CHID = (*it_clusters).first; // CH의 Id

				set<int> clusternodes = (*it_clusters).second;

				// 메시지 생성
				sec->setCntrlMsg(CHID); // cluster ID (= CH's ID) 설정
				sec->genCntrlMsg(CLSTR_ACK, nodes[CHID]); // Cluster ACK 메시지 생셩
				
				cntlMsg = sec->getCntrlMsg(); // CLSTR_ACK 메시지 획득		

				/// 전달 노드 정보 리셋 
				fNodes.clear();
				forwarded.clear();
				nxtNds.clear();
				

				nxtNds = rtn->selNext(cntlMsg, nodes[CHID]); // 초기 전달 노드 목록 획득
			
				list<Node*>::iterator it_nxtNds;

				bool fwd = true;

				for(it_nxtNds = nxtNds.begin(); it_nxtNds != nxtNds.end(); it_nxtNds++) // 초기 노드들에게 전달
				{
					// CH -> {초기 전달 노드} 메시지 전달
					Node* nextNode = (*it_nxtNds);
					int nextNodeID = nextNode->id;

					sec->setReceiver(cntlMsg, nextNode); // 수신 노드 설정
					
					nodes[CHID]->txMsg(cntlMsg); // 송신

					bool fwd = true;

					fwd = nextNode->rxMsg(cntlMsg); // 수신
					
					if ( fwd == true && forwarded[nextNodeID] == false) // 전달 노드 목록에 포함된 적이 없으면 목록에 추가
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;				
					}					

				} // 초기 노드들에게 전달 완료 // (2014.05.26) 여기까지 확인!!

				while ( !fNodes.empty() ) // 전달노드 목록 내 노드마다 반복
				{
					// 중간 노드 ->  {중간 전달 노드} 메시지 전달

					 // 현재 노드 획득, 전달 노드 목록에서 제거 
					Node* cNode = fNodes.front(); 
					fNodes.pop_front();

					sec->genCntrlMsg(CLSTR_ACK, cNode); // 현재 노드에서 제어 메시지 재 생성
					cntlMsg = sec->getCntrlMsg();
					// 18:14 여기까지 확인
					list<Node*> nxtNds = rtn->selNext(cntlMsg, cNode); // 현재 노드의 다음 전달 노드 목록 결정 및 획득
					// 20:12 여기까지 확인
					list<Node*>::iterator it;

					for (it = nxtNds.begin(); it != nxtNds.end(); it++)  // 현재 노드의 다음 전달 노드들에 대해 반복
					{
						Node* nextNode = *it;
						int nextNodeID = nextNode->id;

						/// 메시지 송수신
						sec->setReceiver(cntlMsg, nextNode);
						cNode->txMsg(cntlMsg); // 송신

						bool fwd = true; // 계속 전달 여부
						fwd = nextNode->rxMsg(cntlMsg); // 수신

						if (forwarded[nextNodeID]==false && fwd == true) // 수신 노드를 전달 노드 목록에 추가 (조건부)
						{
							forwarded[nextNodeID] = true;
							fNodes.push_back(nextNode);
						}		

					} // 현재 클러스터의 CLSTR_ACK 전달 완료

				}
			}
		} // for 문 종료
		*/
		// 4, 5단계 완료 - 기존 키 관리 기법의 경우에만 실행
		
		// Clstr_ACK 메시지 생성 및 완료
		

		//kmr->testPairKey(); // pair key 정보 확인

		/* 6. 키 그래프 (재) 구성 */
		
		cout << "/* 6. 키 그래프 (재) 구성 */" << endl;

		/*
		if (round > 0)
		{		
			kmr->dstrctKTree(); // 기존 키 트리 제거
		}
		*/

		if (round == 0) // 첫 번째 라운드의 경우, 초기 키 트리 구축
		{
			kmr->cnstrctKTree(); // 키 트리 구축
		}

		// 
		else
		{
			kmr->updateKTree(addedNodes, deletedNodes); // 키 트리 갱신 (또는 재구성)
		}
		

		kmr->printKTree(); // 키 트리 출력
		//cout << endl;

		// (2014.06.12) 여기까지 완료!

		//*

		// 계층 별 노드 수 출력
		/*
		cout << endl;

		for (int i = 0; i <= kmr->getKTreeHeight(); i++)
		{
			cout << "Level " << i << ": " ;
			cout << kmr->getNumNodesAtLevel(i) << endl;
		}

		/*/

		/* 7. NEW_LKH 실제 노드 대응 관계 */
		
		cout << "/* 7. leaf node - 실제 노드 대응 관계 설정*/" << endl;

		if (round == 0) // 첫 번째 라운드의 경우, 초기 물리적 그룹 형성
		{
			kmr->cnstrctLBHG();
		}

		else
		{
			kmr->updateLBHG(); // 이후 라운드의 경우, 물리적 그룹 갱신
		}

		kmr->printKTree();

		rtn->testCHs();

		double res = rtn->testConect();

		cout << "\n round " << round << ": Percentage of isolated CHs: " << res << endl << endl;
		
		out_isolRatio << res << ",";

		///** 연관노드 및 부모 CH 테스트 
		
		//sec_IHA->testAssoc(); // 연관 노드 테스트		

		rtn->testParentCHs(); // 부모 CH 노드 테스트
		
		///**

		/* 8. 그룹 키 갱신 */ 				
		
		cout << " /* 8. 그룹 키 갱신 */ " << endl;

		/// 키 재분배 메시지 생성
		kmr->genKeyMsg(); // 키 갱신 메시지 생성		


		map<int, Node*>::iterator it_nd;

		for (it_nd = nodes.begin(); it_nd != nodes.end(); it_nd++) // 키 재분배 이후, 현재 라운드 노드들의 'newbee' 플래그를 false로 리셋 - 다음 라운드 키 재분배 목적
		{
			Node* pNode = it_nd->second;

			if (pNode == NULL) // (2014.09.22) 
			{
				continue;
			}

			pNode->newbee = false;
		}

		

		// (2014.07.31) 여기까지 확인!!

		/// 키 재분배 메시지 전달
		
		map<int, struct _tagKeyMsg*> keyMsgs = kmr->getKeyMsgs(); // 키 재분배 메시지 map 획득

		map<int, KeyMsg*>::iterator it_km; // 키 재분배 메시지 반복자

		for (it_km = keyMsgs.begin(); it_km != keyMsgs.end(); it_km++)
		//for (unsigned int km = 0; km < keyMsgs.size(); km++) // 각각의 키 재분배 메시지를 BS에서 최종 수신 노드(들)에게 전달
		{
			int km = (*it_km).first;

			
			// 전달 노드 정보 리셋 
			fNodes.clear();
			forwarded.clear();

			// 다음 전달 노드 - {목적 노드} map 선언

			map<int, list<Node*> > recipentMap;

			// 초기 전달 노드 목록 획득 
			KeyMsg* keyMsg = keyMsgs[km]; // 키 메시지 획득
			
			// (2014.08.07) debugging용 - 키 재분배 메시지의 목적 노드가 2개 이상일 경우 전달 과정
			if (keyMsg->recipents.size() > 1) 
			{
				;//cout << "keyMsg->recipents.size() > 1" << endl ; // do nothing (for debugging)
			}
			
			
			list<Node*> nxtNds = rtn->selNext(keyMsg, &bs); // BS의 다음 전달 노드 목록 획득 	// 초기 전달 노드 목록 획득
			
			list<Node*>::iterator it;

			bool fwd = true;

			for (it = nxtNds.begin(); it!= nxtNds.end(); it++)
			{
				Node* nextNode = (*it);

				if (nextNode == NULL)
				{
					continue;
				}

				int nextNodeID = nextNode->id;

				// BS -> 노드 전달 //

				bs.txMsg(keyMsg, nextNode); // BS의 송신 함수 호출
				//bs.txMsg(keyMsg); // BS의 송신 함수 호출
				
				bool fwd = true; // 계속 전달 여부

				fwd = nextNode->rxMsg(keyMsg); // BS의 이웃 노드의 수신 함수 호출

				if (fwd == false)
				{
					//break;
					continue;
				}

				if ( fwd == true && forwarded[nextNodeID] == false) // 전달 노드 목록에 포함된 적이 없으면 목록에 추가
				{
					fNodes.push_back(nextNode);
					forwarded[nextNodeID] = true;
					kmr->storeKMsg(keyMsg, nextNode); // 수신된 키 재분배 메시지 저장	
				}
			}		

			// 노드 -> 노드 전달 
			while( !fNodes.empty())
			{
				// 전달 노드 목록의 첫 번째 노드 획득
				Node* cNode = fNodes.front(); 
				fNodes.pop_front(); 

				kmr->restoreKMsg(keyMsg, cNode); // 이전에 저장되었던 키 재분배 메시지 복원 (현재 노드 기준)

				list<Node*> nxtNds = rtn->selNext(keyMsg, cNode); // 현재 노드의 다음 전달 노드 목록 획득
				
				//cNode->txMsg(keyMsg); // 현재 노드의 송신 함수 호출

				list<Node*>::iterator it;

				for (it = nxtNds.begin(); it!= nxtNds.end(); it++)
				{
					Node* nextNode = (*it);

					if (nextNode == NULL) // (2014.09.30) 
					{
						continue;
					}

					int nextNodeID = nextNode->id;

					cNode->txMsg(keyMsg, nextNode); // 현재 노드의 송신 함수 호출

					bool fwd = true; // 계속 전달 여부
					
					fwd = nextNode->rxMsg(keyMsg);				
					
					if (fwd == false)
					{
						break;
					}

					if ( fwd == true && forwarded[nextNodeID] == false) // 전달 노드 목록에 포함된 적이 없으면 목록에 추가
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;
						kmr->storeKMsg(keyMsg, nextNode); // 수신된 키 재분배 메시지 저장	
					}
				}
				kmr->delRecvdKMsg(cNode); // 현재 노드의 수신된 키 재분배 메시지 제거
			}
		} // for문 완료 - 키 갱신 메시지 전달 완료
		
		// ... 키 갱신 메시지 전달 완료

		rtn->resetUpdatdClstr(); // 노드들의 클러스터 갱신 여부 리셋
		sec_IHA->resetUpdatedAssoc(); // 노드들의 연관 노드 갱신 여부 리셋

		kmr->delKeyMsg(); // 키 갱신 메시지 제거
		

		kmr->resetUpdatedKeys();// key tree의 각 k-node 갱신 여부 재 초기화 필요 (updated 멤버 변수)

		kmr->printReceivedNodes();
		kmr->resetReceivedNodes(); // 키 재분배 메시지 수신 노드 정보 리셋

		/// (2014.09.24) 이벤트 보고서 발생 전 고갈 노드 목록 구성 
		// 
		map<int, Node*>::iterator it_dpltdNode;

		for ( it_dpltdNode = nodes.begin(); it_dpltdNode != nodes.end(); it_dpltdNode++) // 
		{
			Node* node = (*it_dpltdNode).second;

			if (node == NULL) // (2014.09.22) 
			{
				continue;
			}

			if (node->depleted() == true) // 현재 노드의 에너지가 고갈된 경우
			{
				depletedNodes.insert(node->id); // 고갈 노드 목록에 해당 노드의 ID 추가
			} // 고갈 노드를 목록에 추가

		} // 고갈 노드 목록 구성

		depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();

		///  (2014.09.24) 수정 완료

		// 9. 이벤트 보고서 생성 

		cout << " /* 9. 이벤트 보고서 생성  */ " << endl;

		int event_no = 0;
		
		numCutoffsInRound = 0; // (2014.09.24) 현재 라운드 내 cutoff 횟수 리셋
		numEventsInRound = 0; // (2014.09.24) 현재 라운드 내 이벤트 발생 횟수 리셋

		//map<int, set<int>> clusters;
		set<int> cluster;

		map<int, set<int>>::iterator it_clstr;

		Node* CoS;

		int chIdx=0;

		EvntMsg evntMsg; // 이벤트 보고서
	
		rtn->arrangeClusters(); // 클러스터 목록 정리 (크기가 0인 클러스터 정보 제거)
		clusters = rtn->getClusters(); // 클러스터 목록 획득		

		// (2014.08.31 수정) srcCluster 목록 구성

		srcClusters.clear();

		for(it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++ ) // cluster 목록 중 CH가 BS에서 (T+1) 홉 이상 떨어져 위치한 클러스터 정보를 저장
		{
			int CHID = (*it_clstr).first;
			set<int> cluster = (*it_clstr).second;

			// (2014.09.21) 수정 - 		

			if ( nodes[CHID] == NULL) // CH가 이미 제거된 노드이면 다음 클러스터로 이동
			{
				continue;
			}
		// (2014.09.21 수정 완료) CH가 이미 제거된 노드이면 다음 클러스터로 이동

			//if ( nodes[CHID]->distance >= (INTERLEAVING_T+1) * TRANS_RANGE)
			
			if ( nodes[CHID]->x <= 50 ||  nodes[CHID]->x >= (FIELD_WIDTH-50) || nodes[CHID]->y <= 50 || nodes[CHID]->y >= FIELD_HEIGHT-50)			
			{
				srcClusters[CHID] = cluster;
			}

		}
		// (2014.08.31 수정 완료) srcCluster 목록 구성 완료



		//kmr->testPairKey(); // pair key 정보 테스트



		cutoffRatio = 0;

		//for (event_no = 0; event_no < 0; event_no++)
		//for (event_no = 0; event_no < 10; event_no++)
		for (event_no = 0; event_no < NUM_EVENTS; event_no++) // 한 라운드 당 NUM_EVENTS 만큼의 이벤트 발생
		{
			// 이벤트 보고서 생성 
			 
			cutoffOccur = false;
			
			  // 이벤트 발생 - CH들중 하나를 랜덤하게 선택

			totNumEvents++; // 이벤트 발생 횟수 +1 증가
			numEventsInRound++; // (2014.09.24) 현재 라운드 내 이벤트 발생 횟수 +1 증가

			if (totNumEvents % 500 == 0)
			{

				cout << "-periodic report-" << endl;
				cout << "event no. " << totNumEvents << " (" << numEventsInRound << ") " <<  endl;

				cout << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";

				cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;

				cout << "\t cutoff ratio: " << cutoffRatio << endl;

			}
			// 클러스터 목록 정리 - 크기가 0인 클러스터 정보를 클러스터 목록에서 제거			

			CoS = NULL;					
			
			//* (2014.08.31) Cos 결정 과정 개선 (BS로부터의 거리가 t+1 이상인 CH만 선택

			chIdx = gen2.genRandInt(srcClusters.size()) ; // 몇 번째 CH를 선택할 것인지를 확률적으로 결정

			int t_chIdx = 0; // 임시 변수

			for(it_clstr = srcClusters.begin(); it_clstr != srcClusters.end(); it_clstr++ ) // 랜덤하게 선택된 CH 정보 획득
			{
				if ( (*it_clstr).second.size() ==0) // 크기가 0이면 skip
				{
					continue;
				}

				if ( t_chIdx == chIdx )
				{		
					int CHID =  (*it_clstr).first;
					
					CoS = nodes[CHID];
					cluster = (*it_clstr).second;
				}

				t_chIdx++;
			}// for문 종료 - CoS 결정			


			// CoS의 에너지 고갈 여부 검사
			if ( CoS->depleted() == true) // CoS의 에너지가 고갈된 경우, cutoff 발생
			{
				cutoffOccur = true; // CoS의 에너지 고갈로 인한 cutoff 발생 - cutoff 발생 여부를 '참'으로 설정
				numCutoffsCoS++; // CoS의 에너지 고갈로 인한 cutoff 발생횟수 +1 증가

				if (cutoff[CoS->id] == false) // 해당 클러스터의 cutoff 발생 여부를 '참'으로 설정, cutoff 발생 클러스터 수 +1 증가
				{
					cutoff[CoS->id] = true;
					numCutoffClusters++;
				}				

				// (2014.09.24 수정)
				depletedNodes.insert(CoS->id); // 고갈 노드 목록에 해당 노드의 ID 추가
			

				// 전체 노드에 대한 고갈 노드 수의 비율 계산
				depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();
				
			
			} // if문 종료 - CoS의 에너지가 고갈된 경우 처리 완료

			Node* cur = CoS; // 현재 노드 선택		

			if (cutoffOccur == false) // (2014.09.24) CoS에서 cutoff가 발생하지 않은 경우에만 보고서 생성, 전달 등의 과정을 수행
			{				

				// 보고서 생성 

				sec->initEventMsg(&evntMsg, CoS); // 이벤트 보고서 초기화
				//sec->writeEventMsg(out_event, totNumEvents, &evntMsg); // 파일에 이벤트 보고서 내용 출력

				// 기존 MAC 제거

				list<MAC*>::iterator itMAC = evntMsg.macs.begin(); 

				for (itMAC; itMAC != evntMsg.macs.end(); itMAC++) // 이벤트 보고서 내 기존 MAC을 위한 메모리 반환
				{
					MAC* mac = *itMAC;

					if (mac != NULL)
					{
						delete (mac);
					}
				} // FOR문 종료 - 기존 MAC을 위한 메모리 반환 완료

				evntMsg.macs.clear(); // 이벤트 보고서 내 기존 MAC 목록 제거
				//sec->resetTmpLst(); // 기존 임시 MAC 목록 리셋

				/// 다음 전달 노드 결정

				Node* next = rtn->selNext(&evntMsg, cur); // Rtn - 수신 노드 선택

				evntMsg.receiver = next; // 수신 노드 설정

				/// 다음 전달 노드를 고려하여 보고서 내 MAC 설정
				sec->setMAC(&evntMsg, CoS->id); 

				// 위조 보고서 비율에 따라 랜덤하게 위조 MAC 설정

				if ( gen3.genRandInt(100) < FTR) // FTR 비율만큼 확률적으로 위조 보고서 공격 발생
				{
					atk->forgeEvntMsg(&evntMsg); // Atk - 위조 보고서 공격 발생	
					//cout << "false report" << endl;
				}
				else
				{
					;//cout << "normal report" << endl;
				}


				sec->writeEventMsg(out_event, totNumEvents, &evntMsg); // 파일에 이벤트 보고서 내용 출력

				// (2014.08.20) 여기까지 확인!!

				// 10. 보고서 전달 및 필터링 		

				//cout  << "/* 10. 보고서 전달 및 필터링 	*/ " << endl;

				while (next != NULL) // 다음 전달 노드가 NULL이 아니면 반복
				{
					//sec->resetTmpLst(); // 기존 임시 MAC 목록 리셋

					

					if (cutoffOccur == true) // 이벤트 보고서 전달 중 cutoff 발생하면 while문 탈출
					{
						break; //  while 문 탈출
					}

					cur->txMsg(&evntMsg); // 현재 노드의 송신 함수 호출

					if ( next->rxMsg(&evntMsg) == false) // 다음 노드의 수신 함수 호출 - 반환값에 따라 계속 전달 여부를 결정
					{
						fwd = false; // 계속 전달 안 함
					}
					else
					{
						fwd = true; // 계속 전달
					}			

					cur = next; // 현재 노드 갱신					

					if (fwd == false) // 수신 함수 반환값이 false이면 전달을 중단함
					{
						//evntMsg.macs.clear();
						//sec->resetTmpLst(); // 기존 임시 MAC 목록 리셋
						break;
					}

					evntMsg.sender = cur; // 송신 노드 설정

					next = rtn->selNext(&evntMsg, cur); // 다음 전달 노드 선택
					evntMsg.receiver = next; // 수신 노드 설정			

					if (next == NULL) // (2014.08.28 수정)
					{
						break;
					}

					sec->updateMAC(&evntMsg); // 갱신된 현재 노드의 MAC 갱신 함수 호출

					sec->writeEventMsg(out_event, totNumEvents, &evntMsg); // 파일에 이벤트 보고서 내용 출력

					if (next->depleted() == true) // 다음 노드의 에너지가 고갈된 경우, 관련 정보 저장			
					{					
						cutoffOccur = true; // 중간노드의 에너지 고갈로 인한 cutoff 발생 - cutoff 발생 여부를 '참'으로 설정
						//numCutoffsFwd++; // (2014.09.30) 중간 노드의 에너지 고갈로 인한 cutoff 발생 횟수 +1 증가

						if (cutoff[CoS->id] == false) // 해당 클러스터의 cutoff 발생 여부를 '참'으로 설정, cutoff 발생 클러스터 수 +1 증가
						{
							cutoff[CoS->id] = true;
							numCutoffClusters++;
						}				

						depletedNodes.insert(next->id); // (2014.09.24) 고갈 노드 목록에 다음 노드 ID 추가
						
						break; // 보고서 전달을 위한 반복문 탈출
					} // 다음 노드 에너지 고갈 시, 관련 정보 저장

				} // 이벤트 보고서 전달 완료

				// 보고서에 남아 있는 MAC의 메모리를 반환하고 목록에서 제거

				totalNumVer += evntMsg.numVer; // 보고서 검증 횟수 누적				
			
				/*
				if ( evntMsg.detected == true)
				{
					avgHopCntDetectedER+= evntMsg.hop; // 검출된 위조 보고서의 홉 수 증가
				}

				if ( evntMsg.detected == true || (cur->distance) < TRANS_RANGE ) // 보고서 전달 과정에서 공격이 검출되었거나 BS까지 전달 되었다면 평균 전달 홉 수에 누적
				//if ( sec->isAtkDtctd()== true || (cur->distance) < TRANS_RANGE ) // 보고서 전달 과정에서 공격이 검출되었거나 BS까지 전달 되었다면 평균 전달 홉 수에 누적
				{
					countedEvents++; // 평균 전달 홉 수에 반영될 보고서 개수 +1 증가
					avgHopCnt += evntMsg.hop;	// 보고서 전달 홉 수 누적
				}

				// 공격이 검출되지 않고 현재 노드가 BS에서 전송 범위 이내이면 BS에게 전송

				if ( evntMsg.detected == false && (cur->distance) < TRANS_RANGE )
				{
					cur->txMsg(&evntMsg);	
					rtn->numEvntRcvd++; // BS에서 수신된 보고서 개수 +1 증가
				}
				*/


			} // (2014.09.24 수정 완료) CoS에서 cutoff 가 발생하지 않은 경우에 대한 처리
			
				if (evntMsg.detected == true) // 공격이 검출된 경우
				{
					countedEvents++; // 평균 전달 홉 수에 반영될 보고서 개수 +1 증가
					avgHopCnt += evntMsg.hop;	// 보고서 전달 홉 수 누적
					avgHopCntDetectedER+= evntMsg.hop; // 검출된 위조 보고서의 홉 수 증가
				}
				else // 공격이 검출 안 된 경우
				{
					if (cur->distance < TRANS_RANGE)
					{
						avgHopCnt += evntMsg.hop;	// 보고서 전달 홉 수 누적
						//avgHopCntDetectedER+= evntMsg.hop; // 검출된 위조 보고서의 홉 수 증가

						countedEvents++; // 평균 전달 홉 수에 반영될 보고서 개수 +1 증가

						cur->txMsg(&evntMsg); // BS까지 보고서 전달
						rtn->numEvntRcvd++; // BS에서 수신된 보고서 개수 +1 증가
					}
					else // 공격이 검출되지 않았음에도 BS까지 도달하지 못한 보고서
					{
						numCutoffs++; //
						numCutoffsInRound++;
						numCutoffsFwd++; // (2014.09.30) 중간 노드의 에너지 고갈로 인한 cutoff 발생 횟수 +1 증가

						if (cutoffOccur == false)
						{
							numCutoffsIsol++; // (2014.09.30) 노드의 고립으로 인한 cutoff 발생 횟수 +1 증가
						}

						depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();

						cutoffRatio = (double) numCutoffsInRound / (double) numEventsInRound; // (2014.09.24) 현재 라운드 내에서 발생한 이벤트 중 cutoff의 비율 계산

						if (noCutoff == true)
						{
							noCutoff = false;
							firstCutoff = totNumEvents; // first cutoff 기록		
						}
						else if ( cutoffRatio >= Exit_Cond)
						{
							cout << "\n****Current round exit condition****" << endl;

							lastCutoff = totNumEvents; // last cutoff 기록

							cout << endl << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";
					
							cout << ", event no.: " << totNumEvents << "( " << numEventsInRound << ")" ;				

							cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;

							cout << "\t cutoff ratio: " << cutoffRatio << endl;
							break;
						}

					} // if-else 문 종료 - 공격이 검출되지 않은 경우, BS까지 도달한 경우와 그렇지 않은 경우 (cutoff) 처리 완료

				} // IF-ELSE 문 종료 - 공격이 검출된 경우와 그렇지 않은 경우 처리 완료


			
			/*
			if (cutoffOccur == true) // 현재 이벤트 보고서에 대해 cutoff가 발생한 경우, cutoff 관련 지표 저장
			{
				numCutoffs++;
				numCutoffsInRound++;

				//cout << endl << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";

				//cout << ", event no.: " << totNumEvents << "( " << numEventsInRound << ")" ;

				depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();

				//cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;
				
				
				cutoffRatio = (double) numCutoffsInRound / (double) numEventsInRound; // (2014.09.24) 현재 라운드 내에서 발생한 이벤트 중 cutoff의 비율 계산
				//cutoffRatio = numCutoffs / totNumEvents;

				//cout << "\t cutoff ratio: " << cutoffRatio << endl;

				if (noCutoff == true)
				{
					noCutoff = false;
					firstCutoff = totNumEvents; // first cutoff 기록		
				}
				else if ( cutoffRatio >= Exit_Cond)
				{
					lastCutoff = totNumEvents; // last cutoff 기록

					cout << endl << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";
					
					cout << ", event no.: " << totNumEvents << "( " << numEventsInRound << ")" ;				

					cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;

					cout << "\t cutoff ratio: " << cutoffRatio << endl;
					break;
				}
				
			} // cutoff 관련 성능 지표 계산	, 다음 라운드로 이동
			*/			
			

		} // for - 이벤트 발생 종료

		// 11. 실험 종료 조건 확인 			

		//break; // (2014.10.20) 실험 종료 - 이벤트 발생 전 단계까지의 에너지 소모량 측정 목적

		if ( (++round > NUM_ROUNDS) )// cutoff 비율이 종료 조건을 넘어섰거나 모든 라운드를 진행한 경우, 시뮬레이션 종료)
		//if ( (lastCutoff > 0) || (++round > NUM_ROUNDS) )// cutoff 비율이 종료 조건을 넘어섰거나 모든 라운드를 진행한 경우, 시뮬레이션 종료)
		{
			break;
		}
		
		else
		{
			// 다음 라운드로 전이하기 전 노드들의 신규 편입 여부를 false로 설정함 (키 재분배 목적)

			for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
			//for (unsigned int nd = Node::numNodesEvctd; nd < Node::totNumNodes; nd++) 
			{
				Node* pN = (*it_node).second;

				if (pN == NULL)
				{
					continue;
				}

				//Node* pN = nodes[nd];
				pN->newbee = false; // 
			}
		}
		
	} //  round 종료
	
	out_event.close(); // 이벤트 정보 출력 파일 닫기
	out_isolRatio.close(); // (2014.10.31) 고립 노드 (또는 CH) 비율 출력 파일 닫기

	/* 12. 통계 지표 도출, 기록 */

	/// 공격 검출 관련

	int numAtk = atk->getNumAtk();
	int numDtc = sec->getNumDtctn();

	if (numDtc > 0)
	{
		avgHopCntDetectedER /= (double) numDtc;
	}
	

	/// 이벤트 발생 횟수 - 전체, 부분 (BS 도달, 검출, CUTOFF)

	cout << "\ntotal number of events: " << totNumEvents << endl;	

	cout << "\n 1) BS에서 수신된 이벤트 보고서 개수: " << rtn->numEvntRcvd << endl;

	cout << " 2) num detected reports: " << numDtc << endl;

	cout << " 3) num cutoffs: " << numCutoffs << endl;

	cout << " 3-1) cutoff (CoS) " << numCutoffsCoS << endl;

	cout << " 3-2) cutoff (Fwd) " << numCutoffsFwd-numCutoffsCoS << "( cutoff (isol) = " << numCutoffsIsol << " )" << endl;

	cout << "마지막 라운드의 고갈 노드 수 비율" << depletNodeRatio << endl; // (2014.12.12 추가)


	/// 평균 진행 홉 수

	avgHopCnt /= countedEvents; // 이벤트 보고서 평균 진행 홉 수 계산 후 출력
	
	
	cout << "\n countedEvents: " << countedEvents << endl;

	cout << "\n보고서 평균 진행 홉 수" << avgHopCnt;

	/// 노드들의 에너지 소모량 출력

	nodes = arrangeNodes(nodes); // node 목록 정리 ( Value = NULL 인 요소 제거)

	int totalNumNodes = nodes.size() + delNodes.size();

	cout << "\n 전체 노드 수: " <<  totalNumNodes;
	cout << "\n 총 에너지 소모량" << Node::totEnrgCnsmptn << ", 평균 에너지 소모량" << Node::totEnrgCnsmptn / totalNumNodes << endl;

	/// 네트워크 수명	


	cout << "first cutoff: " << firstCutoff;
	//cout << ", last cutoff: " << lastCutoff;

	
	/// 공격 관련

	cout << "\n num. of false reports: " << numAtk << endl;	

	if (numDtc >0)
	{
		cout << " avg hop count of detected false reports: " << avgHopCntDetectedER << endl;
	}
	/* 13. 종료 */

	// 노드 객체 메모리 반환

	map<int, Node*>::iterator it_nodes;

	for (it_nodes = nodes.begin(); it_nodes!= nodes.end(); it_nodes++)
	{
		Node* pNode = it_nodes->second;

		if (pNode != NULL)
		{
			delete (pNode);
		}
	}

	nodes.clear(); // 노드 맵 요소 제거

	for (it_nodes = delNodes.begin(); it_nodes!= delNodes.end(); it_nodes++)
	{
		Node* pNode = it_nodes->second;
		delete (pNode);
	}
	 
	delNodes.clear(); // 노드 맵 요소 제거


	char temp_c;
	cin >> temp_c; // 프로그램이 디버그 모드에서 바로 종료되는 것을 방지

	return 0;
	


} // main 함수 완료

void deployNode ( Node* pNode, int xMin, int xMax, int yMin, int yMax)
{	
	int rand_x = xMin + gen1.genRandInt(xMax - xMin);
	int rand_y = yMin + gen1.genRandInt(yMax - yMin);

	pNode->x = rand_x;
	pNode->y = rand_y;

	pNode->distance = sqrt( pow( (double) pNode->x - BS_X , 2) + pow ( (double) pNode->y - BS_Y , 2) );
	
}

void testNode(map<int, Node*> nodes)
{
	ofstream out("tst_nodes.txt");

	map<int, Node*>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++)
	{
		Node* pNode = NULL;
		pNode = it->second;

		if (pNode ==NULL) // (2014.09.22) 
		{
			continue;
		}

		out << "node ID: " << it->first;
		//cout << "node ID" << it->first;

		out << "cell ID: " << pNode->cell << endl;

		out << "\t (  " << pNode->x << ", " << pNode->y << ") " ;
		//cout << "\t ( << " << pNode->x << ", " << pNode->y << ") \n" ;



		out << ", distance: " << (int) pNode->distance << endl;

		if ( pNode->cmprmsd == true)
		{
			out << "compromised " << endl;
		}
	}

	out.close();
}

map<int, Node*> arrangeNodes( map<int, Node*> nodes) // nodes map 에 포함된 요소들 중 Node 포인터가 NULL인 요소들 제거
{
	set<int> entitiesToBeDeleted;

	map<int, Node*>::iterator it;
	set<int>::iterator it_set;

	for (it = nodes.begin(); it != nodes.end(); it++) // nodes map의 요소들 중 value = NULL 인 요소가 있으면 해당 key를 제거 목록에 추가
	{
		Node* node = (*it).second;

		if (node == NULL)
		{
			entitiesToBeDeleted.insert((*it).first);
		}
	}//  for문 - // nodes map의 요소들 중 value = NULL 인 요소가 있으면 제거 목록에 추가

	for (it_set = entitiesToBeDeleted.begin(); it_set != entitiesToBeDeleted.end(); it_set++)
	{
		int key = *it_set;

		nodes.erase(key);
	}

	return nodes;
}