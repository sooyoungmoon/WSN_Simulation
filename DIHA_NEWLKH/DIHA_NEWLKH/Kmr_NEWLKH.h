// 2014.07.31
// 문수영
// Kmr.h
// NEW_LKH 기반 키 관리 클래스 선언

#ifndef _KMR_Kmr_NEWLKH_H
#define _KMR_Kmr_NEWLKH_H

#include "Kmr.h"
#include "Sec_IHA.h"

class Kmr_NEWLKH : public Kmr// key manager
{
public:

	// 그룹 키 재분배 관련 함수
	void updateKey(struct _tagKNode* kNode); // 그룹 키 갱신 함수
	//void resetUpdatedKeys(struct _tagKNode* kNode); // (2014.07.31) key tree의 각 k-node 갱신 여부 재 초기화 

	void genKeyMsg(); // 키 재분배 메시지 생성
	void testKeyMsg(); // 키 재분배 메시지 테스트
	int getKeyMsgLen(struct _tagKeyMsg* keyMsg); // 키 메시지 길이 반환
	bool handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode); // 키 메시지 처리 함수

	//void genKeyMsg();  // 키 재분배 메시지 생성	
	//KeyMsg* getKeyMsg(); // 키 재분배 메시지 반환

	// 필터링 키 관리
	//void addPairKey (int nID, int nID2);
	// (2013.08.29) 두 노드의 iD를 입력으로 pairwise key를 추가 // pair key 추가

	// key tree 관리
	int cnstrctKTree(); // key tree 구축
	int cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes); // (2014.06.05) key tree (재)구축 - 키 트리의 높이 반환
	int cnstrctKTree(struct _tagKNode* root, set<int> nodes); // (2014.06.08) key tree (재)구축 - 키 트리의 높이 반환


public:
	int updateKTree(IDSet addedNodes, IDSet deletedNodes); // (2014.05.29) 키 트리 갱신 함수 - 입력: 추가 노드 목록 (addedNodes), 제거 노드 목록 (deletedNodes)
public:
	void dstrctKTree(); //  key tree 제거
	void dstrctKTree(struct _tagKNode*); // key tree 제거 (재귀 함수)

	// 노드들의 물리적 그룹 관리
	void cnstrctLBHG(); // 계층적 그룹 형성
	void cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp);  // (2014.06.13) 계층적 그룹 갱신
	void updateLBHG(); // // (2014.06.13) 계층적 그룹 갱신 (모든 재 구조화 지점에 대해 해당 그룹 갱신)

	void updateClstr(); // (2014.10.11) 고립 노드를 인접 클러스터에 추가, 각 클러스터의 무게중심 & CH 재도출, 노드들의 클러스터 갱신

	//void updateClstr (Chromosome solution); // (2014.05.19) GA로부터 도출된 해를 기반으로 클러스터 구조 갱신

	//void updateClstr (Chromosome solution, list<int> isolatedNodes); // (2014.09.03) GA로부터 도출된 해를 기반으로 클러스터 구조 갱신, 고립 노드를 인접 클러스터에 추가

	void updateAssoc ( ); // (2014.05.27) 갱신된 클러스터 구조를 기반으로 연관 노드 정보 갱신

	void expandLevel(int level, int degree); // (2014.04.24) key tree의 현재 level 확장
	void expandLevel(int degree); // (2014.04.18) key tree의 현재 level 확장 (현재 level의 각 노드마다 degree 만큼의 자식 노드 생성)

	void printIsolatedNodes(); // (2014.09.02) 고립된 노드 목록 출력 {node ID}

	Kmr_NEWLKH(void);
	~Kmr_NEWLKH(void);

private:

	//list<struct _tagKNode*> cur_level_knodes; // (2014.04.18) 구축 중인 key tree의 현재 level에 존재하는 knode들의 목록
	list<int> isolatedNodes; // (2014.09.02) GA 알고리즘 수행 결과 클러스터에 속하지 못한 노드들
};

#endif