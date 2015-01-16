// 2014.05.27
// 문수영
// Sec_SEF.cpp
// Sec_SEF 클래스 정의


#include "Sec_SEF.h"
#include <iostream>

using namespace std;

void Sec_SEF::setMAC (EvntMsg* msg)
{
	// 수정 필요 - 노드 iD로부터 클러스터 ID 구하는 과정

	/*
	int clstrID = (msg->CoS->id) / NODES_PER_CLSTR; // (2012.11.26 수정) 클러스터의 ID는 노드 id를 클러스터 당 노드 수로 나눈 몫

	setMAC(msg, clstrID);
	*/
}

void Sec_SEF::setMAC (EvntMsg* msg, int clstrID) // (2014.08.20)
{
	// 기존에 남아 있는 MAC 제거 (메모리 반환)

	list<MAC*>::iterator itMAC = msg->macs.begin();

	for ( itMAC; itMAC!= msg->macs.end(); itMAC++)
	{
		MAC* mac = *itMAC;
		delete mac;
	}

	msg->macs.clear(); 

	// hop count가 0인지 확인하고 그렇지 않다면 함수 반환

	if (msg->hop != 0)
	{
		return;
	}

	// 보고서 생성에 참여하는 노드들의 ID 확보 ( 상황에 따라 t+1 보다 작은 값을 가질 수 있음)

	list<int> sourceNodes; // 보고서 생성에 참여하는 노드들의 ID 목록	

	set<int> clstrNodes = rtn->getClstr(clstrID);  // 소속 노드들의 ID 추가 (최대 T개)

	set<int>::iterator it_clstrNodes;

	int idx = 1; // 보고서 생성에 참여하는 일반 (비 CH) 노드들의 인덱스

	for (it_clstrNodes = clstrNodes.begin();  it_clstrNodes != clstrNodes.end(); it_clstrNodes++) // 클러스터 소속 노드들 중 보고서 생성에 참여하는 노드들을 목록에 저장
	{
		int nodeID = *it_clstrNodes;

		if ( nodeID == clstrID)
		{
			continue;
		}

		sourceNodes.push_back(nodeID);

		if (idx >= INTERLEAVING_T) // 보고서 생성에 참여하는 노드의 수가 T+1개가 되면 반복문 탈출
		{
			break;
		}

	} // for문 - 보고서 생성에 참여하는 노드들을 목록에 저장

	sourceNodes.push_back(clstrID); // CH의 id 추가

	msg->prevNodes = sourceNodes;// 이벤트 보고서 내에 이전 노드 정보 저장

	if (msg->receiver == NULL) // 메시지 수신 노드가 존재하지 않으면 함수 종료
	{
		return;
	}


	// 보고서 생성에 참여하는 노드들 각각에 대해 반복

	list<int>::iterator it_sourceNode;

	for (it_sourceNode = sourceNodes.begin(); it_sourceNode != sourceNodes.end(); it_sourceNode++) // 보고서 생성 참여 노드 순회 - MAC 생성 및 저장
	{
		int sourceNodeID = *it_sourceNode;

		// 현재 노드의 상위 연관 노드 목록 획득
		set<int> upNodes = upperNodes[sourceNodeID][INTERLEAVING_T +1];

		set<int>::iterator it_upNode;

		for (it_upNode = upNodes.begin(); it_upNode != upNodes.end(); it_upNode++) // 현재 노드의 각 상위 연관 노드에 대해 반복
		{

			int upNodeID = *it_upNode;

			PairKey* pk = kmr->getPairKey(sourceNodeID, upNodeID); // pairwise key 획득

			MAC* mac = new MAC; // pairwise MAC 생성 및 추가

			mac->origin = nodes[sourceNodeID]; // MAC 생성 노드 설정
			mac->pKey = pk; // MAC의 pairwise key 설정
			mac->code = pk->kVal + msg->val; // 메시지 인증 코드 설정

			msg->macs.push_back(mac); // 보고서 내 MAC 목록에 신규 MAC 추가

			// 현재 노드의 MAC 생성으로 인한 에너지 소모량 누적

				nodes[sourceNodeID]->enrgyDiff = hashEnrg; // 노드의 현재 에너지 소모량
				nodes[sourceNodeID]->eCnsmptn += nodes[sourceNodeID]->enrgyDiff; // (2013.10.04) MAC 계산에 따른 에너지 소모량 누적
				Node::totEnrgCnsmptn += nodes[sourceNodeID]->enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)

				nodes[upNodeID]->remEnrgy -= nodes[upNodeID]->enrgyDiff; // (2013.10.04) MAC 계산에 따른 잔여 에너지 감소

		} // for문 - 현재 노드의 MAC 생성 및 저장	



	} // for문 - 보고서 생성 참여 노드 순회 - MAC 생성 및 저장 완료 


	/*
	// 기존에 남아 있는 MAC 모두 제거
	
	list<MAC*>::iterator itMAC = msg->macs.begin();

	for ( itMAC; itMAC!= msg->macs.end(); itMAC++)
	{
		MAC* mac = *itMAC;
		delete mac;
	}

	msg->macs.clear(); 

	// 클러스터 내 최소 ID, 최대 ID 계산
	if (msg->hop == 0) // // hop count가 0일 때만 보고서 내 MAC 설정
	{
		int minID = clstrID * NODES_PER_CLSTR; // 클러스터의 멤버 중 최소 ID
		int maxID = minID + NODES_PER_CLSTR - 1; // 클러스터의 멤버 중 최대 ID

		// 최소 id 부터 t+1개의 노드들에 대해 다음 과정 반복
		for (int i = 0; i < IHA_T+1; i++)
		{
			int cNodeID = minID + i; // 현재 노드의 ID

			IDLIST upNodes = upperNodes[cNodeID][0]; // 현재 노드의 상위 연관 노드 목록 획득

			IDLIST::iterator it = upNodes.begin();

			for (it; it !=upNodes.end(); it++) // 현재 노드의 각 상위 연관 노드에 대해 반복
			{
				int upNodeID = *it; // 상위 연관 노드 ID

				PairKey* pk = kmr->getPairKey(cNodeID, upNodeID); // pairwise key 획득
				MAC* mac = new MAC; // pairwise MAC 생성 및 추가

				mac->origin = (nodes + cNodeID); // (2013.09.02) MAC 생성 노드 설정
				mac->pKey = pk; // MAC의 pairwise key 설정
				mac->code = pk->kVal + msg->val; // 메시지 인증 코드 설정

				msg->macs.push_back(mac); // 보고서 내 MAC 목록에 신규 MAC 추가


				// (2013.10.31) 테스트용 - 임시로 주석 처리 
				nodes[cNodeID].enrgyDiff = hashEnrg; // 노드의 현재 에너지 소모량
				nodes[cNodeID].eCnsmptn += nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC 계산에 따른 에너지 소모량 누적
				Node::totEnrgCnsmptn += nodes[cNodeID].eCnsmptn; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)

				nodes[cNodeID].remEnrgy -= nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC 계산에 따른 잔여 에너지 감소
				
			}

			if (cNodeID == rtn->getCH(clstrID)->id ) // 현재 노드가 CH이면 BLOOM filter 사용에 따른 추가적인 에너지 소모 누적
			{
				int numMACs = msg->macs.size();

				// (2013.10.31) 테스트용 - 임시로 주석 처리
				nodes[cNodeID].enrgyDiff = (numMACs * bloom_filter_k) * hashEnrg; //(2013.10.04) MAC 개수 * bloom_filter_k 횟수만큼 해쉬 연산 수행
				nodes[cNodeID].eCnsmptn += nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC 검증에 따른 에너지 소모량 누적
				Node::totEnrgCnsmptn += nodes[cNodeID].eCnsmptn; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)

				nodes[cNodeID].remEnrgy -= nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC 검증에 따른 잔여 에너지 감소
				
			}

		}
	}	
	*/
}


