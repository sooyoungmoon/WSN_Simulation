// 2014.05.27
// 문수영
// Sec.h
// security 클래스 선언

#ifndef _SEC_H
#define _SEC_H

#include "params.h"
#include "Node.h"
#include "BS.h"
#include "RAND.h"
#include "typedef.h"
#include "Kmr.h"
#include <set>
#include <list>
#include <map>

class Node;
class BS;
class Kmr;
class Rtn;

using std::map;

class Sec
{

public:

	// 멤버 변수 값 설정

	void setNodes(map<int, Node*> nodes); // 노드 목록 설정
	void setRtn (Rtn* rtn); //  라우팅 객체 주소 설정
	void setKmr( Kmr* kmr); // 키 관리자 객체 주소 설정

	// 이벤트 보고서 처리 관련

	void initEventMsg(struct _tagEvntMsg* msg, Node* CoS); // 이벤트 보고서 초기화

	virtual void setMAC (struct _tagEvntMsg* msg) = 0;  // MAC 초기 생성
	virtual void setMAC (struct _tagEvntMsg* msg, int clstrID) = 0;  // MAC 초기 생성 (클러스터 기반) 
	virtual bool verMAC (struct _tagEvntMsg* msg) = 0; // MAC 검증 (성공 시 true, 실패 시 false 반환)

	virtual void updateMAC(struct _tagEvntMsg* msg) = 0; // MAC 갱신
	//virtual void updateMAC(struct _tagEvntMsg* msg, int clstrID) = 0; //  MAC 갱신	
	virtual void doEventMsgProc(struct _tagEvntMsg* msg) = 0;  // 수신된 이벤트 보고서 처리	

	virtual void writeEventMsg(std::ofstream& out, int event_no, struct _tagEvntMsg* msg) = 0; // 생성된 이벤트 보고서 테스트 (파일에 기록)
	
	// 제어 메시지 처리 관련

	virtual void genCntrlMsg(enum ctrlMsgType type, BS* bs)=0;  // 제어 메시지 생성
	virtual void genCntrlMsg(enum ctrlMsgType type, Node* cNode)=0;  // 제어 메시지 생성
	void setReceiver(struct _tagCntrlMsg* msg, Node* nextNode); // 제어 메시지 수신 노드 설정

	virtual void testCntrlMsg()=0; // 제어 메시지 테스트 
	virtual void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) = 0; // 수신된 제어 메시지 처리
	struct _tagCntrlMsg* getCntrlMsg(); // 제어 메시지 획득
	void setCntrlMsg(int clstrID); // 제어 메시지 내 클러스터 ID 설정	
	
	// 에너지 소모량 계산 관련

	virtual int getEventMsgLen(struct _tagEvntMsg* msg) = 0; // 이벤트 보고서 길이 반환
	virtual int getCntrlMsgLen(struct _tagCntrlMsg* cm) = 0; // 제어 메시지 길이 반환 	 

	// 공격 탐지 관련
	virtual bool detect (struct _tagEvntMsg* msg) = 0; // 공격 검출 
	int getNumDtctn(); // 공격 탐지 횟수 반환
	int getNumVer(); // MAC 검증 횟수 반환	 
	bool isAtkDtctd(); // (2014.08.21) 공격 검출 여부 반환

	// 연관 노드 갱신 관련
	virtual void updateAssocNodes(int nodeID, int assocType, int hopDist, std::set<int> assocNodes) = 0; // assocType: 0: upper assoc, 1: lower assoc
	
	// MAC 연산 관련
	void resetTmpLst(); // 임시 MAC 목록 reset

	Sec();
	~Sec();

