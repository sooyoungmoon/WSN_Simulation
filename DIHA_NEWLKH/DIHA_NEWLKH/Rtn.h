// 2014.07.14
// Rtn.h
// routing 클래스 선언

#ifndef _RTN_H
#define _RTN_H

#include "Node.h"
#include "BS.h"

#include "Atk.h"
#include "Sec.h"
#include "Kmr.h"
#include "Fuzzy.h"
#include "params.h"
#include "typedef.h"
#include <stdio.h>
#include <iostream>
#include <map>

using std::list;
using std::map;

class Sec;
class BS;

class Rtn
{
public:
	// 클러스터 구성 관련
	void setNodes(map<int, Node*> nodes); // 노드 리스트 초기화 함수
	void setBS( BS* bs); // 기지 노드 객체 주소 설정
	virtual void findNbrs(); // 이웃 노드 탐색 함수
	virtual void setCHs() = 0; // CH 선택 함수
	Node* getCH (int clstrID); // CH 획득 함수
	set<int> getClstr (int clstrID); // (2014.05.21) 입력: 클러스터 ID (ch's ID), 출력: 클러스터 소속 노드들의 iD 집합
	map<int, set<int>> getClusters(); // 클러스터 정보 반환
	int getClstrID (int nodeID); // (2014.04.10) 입력: 노드 id, 출력: 클러스터 ID (= CH's ID)
	void changeCluster (int nodeID, int clstrID); // 클러스터 변경 함수
	void changeCluster (int nodeID, int clstrID, int CH_X, int CH_Y); // 클러스터 변경 함수 - CH_X: 신규 CH의 X 좌표, CH_Y: 신규 CH의 Y 좌표
	void removeFromCluster (int nodeID); // (2014.07.14) 클러스터에서 해당 노드 제거
	void removeCH (int CHID); //(2014.09.19) 제거된 노드가 CH일 경우 해당 클러스터 정보 제거

	bool hasUpdatedCH (int nodeID);// (2014.05.19) 주어진 nodeID에 대응되는 노드의 클러스터 (CH' ID)가 변경되었는지 검사

	void arrangeClusters(); // (2014.08.11) 클러스터 목록 중 크기가 0인 클러스터 정보를 제거 

	// 다음 노드 선택 관련
	//void addParent (Node* cNode, Node* prevNode); // (2013.10.11 추가) (일반) 부모 노드 추가 (한 노드가 여러 개의 부모 노드를 가질 수 있음)	
	
	
	///*** 부모 (일반/CH) 노드 추가
	void addParentNode (Node* cNode, Node* prevNode); // (2014.08.17 추가) (일반) 부모 노드 추가 (한 노드가 여러 개의 부모 노드를 가질 수 있음)	
	void addParentCH (Node* cNode, Node* parentCH); // (2014.08.14) 부모 CH 노드 추가
	///***
	
	//std::list<Node*> getParents(Node* cNode); // // (2013.10.11 추가) 부모 노드 리스트 반환
	
	///** 최단경로 부모 (일반/CH) 노드 목록 획득
	std::list<Node*> getParentNodes(Node* cNode); // // (2014.08.17 추가) 부모 노드 리스트 반환
	std::list<Node*> getParentCHs(Node* cNode); // // (2014.08.14 추가) 부모 CH 노드 리스트 반환
	///**


	Node* getSPParent (Node* cNode); // (2013.09.09) 부모 노드 반환
	
	///*** 최단 경로 부모 (일반/CH) 노드 목록 획득
	Node* getSPParentNode (Node* cNode); // (2014.08.17 추가) 부모 노드 반환
	Node* getSPParentCH (Node* cNode); // (2014.08.17 추가) 부모 CH 노드 반환
	///***


	void setKmr( Kmr* kmr); // (2012.11.10 추가) kmr 객체 멤버변수 설정	
	void setFuzzy (Fuzzy* fuzzy); // fuzzy 객체 멤버변수 설정

