// 2014.06.24
// 문수영
// Sec_DIHA
// DIHA의 보안 클래스 정의


#include "Sec_DIHA.h"

// 이벤트 보고서 처리 관련	

void Sec_DIHA::setMAC (EvntMsg* msg) // MAC 설정
{

}

void Sec_DIHA::setMAC (struct _tagEvntMsg* msg, int clstrID) // (2014.08.19) MAC 설정 
{
	// 기존에 남아 있는 MAC 모두 제거

	list<MAC*>::iterator itMAC = msg->macs.begin();

	for ( itMAC; itMAC!= msg->macs.end(); itMAC++)
	{
		MAC* mac = *itMAC;
		delete mac;
	}

	msg->macs.clear(); 	

	if (msg->hop != 0) // hop count가 0일 때만 보고서 내 MAC 설정
	{
		return;
	}

	// 보고서 생성에 참여하는 노드들의 ID 확보 ( 상황에 따라 t+1 보다 작은 값을 가질 수 있음)

	// (2014.10.10) 이미 제거된 노드들의 id가 클러스터 정보에 포함된 경우 이를 제거하는 것이 필요

	set<int> deledtedNds; // 제거된 노드 집합



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

		if (nodes[nodeID] == NULL)
		{
			deledtedNds.insert(nodeID); // (2014.10.10) 제거된 노드 목록에 추가
			continue;
		}


		sourceNodes.push_back(nodeID);

		if (idx >= INTERLEAVING_T) // 보고서 생성에 참여하는 일반 노드 (ch 제외)의 수가 T개가 되면 반복문 탈출
		{
			break;
		}

		idx++; // (2014.10.10)

	} // for문 - 보고서 생성에 참여하는 노드들을 목록에 저장


	set<int>::iterator it_del;

	for (it_del = deledtedNds.begin(); it_del != deledtedNds.end(); it_del++) // (2014.10.10) 제거된 노드 정보를 클러스터에서 제거
	{
		int delNodeID = *it_del;
		
		rtn->removeFromCluster(delNodeID);
	} // for문 완료 - 제거된 노드 정보를 클러스터에서 제거

	msg->prevNodes = sourceNodes;// 이벤트 보고서 내에 이전 노드 정보 저장
	msg->prevNodes.push_back(clstrID);

	//sourceNodes.push_back(clstrID); // CH의 id 추가

	// 소스 노드 중 T개의 일반 노드 (비 CH)가 존재하면 ch의 다음 전달 노드를 고려하여 MAC 생성	(소스 노드: id가 작은 순서대로 첫 번째에 해당하는 일반 노드)	

	if (msg->receiver == NULL) // 메시지 수신 노드가 존재하지 않으면 함수 종료
	{
		return;
	}


	int upNodeID = msg->receiver->id;


	int MACGeneratNodeID;

	PairKey* pk = NULL;

	MAC* mac = NULL; 


	if ( !sourceNodes.empty() ) // CH가 아닌 소스 노드가 존재하면 첫 번째 소스 노드와 다음 전달 노드와의 pair key로 MAC 생성 후 첨부
	//if (sourceNodes.size() == INTERLEAVING_T) // CH가 아닌 소스 노드의 수가 T이면 첫 번째 소스 노드와 다음 전달 노드와의 pair key로 MAC 생성 후 첨부
	{
		MACGeneratNodeID = sourceNodes.front();

		pk = kmr->getPairKey(MACGeneratNodeID, upNodeID); // pairwise key 획득

		if (pk == NULL)
		{
			cout << " pk == NULL " << endl;
			
			kmr->addPairKey(MACGeneratNodeID, upNodeID); // 
			pk = kmr->getPairKey(MACGeneratNodeID, upNodeID); // pairwise key 획득
						
		}

		mac = new MAC; // pairwise MAC 생성 및 추가

		mac->origin = nodes[MACGeneratNodeID]; // MAC 생성 노드 설정
		mac->pKey = pk; // MAC의 pairwise key 설정
		mac->code = pk->kVal + msg->val; // 메시지 인증 코드 설정

		msg->macs.push_back(mac); // 보고서 내 MAC 목록에 신규 MAC 추가

		// 현재 노드의 MAC 생성으로 인한 에너지 소모량 누적

		nodes[MACGeneratNodeID]->enrgyDiff = hashEnrg; // 노드의 현재 에너지 소모량
		nodes[MACGeneratNodeID]->eCnsmptn += nodes[MACGeneratNodeID]->enrgyDiff; // (2013.10.04) MAC 계산에 따른 에너지 소모량 누적
		Node::totEnrgCnsmptn += nodes[MACGeneratNodeID]->enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)

		nodes[MACGeneratNodeID]->remEnrgy -= nodes[MACGeneratNodeID]->enrgyDiff; // (2013.10.04) MAC 계산에 따른 잔여 에너지 감소
	}

}

