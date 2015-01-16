// 2014.06.24
// ������
// Sec_IHA.h
// Sec_IHA Ŭ���� ����

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
	
	// �̺�Ʈ ���� ó�� ����		
	virtual void setMAC (EvntMsg* msg); // MAC ����
	virtual void setMAC (struct _tagEvntMsg* msg, int clstrID); // MAC ���� - msg: �̺�Ʈ ����, clstrID: Ŭ������ ID (= CH's ID)
	bool verMAC (EvntMsg* msg); // MAC ����
	virtual void updateMAC(struct _tagEvntMsg* msg); // (2014.08.20) MAC ����
	//virtual void updateMAC(struct _tagEvntMsg* msg, int clstrID); // (2013.11.19) MAC ����
	virtual void doEventMsgProc(struct _tagEvntMsg* msg); // (2013.09.30) �̺�Ʈ ���� ó�� (MAC ����)
	
	void writeEventMsg(ofstream& out, int event_no, struct _tagEvntMsg* msg); // (2014.08.11) ������ �̺�Ʈ ���� �׽�Ʈ (���Ͽ� ���)

	// ���� �޽��� ó�� ����

	void genCntrlMsg(ctrlMsgType type, BS* bs); // (2013.08.23 �߰�) ���� �޽��� ���� �Լ�
	void genCntrlMsg(ctrlMsgType type, Node* cNode); // (2013.08.23 �߰�) ���� �޽��� ���� �Լ�
	void testCntrlMsg();
	virtual void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode); // (2013.08.16 �߰�) ���� �޽��� ���ſ� ���� ó��
	virtual void testControl(); // (2013.11.22) �ӽ� �׽�Ʈ �Լ�

	// ������ �Ҹ� ��� ����
	
	virtual int getEventMsgLen(struct _tagEvntMsg* msg); // (2013.09.30) �̺�Ʈ ���� ���� ��ȯ
	virtual int getCntrlMsgLen(struct _tagCntrlMsg* cm); // (2014.01.10) ���� �޽��� ���� ��ȯ


	// ���� Ž�� ����
	bool detect (EvntMsg* msg); // ���� ����	

	// ���� ��� ����
	virtual void updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes);
	bool compareAssoc ( set<int> oldAssoc, set<int> newAssoc); // (2014.05.27) ���� ���� ��� ��ϰ� �ű� ���� ��� ����� �������� �� - ������ ��� true, �׷��� �ʴٸ� false ��ȯ
	
	void resetAssoc(); // (2014.08.21) ���� Ű ���� ������� ��� - ���� ������ ���� ��� ������ ����

	// Ű ��й� ����
	bool hasUpdatedAssoc (int nodeID); // (2014.07.17) �־��� nodeID�� ���� ����� ���� ��尡 ���� ����� ���Ͽ� ����Ǿ����� TRUE, �׷��� �ʴٸ� false ��ȯ
	void resetUpdatedAssoc(); // (2014.07.22) ������ ���� ��� ���� ���θ� false �� ���� (Ű ��й� ����)

	// ���� ��� �׽�Ʈ
	void testAssoc(); // (2014.07.17) ������ ���� ��� ��� ���Ͽ� ���


	Sec_IHA();
	~Sec_IHA();

public:
	std::map<int,  std::map <int, std::set<int>> > upperNodes; //  (2014.04.25 ����) ���� ��� ��� (map) - <node ID, <hop count, {upper node ID}>
	std::map<int,  std::map <int, std::set<int>> > lowerNodes; //  (2014.04.25 ����) ���� ��� ��� (map) - <node ID, <hop count, {lower node ID}>
	
	// ���� ���� ����
	map <int, bool> assocUpdated; // ���� ��� ���� ���� <node ID, ���� ���� (T/F) >
};
	
#endif