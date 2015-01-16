// 2014.05.20
// 문수영
// Rtn_DIHA.h
// DIHA의 라우팅 클래스 선언

#ifndef _RTN_DIHA_H
#define _RTN_DIHA_H

#include "Rtn.h"
#include "params.h"
#include "typedef.h"
#include "Fuzzy.h"

class Node;

class Rtn_DIHA : public Rtn// 라우팅 담당 클래스
{
public:

	// 클러스터 구성 관련	
	void findNbrs(); // (2013.09.09 추가) 이웃 노드 선택 함수 재정의	
	void setCHs(); // CH 설정 함수

	/* 다음 홉 전달 노드 선택 관련*/

	list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs); // // 제어 메시지를 위한 다음 노드 선택

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

	double eval_fuzzy (double dist, double eCnpmtn, double numKeys); // fuzzy logic에 의한 후보 노드 평가

	double eval(Node* recipent, Node* candi);
	// 키 재분배 메시지 전달을 위한 후보 노드 평가

	//double eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi);
	// 키 재분배 메시지 전달을 위한 후보 노드 평가

	double testConect();

	Rtn_DIHA(void);
	~Rtn_DIHA(void);

private:
	
	Fuzzy fuzzy; //  퍼지 로직 추론 객체

};

#endif