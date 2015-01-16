// 2014.07.31
// 문수영
// Kmr.h
// NEW_LKH 기반 키 관리 클래스 정의

#include "Kmr_NEWLKH.h"
#include "SrchNode_KTS.h"

void Kmr_NEWLKH::updateKey(struct _tagKNode* kNode) // (2014.07.08) 그룹 키 갱신 함수
{
	ofstream out("tst_KeyRecipentSet.txt");

	/*
	if (kNode->level == 1 && kNode->index == 0) // 임시 - 디버깅 용
	{
		kNode->updated_eviction = false;
	}
	*/

	 // 키 트리 상에서 update_addition 또는 update_eviction 플래그가 참(true)으로 설정된 kNode의 키 갱신 (재귀적 호출)

	//if (kNode->updated_addition == true || kNode->updated_eviction == true) // 만약 해당 키의 갱신이 필요한 경우, 키 값 갱신
	if ( (kNode->updated_addition == true) || (kNode->updated_eviction == true) ||  // (2014.07.31) 만약 해당 키의 갱신이 필요한 경우, 키 값 갱신
		( (kNode->created == true) && (kNode->unode==NULL) ) ) // 1) 노드 추가에 의한 키 갱신, 2) 노드 제거에 의한 키 갱신, 3) 신규 kNode 생성으로 인한 키 갱신
	{
		GroupKey* gk;
		gk = kNode->groupKey; // 그룹 키 획득

		gk->oldVal = gk->newVal;
		gk->newVal = gen1.genRandInt(Max_Key_Value); // 그룹 키 갱신

		set<int> recipents;

		if (kNode->updated_eviction == true)
		{
			// 자식 노드들의 키로 암호화 -> 각각의 자식 노드 키를 소유한 센서 노드 그룹에 배포

			struct _tagKNode* tkNode;

			tkNode = kNode->leftChild;

			while (tkNode != NULL) // 각각의 자식 노드에 대해 반복
			{			
				recipents = this->getNdsInGrp(tkNode);

				if (!recipents.empty())
				{
					keyRecipentSet.insert ( (make_pair) (kNode->groupKey, recipents));
				}
				tkNode = tkNode->rightSibling;
			} // 키 - 수신 노드 pair 추가
		} // if 문 - updated_eviction == true 인 경우 처리 완료
		else if (kNode->updated_addition == true)
		{
			// 기존 키로 암호화 -> 기존 노드들에게 배포
			// 신규 노드들의 키로 암호화 -> 신규 노드들에게 배포
			
			recipents = this->getNdsInGrp(kNode);

			set<int> existingNodes; // 수신 노드 중 이전 라운드부터 존재하던 노드
			set<int> newNodes; // 수신 노드 중 현재 라운드에서 추가된 노드

			set<int>::iterator it_recipent;

			for( it_recipent = recipents.begin(); it_recipent != recipents.end(); it_recipent++) // 수신 노드 목록 순회 - 기존 노드와 신규 노드 구분
			{
				int nodeID;
				Node* node;
				nodeID = *it_recipent;
				node = nodes[nodeID];

				if (node->newbee == true)
				{
					newNodes.insert(nodeID);
				}
				else
				{
					existingNodes.insert(nodeID);
				}
			} // 기존 노드 목록과 신규 노드 목록 구분 완료

			keyRecipentSet.insert(  (make_pair) (kNode->groupKey, existingNodes)); // 기존 노드를 위한 <갱신 키, {수신 노드 ID}> pair 추가

			set<int>::iterator it_newRecipent;

			for(it_newRecipent = newNodes.begin(); it_newRecipent != newNodes.end(); it_newRecipent++) // 수신 노드 중 신규 노드 목록 순회
			{
				int newNodeID;
				newNodeID = *it_newRecipent;

				set<int> newRecipent;
				newRecipent.insert(newNodeID);

				keyRecipentSet.insert( (make_pair) (kNode->groupKey,  newRecipent) );

			} // 신규 노드를 위한 <갱신 키, {수신 노드 ID}> 구성 완료


		} // else-if 문 - updated_eviction == false이고 updated_addition == true인 경우 처리 완료

		else if (kNode->created == true && kNode->unode == NULL ) // (2014.07.31) 새로 생성된 그룹 키를 포함하는 kNode의 경우
		{
			// 해당 그룹에 포함된 모든 센서 노드들의 키로 암호화

			recipents = this->getNdsInGrp(kNode); // 해당 그룹에 속한 센서 노드들의 집합 획득

			set<int>::iterator it_recipent;

			for( it_recipent = recipents.begin(); it_recipent != recipents.end(); it_recipent++) // 수신 노드 목록 순회
			{
				int nodeID = *it_recipent;
				set<int> newRecipent;

				newRecipent.insert(nodeID);

				keyRecipentSet.insert ( (make_pair) (kNode->groupKey,  newRecipent) );

			} // for 문 종료 - 새로 생성된 kNode에 대응되는 그룹에 속한 노드들을 위한 <갱신 키, {수신 노드 ID} 구성 완료


		} // else-if 문 - created == true 이고 kNode->uNode == NULL 인 경우 (즉, 새로 생성된 group key의 경우) 처리 완료

	}

	  // 재귀 호출 종료 조건 - 해당 kNode가 노드의 individual key에 해당
	if (kNode->unode != NULL)
	{
		return;
	}
	
	 // 키 트리 순회

	if (kNode->leftChild != NULL)
	{
		updateKey(kNode->leftChild); // 해당 kNode의 leftChild에 대해 재귀 호출
	}

	if ( kNode != root && kNode->rightSibling != NULL)
	{
		updateKey(kNode->rightSibling); // 해당 kNode의 rightChild에 대해 재귀 호출
	}


	// { < 갱신 키 - {수신 노드 목록}>  } 확인
	 set <pair <GroupKey*, set<int>> >::iterator it;

	 for (it = keyRecipentSet.begin(); it != keyRecipentSet.end(); it++) // 모든  < 갱신 키 - {수신 노드 목록}> pair에 대해 반복
	 {
		 GroupKey* gk;
		 gk = (*it).first;

		 out << "groupKey (" << gk->oldVal << ", " << gk->newVal << ")";

		 set<int> recipents;
		 recipents = (*it).second;

		 set<int>::iterator it_recipent;

		 out << ", recipents: {";

		 for (it_recipent = recipents.begin(); it_recipent != recipents.end(); it_recipent++) // 수신 노드 출력
		 {
			 int nodeID;
			 nodeID = *it_recipent;
			 out << nodeID << ", ";

		 } // for문 종료 - 주어진 갱신 키의 수신 노드 목록 출력 완료

		 out << " }" << endl;
	 }


	out.close();
}
/*
void Kmr_NEWLKH::resetUpdatedKeys(struct _tagKNode* kNode)
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
void Kmr_NEWLKH::genKeyMsg() // 키 재분배 메시지 생성
{
	/// 키 갱신 - keyRecipentSet ( {<갱신 키 - 수신 노드 목록> } ) 구성

	int idx_keyMsg = 0;
	int idx_protokeyMsg = 0;
	this->keyRecipentSet.clear(); // 키 - 수신 노드 map 리셋
	//this->delKeyMsg(); // 키 재분배 메시지 제거 (메모리 반환)
	updateKey(root); // 키 갱신

	map<int, struct _tagProtoKeyMsg*> protoKeyMsgs; // 키 재분배 메시지 원형 맵

	// 키 재분배 메시지 프로토타입 (prototype) 생성 set <pair <GroupKey*, set<int>> >::iterator it;

	cout << " sizeof key msg: " << sizeof(KeyMsg) << endl;
	cout << " sizeof proto key msg: " << sizeof(ProtoKeyMsg) << endl;

	 set <pair <GroupKey*, set<int>> >::iterator it;

	 for (it = keyRecipentSet.begin(); it != keyRecipentSet.end(); it++) // 모든  < 갱신 키 - {수신 노드 목록}> pair에 대해 반복
	 {
		 GroupKey* gk;
		 gk = (*it).first;
		 
		 set<int> recipents;
		 recipents = (*it).second;
		 
		 ProtoKeyMsg* protokeyMsg = new ProtoKeyMsg(); // 키 메시지 원형 생성
		

		 protokeyMsg->newKeys.push_back(*gk); // 키 재분배 메시지에 그룹 키 정보 포함 
		 protokeyMsg->recipents = recipents; // 키 재분배 메시지에 수신 노드 ID 집합 저장		 

		 protoKeyMsgs[idx_protokeyMsg++] = protokeyMsg;

		
	 } // for문 종료 - 키 재분배 메시지 원형 생성
	
	// 키 재분배 메시지 프로토타입의 내용: {갱신된 (하위) 그룹 키 목록, 수신 노드 집합

	 
	 this->keyRecipentSet.clear(); // 키 - 수신 노드 map 리셋


	 map<int, ProtoKeyMsg*>::iterator it_protokm1;
	 map<int, ProtoKeyMsg*>::iterator it_protokm2;

	 set<int> emptyProtoKeyMsgs; // 삭제된 키 재분배 메시지 원형의 map 상에서의 key

	 ///***
	 for ( it_protokm1 = protoKeyMsgs.begin(); it_protokm1 != protoKeyMsgs.end(); it_protokm1++) // 키 재분배 메시지 원형들 간의 통폐합
	 {
		 int i, j;
		 i = (*it_protokm1).first;

		 if (protoKeyMsgs[i]->recipents.empty() ) // (2014.10.22) 키 재분배 메시지의 수신 노드 집합이 비어 있는 경우 다음 메시지로 이동
		 {
			 continue;
		 }

		 for ( it_protokm2 = protoKeyMsgs.begin(); it_protokm2 != protoKeyMsgs.end(); it_protokm2++) // (2014.08.05)		
		 {
			 j = (*it_protokm2).first;

			  if (protoKeyMsgs[j]->recipents.empty() ) // (2014.10.22) 키 재분배 메시지의 수신 노드 집합이 비어 있는 경우 다음 메시지로 이동
			 {
				 continue;
			 }

			 if (i == j)
			 {
				 continue;
			 }

			 bool isSubset = false;

			 isSubset = this->isSubset( protoKeyMsgs[j]->recipents, protoKeyMsgs[i]->recipents);

			if (isSubset == true) // (j번째) 키 재분배 메시지에 포함된 그룹 키 집합이 (i번째) 키 재분배 메시지에 포함된 그룹키 집합의 부분집합일 경우
			{
				IDSet::iterator it;

				for (it = protoKeyMsgs[j]->recipents.begin(); it != protoKeyMsgs[j]->recipents.end(); it++) // (j번째) 키 재분배 메시지에 포함된 수신 노드 ID를 (i번째) 키 재분배 메시지에서 제거
				{
					int nodeID;
					nodeID = *it;

					protoKeyMsgs[i]->recipents.erase(nodeID);
				} // for 문 종료 - (j번째) 키 재분배 메시지에 포함된 노드 ID를 (i번째) 키 재분배 메시지에서 제거

				list<GroupKey>::iterator it_newKeys;

				for ( it_newKeys = protoKeyMsgs[i]->newKeys.begin(); it_newKeys != protoKeyMsgs[i]->newKeys.end(); it_newKeys++) // (i번째) 키 재분배 메시지에 포함된 그룹 키를 (j번째) 키 재분배 메시지에 저장
				{
					GroupKey gk = *it_newKeys;
					protoKeyMsgs[j]->newKeys.push_back(gk);
				}// for문 종료 - i번째 키 재분배 메시지에 저장된 그룹 키를 j번째 키 재분배 메시지

				if (protoKeyMsgs[i]->recipents.empty() == true) // 2014.10.22 제거된 키 재분배 메시지 정보 저장 (위치 이동)
				{
					emptyProtoKeyMsgs.insert(i); // 제거된 키 재분배 메시지의 key 저장
					break; // inner for-loop 탈출
				}

 			} // if 문 - (i번째&j번째) 키 재분배 메시지 통합

			else
			{
				
			}
		 } // inner for-loops- (i번째) 키 재분배 메시지와 (j번째) 키 재분배 메시지를 비교 완료
		 /*
		 if (protoKeyMsgs[i]->recipents.empty() == true)
		 {
			 emptyProtoKeyMsgs.insert(i); // 제거된 키 재분배 메시지의 key 저장
		 }
		 */

	 } // outer for-loops - (i번째) 키 재분배 메시지와 나머지 키 재분배 메시지들 간의 비교 완료  

	 // 수신 노드 목록이 비어 있는 키 재분배 메시지 원형은 제거

	 set<int>::iterator it_protoKeyMsg;

	 for (it_protoKeyMsg = emptyProtoKeyMsgs.begin(); it_protoKeyMsg !=emptyProtoKeyMsgs.end(); it_protoKeyMsg++) // 수신 노드 목록이 비어 있는 키 재분배 메시지 원형 제거
	 {
		 int key = *it_protoKeyMsg;

		 delete (protoKeyMsgs[key]);
		 protoKeyMsgs.erase(key);
	 }	// 수신 노드 목록이 비어 있는 키 재분배 메시지 원형 제거 완료

	 ///***

	 /// 키 재분배 메시지 생성



	 for ( it_protokm1 = protoKeyMsgs.begin(); it_protokm1 != protoKeyMsgs.end(); it_protokm1++) // 키 재분배 메시지 원형으로부터 키 재분배 메시지 생성
	 {
		 ProtoKeyMsg* protoKeyMsg = (*it_protokm1).second; 

		 KeyMsg* keyMsg = new KeyMsg();
		 this->initKeyMsg(keyMsg);

		 keyMsg->newKeys = protoKeyMsg->newKeys;
		 keyMsg->recipents = protoKeyMsg->recipents;

		 keyMsgs[idx_keyMsg++] = keyMsg;
	 } // for문 - 키 재분배 메시지 초기 생성 완료

	 /// 키 재분배 메시지 원형 제거

	 for ( it_protokm1 = protoKeyMsgs.begin(); it_protokm1 != protoKeyMsgs.end(); it_protokm1++) // 키 재분배 메시지 원형으로부터 키 재분배 메시지 생성
	 {

		  ProtoKeyMsg* protoKeyMsg = (*it_protokm1).second; 
		  delete(protoKeyMsg);

	 } // 키 재분배 메시지 원형 제거 완료

	 protoKeyMsgs.clear();

	  /// 각 노드의 변경된 클러스터 정보를 키 재분배 메시지에 저장

	   // 클러스터가 변경된 노드 목록 구성

	set<int> nodesWithNewCH;	

	map<int, set<int>> clusters = rtn->getClusters(); // 클러스터 정보 획득

	map<int, set<int>>::iterator it_clstr; 

	for (it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++) // 클러스터 맵 순회 - 각 클러스터에 속한 노드들에 대해 클러스터 변경 여부 검사
	{
		set<int> nodesInCluster = (*it_clstr).second;	
		 set<int>::iterator it_ndID;

		 // (2014.09.21) 수정 - 
		int CHID = (*it_clstr).first;

		if ( nodes[CHID] == NULL) // CH가 이미 제거된 노드이면 다음 클러스터로 이동
		{
			continue;
		}
		// (2014.09.21 수정 완료) CH가 이미 제거된 노드이면 다음 클러스터로 이동

		for (it_ndID = nodesInCluster.begin(); it_ndID != nodesInCluster.end(); it_ndID++) // 클러스터 내 노드 집합 순회
		{
			bool clstrChange;

			int nodeID;
			nodeID = *it_ndID;

			if (nodes[nodeID] == NULL) // (2014.09.29) 이미 제거된 노드이면 건너 뛰기 
			{
				continue;
			}


			if (nodes[nodeID]->newbee == true) // 신규 노드일 경우
			{
				nodesWithNewCH.insert(nodeID);
			}

			clstrChange = rtn->hasUpdatedCH(nodeID);

			if (clstrChange == true)
			{
				nodesWithNewCH.insert(nodeID);
			}
		} // for문 종료 - 현재 클러스터 내 노드 목록 검사 완료 - 클러스터 변경 노드 탐색

	} // for문 종료 - CH가 변경된 노드들의 목록 구성 완료

	

	 // (2014.07.15) 여기까지 확인

	 // '클러스터 변경 노드' 목록 순회 -> 키 재분배 메시지 수정

	 set<int>::iterator it_ndWithNewCH;

	 for ( it_ndWithNewCH = nodesWithNewCH.begin(); it_ndWithNewCH != nodesWithNewCH.end(); it_ndWithNewCH++) // CH가 변경된 노드들의 목록 순회
	 {
		 int nodeID = *it_ndWithNewCH;

		  map<int, KeyMsg*>::iterator it_km3;

		  for ( it_km3 = keyMsgs.begin(); it_km3 != keyMsgs.end(); it_km3++) // 키 재분배 메시지 맵 순회
		  {
			  KeyMsg* km = (*it_km3).second;

			  if (km->recipents.find ( nodeID) != km->recipents.end()) // 만약 클러스터 변경 노드의 ID가 수신 노드 목록에 포함된 경우
			  {
				   int clusterID = rtn->getClstrID(nodeID); // 해당 노드의 변경된 CH' id를 키 재분배 메시지에 기록

				  if ( km->recipents.size() == 1) // 키 재분배 메시지의 수신 노드가 1개인 경우
				  {				

					  km->CHID = clusterID; 
				  } // if 문

				  else // 키 재분배 메시지의 수신 노드가 2개 이상인 경우
				  {
					    KeyMsg* keyMsg = new KeyMsg();
						this->initKeyMsg(keyMsg);

						keyMsg->newKeys = km->newKeys; // 그룹 키 목록 복사
						keyMsg->CHID = clusterID; // 신규 키 재분배 메시지에 클러스터 정보 저장
						keyMsg->recipents.insert(clusterID); // 신규 키 재분배 메시지로 수신 노드 정보 이동
						km->recipents.erase(clusterID); // 기존 키 재분배 메시지에서 수신 노드 정보 제거

						keyMsgs[idx_keyMsg++] = keyMsg; // 키 재분배 메시지 맵에 추가
					  
				  } // else 문

			  } // if문 종료 - 현재 '클러스터 변경 노드'에 대한 키 재분배 메시지 수정 
		  } // for문 종료 - // 키 재분배 메시지 맵 순회

	 } // for문 종료 - CH가 변경된 노드들의 목록 순회 - 키 재분배 메시지에 클러스터 정보 저장

	 // (2014.07.16) 여기까지 확인!!

	 /// 각 노드의 변경된 연관 노드 정보를 키 재분배 메시지에 저장

	   // '연관 노드가 변경된 노드 목록' 구성

	set<int> nodesWithNewAssoc; // 연관 노드가 변경된 노드 목록	

	for (it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++) // 클러스터 맵 순회 - 각 클러스터에 속한 노드들에 대해 연관 노드 변경 여부 검사
	{
		set<int> nodesInCluster = (*it_clstr).second;		

		set<int>::iterator it_ndID;

		// (2014.09.21) 수정 - 
		int CHID = (*it_clstr).first;

		if ( nodes[CHID] == NULL) // CH가 이미 제거된 노드이면 다음 클러스터로 이동
		{
			continue;
		}
		// (2014.09.21 수정 완료) CH가 이미 제거된 노드이면 다음 클러스터로 이동

		for (it_ndID = nodesInCluster.begin(); it_ndID != nodesInCluster.end(); it_ndID++) // 클러스터 내 노드 집합 순회
		{
			int nodeID = *it_ndID;

			bool assocChange; // 연관 노드 변경 여부

			if (nodes[nodeID] == NULL) // (2014.09.29) 이미 제거된 노드이면 건너 뛰기 
			{
				continue;
			}
			

			Sec_IHA* sec_IHA = dynamic_cast <Sec_IHA*> (sec); 

			assocChange = sec_IHA->hasUpdatedAssoc(nodeID);

			if (assocChange == true) // 연관 노드가 변경된 경우, 연관 노드 변경 노드 목록에 추가
			{
				nodesWithNewAssoc.insert(nodeID);
			}// if 문 종료 - 연관 노드 변경 노드 목록에 추가

		} // inner for loop 종료 - 클러스터 내 노드 집합 검사 완료

	} // outer for loop - 모든 클러스터의 노드 검사 완료
	
	// (2014.07.22) 여기까지 확인!!


	  // 키 재분배 메시지에 연관 노드 정보 저장

	set<int>::iterator it_ndWithNewAssoc; // 연관 노드가 변경된 노드 목록의 반복자 (iterator)

	for ( it_ndWithNewAssoc = nodesWithNewAssoc.begin(); it_ndWithNewAssoc != nodesWithNewAssoc.end(); it_ndWithNewAssoc++) // 연관 노드가 변경된 노드 목록 순회
	{

		int nodeID = *it_ndWithNewAssoc;

		map<int, KeyMsg*>::iterator it_km4;

		for ( it_km4 = keyMsgs.begin(); it_km4 != keyMsgs.end(); it_km4++) // 키 재분배 메시지 맵 순회
		{
			 KeyMsg* km = (*it_km4).second;

			 if (km->recipents.find ( nodeID) != km->recipents.end()) // 만약 연관 노드 변경 노드의 ID가 수신 노드 목록에 포함된 경우
			 {
				 Sec_IHA* sec_IHA = dynamic_cast <Sec_IHA*> (sec); 

				 std::set<int> upperNodes = sec_IHA->upperNodes[nodeID][INTERLEAVING_T+1];
				 std::set<int> lowerNodes = sec_IHA->lowerNodes[nodeID][INTERLEAVING_T+1];

				  if ( km->recipents.size() == 1) // 키 재분배 메시지의 수신 노드가 1개인 경우
				  {	
					  km->upAssocNds = upperNodes; // 키 재분배 메시지에 상위 연관 노드 정보 저장
					  km->downAssocNds = lowerNodes; //		"			하위		"
					 

				  } // if문 완료

				  else // 키 재분배 메시지의 수신 노드가 2개 이상인 경우
				  {
					    KeyMsg* keyMsg = new KeyMsg();
						this->initKeyMsg(keyMsg);

						keyMsg->newKeys = km->newKeys; // 그룹 키 목록 복사
						keyMsg->CHID = km->CHID; // 신규 키 재분배 메시지에 클러스터 정보 복사

						keyMsg->recipents.insert (nodeID); // 신규 키 재분배 메시지에 수신 노드 ID 저장
						km->recipents.erase (nodeID); // 기존 키 재분배 메시지에서 수신 노드 id 제거

						keyMsgs[idx_keyMsg++] = keyMsg; // 키 재분배 메시지 맵에 추가

				  } // else 문 종료 - 신규 키 재분배 메시지 생성 및 연관 노드 정보 저장
				  break;
			 } // if 문 완료 - 현재 키 재분배 메시지에 연관 노드 정보 저장 완료

		} // for문 완료 - 키 재분배 메시지에 연관 노드 정보 저장 완료

	} // for문 완료 - 키 재분배 메시지에 연관 노드 정보 추가 완료

	 testKeyMsg(); // 키 재분배 메시지 테스트

}

