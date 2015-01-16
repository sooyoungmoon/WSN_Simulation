

// 2014.06.13
// 문수영
// Kmr_IHA.h
// IHA의 키 매니저 클래스 선언

#ifndef _KMR_IHA_H
#define _KMR_IHA_H

#include "Kmr.h"

class Kmr_IHA : public Kmr // key manager
{
public:
	
	// 그룹 키 재분배 관련 함수
	void updateKey(struct _tagKNode* kNode); // 그룹 키 갱신 함수
	//void resetUpdatedKeys(struct _tagKNode* kNode); // (2014.07.31) key tree의 각 k-node 갱신 여부 재 초기화 

	void genKeyMsg(); // 키 재분배 메시지 생성
	void testKeyMsg(); // 키 재분배 메시지 테스트
	int getKeyMsgLen(struct _tagKeyMsg* keyMsg); // 키 메시지 길이 반환
	bool handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode);

	//void genKeyMsg();  // 키 재분배 메시지 생성	
	//KeyMsg* getKeyMsg(); // 키 재분배 메시지 반환

	// 필터링 키 관리
	//void addPairKey (int nID, int nID2);
	// (2013.08.29) 두 노드의 iD를 입력으로 pairwise key를 추가 // pair key 추가

	// key tree 관리
	int cnstrctKTree(); // key tree 구축
	int cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes); // (2014.06.05) key tree (재)구축 - 키 트리의 높이 반환
	int cnstrctKTree(struct _tagKNode* root, set<int> nodes); // (2014.06.08) key tree (재)구축 - 키 트리의 높이 반환

	int updateKTree(IDSet addedNodes, IDSet deletedNodes); // (2014.05.29) 키 트리 갱신 함수 - 입력: 추가 노드 목록 (addedNodes), 제거 노드 목록 (deletedNodes)
	void dstrctKTree(); //  key tree 제거

	// 노드들의 물리적 그룹 관리
	void cnstrctLBHG(); // 계층적 그룹 형성
	void cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp); //  // (2014.06.13) 계층적 그룹 갱신
	void updateLBHG(); // // (2014.06.13) 계층적 그룹 갱신 (모든 재 구조화 지점에 대해 해당 그룹 갱신)

	Kmr_IHA();
	~Kmr_IHA();

};
/*
	void addPairKey (int nID, int nID2);// (2013.08.29) 두 노드의 iD를 입력으로 pairwise key를 추가
	void setPairKeys(); // (2013.08.29) pairwise key 설정 
	//int getNumPairwiseKeys(Node* candi) const const; // 후보 노드가 가진 pairwise key 개수 반환
	//PairKey* getPairKey (int nID1, int nID2); // (2013.08.19) 두 노드가 공유하는 pairwise key 계산 및 반환

	// test 함수
	void testKeyInfo(); // 키 정보 출력

	Kmr_IHA(void);
	~Kmr_IHA(void);
};
*/


#endif