bool Sec_SEF::verMAC(EvntMsg* msg) // (2013.08.20)
{
	bool vResult = true;

	// 보고서 내 MAC을 하나씩 검사	

	std::list<MAC*>::iterator it = msg->macs.begin();

	for (it; it != msg->macs.end(); it++)
	{
		MAC* mac = *it; // 현재 검사 중인 MAC

		PairKey* pKey = mac->pKey; // pairwise key

		// MAC 생성 노드와 현재 노드가 연관 관계에 있는지 검사

		Node* origin = mac->origin;
		Node* receiver = msg->receiver;

		if ((pKey->nID == origin->id && pKey->nID2 == receiver->id) || (pKey->nID == receiver->id && pKey->nID2 == origin->id))
		{
			// MAC 생성 노드와 현재 노드는 연관 관계에 해당함

			// MAC 코드 검증
			msg->numVer++; // (2013.10.29) 검증 횟수 증가

			int cNodeID = msg->receiver->id;			
			
			nodes[cNodeID]->enrgyDiff = hashEnrg; // MAC 계산 1회

			nodes[cNodeID]->eCnsmptn += nodes[cNodeID]->enrgyDiff;
			Node::totEnrgCnsmptn += nodes[cNodeID]->enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
			nodes[cNodeID]->remEnrgy -= nodes[cNodeID]->enrgyDiff;			
			

			int correctCode = pKey->kVal + msg->val; // 정상 코드

			if (mac->code == correctCode) // 검증 성공
			{
				// 아무것도 안 함
			}
			else // 검증 실패 (위조 보고서 탐지)
			{
				vResult = false;
				break;
			}
		}
	}

	return vResult; // 검증 결과 반환
	
}


void Sec_SEF::updateMAC(struct _tagEvntMsg* msg) // (2014.08.20)
{
	int cNodeID = msg->sender->id;	
	
	// 보고서 내 id 목록 갱신
	msg->prevNodes.pop_front(); // T+1 홉 이전 노드의 id 제거
	msg->prevNodes.push_back(msg->sender->id); // 송신 노드의 ID 추가


	// 자신과 상위 연관 노드가 공유하는 키로 MAC을 생성하고 보고서 내 mac 목록에 추가
	
	// (2014.04.25 수정 - 상위/하위 연관 노드 자료형 변경 - array of list -> map 

	std::set<int> upNodes = upperNodes[cNodeID][INTERLEAVING_T+1]; // 해당 노드의 (T+1) 홉 상위 노드 목록

	std::set<int>::iterator itID2 = upNodes.begin();
	

	for (itID2; itID2 != upNodes.end(); itID2++)
	{
		int upNodeID = *itID2; // 상위 연관 노드 ID
		PairKey* pk = kmr->getPairKey(cNodeID, upNodeID); // pairwise key 획득
		MAC* mac = new MAC; // pairwise MAC 생성 및 추가

		mac->origin = nodes[cNodeID]; // (2013.09.02) MAC 생성 노드 설정
		mac->pKey = pk; // MAC의 pairwise key 설정
		mac->code = pk->kVal + msg->val; // 메시지 인증 코드 설정

		msg->macs.push_back(mac); // 보고서 내 MAC 목록에 신규 MAC 추가
		//tmplst.push_back(mac); // 임시 목록에 신규 MAC 추가
		
	}

	/*
	msg->macs.clear(); // 보고서 내 MAC 목록 전부 제거

	//임시 목록의 요소들을 보고서 내 MAC 목록에 추가
	std::list<MAC*>::iterator itMAC2 = tmplst.begin();

	for ( itMAC2; itMAC2 != tmplst.end(); itMAC2++)
	{
		MAC* mac = *itMAC2;
		msg->macs.push_back (mac);
	}
	
	tmplst.clear();// 임시 목록 재 초기화
	*/

	/*  MAC 갱신 과정에서의 에너지 소모 (추후 필요 시 사용) */
	
	nodes[cNodeID]->enrgyDiff = hashEnrg; //(2014.08.21)  해쉬 연산 수행
	nodes[cNodeID]->eCnsmptn += nodes[cNodeID]->enrgyDiff; // (2013.10.04) MAC 생성에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += nodes[cNodeID]->enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)

	nodes[cNodeID]->remEnrgy -= nodes[cNodeID]->enrgyDiff; // (2013.10.04) MAC 검증에 따른 잔여 에너지 감소
	
}


