// 2014.06.24
// 문수영
// Sec_DIHA
// DIHA의 보안 클래스 선언

#ifndef _SEC_DIHA_H
#define _SEC_DIHA_H

#include "Sec_IHA.h"

class Sec_DIHA : public Sec_IHA // 보안 담당 클래스
{

public:
	
	// 이벤트 보고서 처리 관련	

	void setMAC (EvntMsg* msg); // MAC 설정
	void setMAC (struct _tagEvntMsg* msg, int clstrID); // MAC 설정
	void updateMAC(struct _tagEvntMsg* msg); // (2014.08.20) MAC 갱신
	//void updateMAC(struct _tagEvntMsg* msg, int clstrID); // (2013.11.19) MAC 갱신
	void doEventMsgProc(struct _tagEvntMsg* msg); // (2013.09.30) 이벤트 보고서 처리 (MAC 갱신)

	// 제어 메시지 처리 관련
	void testControl(); //
	void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode); // (2013.08.16 추가) 제어 메시지 수신에 따른 처리

	// 에너지 소모량 계산 관련
	int getEventMsgLen(struct _tagEvntMsg* msg);
	int getCntrlMsgLen(struct _tagCntrlMsg* cm);

	// 연관 노드 갱신

	void updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes); // 연관 노드 갱신

	Sec_DIHA(void);
	~Sec_DIHA(void);

private:

};

#endif