	virtual list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs) = 0; // (2014.05.20) 제어 메시지 전달 경로 상의 다음 노드 선택
	virtual list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode) = 0; // (2013.08.06) 제어 메시지 전달 경로 상의 다음 노드 선택
	virtual Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode) = 0; // 데이터 메시지 전달 경로 상의 다음 노드 선택
	virtual list<Node*> selNext (struct _tagTREQ* treq, Node* cNode) = 0; // (2014.05.10) TREQ 메시지 다음 전달 노드 결정 함수
	virtual list<Node*> selNext (struct _tagTRES* tres, Node* cNode) = 0; // (2014.05.10) TRES 메시지 다음 전달 노드 결정 함수	
	
	virtual double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) = 0; // (2013.08.06) 제어 메시지 전달을 위한 후보 노드 평가
	virtual double eval (struct _tagEvntMsg* evntMsg, Node* candi) = 0; // 데이터 메시지 전달을 위한 후보 노드 평가	
	virtual double eval(Node* recipent, Node* candi) = 0; // 키 재분배 메시지 전달을 위한 후보 노드 평가

	// 키 재분배 관련
	virtual list<Node*> selNext (struct _tagKeyMsg* keyMsg, BS* bs) = 0;
	virtual list<Node*> selNext (struct _tagKeyMsg* keyMsg, Node* cNode) = 0;
	void setRecipent (struct _tagKeyMsg* keyMsg, Node* next); // (2014.05.12) 키 재분배 메시지의 목적 노드 설정
	void resetUpdatdClstr(); // (2014.07.15) 노드들의 클러스터 업데이트 여부를 의미하는 플래그를 false로 리셋

	// topology 관련
	void resetTopology(); // topology 재 초기화

	// TREQ, TRES 메시지 처리 관련
	
	void genTREQMsg (int round); // bs에서 TREQ 메시지 생성
	struct _tagTREQ* getTREQMsg(); // TREQ 메시지 획득	
	void updatTREQMsg (Node* cNode); // 중간 노드에서 TREQ 메시지 필드 업데이트
	void genTRESMsg (Node* cNode);
	struct _tagTRES* getTRESMsg(); // TRES 메시지 획득	
	void handleTREQ(struct _tagTREQ* treq, Node* cNode); // 수신된 TREQ 메시지 처리
	int getTRESMsgLen(struct _tagTRES* tres); // TRES 메시지 길이 반환 

	void resetNxtNds(); // 다음 노드 목록 초기화

	// sorting 관련
	void quicksort( int start, int finish);
	int partition( int start, int finish);	

	// test 함수들
	void testNbrs(); // 각 노드의 이웃 노드 정보 파일에 출력
	void testCHs(); // 각 클러스터 CH 정보 파일에 출력

	//void testCandis(); // 후보 부모 노드 정보 테스트
	void testParentNodes(); // 후보 부모 노드 정보 테스트
	void testParentCHs(); // 후보 CH 노드 정보 테스트

	
	virtual double testConect()=0; // (2014.10.31) CH들로부터 BS까지의 경로가 연결되어 있는지 확인 - 반환 값: BS로부터 단절된 CH의 비율



	Rtn(void);
	~Rtn(void);

public:
	int numEvntRcvd; // (2014.08.21) BS에서 수신된 이벤트 보고서 개수

protected:
	map<int, Node*> nodes; // 노드 목록 ( {node ID, node pointer) }

	// 클러스터 관련
	std::map<int, int> ndsClstrMap; // (2014.04.10)  <노드 ID - 클러스터 ID > ( 클러스터 ID = CH's ID)
	std::map<int, bool> clstrUpdated; // (2014.05.19) <노드 iD - 클러스터 갱신 여부(T/F)> - 이전 라운드와 비교하여 소속 클러스터의 CH가 바뀌었는지?
	std::map<int, std::set<int> > clusters; // (2014.04.10) <클러스터 ID - {노드 ID} > (클러스터에 속한 노드들의 ID 집합 관리)
	
	BS* bs; // 기지 노드 객체 주소
	
	map<int, list<Node*>> nbrLsts; // 각 노드의 이웃 노드 목록
	//map<int, list<Node*>> parentsLsts; // 각 노드의 후보 부모 노드 목록 <id, parent list>- 일반 노드
	
	///***
	map<int, list<Node*>> parentsNodes; // 각 노드의 후보 부모 노드 목록 <id, parent list>- 일반 노드
	map<int, list<Node*>> parentCHs; // (2014.08.14)  각 노드의 후보 CH 노드 목록 <id, parent list>- CH 노드
	///***

	map <int, std::pair<int, double> > eval_result; // 후보 노드 평가 결과 - <index, node iD, eval. result>
	map<int, Node*> candis; // 후보 노드 <노드 ID, 노드 주소> map		

	// 다음 전달 노드 관련
	list<Node*> nxtNds; // TREQ/TRES 메시지, 제어 메시지, 키 재분배 메시지 전달을 위한 다음 전달 노드 목록 
	map<int, set<int> > next_recipent_Map; // < 다음 노드 ID, {목적 노드 ID} > map	

	Kmr* kmr; // 키 관리자 객체
	Fuzzy* fuzzy; // fuzzy 객체
	RAND gen1; // (2014.03.24) CH 선택을 위한 난수 생성기
	unsigned int seed1; // CH 선택을 위한 seed	

	struct _tagTREQ* treqMsg; // TREQ 메시지
	struct _tagTRES* tresMsg; // TRES 메시지
};