void Kmr_NEWLKH::testKeyMsg() // 키 재분배 메시지 테스트
{
	ofstream out("tst_keyMsg.txt");

	 map<int, KeyMsg*>::iterator it_km1;
	

	for ( it_km1 = keyMsgs.begin(); it_km1 != keyMsgs.end(); it_km1++)
	//for (int i = 0; i < keyMsgs.size(); i++) // 각각의 키 재분배 메시지 순회 및 파일에 출력
	{
		int i = (*it_km1).first;

		if (keyMsgs[i]->recipents.empty() == true)
		{
			continue;
		}

		list<GroupKey>::iterator it_newKeys;

		for(it_newKeys = keyMsgs[i]->newKeys.begin(); it_newKeys != keyMsgs[i]->newKeys.end(); it_newKeys++) // i번째 키 재분배 메시지의 그룹 키 목록 출력
		{
			GroupKey gk = *it_newKeys;
			out << "(old key: " << gk.oldVal << " new key: " << gk.newVal << "), ";
		} // for 문 종료 - 그룹 키 노드 목록 출력


		IDSet::iterator it_node;

		out << "recipents: {";

		for (it_node = keyMsgs[i]->recipents.begin(); it_node != keyMsgs[i]->recipents.end(); it_node++)  // i번째 키 재분배 메시지의 수신 노드 목록 출력
		{
			int nodeID;
			nodeID = *it_node;

			out << nodeID << ", ";
		} // for 문 종료 - 수신 노드 목록 출력

		out << " } " << endl;

		if ( keyMsgs[i]->CHID != -1)
		{
			out << "cluster Info. : "; // 클러스터 정보 출력
			out << keyMsgs[i]->CHID;
			out << endl;
		}
		// 연관 노드 정보 출력

		if ( !keyMsgs[i]->upAssocNds.empty())
		{
			IDSet::iterator it_upNd;

			out << "upper assoc. nodes: {"; // 상위 연관 노드 정보 출력

			for ( it_upNd = keyMsgs[i]->upAssocNds.begin(); it_upNd != keyMsgs[i]->upAssocNds.end(); it_upNd++) // 상위 연관 노드 정보 출력
			{
				int nodeID = *it_upNd;

				out << nodeID << ", ";
			}// for문 종료 - i 번째 메시지의 상위 연관 노드 정보 출력 완료

			out << " } " << endl;

		}// 

		if (! keyMsgs[i]->downAssocNds.empty())
		{
			IDSet::iterator it_downNd;

			out << "lower assoc. nodes: {"; // 하위 연관 노드 정보 출력

			for ( it_downNd = keyMsgs[i]->downAssocNds.begin(); it_downNd != keyMsgs[i]->downAssocNds.end(); it_downNd++) // 하위 연관 노드 정보 출력
			{
				int nodeID = *it_downNd;

				out << nodeID << ", ";
			}// for문 종료 - i 번째 메시지의 하위 연관 노드 정보 출력 완료

			out << " } " << endl;
		} // 

	} // for 문 종료- 키 재분배 메시지 출력 

	out.close();

	return;	
	
}