void Sec_SEF::doEventMsgProc(struct _tagEvntMsg* msg) // (2014.08.21) 수신된 이벤트 보고서 처리
{	
	
	/// 현재 노드에 의해 검증된 MAC을 이벤트 보고서에서 제거 */
		

	int cNodeID = msg->receiver->id;
	
	std::list<MAC*>::iterator itMAC = msg->macs.begin();


	//list<MAC*> tmplst;

	for (itMAC; itMAC != msg->macs.end(); itMAC++) // 이벤트 보고서에 포함된 mac을 하나씩 검사	
	{
		MAC* mac = *itMAC; // 현재 검사중인 MAC

		//IDSet::iterator itID = lowerNodes[cNodeID][INTERLEAVING_T].begin(); // 보고서 수신 노드의 하위 연관 노드 목록의 iterator		

		bool matched = false; // MAC 생성 노드가 하위 연관 노드와 동일한 경우 true로 설정

		PairKey* pKey = mac->pKey; // pairwise key

		// MAC 생성 노드와 현재 노드가 연관 관계에 있는지 검사

		Node* origin = mac->origin;
		Node* receiver = msg->receiver;

		if ((pKey->nID == origin->id && pKey->nID2 == receiver->id) || (pKey->nID == receiver->id && pKey->nID2 == origin->id))
		{
			// MAC 생성 노드와 현재 노드는 연관 관계에 해당함. 해당 MAC을 보고서 내 목록에서 제거

			/*
			matched = true;
			delete (mac);// 해당 MAC 반환	
			*/

			tmplst.push_back(mac); // (2014.08.25) 검증된 MAC을 임시 목록에 추가하였다가 이벤트 보고서 내 mac 목록에서 제거하고 메모리 반환
		}	

		else
		{
			//tmplst.push_back(mac); // 검증된 MAC을 제외한 나머지 MAC은 임시 목록에 추가 (updateMAC() 함수에서 사용)
		}		
	}	

	// (2014.08.25 수정) 검증된 MAC의 메모리 반환, 목록에서 제거

	
	if ( !tmplst.empty() ) // 검증된 MAC이 존재 - 이벤트 보고서 내 MAC 목록에서 가장 앞에 위치한 mac
	{
		MAC* mac = * (msg->macs.begin()); // 가장 앞에 위치한 MAC의 메모리를 반환하고 목록에서 제거

		delete mac;

		msg->macs.pop_front();
	}
	
	tmplst.clear(); // 임시 MAC 목록 리셋
	
}

void Sec_SEF::writeEventMsg(ofstream& out, int event_no, struct _tagEvntMsg* msg) // (2014.08.11) 생성된 이벤트 보고서 테스트 (파일에 기록)
{
	out << "\nevent_no: " << event_no << endl;
	
	out << "\t (x, y) = (" << msg->x << ", " << msg->y << "), "; // 좌표

	out << " value = " << msg->val << endl; // 이벤트 값

	out << "\t 진행 홉 수 = " << msg->hop << endl; // 진행 홉 수

	out << "\tsender: " << msg->sender->id ; // 송신 노드

	if (msg->receiver != NULL) // 수신 노드
	{
		out << ", receiver: " << msg->receiver->id ;
	}

	out << "\n\n\tprev nodes: { " ; // 이전 노드들의 ID

	std::list<int>::iterator it_prevNodes;

	for (it_prevNodes = msg->prevNodes.begin(); it_prevNodes != msg->prevNodes.end(); it_prevNodes++) // 이전 노드 정보 출력
	{
		int nodeID = *it_prevNodes;

		out << nodeID << ", ";

	} // for 문 종료 - 이전 노드 정보 출력 완료

	out << "}" << endl; 

	std::list<MAC*>::iterator it_MAC;

	out << "\n\t MACs: " << endl;

	int mac_cnt = 0;

	for ( it_MAC = msg->macs.begin(); it_MAC != msg->macs.end(); it_MAC++) // MAC 정보 출력
	{
		MAC* mac = *it_MAC;

		out << "\n\t(" << mac_cnt << ")";

		Node* origin = mac->origin;

		out << "\n\torigin = " << origin->id;

		PairKey* pk = mac->pKey;

		out << "\n\tnID = " << pk->nID;
		out << "\n\tnID2 = " << pk->nID2;		
		out << "\n\tcode = " << mac->code;

		// MAC 생성 노드
	} //for 문 종료 - MAC 정보 출력 완료
	
	out << "\n\tnumVer: " << msg->numVer;

	out << endl;		// 보고서 내 MAC 검증 횟수
		
}

void Sec_SEF::genCntrlMsg(ctrlMsgType type, BS* bs) // (2014.05.20 추가) BS에서 제어 메시지 생성
{
	if (bs == NULL)
	{
		cout << "(bs == NULL)" << endl;
		exit(-1);
	}	

	// 제어 메시지 타입 설정

	cntlMsg->cmType = type; 
	cntlMsg->sender = NULL;
	cntlMsg->receiver = NULL;

	// 제어 메시지에 저장된 ID 목록 리셋
	for (int i = 1; i <= INTERLEAVING_T+1; i++)
	{
		//IDSet idSet = cntlMsg->IDSets[i];

		//if ( !idSet.empty() )
		{
			cntlMsg->IDSets[i].clear(); 
		}
	}

	// 연관 노드 목록은 중간 노드에서 추가됨 - BS에서는 재 초기화만 수행
}