/*
{
public:

	// 클러스터 구성 관련
	void setNodes( Node nodes[]); // 노드 객체 포인터 배열 초기화 함수
	virtual void findNbrs(); // 이웃 노드 탐색 함수
	virtual void setCHs() = 0; // CH 선택 함수
	Node* getCH (int clstrID); // CH 획득 함수
	
	// 다음 노드 선택 관련
	void addParent (Node* cNode, Node* prevNode); // (2013.10.11 추가) 부모 노드 추가 (한 노드가 여러 개의 부모 노드를 가질 수 있음)	
	std::list<Node*> getParents(Node* cNode); // // (2013.10.11 추가) 부모 노드 리스트 반환
	Node* getSPParent (Node* cNode); // (2013.09.09) 부모 노드 반환
	void setKmr( Kmr* kmr); // (2012.11.10 추가) kmr 객체 멤버변수 설정
	void setSec (Sec *sec); // (2013.10.17 추가) sec 객체 멤버변수 설정

	//void setParent (Node* cNode, Node* prevNode); // (2013.09.02 추가) 부모 노드 설정
	//virtual void setCandis() = 0; // 후보 노드 설정 함수
	//virtual void setCandis( std::list<Node*> lst, Node* node) = 0; // (2013.10.17 추가) 
		
	
	virtual Node** selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode) = 0; // (2013.08.06) 제어 메시지 전달 경로 상의 다음 노드 선택
	virtual Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode) = 0; // 데이터 메시지 전달 경로 상의 다음 노드 선택	
	virtual double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) = 0; // (2013.08.06) 제어 메시지 전달을 위한 후보 노드 평가
	virtual double eval (struct _tagEvntMsg* evntMsg, Node* candi) = 0; // 데이터 메시지 전달을 위한 후보 노드 평가


	void resetNxtNds(); // 다음 노드 목록 초기화

	// sorting 관련
	void quicksort(Node* candis[], int start, int finish);
	int partition(Node* candis[], int start, int finish);

	// test 함수들
	void testNbrs(); // 각 노드의 이웃 노드 정보 파일에 출력
	void testCHs(); // 각 클러스터 CH 정보 파일에 출력
	void testCandis(); // 후보 부모 노드 정보 테스트

	Rtn(void);
	~Rtn(void);

public:

	// cutoff 탐지 관련
	int numEvntRcvd; // BS에서 수신된 이벤트 보고서 개수

	// 노드 정보 
	//Node* nodes; // 노드 정보 배열의 시작 주소	

	list<Node*> nodes_lst; // 노드 객체 리스트

	Node* nbrs[NUM_NODES][NBRS_PER_NODE]; // 이웃 노드 정보
	int numNbrs[NUM_NODES];// (2013.08.13 추가) 각 노드의 이웃 노드 수

	Node* clstr_hds[NUM_CHS]; // ch 정보

	// 후보 노드 관련
	
	std::list<Node*> parents[NUM_NODES]; // (2013.10.11 수정) 부모 노드 정보 (고정된 라우팅 경로를 이용하는 프로토콜에 사용)
	Node* candis[NUM_NODES][CANDIS_PER_NODE]; // 후보 부모 노드 정보
	int numParents[NUM_NODES] ; // (2013.11.27) 부모 노드 수
	int numCandis[NUM_NODES]; // 후보 노드 수
	double eval_result[CANDIS_PER_NODE]; // 현재 노드의 후보 부모 노드들에 대한 평가 함수의 결과 값 저장

	// 다음 전달 노드 관련
	Node* nxtNds[MAX_NXT_NODES]; // 다음 전달 노드 (키 배포 시) 배열
	
	// 키 관련
	Kmr* kmr; // 키 관리자 객체 (2012.11.09 추가)
	Sec* sec; // 보안 프로토콜 객체 (2013.10.17 추가)

	// 난수 생성 관련
	RAND gen1; // (2014.03.24) CH 선택을 위한 난수 생성기
	unsigned int seed1; // CH 선택을 위한 seed

	//random_gen gen1; // CH 선택을 위한 난수 생성기	

};
*/

#endif