int Kmr_NEWLKH::getKeyMsgLen(struct _tagKeyMsg* keyMsg) // 키 메시지 길이 반환
{
	int len = 0;
	
	len += 2 + 2; // sender / receiver ID size;

	// 키 길이 계산	
	len += keyMsg->newKeys.size() * Group_Key_Size;

	// 목적 노드 ID 크기
	len += keyMsg->recipents.size() * ID_LEN;

	// CH ID, 연관 노드 id는 상황에 따라 포함될 수도, 포함 안 될 수도 있음

	// CH ID 크기
	if (keyMsg->CHID > -1)
	{
		len += ID_LEN;
	}
	// 연관 노드 ID 크기
	len += keyMsg->upAssocNds.size() * ID_LEN;
	len += keyMsg->downAssocNds.size() * ID_LEN;
	
	return len;
}

bool Kmr_NEWLKH::handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode)
{
	
	bool res = true; // 전달 계속 여부 (true: 계속 전달, false: 전달 완료)

	bool thisIsRecipent = false; // 현재 키 재분배 메시지를 수신한 노드가 해당 키 재분배 메시지의 목적 노드 중 하나인지 여부 (T/F)

	// 수신 노드의 ID가 목적 노드 목록에 있는 지 검사

	IDSet::iterator it;	

	for ( it = keyMsg->recipents.begin(); it != keyMsg->recipents.end(); it++) // 현재 수신 노드가 목적 노드 목록에 포함되어 있는지 확인
	{
		int recipentID = (*it);

		if ( cNode->id == recipentID)
		{
			this->receivedNodes.insert(cNode->id); // (2014.08.07) 키 재분배 메시지 수신 노드 정보 저장 (debugging 용)
			thisIsRecipent = true;		
		}
	} // for문 종료 - 목적 노드 목록 확인 완료

	if (thisIsRecipent == true)
	{
		keyMsg->recipents.erase(cNode->id); // 현재 노드가 목적 노드 목록에 포함되어 있으면 해당 목록에서 현재 노드 ID 제거

		if ( keyMsg->recipents.empty())
		{
			res = false;
		}
	} // IF문 종료 - 현재 노드가 목적 노드인 경우의 처리 완료

	/*
	if (thisIsRecipent == false) // 현재 노드가 목적 노드가 아니면 반환
	{
		return res;
	}
	*/


	// 그룹 키 갱신 (키 트리에 있는 그룹 키의 내용대로 변경되었다고 가정)


	// 클러스터 정보 갱신 (rtn)
	/*
	int cNodeID = cNode->id;
	int clusterID = keyMsg->CHID;

	if (clusterID > -1)
	{
		rtn->changeCluster(cNodeID, clusterID); 
	}

	// 연관 노드 정보 갱신

	NodeIDLst::iterator NodeIDLst_it;

	IDSet::iterator NodeIDSet_it;

	if (!keyMsg->upAssocNds.empty())
	{

		std::set<int> upperAssocnodeset;

		for (NodeIDSet_it = keyMsg->upAssocNds.begin(); NodeIDSet_it != keyMsg->upAssocNds.end(); NodeIDSet_it++)
		//for (NodeIDLst_it = keyMsg->upAssocNds.begin(); NodeIDLst_it != keyMsg->upAssocNds.end(); NodeIDLst_it++)
		{
			int nodeID = (*NodeIDSet_it);

			upperAssocnodeset.insert(nodeID);
		}

		sec->updateAssocNodes(cNodeID, 0, INTERLEAVING_T+1, upperAssocnodeset); // 상위 연관 노드 정보 갱신 (iha, diha)
		//sec->updateAssocNodes(cNodeID, 0, upperAssocnodeset); // 상위 연관 노드 정보 갱신 (iha, diha)
	}

	if (!keyMsg->downAssocNds.empty())
	{

		std::set<int> lowerAssocnodeset;

		for (NodeIDSet_it = keyMsg->downAssocNds.begin(); NodeIDSet_it != keyMsg->downAssocNds.end(); NodeIDSet_it++)
		//for (NodeIDLst_it = keyMsg->downAssocNds.begin(); NodeIDLst_it != keyMsg->downAssocNds.end(); NodeIDLst_it++)
		{
			int nodeID = (*NodeIDSet_it);

			lowerAssocnodeset.insert(nodeID);
		}

		sec->updateAssocNodes(cNodeID, 1, INTERLEAVING_T+1, lowerAssocnodeset); // 하위 연관 노드 정보 갱신 (iha, diha)
		//sec->updateAssocNodes(cNodeID, 1, lowerAssocnodeset); // 하위 연관 노드 정보 갱신 (iha, diha)
	}
	*/
	return res;
}

/*
void Kmr_NEWLKH::genKeyMsg()
{

}
*/
/*
KeyMsg* Kmr_NEWLKH::getKeyMsg() // 키 재분배 메시지 반환
{

}
*/
/*
void Kmr_NEWLKH::addPairKey (int nID, int nID2)
{

}
*/
int Kmr_NEWLKH::cnstrctKTree() // 키 트리 초기 구축
{
	int height = 0;

	this->root = new KNode;


	// (2014.04.23) root 노드 초기화 필요!!
	// root 노드 초기화
	this->initKNode (root);
	//root->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

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
	int numNodes = nodes.size();

	SrchNode_KTS::setNumNodes(numNodes); // (2014.04.22) 상태 구성에 필요한 변수 (센서 노드 개수) 값 설정

	SrchNode* solution = astar->findSolution(); // key tree 구성에 필요한 degree 수열 획득

	//std::list<SrchNode*> degreeSeq = astar->findSolution(); // key tree 구성에 필요한 degree 수열 획득

	// key tree 구성

	//list<SrchNode*>::iterator it;

	int cur_level = 0;	

	map <int, int> degreeSeq;

	try 
	{

		SrchNode_KTS* srchNode_KTS = dynamic_cast<SrchNode_KTS*>(solution);

		degreeSeq = srchNode_KTS->getDegreeSeq();  // 발견된 해에 대응되는 degree 순열 획득

	} catch (exception& e) {cout << "Exception: " << e.what();}

	height = degreeSeq.size() +1; // (2014.06.10) 키 트리의 높이 = degree seq.의 크기 +1 
	this->kTreeHeight = height;

	

	map <int, int>::iterator it;

	int currentLevel = 0;
	numNodesAtLevel[0] = 1; // level 0의 노드 수 저장

	cur_level_knodes.clear();
	cur_level_knodes.push_back(root); // 현재 level 노드 목록 구성 (level 0: root)

	for(it = degreeSeq.begin(); it != degreeSeq.end(); it++)
	{
		currentLevel = it->first + 1;
		int degree = it->second;
		numNodesAtLevel[currentLevel] = numNodesAtLevel[currentLevel-1] * degreeSeq[currentLevel-1]; // 현재 level의 노드 수 저장
		expandLevel(currentLevel, degree);
		//this->expandLevel(degree);
	} // level 0 ~ (h-1) 까지 구성


	// /
	list<struct _tagKNode*>::iterator it2;

	for (it2 = cur_level_knodes.begin(); it2!= cur_level_knodes.end(); it2++)
	{
		struct _tagKNode* kNode = *it2;

		UNode* uNode = new UNode; // u-node 생성	 
		uNode->sensorNode = NULL;
		uNode->isCH = false;
		kNode->unode = uNode; // k-node, u-node 간 포인터 설정
		uNode->parent = kNode;

		if (kNode->parent != NULL)// (2014.06.10) cluster key 노드 목록 저장, isCluster 플래그 설정
		{
			kNode->parent->isCluster = true; // 
			this->cKNodes.insert(kNode->parent);
		}
	}
	// level h (u-node) 구성

	//height = currentLevel + 1; // height = cluster key level +2

	numNodesAtLevel[height] = numNodesAtLevel[height-1]; // level-h의 노드 수 (u-node)
		

	// ...
	//this->kTreeHeight = height;
	return height; // key tree의 높이 반환
}

/*
int Kmr_NEWLKH::cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes) // (2014.06.05) // 키 트리 부분적 재구성
{
	int height = 0;

	// A*에 의한 degree seq. 구함 

	int numNodes;
	numNodes = nodes.size();

	SrchNode_KTS::setNumNodes(numNodes); // (2014.04.22) 상태 구성에 필요한 변수 (센서 노드 개수) 값 설정

	SrchNode* solution = astar->findSolution(); // key tree 구성에 필요한 degree 수열 획득

	int cur_level = 0;	

	map <int, int> degreeSeq;

	try 
	{

		SrchNode_KTS* srchNode_KTS = dynamic_cast<SrchNode_KTS*>(solution);

		degreeSeq = srchNode_KTS->getDegreeSeq();  // 발견된 해에 대응되는 degree 순열 획득

	} catch (exception& e) {cout << "Exception: " << e.what();
	}

	// 부분적 key tree 구성 

	// 기존에 root 에 딸려 있는 하위 트리를 제거
	struct _tagKNode* prevKNode;
	struct _tagKNode* curKNode;
	prevKNode = curKNode = NULL;

	curKNode = root->leftChild;

	while (curKNode != NULL) // root의 각 자식 knode를 root로 하는 subtree 제거
	{
		prevKNode = curKNode;
		curKNode = curKNode;
		delete (prevKNode);
	} // root 노드의 하위 트리 정리 완료

	cur_level_knodes.clear();
	cur_level_knodes.push_back(root); // 현재 level 노드 목록 구성 (level 0: root)

	map <int, int>::iterator it;

	for(it = degreeSeq.begin(); it != degreeSeq.end(); it++)
	{
		cur_level = it->first + 1;
		int degree = it->second;
		//numNodesAtLevel[currentLevel] = numNodesAtLevel[currentLevel-1] * degreeSeq[currentLevel-1]; // 현재 level의 노드 수 저장
		expandLevel(cur_level, degree);
		
	} // level 0 ~ (h-1) 까지 구성

	list<struct _tagKNode*>::iterator it2;

	for (it2 = cur_level_knodes.begin(); it2!= cur_level_knodes.end(); it2++) // (h-1)-level의 k-node들에 대해 반복
	{
		struct _tagKNode* kNode = *it2;

		UNode* uNode = new UNode; // u-node 생성	 
		this->initUNode(uNode); // u-node 초기화
		
		kNode->unode = uNode; // k-node, u-node 간 포인터 설정
		uNode->parent = kNode;
	}
	// level h (u-node) 구성

	height = cur_level + 1; // height = individual key level +1

	return height;
}*/