void Sec_DIHA::updateMAC(struct _tagEvntMsg* msg) // (2014.08.20) MAC 갱신
{
	// 보고서 내 id 목록 갱신
	//msg->prevNodes.pop_front(); // T+1 홉 이전 노드의 id 제거
	//msg->prevNodes.push_back(msg->sender->id); // 송신 노드의 ID 추가	

	// IHA_T 홉 이전 노드와 다음 전달 노드가 공유하는 키로 MAC을 생성하고 보고서 내 mac 목록에 추가

	std::list<int> pNodes = msg->prevNodes;

	int lowerNodeID = -1; // T 홉 이전 노드의 id

	if (msg->prevNodes.size() > INTERLEAVING_T)
	{
		msg->prevNodes.pop_front(); // T+1 홉 이전 노드의 id 제거
		lowerNodeID = msg->prevNodes.front(); // IHA_T 홉 이전 노드의 ID 획득
	}
	else if ( msg->prevNodes.size() == INTERLEAVING_T)
	{
		// 만약 prevNodes의 첫 번째에 위치한 노드가 CoS이면 해당 노드가 T홉 이전 노드임
		int tempID = pNodes.front();

		if (tempID == msg->CoS->id)
		{
			lowerNodeID = tempID; // IHA_T 홉 이전 노드의 ID 획득
		}
	}
	
	msg->prevNodes.push_back(msg->sender->id); // 송신 노드의 ID 추가

	if (lowerNodeID < 0) // 만약 T홉 이전 노드가 존재하지 않으면 함수 종료
	{
		return;
	}

	//int lowerNodeID = pNodes.front(); // IHA_T 홉 이전 노드의 ID 획득
	int nextNodeID = msg->receiver->id;
	PairKey* pk = kmr->getPairKey(lowerNodeID, nextNodeID); // pairwise key 획득

	if (pk == NULL) // 사전에 두 노드가 연관 관계를 맺지 않은 경우
	{
		kmr->addPairKey (lowerNodeID,  nextNodeID); // (2013.11.27) pair key 추가 (상위 연관 노드와 공유하는 키)								
		pk = kmr->getPairKey(lowerNodeID,  nextNodeID);
	}

	// pairwise MAC 생성 및 추가
	MAC* mac = new MAC; // pairwise MAC 생성 및 추가

	mac->origin = nodes[lowerNodeID]; // (2013.09.02) MAC 생성 노드 설정
	mac->pKey = pk; // MAC의 pairwise key 설정
		
	mac->code = pk->kVal + msg->val; // 메시지 인증 코드 설정
	
	///** (2014.08.22 수정) 위조 보고서인 경우, 만약 t 홉 이전 노드가 source 노드이고 훼손 노드가 아니면 잘못된 코드 값(-1)이 저장되도록 수정
	if (msg->falseReport == true) // 위조 보고서인 경우, T 홉 이전 노드가 SOURCE 노드인지 확인,
	{
		if ( msg->CoS->id == rtn->getClstrID(lowerNodeID)) // T홉 이전 노드가 SOURCE 노드이면 훼손 노드인지 확인
		{
			
			if ( nodes[lowerNodeID]->cmprmsd == false) // (2014.08.31) T 홉 이전 노드가 훼손 노드가 아니면 잘못된 코드 값 저장
			//if (msg->CoS->cmprmsd == false) // 훼손 노드가 아니면 잘못된 코드 값 저장
			{
				mac->code = -1;
			}
		}
	} // IF문  - 위조 보고서인 경우, T 홉 이전 노드가 SOURCE 노드인지 확인,
	///** (2014.08.22 수정 완료)


	msg->macs.push_back(mac); // 보고서 내 MAC 목록에 신규 MAC 추가
	
	
	// 에너지 소모량 누적

	nodes[lowerNodeID]->enrgyDiff = hashEnrg; // 노드의 현재 에너지 소모량
	nodes[lowerNodeID]->eCnsmptn += nodes[lowerNodeID]->enrgyDiff; // (2013.10.04) MAC 계산에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += nodes[lowerNodeID]->enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)

	nodes[lowerNodeID]->remEnrgy -= nodes[lowerNodeID]->enrgyDiff; // (2013.10.04) MAC 계산에 따른 잔여 에너지 감소
}