protected:

	// 객체 멤버 변수
	map<int, Node*> nodes; // 노드 목록 ( {node ID, node pointer) }
	Rtn* rtn; // 라우팅 객체
	Kmr* kmr; // 키 관리자 객체

	struct _tagCntrlMsg* cntlMsg; // 제어 메시지	

	// 공격 탐지 관련
	bool atkDtctd; // 공격 탐지 여부 (예: 허위 보고서 공격)
	int numDtctn; // // 공격 탐지 횟수 (예: 허위 보고서 공격)	
	int numVer; // 보고서 검증 횟수

	// MAC 연산 관련
	std::list<struct _tagMAC*> tmplst;// (2013.11.19) 임시 MAC 목록 (보고서 내 MAC 갱신 과정에서 사용)	

};

/*

class Sec
{
public:

	void setNodes( Node nodes[]); // (2013.08.28 추가) 

	void setRtn (Rtn* rtn); // (2013.09.09) 라우팅 객체 주소 설정
	void setKmr( Kmr* kmr); // 키 관리자 객체 주소 설정

	// 이벤트 보고서 처리 관련

	virtual void setMAC (struct _tagEvntMsg* msg) = 0; // MAC 설정

	virtual void setMAC (struct _tagEvntMsg* msg, int clstrID) = 0; // MAC 설정 (클러스터 기반)

	virtual void updateMAC(struct _tagEvntMsg* msg, int clstrID) = 0; // (2013.11.19) MAC 갱신 함수

	virtual bool verMAC (struct _tagEvntMsg* msg) = 0; // MAC 검증 (성공 시 true, 실패 시 false 반환)

	virtual bool detect (struct _tagEvntMsg* msg) = 0; // 공격 검출 (공격 검출 시 true, 아니면 false 반환)

	virtual void doEventMsgProc(struct _tagEvntMsg* msg) = 0; // 이벤트 보고서 처리 함수 (2013.09.30 추가)

	virtual int getEventMsgLen(struct _tagEvntMsg* msg) = 0; // 보고서 길이 반환 함수 (2013.09.30 추가)

	virtual int getCntrlMsgLen(struct _tagCntrlMsg* cm) = 0; // 제어 메시지 길이 반환 함수 (2014.01.10 추가)

	// 제어 메시지 생성 (2013.08.21 추가)
	virtual void genCntrlMsg(enum ctrlMsgType type, Node* cNode)=0; // 제어 메시지 생성

	virtual void testCntrlMsg(); // 제어 메시지 테스트 (2013.09.05 추가)

	CntrlMsg* getCntrlMsg(); // 제어 메시지 획득 (2013.09.05 추가)

	void setCntrlMsg(int clstrID); // 제어 메시지 내 cluster ID 설정 (2013.09.16) 추가

	// 제어 메시지 처리 관련 (2013.08.16 추가)
	virtual void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) = 0; // 제어 메시지 수신에 따른 처리

	
	int getNumDtctn(); // 공격 탐지 횟수 반환
	int getNumVer(); // 보고서 검증 횟수 반환

	virtual void testControl() = 0;// 임시 테스트 함수

	Sec(void);
	~Sec(void);

public:

	// 노드 정보 
	Node* nodes; // 노드 정보 배열의 시작 주소

	// BS 정보
	BS* bs; // 기지 노드 객체 주소 (2014.03.24 추가)

	Rtn* rtn; // 라우팅 객체
	Kmr* kmr; // 키 관리자 객체	

	struct _tagCntrlMsg cntlMsg;// 제어 메시지 (2013.08.21)

	bool atkDtctd; // 공격 탐지 여부 (예: 허위 보고서 공격)
	int numDtctn; // // 공격 탐지 횟수 (예: 허위 보고서 공격)	

	int numVer; // 보고서 검증 횟수

	std::list<struct _tagMAC*> tmplst;// (2013.11.19) 임시 MAC 목록 (보고서 내 MAC 갱신 과정에서 사용)

	/*
	IDLIST upperNodes[NUM_NODES][IHA_T+1]; //  (2013.11.22 변경) 상위 노드 목록 (array of list)
	IDLIST lowerNodes[NUM_NODES][IHA_T+1]; //  (2013.11.22 변경) 하위 노드 목록	
	*/

//};

#endif