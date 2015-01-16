// 2014.06.24
// ������
// Sec_DIHA
// DIHA�� ���� Ŭ���� ����

#ifndef _SEC_DIHA_H
#define _SEC_DIHA_H

#include "Sec_IHA.h"

class Sec_DIHA : public Sec_IHA // ���� ��� Ŭ����
{

public:
	
	// �̺�Ʈ ���� ó�� ����	

	void setMAC (EvntMsg* msg); // MAC ����
	void setMAC (struct _tagEvntMsg* msg, int clstrID); // MAC ����
	void updateMAC(struct _tagEvntMsg* msg); // (2014.08.20) MAC ����
	//void updateMAC(struct _tagEvntMsg* msg, int clstrID); // (2013.11.19) MAC ����
	void doEventMsgProc(struct _tagEvntMsg* msg); // (2013.09.30) �̺�Ʈ ���� ó�� (MAC ����)

	// ���� �޽��� ó�� ����
	void testControl(); //
	void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode); // (2013.08.16 �߰�) ���� �޽��� ���ſ� ���� ó��

	// ������ �Ҹ� ��� ����
	int getEventMsgLen(struct _tagEvntMsg* msg);
	int getCntrlMsgLen(struct _tagCntrlMsg* cm);

	// ���� ��� ����

	void updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes); // ���� ��� ����

	Sec_DIHA(void);
	~Sec_DIHA(void);

private:

};

#endif