void Sec_DIHA::doEventMsgProc(struct _tagEvntMsg* msg) // (2014.08.21) 이벤트 보고서 처리 (MAC 갱신)
{
	int cNodeID = msg->receiver->id;

	std::list<MAC*>::iterator itMAC = msg->macs.begin();

	list<MAC*> tmplst;

	for (itMAC; itMAC != msg->macs.end(); itMAC++) // 이벤트 보고서에 포함된 mac을 하나씩 검사	
	{
		MAC* mac = *itMAC; // 현재 검사중인 MAC

		//IDSet::iterator itID = lowerNodes[cNodeID][INTERLEAVING_T].begin(); // 보고서 수신 노드의 하위 연관 노드 목록의 iterator

		//bool matched = false; // MAC 생성 노드가 하위 연관 노드와 동일한 경우 true로 설정

		PairKey* pKey = mac->pKey; // pairwise key

		// MAC 생성 노드와 현재 노드가 연관 관계에 있는지 검사

		Node* origin = mac->origin;
		Node* receiver = msg->receiver;

		if ((pKey->nID == origin->id && pKey->nID2 == receiver->id) || (pKey->nID == receiver->id && pKey->nID2 == origin->id))
		{
			// MAC 생성 노드와 현재 노드는 연관 관계에 해당함. 해당 MAC을 보고서 내 목록에서 제거
			//matched = true;
			//delete (mac);// 해당 MAC 반환	

			tmplst.push_back(mac); // (2014.08.25) 검증된 MAC을 임시 목록에 추가하였다가 이벤트 보고서 내 mac 목록에서 제거하고 메모리 반환
		}	

		/*
		if (matched == false)
		{			
			tmplst.push_back(mac); // 하위 연관 노드가 생성한 MAC을 제외한 나머지 mac들을 임시 목록에 추가
		}
		*/
	}

	// (2014.08.25 수정) 검증된 MAC의 메모리 반환, 목록에서 제거

	
	if ( !tmplst.empty() ) // 검증된 MAC이 존재 - 이벤트 보고서 내 MAC 목록에서 가장 앞에 위치한 mac
	{
		MAC* mac = * (msg->macs.begin()); // 가장 앞에 위치한 MAC의 메모리를 반환하고 목록에서 제거

		delete mac;

		msg->macs.pop_front();
	}
	
	tmplst.clear(); // 임시 MAC 목록 리셋


	//msg->macs.clear(); // 보고서 내 MAC 목록 전부 제거
}

// 제어 메시지 처리 관련
void Sec_DIHA::testControl() //
{

}