void Sec_SEF::genCntrlMsg(ctrlMsgType type, Node* cNode) // (2013.08.23 추가) 노드에서 제어 메시지 생성
{
	if (cNode == NULL)
	{
		cout << "(cNode == NULL)" << endl;
		exit(-1);
	}

	/* 키 메시지 생성 함수 참조 */

	// 제어 메시지 타입 설정
	this->cntlMsg->cmType = type; 
	cntlMsg->sender = NULL;
	cntlMsg->receiver = NULL;

	// 제어 메시지에 저장된 ID 목록 리셋
	for (int i = 0; i <= INTERLEAVING_T+1; i++)
	{
		cntlMsg->IDSets[i].clear(); 
	}

	cntlMsg->sender = cNode; // 송신 노드 주소 저장

	int idx = cNode->id; 
	// 14:57 여기까지 확인
	if ( type == BS_HELLO) // BS_HELLO 메시지인 경우
	{
		// 노드 ID 목록 구성
	
		for (int i = 2; i <= INTERLEAVING_T+1; i++)
		{
			IDSet upNdsSet = upperNodes[idx][i-1]; // i-hop 거리에 위치한 상위 노드 집합

			// 목록이 비어 있는 경우 다음 loop로 이동
			if (upNdsSet.empty())
			{
				continue;
			}

			IDSet::iterator it = upNdsSet.begin(); // 첫 번째 요소에 대한 iterator 

			cntlMsg->IDSets[i] = upNdsSet; // i-hop 거리의 상위 노드 집합을 제어 메시지에 저장

			// 노드의 'i번째 상위 노드 목록'을 메시지의 'i-1번째 상위 노드 목록'에 저장 (1<= i <= IHA_T)	 				
		}
		//마지막 목록에 자신의 ID 추가		
		
		cntlMsg->IDSets[1].insert (cNode->id);
		//cntlMsg->IDLists[INTERLEAVING_T].push_front ( cNode->id );
			
	}	
	else if (type == CLSTR_ACK) // CLSTR_ACK 메시지인 경우
	{
		int cNodeID = 0;
		cNodeID = cNode->id;
				
		//cntlMsg->IDSets[1].insert( cNode->id );	 		
		//cntlMsg->clstrID = cNodeID;
		
		// 노드 ID 목록 구성

		int clstrID = cntlMsg->clstrID; // main 함수에 의해 Src 클러스터의 ID 필드가 미리 설정됨						

		if ( cNodeID == clstrID) // Src CH 인 경우
		{
			// Src 클러스터에 속한 노드들의 ID를 사용하여 하위 노드 목록 구성

			cntlMsg->IDSets[1].insert(clstrID);// CH의 id를 우선 저장

			int cnt = 1; // 제어 메시지에 저장된 노드 ID 개수
			//int hopCnt = 2; // Src CH의 다음 노드를 기준으로 했을 때 홉 거리 (하위) 
			int hopCnt = INTERLEAVING_T+1; // Src CH의 다음 노드를 기준으로 했을 때 홉 거리 (하위) 


			IDSet::iterator it;

			IDSet cluster = rtn->getClstr(clstrID);

			for (it = cluster.begin(); it != cluster.end(); it++)
			{
				int nID = *it;
				if (nID == clstrID) // CH의 id는 이미 저장했으므로 skip
				{
					continue;
				}

				cntlMsg->IDSets[hopCnt--].insert(nID);
				//cntlMsg->IDSets[hopCnt++].insert(nID);
				cnt++;
				
				if (cnt > INTERLEAVING_T) // 제어 메시지에 t+1개의 id가 저장되면 반복문 탈출
				{
					break;
				}					
			}
		}
		else // Src CH가 아닌 경우
		{
			// 기존에 저장된 하위 노드 목록을 사용하여 제어 메시지의 하위 노드 목록 구성				
			cntlMsg->IDSets[1].insert(cNodeID);// CH의 id를 우선 저장

			for (int hopCnt = 2; hopCnt <= INTERLEAVING_T+1; hopCnt++)
			{
				cntlMsg->IDSets[hopCnt] = lowerNodes[cNodeID][hopCnt-1]; // 하위 노드 목록 구성
			}
		}			
	}
	
}

void Sec_SEF::testCntrlMsg() // 제어 메시지 테스트 함수 재정의 (2013.09.09 추가)
{
	ofstream out("test_Sec_CntrlMsg.txt", std::ofstream::app);

	out << endl;

	if (this->cntlMsg->cmType == BS_HELLO)
	{
		out << "cmType = BS_HELLO\n";
	}
	else if (this->cntlMsg->cmType == CLSTR_ACK)
	{
		out << "cmType = CLSTR_ACK" << cntlMsg->clstrID << endl;
	}
	else
		out << "wrong message type!\n";

	if (cntlMsg->sender != NULL)
	{
		out << "sender: " << cntlMsg->sender->id;
	
		out << " (" << cntlMsg->sender->x << ", " << cntlMsg->sender->y << ") ";	
		// 송신 노드의 id list 출력

		out << "{ " ;

		for (int i = 0; i < INTERLEAVING_T+1; i++)
		{
			out << "{ " ;

			IDSet set;
			//IDLIST lst;

			if (this->cntlMsg->cmType == BS_HELLO)
			{
				set = upperNodes[cntlMsg->sender->id][i];
			}
			else if (this->cntlMsg->cmType == CLSTR_ACK)
			{
				set = lowerNodes[cntlMsg->sender->id][i];
			}

			if ( ! set.empty() ) // 송신 노드의 i번째 상위 (또는 하위) 연관 노드 목록을 출력
			{
				IDSet::iterator it = set.begin();

				for (it; it != set.end(); it++)
				{
					out << *it << ", ";
				}
			}

			out << "} " ;
		}

		out << "} " << endl; // 송신 노드의 상위 노드 목록 (IHA_T +1개) 출력 완료
	}
	if (cntlMsg->receiver != NULL)
	{
		out << "receiver: " << cntlMsg->receiver->id;
		out << " (" << cntlMsg->receiver->x << ", " << cntlMsg->receiver->y << ") " << endl;	
	}

	out << "ID sets: " << endl;

	for (int i = 0; i < INTERLEAVING_T +1; i++)
	{
		IDSet set = cntlMsg->IDSets[i]; // i 번째 IDLIST 획득
		//IDLIST lst = cntlMsg->IDLists[i]; // i 번째 IDLIST 획득

		out << " [ " << i << " ] "; 

		IDSet::iterator it = set.begin();

		for (it; it != set.end(); it++) // i번째 IDLIST 출력
		{
			int id = *it;
			out << id << ", ";

			// (2013/09.23) ID 목록에 포함된 노드들의 상위 (하위 ) 연관 노드 정보 출력

			out << "{ " ;

			IDSet tmpSet;

			//if (this->cntlMsg->cmType == BS_HELLO)
			{
				tmpSet = upperNodes[*it][0];
			}
			//else if (this->cntlMsg->cmType == CLSTR_ACK)
			{
				//tmpLst = lowerNodes[*it][i];
			}

			if ( ! tmpSet.empty() ) // 송신 노드의 i번째 상위 (또는 하위) 연관 노드 목록을 출력
			{
				IDSet::iterator tmpIt = tmpSet.begin();

				for (tmpIt; tmpIt != tmpSet.end(); tmpIt++)
				{
					out << *tmpIt << ", ";
				}
			}

			out << "} " ;

			
		}

		out << endl;
	}

	out.close();
}

