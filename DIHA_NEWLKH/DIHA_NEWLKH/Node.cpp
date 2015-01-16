// 2014.05.14
// Node.cpp
// 문수영

#include "Node.h"
#include <iostream>
#include <fstream>

using namespace std;

double Node::totEnrgCnsmptn; // 정적 멤버 변수 사용 시 이를 사용하는 소스 파일에서 이와 같이 선언문을 추가해 주어야 함

void Node::txMsg(struct _tagEvntMsg* evntMsg)
{
	(evntMsg->hop)++; // (2013.12.27 위치 이동) 메시지 진행 홉 수 증가

	// 에너지 소모량 계산

	// 이벤트 보고서 메시지 길이 획득 (프로토콜의 종류에 따라 달라짐)

	int msgLen = sec->getEventMsgLen(evntMsg);

	enrgyDiff = msgLen * txEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; // (2013.10.07) 보고서 송신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; // (2013.10.07) 보고서 송신에 따른 잔여 에너지 감소

	if ( remEnrgy < 0)
	{
		remEnrgy = 0;
	}
	// 통신 오버헤드 계산 	
}

bool Node::rxMsg(struct _tagEvntMsg* evntMsg)
{
	bool res = true;	
	//(evntMsg->hop)++; // (2013.12.27 주석처리) 메시지 진행 홉 수 증가
	
	int msgLen = sec->getEventMsgLen(evntMsg); // 이벤트 보고서 메시지 길이 획득

	enrgyDiff = msgLen * rxEnrg; // 현재 에너지 소모량
	
	eCnsmptn += enrgyDiff; // (2013.10.07) 보고서 수신에 따른 현재 노드의 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; // (2013.10.07) 보고서 수신에 따른 현재 노드의 잔여 에너지 감소

	if ( remEnrgy < 0)
	{
		remEnrgy = 0;
	}	

	// (2012.11.23) 공격 검출 함수 호출
	
	if ( sec->detect(evntMsg) == true) // 공격이 검출된 경우 전달 중지를 위해 false 반환
	{
		res = false;
	}
	if ( distance < TRANS_RANGE) // (2012.11.24) 수신 노드가 BS에서 전송 범위 이내이면 더 이상 전달할 필요 없음
	{
		res = false; // (2013.10.08) 주석처리
	}
	// 보안 프로토콜에 따라 이벤트 보고서에 대한 처리 과정 수행
	sec->doEventMsgProc(evntMsg); // (2013.09.30)

	return res;
}

void Node::txMsg(struct _tagKeyMsg* km, Node* nextNode) // 키 메시지 송신 함수
{
	// 이전에 저장된 키 재분배 메시지 복원


	// 에너지 소모량 계산
	int msgLen = kmr->getKeyMsgLen(km);
	

	enrgyDiff = msgLen * txEnrg; // 현재 에너지 소모량
	eCnsmptn += enrgyDiff; // (2013.10.07) 보고서 송신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; // (2013.10.07) 보고서 송신에 따른 잔여 에너지 감소

	if ( remEnrgy < 0)
	{
		remEnrgy = 0;
	}

	rtn->setRecipent(km, nextNode); // 목적 노드 설정

}


bool Node::rxMsg(struct _tagKeyMsg* km) // 키 메시지 수신 함수	
{
	bool res = true;	

	// 에너지 소모량 계산
	int msgLen = kmr->getKeyMsgLen(km);
	
	enrgyDiff = msgLen * rxEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; // (2013.10.07) 보고서 수신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; // (2013.10.07) 보고서 수신에 따른 잔여 에너지 감소


	// 키 메시지 처리 함수 호출

	res = kmr->handleKeyMsg(km, this);

	return res; // 키 재분배 메시지 처리 결과를 반환 (true: 계속 전달, false: 전달 완료)
}

void Node::txMsg(struct _tagCntrlMsg* cm)
{
	// 에너지 소모량 계산
	//this->eCnsmptn += CNTL_MSG_LEN * txEnrg; 

	/* (2014.01.10 수정) 연관 노드 설정 단계에서의 오버헤드 계산 */

	int msgLen = sec->getCntrlMsgLen(cm);

	enrgyDiff = msgLen * txEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; // (2013.10.07) 제어 메시지 송신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; // (2013.10.07) 제어 메시지 송신에 따른 잔여 에너지 감소

	/* (2014.01.10 수정완료) */

}

bool Node::rxMsg(struct _tagCntrlMsg* cm)
{
	bool res = true;

	// 에너지 소모량 계산
	//this->eCnsmptn += CNTL_MSG_LEN * txEnrg; 
	
	/* (2014.01.10 수정) 연관 노드 설정 단계에서의 오버헤드 계산 */

	int msgLen = sec->getCntrlMsgLen(cm);

	enrgyDiff = msgLen * rxEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; // (2013.10.07) 제어 메시지 수신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) 전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; // (2013.10.07) 제어 메시지 수신에 따른 잔여 에너지 감소

	/* (2014.01.10 수정완료) */

	sec->doCntlOp(cm, this);// 제어 메시지 수신에 따른 처리 함수 호출 (부모 노드 설정 포함)


	return res;
}