int Kmr_NEWLKH::cnstrctKTree(struct _tagKNode* root, set<int> nodes) // (2014.06.08) // 키 트리 부분적 재구성
{
	int height = 0;

	root->CHID = -1; // (2014.09.27) CH ID 리셋

	/* A*에 의한 degree seq. 구함 */

	int numNodes;
	numNodes = nodes.size();

	SrchNode_KTS::setNumNodes(numNodes); // (2014.04.22) 상태 구성에 필요한 변수 (센서 노드 개수) 값 설정

	SrchNode* solution = astar->findSolution(); // key tree 구성에 필요한 degree 수열 획득

	int cur_level = 0;	

	map <int, int> degreeSeq;

	try 
	{

		SrchNode_KTS* srchNode_KTS = dynamic_cast<SrchNode_KTS*>(solution);

		degreeSeq = srchNode_KTS->getDegreeSeq();  // 발견된 해에 대응되는 degree 순열 획득

	} catch (exception& e) {cout << "Exception: " << e.what();
	}

	/* 부분적 key tree 구성 */

	// 기존에 root 에 딸려 있는 하위 트리를 제거
	struct _tagKNode* prevKNode;
	struct _tagKNode* curKNode;
	prevKNode = curKNode = NULL;

	curKNode = root->leftChild;

	if (curKNode != NULL)
	{
		dstrctKTree(curKNode); 
	}
	root->leftChild = NULL; // 자식 kNode들과의 연결 해제
	/*
	while (curKNode != NULL) // root의 각 자식 knode를 root로 하는 subtree 제거
	{
		prevKNode = curKNode;
		curKNode = curKNode->rightSibling;
		dstrctKTree(prevKNode); 
		//delete (prevKNode);
	} // root 노드의 하위 트리 정리 완료
	*/
	cur_level_knodes.clear();
	cur_level_knodes.push_back(root); // 현재 level 노드 목록 구성 (level 0: root)

	map <int, int>::iterator it;

	for(it = degreeSeq.begin(); it != degreeSeq.end(); it++)
	{
		cur_level = it->first + 1;
		int degree = it->second;
		//numNodesAtLevel[currentLevel] = numNodesAtLevel[currentLevel-1] * degreeSeq[currentLevel-1]; // 현재 level의 노드 수 저장
		expandLevel(cur_level, degree);
		
	} // level 0 ~ (h-1) 까지 구성

	list<struct _tagKNode*>::iterator it2;

	for (it2 = cur_level_knodes.begin(); it2!= cur_level_knodes.end(); it2++) // (h-1)-level의 k-node들에 대해 반복
	{
		struct _tagKNode* kNode = *it2;

		UNode* uNode = new UNode; // u-node 생성	 
		this->initUNode(uNode); // u-node 초기화
		
		kNode->unode = uNode; // k-node, u-node 간 포인터 설정
		uNode->parent = kNode;

		if (kNode->parent != NULL)// (2014.06.10) cluster key 노드 목록 저장, isCluster 플래그 설정
		{
			kNode->parent->isCluster = true; // 
			this->cKNodes.insert(kNode->parent);
		}
	}
	// level h (u-node) 구성

	height = cur_level + 1; // height = individual key level +1

	return height;
}

int Kmr_NEWLKH::updateKTree(IDSet addedNodes, IDSet deletedNodes) // (2014.05.29) 키 트리 갱신 함수 - 입력: 추가 노드 목록 (addedNodes), 제거 노드 목록 (deletedNodes), 출력: 키 트리의 높이
{

	int height;		

	set<struct _tagKNode*> modiClstrKey; // 노드 추가/제거로 인해 변경된 클러스터 키 목록

	this->restruct_pts.clear(); // 재 구조화 point 목록 리셋

	// root 노드 생성 및 초기화 -> Kmr 객체의 생성자에서 처리

	/* 키 트리 상에서 노드 제거 */
	
	//if (nodes.empty() == false)// 노드 목록이 비어 있지 않은 경우에만 수행
	{
		struct _tagKNode* clstrKey;
		clstrKey = NULL;

		struct _tagKNode* indiKey;
		indiKey = NULL;

	//	int clusterLevel = this->kTreeHeight - 2;
	//	int numClusters;

	//	numClusters = getNumNodesAtLevel(clusterLevel); // 클러스터의 개수 획득

		set<struct _tagKNode*>::iterator it_ckNodes;

		for (it_ckNodes = this->cKNodes.begin(); it_ckNodes != cKNodes.end(); it_ckNodes++) // 클러스터 키 (kNode) 순회
		//for (int cl = 0; cl < numClusters; cl++) // // 각 클러스터에 대해 반복
		{
			clstrKey = *it_ckNodes; // (2014.06.10) 클러스터 키 획득

			//clstrKey = this->getKNode(clusterLevel, cl); // 클러스터 키 획득

			indiKey = clstrKey->leftChild;

			struct _tagKNode* oldKey;
			oldKey = NULL;

			while (indiKey != NULL) //   // 클러스터 내 노드들의 individual key에 대해 반복
			{

				struct _tagUNode* uNode = NULL;
				uNode = indiKey->unode; // u-Node 획득
				Node* node = uNode->sensorNode;

				if (node == NULL)
				{
					indiKey= indiKey->rightSibling; // 다음 individual key 로 이동
					continue;
				}

				IDSet::iterator it_find;
				it_find = deletedNodes.find( node->id); // 제거 노드 목록에 있는 지 검사

				if (it_find != deletedNodes.end()) // 현재 indi_key 와 연관된 노드가 제거 노드 목록에 있는 경우
				{
					modiClstrKey.insert(clstrKey); // 변경된 클러스터 키 목록 추가

					if (clstrKey->CHID == node->id)// (2014.07.03)
					{
						set<int> nodesInGrp;
						
						nodesInGrp = this->getNdsInGrp(clstrKey);

						restruct_pts[clstrKey] = nodesInGrp; // 만약 기존 CH가 제거될 경우에는 해당 클러스터의 키가 저장된 kNode를 재구조화 목록에 추가
					
						rtn->removeCH(node->id);
					
						clstrKey->CHID = -1; // (2014.09.27) 해당 kNode의 CH 정보를 리셋
					}

					oldKey = indiKey;

					struct _tagKNode* tKNode; // eviction-point에서 root까지의 경로에 위치하는 k-node의 update_eviction 플래그 설정

					tKNode = oldKey->parent; // kNode 포인터를 eviction-point로 초기화

					while (tKNode != NULL) // root 노드까지의 경로에 존재하는 kNode들에 대해 반복
					{
						tKNode->updated = true; 
						tKNode->updated_eviction = true; // update_eviction 플래그 설정
						tKNode = tKNode->parent;
					}

					delete (uNode); // u-node 제거

					// (2014.07.14) 클러스터 정보 변경

					rtn->removeFromCluster(node->id); // 네트워크에서 제거된 센서 노드를 클러스터 목록에서 제거

					// 포인터 설정

					if (indiKey->leftSibling == NULL) // 현재 indiKey가 cluster Key의 leftChild 인 경우
					{
						clstrKey->leftChild = indiKey->rightSibling;
					}
					else // indiKey->leftSibling != NULL /// 현재 indiKey가 cluster Key의 leftChild가 아닌 경우
					{
						indiKey->leftSibling->rightSibling = indiKey->rightSibling; // left sibling -> right sibling 연결 설정
					}

					if (indiKey->rightSibling != NULL)
					{
						indiKey->rightSibling->leftSibling = indiKey->leftSibling; // left sibling <- right sibling 연결 설정
					}

					indiKey = indiKey->rightSibling;

					delete (oldKey); 

					clstrKey->outDegree--; // (2014.09.04) eviction point의 outDegree -1감소

					continue;
				} // 현재 노드 제거 완료
			
				indiKey = indiKey->rightSibling;

			} // while 문 종료 - 현재 클러스터 내 노드 제거 완료
		} // for 문 종료 - 모든 클러스터들의 노드 제거 완료
	} // 노드 제거 작업 완료
	// (2014.06.10) 여기까지 확인 완료!

	//this->printKTree(); // debugging 용
	//cout << endl << endl;
	/* 키 트리 상에서 노드 추가 */

	IDSet::iterator it_add;

	//if (nodes.empty() != true)
	{
		//int clusterLevel = this->kTreeHeight - 2;
		//int numClusters;

		//numClusters = getNumNodesAtLevel(clusterLevel); // 클러스터의 개수 획득



		for (it_add = addedNodes.begin(); it_add != addedNodes.end(); it_add++) // 추가된 노드들에 대해 반복
		{
			set<struct _tagKNode*> adjClstrKey; // 인접 클러스터들의 kNode 집합

			int nodeID;

			nodeID = *it_add; // 노드 ID 획득

			Node* newNode;
		
			newNode = nodes[nodeID]; 

			// 신규 노드가 추가될 클러스터 결정 

			struct _tagKNode* clusterKey; // cluster에 대응되는 각각의 k-node들에 대해 반복						

			set<struct _tagKNode*>::iterator it_ckNodes;

			for (it_ckNodes = this->cKNodes.begin(); it_ckNodes != cKNodes.end(); it_ckNodes++) // 클러스터 키 (kNode) 순회
			//for (int cl = 0; cl < numClusters; cl++) // // 각 클러스터에 대해 반복
			{
				clusterKey = *it_ckNodes; // (2014.06.10) 클러스터 키 획득

				//clusterKey = this->getKNode(clusterLevel, cl); // 클러스터 키 획득
				
				// (2014.06.20) 인접 클러스터 계산 방법 개선 

				double dist; // 신규 노드와 기존 클러스터의 무게 중심과의 거리 계산

				dist = sqrt( pow( (double) newNode->x - clusterKey->COG_X, 2) + pow ( (double) newNode->y - clusterKey->COG_Y, 2 ));

				if (dist < TRANS_RANGE) // 신규 노드와 기존 CH 간의 거리가 전송 범위 이내이면 인접 클러스터 키 목록에 KNODE 추가
				{
					adjClstrKey.insert(clusterKey); 
				}

				/*
				int CH_ID ;
				CH_ID = clusterKey->CHID;

				if (CH_ID < 0)
				{
					cerr << "(CH_ID < 0)" << endl;
					exit(-1);
				}

				double dist; // 신규 노드와 기존 CH와의 거리 계산

				dist = sqrt( pow( (double) newNode->x - nodes[CH_ID]->x, 2) + pow ( (double) newNode->y - nodes[CH_ID]->y, 2 ));

				if (dist < TRANS_RANGE) // 신규 노드와 기존 CH 간의 거리가 전송 범위 이내이면 인접 클러스터 키 목록에 KNODE 추가
				{
					adjClstrKey.insert(clusterKey); 
				}
				*/

			} // 인접 클러스터 목록 구성 // 
			// (2014.06.10) 여기까지 확인 완료!
			int minNumNdsInClstr = -1;
			struct _tagKNode* clstrWithMinNumNds = NULL;

			set<struct _tagKNode*>::iterator it_clstr;

			for (it_clstr = adjClstrKey.begin(); it_clstr != adjClstrKey.end(); it_clstr++)
			{
				struct _tagKNode* clstrKey;
				clstrKey = *it_clstr;

				int numNodes = this->getNumNdsInGrp(clstrKey); // 클러스터에 속한 노드 수 계산

				if ( minNumNdsInClstr < 0) // 최소 노드 수가 아직 초기화 되지 않은 경우
				{
					minNumNdsInClstr = numNodes;
					clstrWithMinNumNds = clstrKey;
				}
				else if ( numNodes < minNumNdsInClstr) // 노드 수가 기존 최소 노드 수보다 작으면 노드 수와 클러스터 키를 갱신
				{
					minNumNdsInClstr = numNodes;
					clstrWithMinNumNds = clstrKey;
				}
			}	// 신규 노드가 추가될 클러스터 결정

			// 신규 노드를 선택된 클러스터에 추가

			struct _tagKNode* addPoint;
			addPoint = clstrWithMinNumNds; // addition_point 설정

			if (addPoint == NULL) // 인접한 클러스터가 없을 경우 현재 신규 노드 skip, 다음 신규 노드로 진행
			{
				continue;
			}

			struct _tagKNode* kNode = new struct _tagKNode; // k-node 생성
			this->initKNode(kNode); // k-node 초기화

			struct _tagUNode* uNode = new struct _tagUNode; // u-node 생성
			this->initUNode(uNode);  // u-node 초기화	

			
			kNode->unode = uNode;
			kNode->parent = addPoint;

			uNode->parent = kNode;
			uNode->sensorNode = newNode;

			if (addPoint->leftChild != NULL)
			{
				kNode->rightSibling = addPoint->leftChild;	// left - right sibling 노드 간 연결 설정
				kNode->rightSibling->leftSibling = kNode; 
			}

			addPoint->leftChild = kNode; // addition point의 left child 연결 설정			

			rtn->changeCluster(newNode->id, clstrWithMinNumNds->CHID); // 신규 노드의 클러스터 설정

			modiClstrKey.insert(clstrWithMinNumNds); // 변경된 클러스터 키 목록 추가

			addPoint->outDegree++; // (2014.09.04) addition point의 out degree +1증가


			// 신규 노드 추가 완료

			// add_point에서 root까지의 경로에 존재하는 k-node들의 update_addition 플래그 설정

			struct _tagKNode* tKNode; // addition-point에서 root까지의 경로에 위치하는 k-node의 update_addition 플래그 설정

			tKNode = addPoint;

			while (tKNode != NULL) // root 노드까지의 경로에 존재하는 kNode들에 대해 반복
			{
				tKNode->updated = true; 
				tKNode->updated_addition = true; // updated_addition 플래그 설정
				tKNode = tKNode->parent; // 상위 k-node로 이동
			}
						
			//this->printKTree(); // debugging 용
			//cout << endl << endl;
		} // for 문 - 신규 노드 추가 완료
	}

	// (2014.06.11) 여기까지 확인

	//this->printKTree(); // debugging 용

	/* 재 구조화 지점 탐색 */
	//this->restruct_pts.clear(); // 재 구조화 point 목록 리셋
	struct _tagKNode* restruct_pt = NULL; // 재 구조화 point

	struct _tagKNode* groupKey = NULL; // 그룹 키

	set<struct _tagKNode*>::iterator it_modiClstr;

	for (it_modiClstr = modiClstrKey.begin(); it_modiClstr != modiClstrKey.end(); it_modiClstr++) // 변경된 클러스터들에 대해 반복
	{
		struct _tagKNode* modiCK; // 변경된 클러스터의 클러스터 키
		modiCK = *it_modiClstr;

		groupKey = modiCK; // 현재 검사중인 그룹 키

		int numNodesinGrp; // 그룹에 속한 노드 수
		int numClstrsinGrp; // 그룹에 속한 클러스터 수
		set<int> nodesInGrp; // 그룹에 속한 노드 목록

		numClstrsinGrp = 1; 

		bool reconstruct_needed = false; // 재구조화 필요성 여부 (T: 필요, F: 불필요)

		while (groupKey != root) // '클러스터 당 노드 수'가 적정 범위에 속하는 최소 그룹의 root 계산
		{
			nodesInGrp = this->getNdsInGrp(groupKey);// 그룹에 속한 노드 id 집합 획득

			

			numNodesinGrp = nodesInGrp.size();// 그룹에 속한 노드 수 계산

			//numNodesinGrp = this->getNumNdsInGrp(groupKey); // 그룹에 속한 노드 수 계산
			numClstrsinGrp = this->getNumClstrsInGrp(groupKey); // 그룹에 속한 클러스터 수 계산

			int nodesPerCluster;
			nodesPerCluster = (numNodesinGrp / numClstrsinGrp); // 클러스터 당 평균 노드 수 계산
			
			if ( (nodesPerCluster >= MIN_NNC) && (nodesPerCluster <= MAX_NNC )) // 적정 노드 수를 만족하는 경우 반복문 탈출
			//if ( (nodesPerCluster >= INTERLEAVING_T+1) && (nodesPerCluster <= 2* (INTERLEAVING_T+1) ) ) // 적정 노드 수를 만족하는 경우 반복문 탈출
			{
				if (reconstruct_needed == true) // 재 구조화가 필요할 경우 재구조화 지점 저장
				{
					restruct_pt = groupKey;
				}			
				break;
			}
			else
			{
				reconstruct_needed = true;
				groupKey = groupKey->parent;
			}

		} // '클러스터 당 노드 수'가 적정 범위에 속하는 최소 그룹의 root 계산, 필요 시 재 구조화 지점 저장

		if (reconstruct_needed == true)
		{
			//restruct_pts.insert(groupKey); 

			if (groupKey == root)
			{
				nodesInGrp = this->getNdsInGrp(groupKey);// 재 구조화 지점이 root일 경우 노드 집합 다시 계산
			}
			
			restruct_pts[groupKey] = nodesInGrp; // 재 구조화 지점과 해당 그룹의 소속 노드 ID 집합 저장
			
		}
	}
	// 재 구조화 지점 탐색 완료
	// (2014.08.01) 여기까지 확인

	/// 서로 다른 재 구조화 지점 사이에 subtree 관계가 성립할 경우, 재 구조화 지점 간의 통합 필요
	// (2014.08.01 수정시작)

	map<struct _tagKNode*, set<int> >::iterator it_restrct_pt;
	map<struct _tagKNode*, set<int> >::iterator it_restrct_pt2;

	set<struct _tagKNode*> deleted_restrct_pts;

	for (it_restrct_pt = restruct_pts.begin(); it_restrct_pt != restruct_pts.end(); it_restrct_pt++) // 제거된 재 구조화 지점 목록 구성
	{
		struct _tagKNode* restrct_point = (*it_restrct_pt).first;

		for ( it_restrct_pt2 = restruct_pts.begin(); it_restrct_pt2 != restruct_pts.end(); it_restrct_pt2++) // 현재 재 구조화 지점과 나머지 재 구조화 지점들 간 비교
		{
			struct _tagKNode* restrct_point2 = (*it_restrct_pt2).first;
			struct _tagKNode* temp = restrct_point2;

			bool isDescendent = false; // 2번째 재 구조화 지점이 1번째 재 구조화 지점의 후손

			if (restrct_point == restrct_point2)
			{
				continue;
			}

			while ( 1) // 2번째 재 구조화 지점이 1번째 재 구조화 지점의 후손인지 확인
			{

				if (temp == restrct_point)
				{
					isDescendent = true;
					break;
				}
				else if ( temp == root)
				{
					break;
				}

				temp = temp->parent;
			} // while문 완료 - 	2번째 재 구조화 지점이 1번째 재 구조화 지점의 후손인지 확인 완료		

			if (isDescendent == true) // 현재 재 구조화 지점을 조상으로 하는 다른 재 구조화 지점을 제거될 재 구조화 지점 목록에 추가
			{
				deleted_restrct_pts.insert(restrct_point2);
			} // if문 완료

		} // for 문 완료 - 현재 재 구조화 지점을 조상으로 하는 다른 재 구조화 지점을 제거될 재 구조화 지점 목록에 추가

	} // for 문 완료 - 제거된 재 구조화 지점 목록 구성


	// 재 구조화 지점 간 통폐합 - 임의의 재 구조화 지점에서 root 노드까지의 경로 상에 다른 재 구조화 지점이 존재하는 경우

	set<struct _tagKNode*>::iterator it_del_restrct_pt2;

	for (it_del_restrct_pt2 = deleted_restrct_pts.begin(); it_del_restrct_pt2 !=deleted_restrct_pts.end(); it_del_restrct_pt2++) // 
	{
		struct _tagKNode* del_restrct_pt = *it_del_restrct_pt2;
		restruct_pts.erase (del_restrct_pt);
	} // for문 종료 - 불필요한 재 구조화 지점 제거

	// (2014.08.01 수정완료)

	/* 각각의 재 구조화 지점에 대해 키 트리 부분적 재구성 */

	//map<struct _tagKNode*, set<int> >::iterator it_restrct_pt;
	//set<struct _tagKNode*>::iterator it_restrct_pt;

	for (it_restrct_pt = restruct_pts.begin(); it_restrct_pt != restruct_pts.end(); it_restrct_pt++) // 재 구조화 지점들에 대해, 해당 그룹 내의 노드 목록을 다시 설정
	{
		struct _tagKNode* restrct_point;
		restrct_point = (*it_restrct_pt).first;

		set<int> nodesInGrp;
		nodesInGrp = this->getNdsInGrp(restrct_point);

		(*it_restrct_pt).second = nodesInGrp;

	}



	for (it_restrct_pt = restruct_pts.begin(); it_restrct_pt != restruct_pts.end(); it_restrct_pt++)
	{
		struct _tagKNode* restrct_point;
		restrct_point = (*it_restrct_pt).first;
		
		set<int> nodesInGrp;
		nodesInGrp = (*it_restrct_pt).second;

		this->cnstrctKTree(restrct_point, nodesInGrp); // 키 트리 재구조화
		//this->cnstrctKTree(restrct_point, ); // node map 필요
	}

	// (2014.06.12) 여기까지 확인!

	/* level, index 재설정 */

	height = this->renumberKNodes();
	this->kTreeHeight = height; // (2014.10.09) 키 트리 높이 갱신
	//this->printKTree(); // debugging

	return height;
}

