// 2014.05.12
// 문수영
// Rtn.h
// routing 클래스 선언



/*
#include "Node.h"
#include "Atk.h"
#include "Sec.h"
*/

#ifndef _RTN_IHA_H
#define _RTN_IHA_H



#include "params.h"
#include "typedef.h"
#include "Rtn.h"
#include <stdio.h>

class Rtn_IHA : public Rtn
{
public:
	// 클러스터 구성 관련	
	void findNbrs(); // (2013.09.09 추가) 이웃 노드 선택 함수 재정의	
	void setCHs(); // CH 설정 함수

	/* 다음 홉 전달 노드 선택 관련 */

	list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs);

	list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode); 
	// 제어 메시지 전달 경로 상의 다음 노드 선택

	Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode); 
	// 데이터 메시지 전달 경로 상의 다음 노드 선택
	
	list<Node*> selNext (struct _tagKeyMsg* keyMsg, BS* bs);
	//  키 재분배 메시지 전달 경로 상의 다음 노드 선택

	list<Node*> selNext (struct _tagKeyMsg* keyMsg, Node* cNode);
	// 키 재분배 메시지 전달 경로 상의 다음 노드 선택

	list<Node*> selNext (struct _tagTREQ* treq, Node* cNode);
	// (2014.05.10) TREQ 메시지 전달을 위한 다음 노드 선택

	list<Node*> selNext (struct _tagTRES* tres, Node* cNode);
	// (2014.05.10) TREQ 메시지 전달을 위한 다음 노드 선택

	double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi);
	// 제어 메시지 전달을 위한 후보 노드 평가

	double eval (struct _tagEvntMsg* evntMsg, Node* candi); 
	// 데이터 메시지 전달을 위한 후보 노드 평가

	double eval(Node* recipent, Node* candi);
	// 키 재분배 메시지 전달을 위한 후보 노드 평가

	//double eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi);
	// 키 재분배 메시지 전달을 위한 후보 노드 평가

	
	double testConect();

	Rtn_IHA(void);
	~Rtn_IHA(void);

protected:

};

/*
public:
	// 클러스터 구성 관련	

	void findNbrs(); // (2013.09.09 추가) 이웃 노드 선택 함수 재정의

	
	void setCHs(); // CH 설정 함수
	void setCandis(); // 후보 노드 설정 함수

	// 다음 홉 전달 노드 선택 관련
	Node** selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode); 
	// 제어 메시지 전달 경로 상의 다음 노드 선택

	Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode); 
	// 데이터 메시지 전달 경로 상의 다음 노드 선택
	
	double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi);
	// 제어 메시지 전달을 위한 후보 노드 평가

	double eval (struct _tagEvntMsg* evntMsg, Node* candi); 
	// 데이터 메시지 전달을 위한 후보 노드 평가

	Rtn_IHA(void);
	~Rtn_IHA(void);

protected:

};
*/

/*

class Sec;

class Rtn
{
public:

	// 클러스터 구성 관련
	void setNodes( Node nodes[]); // 노드 객체 포인터 배열 초기화 함수
	void findNbrs(); // 이웃 노드 탐색 함수
	virtual void setCHs() = 0; // CH 선택 함수
	Node* getCH (int clstrID); // CH 획득 함수
	
	// 다음 노드 선택 관련
	void setKmr( Kmr* kmr); // (2012.11.10 추가) kmr 객체 멤버변수 설정
	virtual void setCandis() = 0; // 후보 노드 설정 함수

	
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

protected:


	// 노드 정보 
	Node* nodes; // 노드 정보 배열의 시작 주소	

	Node* nbrs[NUM_NODES][NBRS_PER_NODE]; // 이웃 노드 정보
	int numNbrs[NUM_NODES];// (2013.08.13 추가) 각 노드의 이웃 노드 수

	Node* clstr_hds[NUM_CHS]; // ch 정보

	// 후보 노드 관련

	Node* candis[NUM_NODES][CANDIS_PER_NODE]; // 후보 부모 노드 정보
	int numCandis[NUM_NODES];
	double eval_result[CANDIS_PER_NODE]; // 현재 노드의 후보 부모 노드들에 대한 평가 함수의 결과 값 저장

	// 다음 전달 노드 관련
	Node* nxtNds[MAX_NXT_NODES]; // 다음 전달 노드 (키 배포 시) 배열
	
	// 키 관련
	Kmr* kmr; // 키 관리자 객체 (2012.11.09 추가)

	// 난수 생성 관련
	random_gen gen1; // CH 선택을 위한 난수 생성기
	unsigned int seed1; // CH 선택을 위한 seed

};
*/
#endif