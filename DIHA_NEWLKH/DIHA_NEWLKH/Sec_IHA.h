// 2014.06.24
// 문수영
// Sec_IHA.h
// Sec_IHA 클래스 선언

#ifndef _SEC_IHA_H
#define _SEC_IHA_H

#include "params.h"
#include "Node.h"
#include "Rtn.h"
#include "Sec.h"
#include "RAND.h"
#include "typedef.h"
#include "Kmr.h"
#include <map>
#include <set>

class Node;

class Sec_IHA : public Sec
{
public:
	
	// 이벤트 보고서 처리 관련		
	virtual void setMAC (EvntMsg* msg); // MAC 설정
	virtual void setMAC (struct _tagEvntMsg* msg, int clstrID); // MAC 설정 - msg: 이벤트 보고서, clstrID: 클러스터 ID (= CH's ID)
	bool verMAC (EvntMsg* msg); // MAC 검증
	virtual void updateMAC(struct _tagEvntMsg* msg); // (2014.08.20) MAC 갱신
	//virtual void updateMAC(struct _tagEvntMsg* msg, int clstrID); // (2013.11.19) MAC 갱신
	virtual void doEventMsgProc(struct _tagEvntMsg* msg); // (2013.09.30) 이벤트 보고서 처리 (MAC 갱신)
	
	void writeEventMsg(ofstream& out, int event_no, struct _tagEvntMsg* msg); // (2014.08.11) 생성된 이벤트 보고서 테스트 (파일에 기록)

	// 제어 메시지 처리 관련

	void genCntrlMsg(ctrlMsgType type, BS* bs); // (2013.08.23 추가) 제어 메시지 생성 함수
	void genCntrlMsg(ctrlMsgType type, Node* cNode); // (2013.08.23 추가) 제어 메시지 생성 함수
	void testCntrlMsg();
	virtual void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode); // (2013.08.16 추가) 제어 메시지 수신에 따른 처리
	virtual void testControl(); // (2013.11.22) 임시 테스트 함수

	// 에너지 소모량 계산 관련
	
	virtual int getEventMsgLen(struct _tagEvntMsg* msg); // (2013.09.30) 이벤트 보고서 길이 반환
	virtual int getCntrlMsgLen(struct _tagCntrlMsg* cm); // (2014.01.10) 제어 메시지 길이 반환


	// 공격 탐지 관련
	bool detect (EvntMsg* msg); // 공격 검출	

	// 연관 노드 갱신
	virtual void updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes);
	bool compareAssoc ( set<int> oldAssoc, set<int> newAssoc); // (2014.05.27) 기존 연관 노드 목록과 신규 연관 노드 목록이 동일한지 비교 - 동일한 경우 true, 그렇지 않다면 false 반환
	
	void resetAssoc(); // (2014.08.21) 기존 키 관리 기법에서 사용 - 이전 라운드의 연관 노드 정보를 제거

	// 키 재분배 관련
	bool hasUpdatedAssoc (int nodeID); // (2014.07.17) 주어진 nodeID를 가진 노드의 연관 노드가 이전 라운드와 비교하여 변경되었으면 TRUE, 그렇지 않다면 false 반환
	void resetUpdatedAssoc(); // (2014.07.22) 노드들의 연관 노드 갱신 여부를 false 로 리셋 (키 재분배 목적)

	// 연관 노드 테스트
	void testAssoc(); // (2014.07.17) 노드들의 연관 노드 목록 파일에 출력


	Sec_IHA();
	~Sec_IHA();

public:
	std::map<int,  std::map <int, std::set<int>> > upperNodes; //  (2014.04.25 수정) 상위 노드 목록 (map) - <node ID, <hop count, {upper node ID}>
	std::map<int,  std::map <int, std::set<int>> > lowerNodes; //  (2014.04.25 수정) 하위 노드 목록 (map) - <node ID, <hop count, {lower node ID}>
	
	// 연관 관계 관련
	map <int, bool> assocUpdated; // 연관 노드 갱신 여부 <node ID, 갱신 여부 (T/F) >
};
	
#endif