void Kmr_NEWLKH::dstrctKTree()
{
	// 구현 중 

	// 재귀 방식으로 구현	

	try
	{
		dstrctKTree(root);
	} catch (exception& e) {cout << "Exception: " << e.what();}

}

void Kmr_NEWLKH::dstrctKTree(struct _tagKNode* cur_knode)
{
	struct _tagKNode* leftChild = cur_knode->leftChild;
	struct _tagKNode* rightSibling = cur_knode->rightSibling;

	// (2014.06.05) individual key일 경우, u node 반환
	if (cur_knode->unode != NULL)
	{
		delete (cur_knode->unode);
	}

	if (leftChild != NULL) // left child, right- sibling, 자기 자신의 순서로 제거
	{
		dstrctKTree(leftChild);
	}

	if (rightSibling != NULL)
	{
		dstrctKTree(rightSibling);
	}

	set<struct _tagKNode*>::iterator it_find;

	it_find = this->cKNodes.find(cur_knode);

	if (it_find != cKNodes.end() ) // (2014.06.10) kNode가 삭제될 때, cluster key 목록에 있으면 목록에서 제거
	{
		cKNodes.erase(cur_knode); 
	}

	delete (cur_knode);
}

void Kmr_NEWLKH::cnstrctLBHG() // (2014.09.03 수정) 계층적 그룹 형성
{
	
	/* 1. GA를 활용한 해 도출 */

	Chromosome solution; // 계층적 그룹 형성을 위한 해 (solution)

	// 개체의 길이 (개체 당 유전자 개수) 계산
	
	int indiLen = 0;

	double dist; // 각 노드에서 CH까지의 거리

	for (int i = 0; i < getKTreeHeight(); i++) // 키 트리에 속한 k-node들의 개수를 구함
	{
		indiLen += getNumNodesAtLevel(i);
	}

	ga->setFullSetNodes(nodes); // (2014.09.19) 
	ga->setNodes(nodes);
	ga->setKmr(this);
	ga->setRoot(root);

	ga->setIndiLen(indiLen); // 개체 길이 설정

	solution = ga->findSolution(); // GA 알고리즘을 통하여 해 도출
	ga->testSolution();
	
	/* 2. 도출된 해를 분석하여 key tree의 해당 leaf node에 센서 노드 정보 저장 */

	this->isolatedNodes.clear(); // 고립된 노드 목록 리셋

	unsigned int geneIdx = 0; // 반환된 개체의 염색체를 scan 하기 위한 인덱스

	struct _tagKNode* kNode = root;// 현재 탐색 중인 키 트리 상의 k-node

	int level = 0; // 물리적 그룹 계층 구조 상에서의 level
	int index = 0; // 물리적 그룹 계층 구조 상에서의 index

	int ndIdxInClstr = 0; // 현재 물리적 그룹에 포함된 노드의 인덱스

	map<int, char> solGeneType = ga->getSolGeneType(); // 해에 포함된 유전자의 종류 ( 'G': group, 'N': node)

	for (geneIdx = 0 ; geneIdx < solution.size(); geneIdx++) // 염색체에 포함된 유전자 스캔 -> 키 트리의 leaf node에 실제 센서 노드 주소 저장
	{
		Gene gene = solution[geneIdx];

		switch ( solGeneType[geneIdx])
		{

		case 'G':



			level = gene.first;// 물리적 그룹의 level과 index 획득
			index = gene.second; 

			kNode = getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			ndIdxInClstr = 0;
			kNode->numSensorNodes = ndIdxInClstr; // 물리적 그룹 내 센서 노드 수 리셋 (reset)

			break;

		case 'N':
			{
				// 현재 k-node는 클러스터 키에 해당
				// individual key
				// 클러스터 내 ndIdxInClstr 번째 슬롯에 센서 노드 정보 저장

				struct _tagKNode* tKNode = kNode->leftChild; // 클러스터 키 하위의 첫 번째 individual key로 이동

				for (int j = 0; j < ndIdxInClstr; j++)
				{
					tKNode = tKNode->rightSibling; // 센서 노드 정보를 저장할 슬롯으로 이동
				}

				int nodeID = gene.second; // 센서 노드 ID 획득				
				
				if ( nodeID >=0) // node ID가 존재하고 해당 노드가 CH로부터 전송 범위 이내이면 키 트리에 저장 
				{
					dist =  sqrt (pow ( (double) nodes[nodeID]->x - nodes[kNode->CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[kNode->CHID]->y, 2));	


					if ( dist < TRANS_RANGE)
					{
						struct _tagUNode* uNode = tKNode->unode; // unode 획득
						uNode->sensorNode = nodes[nodeID];
						ndIdxInClstr++;
						kNode->numSensorNodes = ndIdxInClstr; // (2014.09.03) 클러스터의 소속 노드 수를 kNode에 기록
					}
					else
					{
						isolatedNodes.push_back(nodeID); // CH로부터 전송 범위 밖에 위치한 노드를 고립 노드 목록에 추가
					}
				}				
				break;
				/*
				struct _tagKNode* tKNode = kNode->leftChild; // 클러스터 키 하위의 첫 번째 individual key로 이동

				for (int j = 0; j < ndIdxInClstr; j++)
				{
					tKNode = tKNode->rightSibling; // 센서 노드 정보를 저장할 슬롯으로 이동
				}

				int nodeID = gene.second; // 센서 노드 ID 획득
				
				if (nodeID >=0) // Node* pNode = unode->sensorNode;
				{
					struct _tagUNode* uNode = tKNode->unode; // unode 획득
					uNode->sensorNode = nodes[nodeID];
				}
				ndIdxInClstr++;

				break;
				*/
			}
		default:

			cout << "geneType is illegal: ";
			exit(-1);

			break;
		}		
	} // for 문 종료 - 염색체 스캔 완료
	
	//this->printKTree();
	
	this->printIsolatedNodes(); // 고립 노드 출력

	this->updateClstr(); // 
	//this->updateClstr(solution); // (2014.05.19) 클러스터 정보 갱신
	
	this->updateAssoc(); // (2014.05.27) 연관 노드 갱신
	
	
}

void Kmr_NEWLKH::cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp) // (2014.06.13) 계층적 그룹 갱신
{
	/* 1. GA를 활용한 해 도출 */

	Chromosome solution; // 계층적 그룹 형성을 위한 해 (solution)

	int indiLen;

	double dist; // 각 노드에서 CH까지의 거리

	indiLen = this->getNumKNodes(restrct_pt); // 개체의 길이 (개체 당 유전자 개수) 계산

	  // 노드 map 구성

	map<int, Node*> nodesInGrpMap;

	set<int>::iterator it_node;	

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // 하위 트리에 속한 노드들의 id 집합으로부터 node ID map 구성
	{
		int id;

		id = *it_node;

		nodesInGrpMap[id] = nodes[id];
	}

	/// ga 객체의 멤버 변수 설정 ( node map, 키 관리자 객체, 키 트리의 root) 

	ga->setFullSetNodes(nodes); // (2014.09.19) 전체 노드 map 설정
	ga->setNodes(nodesInGrpMap); // node map 설정
	ga->setKmr(this); // 키 관리자 객체
	ga->setRoot(restrct_pt); // 하위 키 트리의 root 노드 설정

	ga->setIndiLen(indiLen); // 개체 길이 설정
	
	cout << "\n restruct_pt- level= " << restrct_pt->level << ", index = " << restrct_pt->index << endl;

	solution = ga->findSolution(); // GA 알고리즘을 통하여 해 도출
	ga->testSolution();

	

	//this->updateAssoc(); // (2014.05.27) 연관 노드 갱신 
	//this->printKTree();

	/* 2. 도출된 해를 분석하여 key tree의 해당 leaf node에 센서 노드 정보 저장 */

	//this->isolatedNodes.clear(); // 고립된 노드 목록 리셋


	  // 도출된 해의 유전자 타입 획득

	unsigned int geneIdx = 0; // 염색체를 탐색하기 위한 인덱스

	struct _tagKNode* kNode = root;// 현재 탐색 중인 키 트리 상의 k-node

	int level = 0; // 물리적 그룹 계층 구조 상에서의 level
	int index = 0; // 물리적 그룹 계층 구조 상에서의 index

	int ndIdxInClstr = 0; // 현재 물리적 그룹에 포함된 노드의 인덱스

	  // // 해에 포함된 유전자 스캔 -> 키 트리의 leaf node에 실제 센서 노드 주소 저장
	
	map<int, char> solGeneType = ga->getSolGeneType(); // 해에 포함된 유전자의 타입 정보 획득 ( 'G': group, 'N': node)

	for (geneIdx = 0 ; geneIdx < solution.size(); geneIdx++) // 염색체에 포함된 유전자 스캔 -> 키 트리의 leaf node에 실제 센서 노드 주소 저장
	{		
		Gene gene = solution[geneIdx];

		switch ( solGeneType[geneIdx])
		{

		case 'G':

			level = gene.first;// 물리적 그룹의 level과 index 획득
			index = gene.second; 

			kNode = getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			ndIdxInClstr = 0;
			kNode->numSensorNodes = ndIdxInClstr; // 물리적 그룹 내 센서 노드 수 리셋 (reset)

			break;

		case 'N':
			{
				// 현재 k-node는 클러스터 키에 해당
				// individual key
				// 클러스터 내 ndIdxInClstr 번째 슬롯에 센서 노드 정보 저장

				struct _tagKNode* tKNode = kNode->leftChild; // 클러스터 키 하위의 첫 번째 individual key로 이동

				for (int j = 0; j < ndIdxInClstr; j++)
				{
					tKNode = tKNode->rightSibling; // 센서 노드 정보를 저장할 슬롯으로 이동
				}

				int nodeID = gene.second; // 센서 노드 ID 획득
			
				
				if ( nodeID >=0) // node ID가 존재하고 해당 노드가 CH로부터 전송 범위 이내이면 키 트리에 저장 
				{
					dist =  sqrt (pow ( (double) nodes[nodeID]->x - nodes[kNode->CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[kNode->CHID]->y, 2));	


					if ( dist < TRANS_RANGE)
					{
						struct _tagUNode* uNode = tKNode->unode; // unode 획득
						uNode->sensorNode = nodes[nodeID];
						ndIdxInClstr++;
						kNode->numSensorNodes = ndIdxInClstr; // (2014.09.03) 클러스터의 소속 노드 수를 kNode에 기록
					}
					else
					{
						isolatedNodes.push_back(nodeID); // CH로부터 전송 범위 밖에 위치한 노드를 고립 노드 목록에 추가
					}
				}				

				break;
				
			}
		default:

			cout << "geneType is illegal: ";
			exit(-1);

			break;
		}				

	} // for 문 종료 - 해에 포함된 유전자 스캔 완료.

	//this->printKTree();
	
	this->printIsolatedNodes(); // 고립 노드 출력

	//this->updateClstr(solution); // (2014.05.19) 클러스터 정보 갱신

	
}

void Kmr_NEWLKH::updateLBHG() // // (2014.06.13) 계층적 그룹 갱신 (모든 재 구조화 지점에 대해 해당 그룹 갱신)
{
	// 모든 재 구조화 지점들에 대해 계층적 그룹 갱신
	
	map<struct _tagKNode*, set<int> >::iterator it_restrct_pts;

	this->isolatedNodes.clear(); // (2014.10.11) 고립된 노드 목록 리셋


	for (it_restrct_pts = restruct_pts.begin(); it_restrct_pts != restruct_pts.end(); it_restrct_pts++) // 모든 재구조화 지점에 대해 계층적 그룹 갱신
	{
		struct _tagKNode* kNode;
		kNode = (*it_restrct_pts).first;

		set<int> nodesInGrp;
		nodesInGrp = (*it_restrct_pts).second;

		cnstrctLBHG(kNode, nodesInGrp); // 주어진 재 구조화 지점에 대해 그룹 갱신 함수 호출
	} // for문 종료 - 모든 재구조화 지점에 대해 계층적 그룹 갱신 완료

	this->updateClstr(); // (2014.10.11) 클러스터 정보 갱신

	this->updateAssoc(); // (2014.05.27) 연관 노드 갱신 // 모든 연관 노드 갱신

	
}

void Kmr_NEWLKH::updateClstr() // (2014.10.11) 고립 노드를 인접 클러스터에 추가, 각 클러스터의 무게중심 &CH 재도출, 모든 노드의 클러스터 갱신
{
	//int level; // key tree 상에서의 level
	//int index; //	"	index	

	map<int, set<int>> clusters; // 클러스터 맵

	unsigned int i = 0;
	int nodeID;

	/// (2014.09.02) 고립된 노드들을 인접 클러스터에 추가

	list<int>::iterator it_node;

	set<struct _tagKNode*>::iterator it_ckNode;

	int minNumNodes = -1;
	int curNumNodes = -1;

	struct _tagKNode* clstrWithMinNodes = NULL; // 인접 클러스터 중 가장 노드 수가 적은 클러스터의 kNode
	struct _tagKNode* tempKNode = NULL; // 현재 검사 중인 클러스터의 kNode
	struct _tagUNode* uNode = NULL; // 현재 검사 중인 UNode

	
	double distance;
	int CHID=-1;

	for (it_node = this->isolatedNodes.begin(); it_node != isolatedNodes.end(); it_node++)
	{
		nodeID = *it_node;

		if ( nodes[nodeID] == NULL) // 제거된 노드이면 SKIP
		{
			continue;
		}

		minNumNodes = -1;
		clstrWithMinNodes = NULL;

		for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // 클러스터들의 kNode들에 대해 반복
		{
			tempKNode = *it_ckNode;

			curNumNodes = tempKNode->numSensorNodes;

			if ( curNumNodes == 0 || curNumNodes >= tempKNode->outDegree ) // 클러스터의 노드 수가 0이거나 빈 슬롯이 존재하지 않으면 다음 클러스터로 진행
			//if ( curNumNodes == 0 || curNumNodes >= (INTERLEAVING_T+1) ) // 클러스터의 노드 수가 0이거나 (t+1) 이상이면 다음 클러스터로 진행
			{
					continue;
			}

			CHID = tempKNode->CHID;

			if (CHID < 0) // 해당 클러스터의 CH가 존재하지 않으면 다음 클러스터로 진행
			{
				continue;
			}

			distance =   sqrt (pow ( (double) nodes[nodeID]->x - nodes[CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[CHID]->y, 2));

			if (distance > TRANS_RANGE) // 고립 노드와 기존 CH 사이의 거리가 전송 범위보다 크면 다음 클러스터 검사
			{
				continue;
			}

			if ( minNumNodes < 0 || curNumNodes < minNumNodes) // 최소 노드 수가 갱신되는 경우 최소 노드를 가진 클러스터 갱신
			{
				minNumNodes = curNumNodes;
				clstrWithMinNodes = tempKNode;
			}

		} // 현재 노드의 인접 클러스터 중 가장 노드 수가 적고 빈 슬롯이 있는 클러스터 탐색

		if (clstrWithMinNodes == NULL) // 해당 고립 노드를 추가할 적절한 클러스터를 찾을 수 없는 경우, 다음 고립 노드로 이동
		{
			continue;
		}

		tempKNode = clstrWithMinNodes->leftChild;

		int nodeIdx =0;

		for ( nodeIdx = 0; nodeIdx < clstrWithMinNodes->outDegree; nodeIdx++)
		//for ( nodeIdx = 0; nodeIdx < minNumNodes; nodeIdx++) // 최소 노드 수를 갖는 클러스터에 대응하는 kNode의 하위 빈 슬롯으로 이동
		{
			if (tempKNode->unode->sensorNode == NULL)
			{
				break;
			}
				tempKNode = tempKNode->rightSibling;			
		}

		if (tempKNode != NULL) // 임시 - 디버깅 용 -> 추후 개선 필요
		{
			tempKNode->unode->sensorNode = nodes[nodeID]; // 빈 슬롯에 센서 노드 주소 저장
			clstrWithMinNodes->numSensorNodes++;
		}
		else
		{
			cout << " tempKNode == NULL " << endl ;
		}
	} // 고립된 노드들을 인접 클러스터 중 가장 노드 수가 적고 빈 슬롯이 있는 클러스터에 추가

	printKTree();
	// (2014.09.04) 여기까지 확인!!


	/// (2014.09.02) 각 클러스터의 무게중심 & CH 재 도출, 각 노드의 클러스터 변경


	struct _tagKNode* ckNode = NULL;
	double avg_X = 0; //				무게중심			x 좌표의 평균
	double avg_Y = 0; //				무게중심			y 좌표의 평균

	set<int> nodesInGrp;
	set<int>::iterator it_clstrNode;	
	int numNodes;
	double minDist; // 소속 노드에서 무게 중심까지의 최소 거리


	for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // 클러스터들의 kNode들에 대해 반복
	{
		ckNode = *it_ckNode;

		avg_X = 0;
		avg_Y = 0;
		numNodes = 0;
		minDist = -1;

		// 해당 클러스터에 속한 센서 노드 집합 획득
		
		nodesInGrp = this->getNdsInGrp(ckNode);		
		

		/// 각 클러스터의 무게 중심 갱신

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // 클러스터의 무게중심 갱신 (고립 노드 추가로 인한)
		{
			nodeID = *it_clstrNode;

			avg_X += nodes[nodeID]->x;
			avg_Y += nodes[nodeID]->y;

			numNodes++;
		} // 해당 클러스터의 무게 중심 구함

		avg_X /= numNodes;
		avg_Y /= numNodes;

		ckNode->COG_X = avg_X; // 갱신된 무게중심 정보를 kNode에 저장
		ckNode->COG_Y = avg_Y;

		/// 각 클러스터의 CH 갱신

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // 클러스터의 CH 갱신 (고립 노드 추가로 인한)
		{
			nodeID = *it_clstrNode;

			distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2)); // 소속 노드에서 무게 중심까지의 거리 계산

			if (minDist < 0 || distance < minDist) // CH 갱신
			{
				minDist = distance;
				CHID = nodeID; 
			} // // CH 갱신

		} // 해당 클러스터의 CH 갱신

		ckNode->CHID = CHID; // 갱신된 CH 정보를 kNode에 저장

		/// 각 클러스터에 속한 노드들의 클러스터 갱신

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // 각 노드의 클러스터 갱신
		{
			nodeID = *it_clstrNode;

			if ( nodes[nodeID] != NULL)
			{
				rtn->changeCluster(nodeID, ckNode->CHID);
			}
		} // 각 노드의 클러스터 갱신

	} // 각 클러스터의 무게 중심 갱신, ch 갱신
}