void Sec_DIHA::doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) // (2014.08.18 추가) 제어 메시지 수신에 따른 처리
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

	if ( msg->cmType == BS_HELLO) // BS_HELLO 메시지인 경우
	{
		if (cNode->distance < TRANS_RANGE) // BS에서 전송 범위 이내이면 아무 처리도 하지 않음
		{
			return;
		}
	

		/*  송신 노드의 iD가 기존 후보 CH 노드 목록에 없으면 목록에 추가 */

		list<Node*> parents = rtn->getParentCHs(cNode);
		
		// (2014.09.11 수정)
		if ( parents.size() >= MAX_NUM_PARENT_CHS) // 부모 ch 개수가 최대 한도에 도달하면 즉시 반환
		{
			return;
		}

		bool exist = false;

		for ( list<Node*>::iterator it = parents.begin(); it != parents.end(); it++) // 기존 부모 노드 목록에 송신 노드 id가 있는지 검사
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

// 에너지 소모량 계산 관련
int Sec_DIHA::getEventMsgLen(struct _tagEvntMsg* msg)
{
	int len = DAT_MSG_LEN + (INTERLEAVING_T+1) * PMAC_LEN;	

	int commOvrhd = 0;


	/* (2013.10.21) 제안 기법에서 추가적인 PAIRWISE MAC으로 인한 오버헤드는 (T+1)개의 BLOOM FILTER를 사용함으로써 상쇄할 수 있다고 가정 */
	/*
	// 추가적인 pairwise MAC으로 인한 통신 오버헤드 (in bytes)
	int numAddPairMAC = 0; // 보고서 내  t+1개의 MAC 외에 추가적인 pairMAC 개수

	numAddPairMAC = msg->macs.size() - IHA_T;

	commOvrhd += numAddPairMAC * PMAC_LEN; // 추가적인 pairMAC으로 인한 통신 오버헤드

	// commOvrhd /= 5; // bloom filter 적용
	*/


	// 보고서 전달 과정에서 각 노드는 T-홉 이전 노드에게 다음 전달 노드의 id 전송 - 메시지 오버헤드 발생 (in bytes)
	commOvrhd += ceil((INTERLEAVING_T) * REDUCED_ID_LEN); 

	commOvrhd += ID_LEN;// (2013.11.27) 보고서에 MAC 생성 노드의 id 포함 (다음 전달 노드에서 MAC 검증을 위해 필요)
	
	len += commOvrhd; // 기본 메시지 길이 + 추가적인 통신 오버헤드의 합계 반환

	return len;

}

int Sec_DIHA::getCntrlMsgLen(struct _tagCntrlMsg* cm)
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

	/*
	int cntrlMsgLen = 0;

	int numIDs = 0; // 제어 메시지에 포함된 ID의 개수

	for (int i = 1; i <= INTERLEAVING_T+1; i++)
	{
		numIDs += cm->IDSets[i].size(); // 제어 메시지 내 ID의 개수 누적
	}

	cntrlMsgLen = numIDs * ID_LEN; // 제어 메시지 길이 = id의 개수 * ID 길이

	return cntrlMsgLen; // 제어 메시지 길이 반환
	*/

}

