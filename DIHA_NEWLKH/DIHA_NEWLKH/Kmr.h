

// 2014.07.31
// 문수영
// Kmr.h
// 키 매니저 클래스 선언

#ifndef _KMR_H
#define _KMR_H

#include "Node.h"
#include "params.h"
#include "typedef.h"
#include "Rtn.h"
#include "Sec.h"
#include "RAND.h"
#include <iostream>

#include "AStar.h"
#include "GA.h"

class Rtn;
class Sec;

class AStar;
class GA;

class Kmr // key manager
{
public:

	// 멤버 변수 값 설정
	void setNodes(map<int, Node*> nodes); // 노드 리스트 초기화 함수
	void setRtn (Rtn* rtn); // rtn 객체 주소 저장
	void setSec (Sec* sec); // sec 객체 주소 저장	

	// 그룹 키 재분배 관련 함수  (2014.05.08)		

	//virtual void updateKey()=0; // 키 트리 상의 그룹 키 갱신
	virtual void updateKey(struct _tagKNode* kNode)=0; // 키 트리 상의 그룹 키 갱신
	void resetUpdatedKeys(); // (2014.07.31) key tree의 각 k-node 갱신 여부 (생성 여부 포함) 재 초기화 
	void resetUpdatedKeys(struct _tagKNode* kNode); // (2014.07.31) key tree의 각 k-node 갱신 여부 재 초기화 - 재귀 호출
	void printReceivedNodes(); // (2014.08.07) 키 재분배 메시지 수신 노드 집합 출력
	void resetReceivedNodes(); // (2014.08.07) 키 재분배 메시지 수신 노드 집합 리셋

	virtual void genKeyMsg()=0; // 키 재분배 메시지 생성
	//void testKeyMsg(); // 키 재분배 메시지 테스트

	bool isSubset (set<int> a, set<int> b); // (2014.07.10) IDSet a가 IDSet b의 부분집합이면 true, 그렇지 않으면 false 반환