/*
void Kmr_NEWLKH::updateClstr (Chromosome solution) // (2014.07.03) GA로부터 도출된 해를 기반으로 클러스터 구조 갱신
{
	/// 주어진 해 스캔
	map<int, char> solGeneType;
	solGeneType = ga->getSolGeneType(); // 해에 포함된 유전자의 종류 ( 'G': group, 'N': node)

	int level; // key tree 상에서의 level
	int index; //	"	index

	//struct _tagKNode* clusterKey; // 클러스터 키

	map<int, set<int>> clusters; // 클러스터 맵

	unsigned int i = 0;
	int nodeID;

	/// (2014.09.02) 고립된 노드들을 인접 클러스터에 추가

	list<int>::iterator it_node;

	set<struct _tagKNode*>::iterator it_ckNode;

	int minNumNodes = -1;
	int curNumNodes = -1;

	struct _tagKNode* clstrWithMinNodes = NULL; // 인접 클러스터 중 가장 노드 수가 적은 클러스터의 kNode
	struct _tagKNode* tempKNode = NULL; // 현재 검사 중인 클러스터의 kNode
	struct _tagUNode* uNode = NULL; // 현재 검사 중인 UNode

	
	double distance;
	int CHID=-1;

	for (it_node = this->isolatedNodes.begin(); it_node != isolatedNodes.end(); it_node++)
	{
		nodeID = *it_node;

		minNumNodes = -1;
		clstrWithMinNodes = NULL;

		for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // 클러스터들의 kNode들에 대해 반복
		{
			tempKNode = *it_ckNode;

			curNumNodes = tempKNode->numSensorNodes;

			if ( curNumNodes == 0 || curNumNodes >= tempKNode->outDegree ) // 클러스터의 노드 수가 0이거나 빈 슬롯이 존재하지 않으면 다음 클러스터로 진행
			//if ( curNumNodes == 0 || curNumNodes >= (INTERLEAVING_T+1) ) // 클러스터의 노드 수가 0이거나 (t+1) 이상이면 다음 클러스터로 진행
			{
					continue;
			}

			CHID = tempKNode->CHID;

			if (CHID < 0) // 해당 클러스터의 CH가 존재하지 않으면 다음 클러스터로 진행
			{
				continue;
			}

			distance =   sqrt (pow ( (double) nodes[nodeID]->x - nodes[CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[CHID]->y, 2));

			if (distance > TRANS_RANGE) // 고립 노드와 기존 CH 사이의 거리가 전송 범위보다 크면 다음 클러스터 검사
			{
				continue;
			}

			if ( minNumNodes < 0 || curNumNodes < minNumNodes) // 최소 노드 수가 갱신되는 경우 최소 노드를 가진 클러스터 갱신
			{
				minNumNodes = curNumNodes;
				clstrWithMinNodes = tempKNode;
			}

		} // 현재 노드의 인접 클러스터 중 가장 노드 수가 적고 빈 슬롯이 있는 클러스터 탐색

		if (clstrWithMinNodes == NULL) // 해당 고립 노드를 추가할 적절한 클러스터를 찾을 수 없는 경우, 다음 고립 노드로 이동
		{
			continue;
		}

		tempKNode = clstrWithMinNodes->leftChild;

		int nodeIdx =0;

		for ( nodeIdx = 0; nodeIdx < clstrWithMinNodes->outDegree; nodeIdx++)
		//for ( nodeIdx = 0; nodeIdx < minNumNodes; nodeIdx++) // 최소 노드 수를 갖는 클러스터에 대응하는 kNode의 하위 빈 슬롯으로 이동
		{
			if (tempKNode->unode->sensorNode == NULL)
			{
				break;
			}
				tempKNode = tempKNode->rightSibling;			
		}

		if (tempKNode != NULL) // 임시 - 디버깅 용 -> 추후 개선 필요
		{
			tempKNode->unode->sensorNode = nodes[nodeID]; // 빈 슬롯에 센서 노드 주소 저장
			clstrWithMinNodes->numSensorNodes++;
		}
		else
		{
			cout << " tempKNode == NULL " << endl ;
		}
	} // 고립된 노드들을 인접 클러스터 중 가장 노드 수가 적고 빈 슬롯이 있는 클러스터에 추가

	printKTree();
	// (2014.09.04) 여기까지 확인!!


	/// (2014.09.02) 각 클러스터의 무게중심 & CH 재 도출, 각 노드의 클러스터 변경


	struct _tagKNode* ckNode = NULL;
	double avg_X = 0; //				무게중심			x 좌표의 평균
	double avg_Y = 0; //				무게중심			y 좌표의 평균

	set<int> nodesInGrp;
	set<int>::iterator it_clstrNode;	
	int numNodes;
	double minDist; // 소속 노드에서 무게 중심까지의 최소 거리


	for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // 클러스터들의 kNode들에 대해 반복
	{
		ckNode = *it_ckNode;

		avg_X = 0;
		avg_Y = 0;
		numNodes = 0;
		minDist = -1;

		// 해당 클러스터에 속한 센서 노드 집합 획득
		
		nodesInGrp = this->getNdsInGrp(ckNode);		
		

		/// 각 클러스터의 무게 중심 갱신

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // 클러스터의 무게중심 갱신 (고립 노드 추가로 인한)
		{
			nodeID = *it_clstrNode;

			avg_X += nodes[nodeID]->x;
			avg_Y += nodes[nodeID]->y;

			numNodes++;
		} // 해당 클러스터의 무게 중심 구함

		avg_X /= numNodes;
		avg_Y /= numNodes;

		ckNode->COG_X = avg_X; // 갱신된 무게중심 정보를 kNode에 저장
		ckNode->COG_Y = avg_Y;

		/// 각 클러스터의 CH 갱신

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // 클러스터의 CH 갱신 (고립 노드 추가로 인한)
		{
			nodeID = *it_clstrNode;

			distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2)); // 소속 노드에서 무게 중심까지의 거리 계산

			if (minDist < 0 || distance < minDist) // CH 갱신
			{
				minDist = distance;
				CHID = nodeID; 
			} // // CH 갱신

		} // 해당 클러스터의 CH 갱신

		ckNode->CHID = CHID; // 갱신된 CH 정보를 kNode에 저장

		/// 각 클러스터에 속한 노드들의 클러스터 갱신

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // 각 노드의 클러스터 갱신
		{
			nodeID = *it_clstrNode;

			rtn->changeCluster(nodeID, ckNode->CHID);
		} // 각 노드의 클러스터 갱신

	} // 각 클러스터의 무게 중심 갱신, ch 갱신	
}
*/

