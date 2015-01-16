

// 2014.05.14
// 문수영
// BS.h
// 베이스 스테이션 클래스 선언

#ifndef _BS_H
#define _BS_H

#include "Kmr.h"
#include "Rtn.h"
#include <list>

using std::list;

class Kmr;
class Rtn;
class Node;

class BS 
{
public:

	void reconstNewLKH (); // NewLKH (재)구성
	void updateGroupKey(); // 그룹 키 갱신
	void setKmr(Kmr* kmr); // 키 관리자 객체 설정
	void setRtn(Rtn* rtn); // 라우팅 객체 설정

	void txMsg(struct _tagCntrlMsg* cm, Node* next); // 제어 메시지 송신 함수 (cm: 제어 메시지, next: 다음 전달 노드)
	void rxMsg(struct _tagCntrlMsg* cm); // 제어 메시지 수신 함수 (cm: 제어 메시지) 

	void txMsg(struct _tagKeyMsg* km, Node* next); // 키 메시지 송신 함수 (km: key message, next: 다음 전달 노드)
	
	void addNbr(Node* nbr); // 이웃 노드 추가

	list<Node*> getNbrLst(); // 이웃 노드 목록 반환
	
	void resetNbrLst(); // (2014.08.07) BS의 이웃 노드 목록 리셋


	BS();
	~BS();

private:

	Kmr* kmr;
	Rtn* rtn;

	std::list<Node*> nbrLst;

};

#endif