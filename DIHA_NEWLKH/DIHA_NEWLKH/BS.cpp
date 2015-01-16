

// 2014.05.20
// 문수영
// BS.cpp
// 베이스 스테이션 클래스 정의

#include "BS.h"

void BS::reconstNewLKH () // NewLKH (재)구성
{

}

void BS::updateGroupKey() // 그룹 키 갱신
{

}

void BS::setKmr(Kmr* kmr) // 키 관리자 객체 설정
{
	this->kmr = kmr;
}

void BS::setRtn(Rtn* rtn) // 라우팅 객체 설정
{
	this->rtn = rtn;
}

void BS::txMsg(struct _tagCntrlMsg* cm, Node* next)
{
	//...
}

void BS::rxMsg(struct _tagCntrlMsg* cm)
{
	//...
}

void BS::txMsg(struct _tagKeyMsg* km, Node* next)
//void BS::txMsg(struct _tagKeyMsg* km)
{	
	rtn->setRecipent(km, next); // 키 메시지의 목적 노드 설정
}

void BS::addNbr(Node* nbr) // 이웃 노드 추가
{
	nbrLst.push_back(nbr);
}

list<Node*> BS::getNbrLst() // 이웃 노드 목록 반환
{
	return this->nbrLst;
}

void BS::resetNbrLst() // (2014.08.07) BS의 이웃 노드 목록 리셋
{
	this->nbrLst.clear();
}

BS::BS()
{

}

BS::~BS()
{

}