void Sec_SEF::doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) // (2014.08.18 수정)
{
	// 제어 메시지 처리 과정 구현
	int cNodeID = cNode->id;
	
	IDSet srcSet; // 제어 메시지 내 노드 Id 목록 (원본)	
	Node* sender = msg->sender;

	/*
	if (sender == NULL) // BS로부터 수신된 경우에는 sender == NULL - 논리적 오류 아님
	{
		//cout << "(sender == NULL) " << endl;
		//exit(-1);
	}
	*/

	list<Node*> parentCHs = rtn->getParentCHs(cNode);

	if ( msg->cmType == BS_HELLO) // BS_HELLO 메시지인 경우
	{
		if ( !parentCHs.empty() || (cNode->distance < TRANS_RANGE) )// BS에서 전송 범위 이내이거나 이미 부모 CH가 저장되어 있으면 아무 처리도 하지 않음
		{
			return;
		}

		/*  송신 노드의 iD가 기존 후보 CH 노드 목록에 없으면 목록에 추가 */

		//list<Node*> parents = rtn->getParentCHs(cNode);
		//list<Node*> parents = rtn->getParents(cNode);

		bool exist = false;

		for ( list<Node*>::iterator it = parentCHs.begin(); it != parentCHs.end(); it++) // 기존 부모 노드 목록에 송신 노드 id가 있는지 검사
		{
			Node* existingParent = *it;
			int parentID = existingParent->id;

			if ( parentID == sender->id)
			{
				exist = true;
				break;
			}
		}

		if (exist == false) // 기존 부모 노드 목록에 없으면 송신 노드 ID를 부모 CH노드 목록에 추가
		{
			//rtn->addParent(cNode, sender);
			rtn->addParentCH(cNode, sender);
		}

		// 메시지에 포함된 상위 노드 정보 저장 (ID 중복 저장 여부 주의)

		int hopCnt;

		for (hopCnt = 1; hopCnt <= INTERLEAVING_T+1; hopCnt++) // 제어 메시지에 저장된 거리 (홉 수) 별 iD 목록을 검사
		{
			
			IDSet srcSet = cntlMsg->IDSets[hopCnt];

			IDSet::iterator it;

			for (it = srcSet.begin(); it != srcSet.end(); it++)  // hopCnt 거리에 해당하는 iD 목록의 요소들을 검사
			{
				int upperNodeID = *it;

				// 중복 여부 검사 부분 추가할 것

				IDSet::iterator find_it;

				find_it = upperNodes[cNodeID][hopCnt].find(upperNodeID);

				if (find_it == upperNodes[cNodeID][hopCnt].end()) // 기존에 상위 노드 목록에 저장되지 않은 경우, 목록에 저장
				{
					upperNodes[cNodeID][hopCnt].insert(upperNodeID);

					if (hopCnt == INTERLEAVING_T+1) // 상위 연관 노드 설정
					{
						kmr->addPairKey( cNodeID, upperNodeID); // pair key 추가
					}
				}			
			}
		}// CH의 상위 노드 목록 저장 완료

		/* 소속 클러스터의 비 CH 노드들의 상위 연관 노드 설정 */

		IDSet clstrNds; // 소속 클러스터의 비 CH 노드 목록 획득
		int clstrID = cNodeID; // CH 들만 제어 메시지 전달 에 참여한다고 가정 - 제어 메시지 수신 노드는 항상 CH임
		clstrNds = rtn->getClstr(clstrID);

		clstrNds.erase(clstrID); // 클러스터 소속 노드 목록에서 CH의 id 제거

		/* 각각의 비 CH 노드에 대해 상위 연관 노드 정보 설정 */

		IDSet::iterator it_clstrNds;

		hopCnt = 1;

		for ( it_clstrNds = clstrNds.begin(); it_clstrNds != clstrNds.end(); it_clstrNds++) // 각각의 비 CH 노드마다 상위 연관 노드 설정
		{
			int clstrNodeID;
			clstrNodeID = *it_clstrNds;

			IDSet srcSet = cntlMsg->IDSets[hopCnt];

			IDSet::iterator it = srcSet.begin();

			for (it = srcSet.begin(); it != srcSet.end(); it++) // 상위 노드 목록 검사
			{
				int upperNodeID = *it;

				IDSet::iterator find_it = upperNodes[clstrNodeID][INTERLEAVING_T+1].find(upperNodeID);

				if (find_it == upperNodes[clstrNodeID][INTERLEAVING_T+1].end())
				{
					upperNodes[clstrNodeID][INTERLEAVING_T+1].insert(upperNodeID);// 상위 연관 노드 추가 및 pair key 설정
					kmr->addPairKey(clstrNodeID, upperNodeID);
				}
			}

			hopCnt++;

			if ( hopCnt > INTERLEAVING_T) // 1~t 홉 거리의 노드들과 연관 관계를 설정한 경우 반복문 탈출
			{
				break;
			}
		} // 비 CH 노드들의 상위 연관 노드 설정 완료

	}

	else if (msg->cmType == CLSTR_ACK) // CLSTR_ACK 메시지인 경우
	{
		// 메시지에 포함된 하위 노드 id 저장 (중복 저장되지 않도록 주의)

		int hopCnt = 0;

		for (hopCnt = 1; hopCnt <= INTERLEAVING_T+1; hopCnt++)
		{
			IDSet srcSet = cntlMsg->IDSets[hopCnt]; // hopCnt 거리의 하위 노드 목록
			
			IDSet::iterator it_lowerNodes;
			
			for (it_lowerNodes = srcSet.begin(); it_lowerNodes != srcSet.end(); it_lowerNodes++) // 제어 메시지 내 하위 노드 목록에 있는 노드 ID 하나씩 검사
			{
				int lowerNodeID = *it_lowerNodes;						

				IDSet::iterator it_find;
			
				it_find = lowerNodes[cNodeID][hopCnt].find(lowerNodeID); // 기존 하위 노드 목록에 저장된 경우, 중복 저장하지 않음

				if (it_find == lowerNodes[cNodeID][hopCnt].end())
				{
					lowerNodes[cNodeID][hopCnt].insert(lowerNodeID);
						
					if (hopCnt == INTERLEAVING_T+1) // 상위 연관 노드 설정
					{
						kmr->addPairKey(cNodeID, lowerNodeID);
					}
				}
			} // 			
		}
	} // 하위 연관 노드 설정 완료

	else
	{
		cout << "cmType is invalid" << endl;
		exit(-1);
	}
}