void Sec_DIHA::updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes) // (2014.06.24) 상하위 연관 노드 갱신
{
	
	switch (assocType)
	{
	case 0: // 상위 연관 노드 목록 갱신
		{
			set<int> newUpperNodes; // 새로운 상위 노드 목록		

			set<int>::iterator it_upperNode;

			for (it_upperNode = assocNodes.begin(); it_upperNode != assocNodes.end(); it_upperNode++) // 임시 상위 노드 목록 순회 (x hop 이내에 도달 가능한 모든 노드)
			{
				int upperNodeID;
				upperNodeID = *it_upperNode;

				double distBetween;
				int hopCnt;

				distBetween = sqrt ( pow((double) nodes[nodeID]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[nodeID]->y - nodes[upperNodeID]->y, 2) ); // 현재 CH로부터 임시 상위 노드까지의 거리 계산

				hopCnt =  (int) ceil (distBetween / TRANS_RANGE);

				if (hopCnt != hopDist)
				{
					continue; // 현재 CH로부터 임시 상위 노드까지의 홉 수 계산, h-hop과 다르면 신규 상위 노드 목록에서 배제
				}

				newUpperNodes.insert(upperNodeID);

				/// 부모 ch 노드 정보 저장

				if (hopCnt == 1)
				{
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

			} //  x-hop 이내 노드 목록 순회 완료 - x-hop 상위 노드 저장완료


			if ( this->compareAssoc ( upperNodes[nodeID][hopDist], newUpperNodes) == false) // CH의 상위 노드 목록 갱신
			{
				assocUpdated[nodeID] =true;
				upperNodes[nodeID][hopDist].clear();
				upperNodes[nodeID][hopDist] = newUpperNodes;


				// 만약 hop == T+1 이면 상위 노드 목록에 속한 노드들과 현재 CH들 간의 PAIR key 추가

				set<int>::iterator it_UN;

				if (hopDist == INTERLEAVING_T+1) 
				{
					for ( it_UN = upperNodes[nodeID][hopDist].begin(); it_UN != upperNodes[nodeID][hopDist].end(); it_UN++) // 상위 노드 목록 순회 - pair key 추가
					{
						int UN_ID = *it_UN;

						kmr->addPairKey(nodeID, UN_ID); // pair key 추가

					} 

				} // 현재 CH와 상위 연관 노드 사잉의 PAIR KEY 추가 완료


			}	// IF문 - CH의 상위 노드 목록 갱신 완료		
			

			/// 해당 클러스터 소속 비 CH 노드들의 상위 연관 노드 설정

			if (upperNodes[nodeID][hopDist].empty()==true) // CH의 h-hop 상위 노드 목록이 비어 있으면 함수 종료
			{
				return;
			}



			set<int> cluster;
			set<int>::iterator it_memberNode;
			int memberNodeID;

			int node_idx; // 클러스터 소속 노드의 인덱스
			node_idx = 1;

			cluster = rtn->getClstr(nodeID); // 클러스터 소속 노드 목록 획득


			for (it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // 클러스터 소속 노드 목록 순회
			{
				memberNodeID = *it_memberNode; // 소속 노드 ID 획득

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

					set<int>::iterator it;

					for (it = upperNodes[nodeID][hopDist].begin(); it != upperNodes[nodeID][hopDist].end(); it++) // CH 의 h-hop 상위 노드 목록 순회 - h-hop 상위 노드의 하위 연관 노드 목록 갱신
					{
						int hHopUpNodeID;
						hHopUpNodeID = *it;

						if ( lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].find(memberNodeID) == lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].end()) // h-hop 상위 노드의 하위 연관 노드 목록에 소속 노드가 존재하지 않을 경우,
						{
							assocUpdated[hHopUpNodeID] = true;
							lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].insert(memberNodeID); // h-hop 상위 노드의 하위 연관 노드 목록에 현재 소속 노드 ID를 저장
						
							// addPairKey

							kmr->addPairKey(memberNodeID, hHopUpNodeID);

						}
					} // CH의 H-HOP 상위 노드들의 하위 연관 노드 목록에 현재 소속 노드 id 저장
					break;
				} // 소속 노드의 상위 연관 노드 설정 및 상위 노드의 하위 연관 노드 설정

				node_idx++;

			} // CH의 h-번째 상위 노드를 h번째 소속 노드의 상위 연관 노드로 설정			


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

					if (lowerNodes[nodeID][hopDist].find(lowerNodeID) == lowerNodes[nodeID][hopDist].end()) // 현재 노드의 h-hop 하위 노드 목록에 신규 하위 노드가 저장되어 있지 않은 경우
					{
						assocUpdated[nodeID] = true;	
						lowerNodes[nodeID][hopDist].insert(lowerNodeID);

						kmr->addPairKey(nodeID, lowerNodeID); // (2014.08.27) 현재 노드와 신규 하위 노드 사이에 pair key 추가
					}

				} // if문 - 임시 하위 노드의 h - hop 상위 노드 목록에 현재 노드가 저장되어 있으면 해당 임시 하위 노드를 현재 노드의 하위 노드 목록에 추가

			} // for문 종료 - 임시 하위 노드 목록 순회 완료


			// 신규 하위 노드 목록 순회

			/// (2014.08.05) 비 CH 소속 노드들의 하위 연관 노드 목록 리셋 필요 (Sec_IHA::updateAssocNodes() 도 마찬가지)

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

			//if (newLowerNodes != lowerNodes[nodeID][hopDist])
			//{
				//assocUpdated[nodeID] = true;		
			//}

			//lowerNodes[nodeID][hopDist] = newLowerNodes; // 현재 노드의 하위 노드 목록 갱신

		break;
		} // 하위 연관 노드 목록 갱신 완료
	}
}


Sec_DIHA::Sec_DIHA()
{

}

Sec_DIHA::~Sec_DIHA()
{

}