void Node::txMsg(struct _tagTREQ* treq)
{
	// 에너지 소모량 계산

	int msgLen = TREQ_MSG_LEN; // TREQ 메시지 길이

	enrgyDiff = msgLen * txEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; //  TREQ 송신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; //  전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; //  TREQ 송신에 따른 잔여 에너지 감소

	// 에너지 소모량 계산 완료
}

int Node::rxMsg(struct _tagTREQ* treq) // (2014.05.10) TREQ 수신 함수	
{	
	// 에너지 소모량 계산

	int msgLen = TREQ_MSG_LEN; // TREQ 메시지 길이
	enrgyDiff = msgLen * rxEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; //  TREQ 수신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; //  전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; //  TREQ 수신에 따른 잔여 에너지 감소
	// 에너지 소모량 계산 완료
	
	rtn->handleTREQ(treq, this);	// TREQ 처리 함수 호출	

	return 0;
}

void Node::txMsg(struct _tagTRES* tres)
{
	// 에너지 소모량 계산

	int msgLen; // TRES 메시지 길이

	msgLen = rtn->getTRESMsgLen(tres);
	enrgyDiff = msgLen * txEnrg; // 현재 에너지 소모량

	eCnsmptn += enrgyDiff; //  TRES 송신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; //  전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; //  TRES 송신에 따른 잔여 에너지 감소
	// 에너지 소모량 계산 완료
}

int Node::rxMsg(struct _tagTRES* tres) // (2014.05.10) TRES 수신 함수	
{	
	// 에너지 소모량 계산

	int msgLen; // TRES 메시지 길이
	msgLen = rtn->getTRESMsgLen(tres);

	enrgyDiff = msgLen * rxEnrg; // 현재 에너지 소모량
	eCnsmptn += enrgyDiff; //  TRES 수신에 따른 에너지 소모량 누적
	Node::totEnrgCnsmptn += enrgyDiff; //  전체 노드들의 총 에너지 소모량 누적 (정적 변수)
	remEnrgy -= enrgyDiff; //  TRES 수신에 따른 잔여 에너지 감소
	// 에너지 소모량 계산 완료	
	
	return 0;
}


void Node::setRtn(Rtn* rtn)
{
	this->rtn = rtn;
}

void Node::setKmr(Kmr* kmr)
{
	this->kmr = kmr;
}

void Node::setSec(Sec* sec)
{
	this->sec = sec;
}

void Node::printNode(std::ofstream &out) // (2013.09.03) 노드 정보 출력 (id, 좌표, 거리, 에너지 소모량, 훼손 노드 여부)
{	
	//int clstrId = id / NODES_PER_CLSTR; // cluster ID
	int clstrId = rtn->getClstrID(this->id); // (2014.04.14) 노드 ID -> 클러스터 ID 도출 방법 변경

	out << "\n cluster ID: " << clstrId << ", " ;
	out << "Node ID: " << id << ","; // 
	out << " (x, y) = (" << x << ", " << y << ") ";	
	out << " dist = " << sqrt (distance) << "m,";
	out << " eCnsmptn = " << this->eCnsmptn << ",";

	Node* parent = rtn->getSPParentNode(this); // (2014.04.14)  부모 노드 도출 방법 변경
	//Node* parent = rtn->getSPParent(this); // (2014.04.14)  부모 노드 도출 방법 변경
	 
	//Node* parent = rtn->getParent(this);

	if (parent != NULL)
	{
		out << " parent Node= " << parent->id  << ",";
	}

	Node* parentCH = rtn->getSPParentCH(this); // (2014.04.14)  부모 노드 도출 방법 변경
	//Node* parent = rtn->getSPParent(this); // (2014.04.14)  부모 노드 도출 방법 변경
	 
	//Node* parent = rtn->getParent(this);

	if (parentCH != NULL)
	{
		out << " parent CH= " << parentCH->id  << ",";
	}

	out << this->cmprmsd? "compromised" : "normal" ; // 훼손 노드 여부
	out << endl;

}

bool Node::depleted(void)
{
	bool ret = false;

	if (this->remEnrgy < eTh)
		ret = true;

	return ret;
}

Node::Node()
{
	id = totNumNodes;
	//id = 0;
	x = 0;
	y = 0;

	distance = 0;

	this->remEnrgy = INIT_ENRGY; // (2013.09.24) 노드의 잔여 에너지량 초기화
	eCnsmptn = 0;
	enrgyDiff = 0;
	cmprmsd = false;

	rtn = NULL;
	sec = NULL;
	kmr = NULL;	

	newbee = true; 

	round = -1;

	this->receivedKMsg = new KeyMsg();
}

Node::~Node()
{
	delete (receivedKMsg);
	//out.close(); // 파일 출력 스트림 닫기
}