void Sec_SEF::testControl()
{
	ofstream out("assoc_test.txt");

	// 각 노드의 상위 연관 노드 목록, 하위 연관 노드 목록 출력	

	for (int j = 0; j < NUM_NODES; j++)
	{	
		IDSet set;

		//if ( j == 129)
			//cout << " j == 129" << endl;

		out << "\n node " << j << endl ;

		for (int k = 0; k <= INTERLEAVING_T; k++)
		{
			out << "\t upper nodes [" << k << "]:" << " { " ;			

			set = upperNodes[j][k];					

			if ( ! set.empty() ) // 송신 노드의 상위 연관 노드 목록을 출력
			{
				IDSet::iterator it = set.begin();

				for (it; it != set.end(); it++)
				{
					out << *it << ", ";
				}
			}

			out << "} " << endl; // 
		}

		out << endl ; // 송신 노드의 상위 연관 노드 목록 출력 완료
		
		for (int k = 0; k <= INTERLEAVING_T; k++)
		{
			set = lowerNodes[j][k];

			out << "\t lower nodes [" << k << "]:" << " { " ;		

			if ( ! set.empty() ) // 송신 노드의 하위 연관 노드 목록을 출력
			{
				IDSet::iterator it = set.begin();

				for (it; it != set.end(); it++)
				{
					out << *it << ", ";
				}
			}

			out << "} " << endl; // 

		}		
			
		out << endl ; // 송신 노드의 하위 연관 노드 목록 출력 완료
	}

	out.close();
}




int Sec_SEF::getEventMsgLen(struct _tagEvntMsg* msg)
{
	int eventMsgLen = 0;

	eventMsgLen = DAT_MSG_LEN + (INTERLEAVING_T+1) * PMAC_LEN; // 기본 데이터 메시지 + (T+1) 개의 MAC

	return eventMsgLen; // SEF의 이벤트 보고서 길이 반환
}

int Sec_SEF::getCntrlMsgLen(struct _tagCntrlMsg* cm) // (2014.01.10) 제어 메시지 길이 반환
{
	int cntrlMsgLen = 0;
	
	/// // 제어 메시지 타입에 따른 기본 길이 계산

	if (cm->cmType == BS_HELLO) // BS_HELLO 메시지의 기본 길이 계산
	{
		int senderIDSize = 2;		
		cntrlMsgLen = senderIDSize; 
	}
	else // Clstr_ACK 메시지의 기본 길이 계산
	{
		int senderIDSize = 2;
		int receiverIDSize = 2;
		int clusterIDSize = 2;
		cntrlMsgLen = senderIDSize + receiverIDSize + clusterIDSize;
	}
	
	int numIDs = 0; // 제어 메시지에 포함된 ID의 개수

	for (int i = 1; i <= INTERLEAVING_T+1; i++)
	{
		numIDs += cm->IDSets[i].size(); // 제어 메시지 내 ID의 개수 누적
	}

	cntrlMsgLen += numIDs * ID_LEN; // 제어 메시지 길이 = 기본 길이 + id의 개수 * ID 길이


	return cntrlMsgLen; // 제어 메시지 길이 반환
}


bool Sec_SEF::detect(EvntMsg* msg) // (2014.10.20)공격이 검출되면 TRUE, 아니면 false 반환
{
	bool res = false;

	if (msg->falseReport == true)
	{

		if ( gen0.genRandInt(100) < FILTERING_PROB_SEF ) // 검출 여부 체크!!!
		{
			this->numDtctn++; // 공격 검출 횟수 +1 증가
			//cout << " 위조 메시지 검출!!" ;
			res = true; //
			atkDtctd = true;
			msg->detected = true; // (2014.09.23) // 보고서 내에 공격 검출 여부 기록
		}
		
	}


	return res;



	/*
	// 
	bool res = false;
	atkDtctd = false; // (2014.08.21) 보고서 검증을 수행하기 전에는 공격이 발생하지 않은 것으로 가정 -> 검증 결과에 따라 공격 검출 시 true로 설정
	// ...

	// 임시로 주석 처리
	if (verMAC(msg) == false) // MAC 검증 결과가 거짓인 경우 공격 검출
	{
		this->numDtctn++; // 공격 검출 횟수 +1 증가
		//cout << " 위조 메시지 검출!!" ;
		res = true; //
		atkDtctd = true;
		msg->detected = true; // (2014.09.23) // 보고서 내에 공격 검출 여부 기록
	}
	
	return res;	
	*/
}