	virtual bool handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode) = 0; // 키 재분배 메시지 처리 함수 (계속 전달 시: true 반환, 전달 완료 시: false 반환)
	virtual void testKeyMsg()=0; // 키 재분배 메시지 테스트
	virtual int getKeyMsgLen(struct _tagKeyMsg* keyMsg) = 0; // 키 재분배 메시지 길이 반환
	
	void initKeyMsg( struct _tagKeyMsg* kMsg); // 키 재분배 메시지 초기화
	map<int, struct _tagKeyMsg*> getKeyMsgs( ); // 키 재분배 메시지 획득 
	void delKeyMsg(); // 키 재분배 메시지 제거
	void delRecvdKMsg(Node* cNode); // (2014.05.15)수신된 키 재분배 메시지 제거	
	void storeKMsg(struct _tagKeyMsg* keyMsg, Node* cNode); // (2014.05.15) 수신된 키 재분배 메시지 저장
	void restoreKMsg(struct _tagKeyMsg* keyMsg, Node* cNode); // (2014.05.15) 수신된 키 재분배 메시지 복원		
	
	list<struct _tagGroupKey> getKeySet(list<Node*> nodes); // 주어진 노드(들)의 그룹 키 목록 반환
	list<Node*> getUserSet(list<struct _tagGroupKey> keys); // 주어진 키(들)의 노드 목록 반환
	set<int> getUserSet( struct _tagKNode* kNode); // (2014.07.08) 주어진 kNode의 키를 소유한 노드 목록 반환
	
	// key tree 관리

	virtual int cnstrctKTree() = 0; // key tree 구축 - 키 트리의 높이 (h) 반환
	virtual int cnstrctKTree(struct _tagKNode* root, set<int> nodes) = 0; // (2014.06.08)  key tree 재구축 - 키 트리의 높이 반환 
	virtual int updateKTree(set<int> addedNodes, set<int> deletedNodes) = 0; // (2014.05.08) key tree 갱신 - 노드의 추가/제거 반영  - 키 트리의 높이 (h) 반환
	virtual void dstrctKTree() = 0; // key tree 제거
	
	int renumberKNodes(); // (2014.06.12) 키 트리 내의 모든 knode들의 level, index 재설정 - 출력: key tree의 높이 (height)
	bool renumberKNodes(int level); // (2014.06.12) 특정 level에 속한 kNode들의 level과 index 재설정 - 출력: 다음 level의 노드가 존재하는 지 여부		

	set<int> getNdsInGrp(struct _tagKNode* root); // (2014.06.08) 주어진 그룹에 포함된 센서 노드 ID 집합 반환
	int getNumNdsInGrp(struct _tagKNode* root); // (2014.05.30) 주어진 그룹에 포함된 센서 노드 수 반환
	int getNumClstrsInGrp(struct _tagKNode* root); // (2014.05.30) 주어진 그룹에 포함된 클러스터 수 반환

	void initKNode (struct _tagKNode* kNode); // (2014.04.24) k-node 초기화
	void initUNode (struct _tagUNode* uNode); // (2014.06.02) u-node 초기화
	
	void printKTree(); // (2014.04.22) key tree 출력
	void printKTree(struct _tagKNode* kNode); // (2014.04.22) key tree 출력
	void printKTree(ofstream& out, struct _tagKNode* kNode); // (2014.08.31) 파일에 key tree 출력

	void addKTreeNode( struct _tagKNode* joinPoint, struct _tagKNode* newNode);  // key tree node 추가
	void deleteKTreeNode( struct _tagKNode* leavePoint, struct _tagKNode* delNode);  // key tree node 제거

	int getKTreeHeight(); // key tree height 반환
	int getNumNodesAtLevel (int level); // 특정 level의 노드 수 반환

	bool isCluster (struct _tagKNode* kNode); // 주어진 k-node가 클러스터에 대응되는 지 확인 - True: 클러스터, False: 클러스터 아님


	// 계층적 그룹 형성 관련

	virtual void cnstrctLBHG()=0; // 초기 계층적 그룹 형성
	virtual void cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp) = 0; // (2014.06.13) 주어진 재 구조화 지점에 대해 해당 그룹 갱신
	virtual void updateLBHG() = 0; // // (2014.06.13) 계층적 그룹 갱신 (모든 재 구조화 지점에 대해 해당 그룹 갱신)
	set<struct _tagKNode*> getKNodesAtLevel(int level);// (2014.06.17) 특정 level에 속한 kNOde 목록 반환
	struct _tagKNode* getKNode( int level, int index); // (2014.04.28) key tree의 주어진 level과 index에 대응되는 knode 반환
	struct _tagUNode* getUNode (struct _tagKNode* clstrKey, int snID); // (2014.05.19) 클러스터에 대응되는 kNode와 센서 노드 ID로부터 해당 uNode 주소 반환	
	int getNumKNodes(struct _tagKNode* root); // (2014.06.13) (하위) 키 트리에 속한 k-node의 개수 반환 

	// 필터링 키 관리
	void addPairKey (int nID, int nID2); // (2013.08.29) 두 노드의 iD를 입력으로 pairwise key를 추가 // pair key 추가	
	struct _tagPairKey* getPairKey (int nID1, int nID2);  // (2013.08.19) 두 노드가 공유하는 pairwise key 계산 및 반환 // pair key 반환

	// pair key test
	void testPairKey(); // (2014.08.19) 연관 노드 설정으로 인한 pair key 정보를 파일에 출력

	set<struct _tagPairKey*> getPKSet (int nID); // (2014.08.19) 주어진 노드의 pair key set 반환

	// 이벤트 보고서 전달 관련

	int getNumPairKeys(Node* candi); // 후보 노드의 pairwirse key 개수 반환
 
	Kmr();
	~Kmr();

protected:
		
	map<int, Node*> nodes; // 노드 목록 ( {node ID, node pointer) }
	map<int, Node*> CHs; // CH 목록  ( {cluster ID, CH pointer) }	
	Rtn* rtn; // 라우팅 객체
	Sec* sec; // 보안 객체

	// 키 관리 관련
	map<int, set<struct _tagPairKey*> > pKeys; // (2014.05.23) <node ID, {pair key*} > 노드의 pairwise key 객체 주소

	// 키 재분배 관련
	map<int, struct _tagKeyMsg*> keyMsgs; // 키 


	set <pair <struct _tagGroupKey*, set<int>> > keyRecipentSet; // <갱신 키 - {수신 노드 ID}>  집합

	set<int> receivedNodes; // (2014.08.07)  {키 재분배 메시지 수신 노드 ID}

	// 키 트리 관련
	struct _tagKNode* root;// 키 트리의 root
	int kTreeHeight; // 키 트리의 높이
	map<int, int> numNodesAtLevel; // 각 level의 노드 수 정보 (key tree 형성 시 설정)
	
	set<struct _tagKNode*> cKNodes; // (2014.06.10) 클러스터에 대응되는 k-node들의 집합
	map<struct _tagKNode*, set<int> > restruct_pts; // (2014.06.08)  <재구조화 지점, 소속 노드들의 id 집합> - 키 트리 재구조화 지점과 해당 그룹 내의 센서 노드 ID 집합
	list<struct _tagKNode*> cur_level_knodes; // (2014.04.18) 구축 중인 key tree의 현재 level에 존재하는 knode들의 목록

	

	// 알고리즘
	AStar* astar;
	GA* ga;	
	RAND gen1; // // 키 갱신을 위한 난수 생성기	
};

#endif