/*
void Kmr_NEWLKH::updateClstr (Chromosome solution, list<int> isolatedNodes)
{

}
*/
void Kmr_NEWLKH::updateAssoc( ) // (2014.05.27) 갱신된 클러스터 구조를 기반으로 연관 노드 정보 갱신
{
	/* 갱신된 클러스터 정보 획득 */

	map<int, set<int>> clusters = rtn->getClusters(); // 

	/* 각각의 클러스터와 나머지 클러스터 간 거리 (홉 수) 계산 */

	map<int, set<int>>::iterator it_clstr1;
	map<int, set<int>>::iterator it_clstr2;

	std::map<int,  std::map <int, std::set<int>> > temp_upperNodes;  // 새로운 상위 노드 목록
	std::map<int,  std::map <int, std::set<int>> > temp_lowerNodes;  // 새로운 하위 노드 목록

	int hop;

	for ( hop = 1; hop <= INTERLEAVING_T+1; hop++) // CH들의 (1~ (t+1)-hop) 상위/하위 연관 노드 목록 설정
	//for ( hop = 1; hop <= INTERLEAVING_T; hop++) // CH들의 (1~ (t+1)-hop) 상위/하위 연관 노드 목록 설정
	{
		for (it_clstr1 = clusters.begin(); it_clstr1 != clusters.end(); it_clstr1++) // 각 CH의 h hop 거리의 상위/하위 연관 노드 목록 계산
		{
			set<int> clstr1 = (*it_clstr1).second;		

			if (clstr1.empty() ) // 클러스터가 비어 있는 경우 skip
			{
				continue;
			}


			int CH_ID1 = (*it_clstr1).first; // CH 구하기

			if (nodes[CH_ID1] == NULL) // (2014.09.21) 해당 노드가 이미 제거된 노드이면 다음 loop로 이동
			{
				continue;
			}

			if (hop == 1) // 1 hop 상위/하위 연관 노드 계산
			{
				for (it_clstr2 = (it_clstr1); it_clstr2 != clusters.end(); it_clstr2++)
				{
					if (it_clstr2 == it_clstr1) // 클러스터가 비어 있는 경우 skip
					{
						continue;
					}				

					set<int> clstr2 = (*it_clstr2).second;		

					if (clstr2.empty() ) // 클러스터가 비어 있는 경우 skip
					{
						continue;
					}

					int CH_ID2 = (*it_clstr2).first; // CH 구하기 


					if (nodes[CH_ID2] == NULL) // (2014.09.21)  해당 노드가 이미 제거된 노드이면 다음 loop로 이동
					{
						continue;
					}

					// 두 CH 간의 거리 구하기

					double dist = sqrt ( pow((double) nodes[CH_ID1]->x - nodes[CH_ID2]->x, 2) + pow((double) nodes[CH_ID1]->y - nodes[CH_ID2]->y, 2) );

					if (dist < TRANS_RANGE) // 두 노드 간 거리가 전송 범위 이내일 경우 상위/하위 연관 노드 목록에 추가
					{
						int upperNodeID;
						int lowerNodeID; 

						if (nodes[CH_ID1]->distance < nodes[CH_ID2]->distance)
						{
							upperNodeID = CH_ID1;
							lowerNodeID = CH_ID2;
						}
						else
						{
							upperNodeID = CH_ID2;
							lowerNodeID = CH_ID1;
						}

						// 상하위 노드 정보 추가
						temp_upperNodes[lowerNodeID][1].insert(upperNodeID);
						temp_lowerNodes[upperNodeID][1].insert(lowerNodeID);

						///** 현재 CH의 H 홉 상위 CH를 위한 임시 하위 노드 목록에 클러스터 소속 비 CH 노드들 추가

						set<int> cluster;
						set<int>::iterator it_memberNode;
						int memberNodeID;

						cluster = rtn->getClstr(lowerNodeID); // 클러스터 소속 노드 목록 획득

						int node_idx;
						node_idx = 1;

						for( it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // 클러스터 소속 노드 목록 순회
						{
							memberNodeID = *it_memberNode; // 클러스터 소속 ID;

							if (memberNodeID == lowerNodeID) // 소속 노드 ID가 ch의 ID와 동일할 경우, 다음 소속 노드로 진행
							{
								continue;
							}

							if ( node_idx == hop) // CH의 h-hop 상위 노드를 h 번째 소속 노드의 상위 연관 노드로 저장
							{
									
								temp_lowerNodes[upperNodeID][INTERLEAVING_T+1].insert(memberNodeID);					

								// CH 의 h-hop 상위 노드 목록 순회 - h-hop 상위 노드의 하위 연관 노드 목록 갱신 완료
								break;
							}

							node_idx++;
						} // CH의 h-번째 상위 노드를 h번째 소속 노드의 상위 연관 노드로 설정

					///** 현재 CH의 H 홉 상위 CH를 위한 임시 하위 노드 목록에 클러스터 소속 비 CH 노드들 추가 (완료)

					} // 1 hop 상/하위 연관 노드 추가		

					


				} // for 문 종료 - 현재 CH의 1 hop 상위/하위 CH 목록 계산 완료
			} // hop ==1인 경우 처리 완료

			else
			{
				std::set<int>::iterator it_upperNode; // 상위 노드 반복자

				std::set<int>::iterator it_upperupperNode; // 상위 노드의 상위 노드 반복자

				int CH_ID2;

				for (it_upperNode = temp_upperNodes[CH_ID1][hop-1].begin(); it_upperNode != temp_upperNodes[CH_ID1][hop-1].end(); it_upperNode++) // 현재 노드의 h-1 홉 상위 노드 목록 순회
				{
					CH_ID2 = *it_upperNode;

					for (it_upperupperNode = temp_upperNodes[CH_ID2][1].begin(); it_upperupperNode != temp_upperNodes[CH_ID2][1].end(); it_upperupperNode++) // (h-1) 홉 상위 노드의 1-hop 상위 노드 순회
					{
						int upperNodeID;

						upperNodeID = *it_upperupperNode;

						temp_upperNodes[CH_ID1][hop].insert(upperNodeID); // (h-1) 홉 상위 노드의 1-hop 상위 노드 ID를 h-hop 상위 노드 목록에 추가


						///** 현재 CH의 H 홉 상위 CH를 위한 임시 하위 노드 목록에 클러스터 소속 비 CH 노드들 추가
						
						set<int> cluster;
						set<int>::iterator it_memberNode;
						int memberNodeID;

						cluster = rtn->getClstr(CH_ID1); // 클러스터 소속 노드 목록 획득

						int node_idx;
						node_idx = 1;

						for( it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // 클러스터 소속 노드 목록 순회
						{
							memberNodeID = *it_memberNode; // 클러스터 소속 ID;

							if (memberNodeID == CH_ID1) // 소속 노드 ID가 ch의 ID와 동일할 경우, 다음 소속 노드로 진행
							{
								continue;
							}

							if ( node_idx == hop) // CH의 h-hop 상위 노드를 h 번째 소속 노드의 상위 연관 노드로 저장
							{
									
								temp_lowerNodes[upperNodeID][INTERLEAVING_T+1].insert(memberNodeID);					

								// CH 의 h-hop 상위 노드 목록 순회 - h-hop 상위 노드의 하위 연관 노드 목록 갱신 완료
								break;
							}

							node_idx++;
						} // CH의 h-번째 상위 노드를 h번째 소속 노드의 상위 연관 노드로 설정
						
						///** 현재 CH의 H 홉 상위 CH를 위한 임시 하위 노드 목록에 클러스터 소속 비 CH 노드들 추가 (완료)


					} // (h-1) 홉 상위 노드의 1-hop 상위 노드 반복 완료


				} // for문 종료 -  현재 노드의 h 홉 상위 노드 목록 구성 완료

				


				std::set<int>::iterator it_lowerNode;// 하위 노드 반복자

				std::set<int>::iterator it_lowerlowerNode; // 하위 노드의 하위 노드 반복자

				for (it_lowerNode = temp_lowerNodes[CH_ID1][hop-1].begin(); it_lowerNode != temp_lowerNodes[CH_ID1][hop-1].end(); it_lowerNode++) // 현재 노드의 h-1 홉 하위 노드 목록 순회
				{
					CH_ID2 = *it_lowerNode;

					for (it_lowerlowerNode = temp_lowerNodes[CH_ID2][1].begin(); it_lowerlowerNode != temp_lowerNodes[CH_ID2][1].end(); it_lowerlowerNode++) // (h-1) 홉 하위 노드의 1-hop 하위 노드 순회
					{
						int lowerNodeID;

						lowerNodeID = *it_lowerlowerNode;

						


						temp_lowerNodes[CH_ID1][hop].insert(lowerNodeID); // (h-1) 홉 하위 노드의 1-hop 하위 노드 ID를 h-hop 하위 노드 목록에 추가
					} // (h-1) 홉 하위 노드의 1-hop 하위 노드 반복 완료
				} // for문 종료 -  현재 노드의 h 홉 하위 노드 목록 구성 완료
			} // else문 종료 - hop = (2~ (T+1))인 경우 처리 완료

			// (2014.06.23) 15:49 여기까지 확인! 

			//sec->updateAssocNodes(CH_ID1, 0, hop, temp_upperNodes[CH_ID1][hop]); // 연관 노드 갱신
			//sec->updateAssocNodes(CH_ID1, 1, hop, temp_lowerNodes[CH_ID1][hop]);
		}
	} // for 문 종료 - CH들의 (1~ (t+1)-hop) 임시 상위/하위 노드 목록 저장 완료

	
	/// 각 CH의 상위 연관 노드 갱신

	set<struct _tagKNode*>::iterator it_cKNode;


	for (it_cKNode = cKNodes.begin(); it_cKNode != cKNodes.end(); it_cKNode++) // 키 트리에 저장된 (클러스터 키) 노드 순회
	{
		int CH_ID;

		CH_ID = (*it_cKNode)->CHID; //  CH ID 획득
		
		for ( hop = 1; hop <= INTERLEAVING_T+1; hop++) // 1~(t+1) 홉 상위 연관 노드 갱신
		{
			sec->updateAssocNodes(CH_ID, 0, hop, temp_upperNodes[CH_ID][hop]);

		} // for문 종료 - 현재 CH의 상위 연관 노드 목록 갱신 완료

	} // for 문 종료 - 각 CH의 상위 연관 노드 목록 갱신 완료

	/// 각 CH의 하위 연관 노드 갱신
	// (2014.06.26) 20:11 여기까지 확인
	for (it_cKNode = cKNodes.begin(); it_cKNode != cKNodes.end(); it_cKNode++)
	{
		int CH_ID;

		CH_ID = (*it_cKNode)->CHID; //  CH ID 획득
		
		if (CH_ID == 13)
		{
			cout << " (lowerNodeID == 13) " ;
		}

		for ( hop = 1; hop <= INTERLEAVING_T+1; hop++) // 1~(t+1) 홉 하위 연관 노드 갱신
		{
			sec->updateAssocNodes(CH_ID, 1, hop, temp_lowerNodes[CH_ID][hop]);

		} // for문 종료 - 현재 CH의 하위 연관 노드 목록 갱신 완료
	} // for 문 종료 - 각 CH의 하위 연관 노드 목록 갱신 완료
	
}



void Kmr_NEWLKH::expandLevel(int level, int degree) // (2014.04.24) key tree의 현재 level 확장
{
	// 구현 중 

	list<struct _tagKNode*>::iterator it;

	list<struct _tagKNode*> next_level_knodes; // 다음 level 노드 목록

	int index = 0; // 동일 level에 속한 kNOde들의 index

	for ( it = cur_level_knodes.begin(); it != cur_level_knodes.end(); it=cur_level_knodes.erase(it) ) // 현재 level 노드들에 대해 반복
	{
		struct _tagKNode* parent = *it;		

		KNode* prev = NULL;
		KNode* current = NULL;			

		parent->outDegree = degree; // (2014.09.04) 

		for (int i = 0; i < degree; i++)
		{
			KNode* kNode = new KNode; // k-node 생성
			this->initKNode (kNode);
			kNode->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

			next_level_knodes.push_back(kNode);

			//UNode* uNode = new UNode; // u-node 생성	  

			current = kNode; // 현재 k-node

			current->level = level; // kNode의 level 설정
			current->index = index++; // kNode의 인덱스 설정

			//kNode->unode = uNode; // k-node, u-node 간 포인터 설정
			//uNode->parent = kNode;

			if (prev != NULL)// left/right sibling 설정
			{
			  prev->rightSibling = current;
			  current->leftSibling = prev;
			}

			kNode->parent = parent; // k-node, root 간 포인터 설정

		    if (i ==0)
			{
			  parent->leftChild = current; // left child 설정
			}

			prev = current; // 이전 k-node 설정
		}
	}

	this->cur_level_knodes.clear(); // 오래된 현재 level 노드 목록 제거
	this->cur_level_knodes = next_level_knodes; // 현재 level 노드 목록을 다음 level로 갱신
}

void Kmr_NEWLKH::expandLevel(int degree) // (2014.04.18) key tree의 현재 level 확장 (현재 level의 각 노드마다 degree 만큼의 자식 노드 생성)
{
	// 구현 중 

	list<struct _tagKNode*>::iterator it;

	list<struct _tagKNode*> next_level_knodes; // 다음 level 노드 목록

	int index = 0; // 동일 level에 속한 kNOde들의 index

	for ( it = cur_level_knodes.begin(); it != cur_level_knodes.end(); it=cur_level_knodes.erase(it) ) // 현재 level 노드들에 대해 반복
	{
		struct _tagKNode* parent = *it;		

		KNode* prev = NULL;
		KNode* current = NULL;			

		parent->outDegree = degree; 

		for (int i = 0; i < degree; i++)
		{
			KNode* kNode = new KNode; // k-node 생성			

			this->initKNode (kNode);

			kNode->groupKey->newVal =gen1.genRandInt(Max_Key_Value);

			next_level_knodes.push_back(kNode);

			//UNode* uNode = new UNode; // u-node 생성	  

			current = kNode; // 현재 k-node

			current->index = index++; // kNode의 인덱스 설정

			//kNode->unode = uNode; // k-node, u-node 간 포인터 설정
			//uNode->parent = kNode;

			if (prev != NULL)// left/right sibling 설정
			{
			  prev->rightSibling = current;
			  current->leftSibling = prev;
			}

			kNode->parent = parent; // k-node, root 간 포인터 설정

		    if (i ==0)
			{
			  parent->leftChild = current; // left child 설정
			}

			prev = current; // 이전 k-node 설정
		}
	}

	this->cur_level_knodes.clear(); // 오래된 현재 level 노드 목록 제거
	this->cur_level_knodes = next_level_knodes; // 현재 level 노드 목록을 다음 level로 갱신

}

void Kmr_NEWLKH::printIsolatedNodes() // (2014.09.02) 고립된 노드 목록 출력
{
	ofstream out("tst_isolatedNodes.txt");

	out << "isolated nodes: " << endl << endl;

	std::list<int>::iterator it;

	for (it = this->isolatedNodes.begin(); it != this->isolatedNodes.end(); it++)
	{
		int nodeID = *it;
		out << nodeID << ", ";
	}

	out.close();
}

Kmr_NEWLKH::Kmr_NEWLKH()
{

}

Kmr_NEWLKH::~Kmr_NEWLKH()
{

}