void Sec_SEF::updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes) // assocType - 0: upper node,1 : lower node
{
	switch (assocType)
	{
	case 0: // 상위 연관 노드 목록 갱신
		{
			set<int> newUpperNodes; // 새로운 상위 노드 목록		

			{
				double dist;
				double minDist;
				int spUpperNode; // 최단 경로 상의 상위 노드 ID
					
				dist = 0;
				minDist = -1;
				spUpperNode = -1;

				set<int>::iterator it_upperNode;

				for (it_upperNode = assocNodes.begin(); it_upperNode != assocNodes.end(); it_upperNode++) // 임시 상위 노드 목록 순회 (x hop 이내에 도달 가능한 모든 노드)
				{
					int upperNodeID;
					upperNodeID = *it_upperNode;

					if ( hopDist == 1) // 1 홉 거리에 있는 노드들의 BS까지의 거리 측정
					{
						//dist = sqrt ( pow((double) nodes[nodeID]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[nodeID]->y - nodes[upperNodeID]->y, 2) ); // 현재 노드와 1-hop 노드들과의 거리 측정
						dist = nodes[upperNodeID]->distance;		


						/// (2014.08.17) 기존 부모 CH 목록에 추가되어 있지 않으면 저장
						  
						list<Node*> parentCHs = rtn->getParentCHs(nodes[nodeID]);// 부모 CH 목록 획득
						list<Node*>::iterator it_pCH;

						bool exist = false;


						for (it_pCH = parentCHs.begin(); it_pCH != parentCHs.end(); it_pCH++) // 기존 부모 CH 목록에 저장되어 있는지 여부 검사
						{
							Node* pCH = *it_pCH;
							
							if ( pCH->id == upperNodeID) // 1홉 상위 노드가 기존 부모 CH 목록에 저장되어 있는지 검사
							{
								exist = true;
							} // if문 - 기존 부모 CH 노드와 비교

						} // for문 완료 - 검사 완료

						if (exist == false)
						{
							rtn->addParentCH(nodes[nodeID], nodes[upperNodeID]); // 1홉 상위 노드를 부모 CH 목록에 저장
						}

						/// (2014.08.17) 수정 완료
					}
					else
					{
						int prevHopSPUpperNode; // (h-1)-hop 상위 노드 ID
						set<int>::iterator it; 
						it = upperNodes[nodeID][hopDist-1].begin();

						if (it == upperNodes[nodeID][hopDist-1].end()) // (h-1)-홉 상위 노드가 존재하지 않으면 h-hop 상위 노드 추가 불가 -> 반복문 탈출
						{
							break;
						}

						prevHopSPUpperNode = *it;

						double distOfOneHop;
						
						distOfOneHop = sqrt ( pow((double) nodes[prevHopSPUpperNode]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[prevHopSPUpperNode]->y - nodes[upperNodeID]->y, 2) ); // (h-1)-hop 노드와 h-hop 노드들과의 거리 측정

						if ( (distOfOneHop > TRANS_RANGE) || (nodes[prevHopSPUpperNode]->distance <= nodes[upperNodeID]->distance ) ) // 이전 홉 상위 노드보다 BS에 가깝고, 이전 홉 상위 노드의 전송 범위 이내에 있는 노드 중에서 bs에 가장 가까운 노드를 h 홉 상위 노드로 결정
						{
							continue; // 다음 후보 상위 노드 검사
						}

						dist = nodes[upperNodeID]->distance;
						//dist = sqrt ( pow((double) nodes[prevHopSPUpperNode]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[prevHopSPUpperNode]->y - nodes[upperNodeID]->y, 2) ); // (h-1)-hop 노드와 h-hop 노드들과의 거리 측정
					}						
						 
					if (minDist < 0) // 최소 거리가 초기화되지 않은 경우
					{
						minDist = dist; // 임시 상위 노드로부터 BS까지의 최소 거리
						spUpperNode = upperNodeID;
					}
					else if ( dist < minDist) // 최소 거리가 갱신되는 경우
					{
						minDist = dist;
						spUpperNode = upperNodeID;
					}
				} // x-hop 이내 노드 목록 순회 완료 - x-hop 상위 노드 저장완료

				if ( (minDist > 0) )
				{
					newUpperNodes.insert(spUpperNode);					
				}								
				
				// 기존 상위 노드 목록과 새로운 상위 노드 목록이 다르면 갱신 여부 기록
				
				if ( newUpperNodes != upperNodes[nodeID][hopDist])
				{
					assocUpdated[nodeID] = true;		
				}
				upperNodes[nodeID][hopDist] = newUpperNodes;

				// (2014.06.26) 여기까지 확인!

				// 클러스터 소속 비 CH 노드의 상위 연관 노드 목록 저장
				
				if (upperNodes[nodeID][hopDist].empty()==true) // CH의 h-hop 상위 노드 목록이 비어 있으면 함수 종료
				{
					return;
				}


				set<int> cluster;
				set<int>::iterator it_memberNode;
				int memberNodeID;

				cluster = rtn->getClstr(nodeID); // 클러스터 소속 노드 목록 획득

				int node_idx;
				node_idx = 1;

				for( it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // 클러스터 소속 노드 목록 순회
				{
					memberNodeID = *it_memberNode; // 클러스터 소속 ID;

					if (memberNodeID == nodeID) // 소속 노드 ID가 ch의 ID와 동일할 경우, 다음 소속 노드로 진행
					{
						continue;
					}

					if ( node_idx == hopDist) // CH의 h-hop 상위 노드를 h 번째 소속 노드의 상위 연관 노드로 저장
					{
						if ( upperNodes[memberNodeID][INTERLEAVING_T+1] != upperNodes[nodeID][hopDist]) // 소속 노드의 기존 상위 연관 노드 목록과 다를 경우, 갱신 여부 체크 후 해당 목록 갱신
						{
							assocUpdated[memberNodeID] = true;						
							upperNodes[memberNodeID][INTERLEAVING_T+1] = upperNodes[nodeID][hopDist];
						}
					

						// CH 의 h-hop 상위 노드의 하위 연관 노드 목록에 해당 소속 노드 ID가 존재하지 않을 경우, 갱신 여부에 체크
						/*
						set<int>::iterator it;

						for (it = upperNodes[nodeID][hopDist].begin(); it != upperNodes[nodeID][hopDist].end(); it++) // CH 의 h-hop 상위 노드 목록 순회 - h-hop 상위 노드의 하위 연관 노드 목록 갱신
						{
							int hHopUpNodeID;
							hHopUpNodeID = *it;

							if ( lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].find(memberNodeID) == lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].end()) // h-hop 상위 노드의 하위 연관 노드 목록에 소속 노드가 존재하지 않을 경우,
							{
								assocUpdated[hHopUpNodeID] = true;
								lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].insert(memberNodeID); // h-hop 상위 노드의 하위 연관 노드 목록에 현재 소속 노드 ID를 저장
							}
						} // CH 의 h-hop 상위 노드 목록 순회 - h-hop 상위 노드의 하위 연관 노드 목록 갱신 완료
						*/
						break;
					}

					node_idx++;
				} // CH의 h-번째 상위 노드를 h번째 소속 노드의 상위 연관 노드로 설정
				

			} // h-hop 상위 노드 목록 갱신 완료								
						
		break;
		} // 상위 연관 노드 목록 갱신 완료

	case 1: // 하위 연관 노드 목록 갱신
		{

			set<int> newLowerNodes; // 새로운 하위 노드 목록

			set<int>::iterator it_lowerNode;

			for (it_lowerNode = assocNodes.begin(); it_lowerNode != assocNodes.end(); it_lowerNode++) // 임시 하위 노드 목록 순회 (x hop 이내에 도달 가능한 모든 노드)
			{
				int lowerNodeID;
				lowerNodeID = *it_lowerNode;

				if (upperNodes[lowerNodeID][hopDist].find(nodeID) != upperNodes[lowerNodeID][hopDist].end()) // 임시 하위 노드의 h-hop 상위 노드 목록에 현재 노드가 존재하면 해당 임시 하위 노드를 현재 노드의 하위 노드 목록에 저장
				{
					newLowerNodes.insert(lowerNodeID);
				}
			} // for문 종료 - 임시 하위 노드 목록 순회 완료

			if (newLowerNodes != lowerNodes[nodeID][hopDist])
			{
				assocUpdated[nodeID] = true;		
			}

			lowerNodes[nodeID][hopDist] = newLowerNodes; // 현재 노드의 하위 노드 목록 갱신

			/// (2014.08.06) 비 CH 소속 노드들의 하위 연관 노드 목록 리셋

			set<int> cluster;
			set<int>::iterator it_memberNode;
			int memberNodeID;

			cluster = rtn->getClstr(nodeID); // 클러스터 소속 노드 목록 획득

			for (it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // 클러스터 소속 노드 목록 순회
			{
				memberNodeID = *it_memberNode; // 소속 노드 ID 획득

				if (memberNodeID == nodeID) // 소속 노드 ID가 ch의 ID와 동일할 경우, 다음 소속 노드로 진행
				{
					continue;
				}

				for (int hop = 1; hop <= INTERLEAVING_T+1; hop++)
				{
					lowerNodes[memberNodeID][hop].clear(); // 소속 노드의 하위 연관 노드 목록 리셋
				}
			} // 클러스터 소속 비 CH 노드들의 하위 연관 노드 목록 리셋

		
		break;
		} // 하위 연관 노드 목록 갱신 완료
	}
}

bool Sec_SEF::compareAssoc ( set<int> oldAssoc, set<int> newAssoc) // (2014.05.27) 두 연관 노드 목록이 동일하며 true,  다르면 false 반환
{
	bool res = true;

	if (oldAssoc.size() != newAssoc.size() )
	{
		res = false;
		return res;
	}

	set<int>::iterator it;
	set<int>::iterator it2;

	for (it = oldAssoc.begin(), it2= newAssoc.begin(); it != oldAssoc.end(), it2 != newAssoc.end(); it++, it2++)
	{
		int id = *it;
		int id2 = *it2;

		if (id != id2)
		{
			res = false;
			break;
		}
	}

	return res;
}

void Sec_SEF::resetAssoc() // (2014.08.21) 이전 라운드의 연관 노드 정보를 리셋
{
	upperNodes.clear();
	lowerNodes.clear();
}


bool Sec_SEF::hasUpdatedAssoc (int nodeID) // (2014.07.17) 특정 노드의 연관 노드 변경 여부 반환
{
	if ( this->assocUpdated[nodeID] == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void Sec_SEF::resetUpdatedAssoc() // (2014.07.22) 노드들의 연관 노드 갱신 여부를 false 로 리셋 (키 재분배 목적)
{
	std::map<int, bool>::iterator it;

	int nodeID;	

	for (it = this->assocUpdated.begin(); it != assocUpdated.end(); it++) // 노드들의 연관 갱신 여부를 false 로 리셋 - 키 재분배의 목적
	{
		nodeID = (*it).first;
		assocUpdated[nodeID] = false;
	} // for문 종료 - 노드들의 연관 노드 갱신 여부를 false 로 리셋
}

void Sec_SEF::testAssoc() // // (2014.07.17) 노드들의 연관 노드 목록 파일에 출력
{
	ofstream out("tst_assoc.txt");

	map<int, set<int>> clusters = rtn->getClusters(); // 클러스터 정보 획득

	map<int, set<int>>::iterator it_clstr; 

	for (it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++) // 클러스터 맵 순회 - 각 클러스터에 속한 노드들에 대해 연관 노드 목록 출력
	{
		set<int> nodesInCluster = (*it_clstr).second;

		set<int>::iterator it_nd;

		// (2014.09.21) 수정 - 
		int CHID = (*it_clstr).first;

		if ( nodes[CHID] == NULL) // CH가 이미 제거된 노드이면 다음 클러스터로 이동
		{
			continue;
		}
		// (2014.09.21 수정 완료) CH가 이미 제거된 노드이면 다음 클러스터로 이동

		for (it_nd = nodesInCluster.begin(); it_nd != nodesInCluster.end(); it_nd++) // 클러스터 내 노드 집합 순회 - 현재 클러스터에 속한 노드들의 연관 노드 목록 출력
		{
			int nodeID = *it_nd;

			/// 현재 노드의 상위 노드 목록 출력
			out << "node ID: " << nodeID;

			for (int hc = 1; hc <= INTERLEAVING_T+1; hc++) // 1~t+1 사이의 홉에 존재하는 상위 노드 목록 출력
			{
				out << ", hc = " << hc << " {";
				
				std::set<int> upNodes = upperNodes[nodeID][hc];

				set<int>::iterator it_upNodes;

				for (it_upNodes = upNodes.begin(); it_upNodes != upNodes.end(); it_upNodes++) // hc 홉에 대응되는 상위 노드 목록 순회
				{
					int upNodeID = *it_upNodes;

					out << upNodeID << ", ";

				} // for문 종료 - 현재 노드의 hc 홉 상위 노드 목록 출력

				out << " } ";

			} // for문 종료 

			out << endl << "\t";

			/// 현재 노드의 하위 노드 목록 출력
			for (int hc = 1; hc <= INTERLEAVING_T+1; hc++) // 1~t+1 사이의 홉에 존재하는 하위 노드 목록 출력
			{
				out << "hc = " << hc << " {";

				std::set<int> lowNodes = lowerNodes[nodeID][hc];

				set<int>::iterator it_lowNodes;

				for (it_lowNodes = lowNodes.begin(); it_lowNodes != lowNodes.end(); it_lowNodes++) // hc 홉에 대응되는 하위 노드 목록 순회
				{
					int lowNodeID = *it_lowNodes;

					out << lowNodeID << ", ";
				} // for문 종료 - 현재 노드의 hc 홉 하위 노드 목록 출력

				out << " } ";

			} // for문 종료 

			out << endl << endl;

		}// for문 종료 - 현재 클러스터에 속한 노드들의 연관 노드 목록 출력

		out << endl << endl << endl << endl;

	} // for 문 종료 - 각 클러스터에 속한 노드들에 대해 연관 노드 목록 출력


	out.close();
}

Sec_SEF::Sec_SEF()
{
	seed0 = 1;
	this->gen0.setSeed(seed0);

}



Sec_SEF::~Sec_SEF()
{
	// 상위/하위 연관 노드 목록에 저장된 요소 삭제

	for (int i = 0; i < NUM_NODES; i++) // 모든 노드에 대해 반복
	{
		for (int j = 0; j < INTERLEAVING_T+1; j++) // 각 노드의 모든 상위/하위 노드 목록에 대해 반복 (IHA_T +1 개) 
		{
			IDSet set = upperNodes[i][j];
			IDSet::iterator it = set.begin();

			for (it; it != set.end(); )
			{
				IDSet::iterator tmpIt = it;
				++it;
				set.erase(tmpIt);
			} // 상위 노드 목록의 요소 제거

			set = lowerNodes[i][j];
			it = set.begin();

			for (it; it != set.end(); )
			{
				IDSet::iterator tmpIt = it;
				++it;
				set.erase(tmpIt);
			} // 하위 노드 목록의 요소 제거
			
		} // 현재 노드의 상/하위 노드 목록 정리 완료
	} // 모든 노드의 상/하위 노드 목록 정리 완료
}