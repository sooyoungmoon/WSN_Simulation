// 2014.05.15
// 문수영
// Node.h
// node 클래스 선언

#ifndef _NODE_H
#define _NODE_H

#include "typedef.h"
#include "Rtn.h"
#include "Sec.h"
#include <fstream>

class Sec;
class Node;

class Node
{

public:
	int id;
	int x;
	int y;
	int cell; // 필드가 그리드 구조를 가질 때 노드가 위치하는 cell의 번호

	double distance; // sink 노드까지의 거리
	double eCnsmptn; // 노드의 에너지 소모량
	double remEnrgy; // 노드의 잔여 에너지량
	double enrgyDiff; // 에너지 변화량
	bool cmprmsd; // 노드가 훼손된 경우 1, 정상인 경우 0

	static double totEnrgCnsmptn; // (2013.10.31) 노드들의 전체 에너지 소모량
	unsigned static int totNumNodes; // 현재까지 생성된 (이미 제거된 노드들도 포함) 총 노드 수 
							// 다음에 생성될 노드의 ID

	unsigned static int numNodesEvctd; // 현재까지 제거된 노드 수 (=다음에 제거될 노드의 ID) - First created, first evicted

	Rtn* rtn; // 라우팅 객체
	Sec* sec; // 보안 객체
	Kmr* kmr; // 키 관리 객체

	bool newbee; // (2014.05.08) 이번 라운드에 새로 추가된 경우 true, 그렇지 않다면 false

	int round; // (2014.05.10) 현재 참여하고 있는 라운드 번호 - topology request/response 단계에서 설정됨- (초기값 = -1)

	struct _tagKeyMsg* receivedKMsg; // 수신된 키 재분배 메시지
	//struct _tagKeyMsg receivedKMsg; // 수신된 키 재분배 메시지
	//static std::ofstream out; // (2013.09.03) 파일 출력 스트림
	
public:
	void txMsg(struct _tagEvntMsg* evntMsg); // 데이터 메시지 송신 함수
	bool rxMsg(struct _tagEvntMsg* evntMsg); // (2012.11.23 수정) 데이터 메시지 수신 함수 (계속 전달이 필요한 경우 true, 전달을 중지해야 할 때는 false 반환)

	void txMsg(struct _tagKeyMsg* km, Node* nextNode); // 키 메시지 송신 함수
	bool rxMsg(struct _tagKeyMsg* km); // 키 메시지 수신 함수	

	void txMsg(struct _tagCntrlMsg* cm); // (2013.08.13) 제어 메시지 송신 함수
	bool rxMsg(struct _tagCntrlMsg* cm); // (2013.08.13) 제어 메시지 수신 함수	

	void txMsg(struct _tagTREQ* treq); // (2014.05.10) TREQ 송신 함수
	int rxMsg(struct _tagTREQ* treq); // (2014.05.10) TREQ 수신 함수	

	void txMsg(struct _tagTRES* tres); // (2014.05.10) TRES 송신 함수
	int rxMsg(struct _tagTRES* tres); // (2014.05.10) TRES 수신 함수	

	void setRtn(Rtn* rtn); // 라우팅 객체 설정
	void setKmr(Kmr* kmr); // 키 관리 객체 설정
	void setSec(Sec* sec); // 보안 객체 설정

	void printNode(std::ofstream &out); // (2013.09.03 추가) 노드 정보 출력

	bool depleted(void); // (2013.09.23)

	Node(void);
	~Node(void);
};

#endif