// 2014.05.27
// ������
// Sec.h
// security Ŭ���� ����

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

	// ��� ���� �� ����

	void setNodes(map<int, Node*> nodes); // ��� ��� ����
	void setRtn (Rtn* rtn); //  ����� ��ü �ּ� ����
	void setKmr( Kmr* kmr); // Ű ������ ��ü �ּ� ����

	// �̺�Ʈ ���� ó�� ����

	void initEventMsg(struct _tagEvntMsg* msg, Node* CoS); // �̺�Ʈ ���� �ʱ�ȭ

	virtual void setMAC (struct _tagEvntMsg* msg) = 0;  // MAC �ʱ� ����
	virtual void setMAC (struct _tagEvntMsg* msg, int clstrID) = 0;  // MAC �ʱ� ���� (Ŭ������ ���) 
	virtual bool verMAC (struct _tagEvntMsg* msg) = 0; // MAC ���� (���� �� true, ���� �� false ��ȯ)

	virtual void updateMAC(struct _tagEvntMsg* msg) = 0; // MAC ����
	//virtual void updateMAC(struct _tagEvntMsg* msg, int clstrID) = 0; //  MAC ����	
	virtual void doEventMsgProc(struct _tagEvntMsg* msg) = 0;  // ���ŵ� �̺�Ʈ ���� ó��	

	virtual void writeEventMsg(std::ofstream& out, int event_no, struct _tagEvntMsg* msg) = 0; // ������ �̺�Ʈ ���� �׽�Ʈ (���Ͽ� ���)
	
	// ���� �޽��� ó�� ����

	virtual void genCntrlMsg(enum ctrlMsgType type, BS* bs)=0;  // ���� �޽��� ����
	virtual void genCntrlMsg(enum ctrlMsgType type, Node* cNode)=0;  // ���� �޽��� ����
	void setReceiver(struct _tagCntrlMsg* msg, Node* nextNode); // ���� �޽��� ���� ��� ����

	virtual void testCntrlMsg()=0; // ���� �޽��� �׽�Ʈ 
	virtual void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) = 0; // ���ŵ� ���� �޽��� ó��
	struct _tagCntrlMsg* getCntrlMsg(); // ���� �޽��� ȹ��
	void setCntrlMsg(int clstrID); // ���� �޽��� �� Ŭ������ ID ����	
	
	// ������ �Ҹ� ��� ����

	virtual int getEventMsgLen(struct _tagEvntMsg* msg) = 0; // �̺�Ʈ ���� ���� ��ȯ
	virtual int getCntrlMsgLen(struct _tagCntrlMsg* cm) = 0; // ���� �޽��� ���� ��ȯ 	 

	// ���� Ž�� ����
	virtual bool detect (struct _tagEvntMsg* msg) = 0; // ���� ���� 
	int getNumDtctn(); // ���� Ž�� Ƚ�� ��ȯ
	int getNumVer(); // MAC ���� Ƚ�� ��ȯ	 
	bool isAtkDtctd(); // (2014.08.21) ���� ���� ���� ��ȯ

	// ���� ��� ���� ����
	virtual void updateAssocNodes(int nodeID, int assocType, int hopDist, std::set<int> assocNodes) = 0; // assocType: 0: upper assoc, 1: lower assoc
	
	// MAC ���� ����
	void resetTmpLst(); // �ӽ� MAC ��� reset

	Sec();
	~Sec();

protected:

	// ��ü ��� ����
	map<int, Node*> nodes; // ��� ��� ( {node ID, node pointer) }
	Rtn* rtn; // ����� ��ü
	Kmr* kmr; // Ű ������ ��ü

	struct _tagCntrlMsg* cntlMsg; // ���� �޽���	

	// ���� Ž�� ����
	bool atkDtctd; // ���� Ž�� ���� (��: ���� ���� ����)
	int numDtctn; // // ���� Ž�� Ƚ�� (��: ���� ���� ����)	
	int numVer; // ���� ���� Ƚ��

	// MAC ���� ����
	std::list<struct _tagMAC*> tmplst;// (2013.11.19) �ӽ� MAC ��� (���� �� MAC ���� �������� ���)	

};

/*

class Sec
{
public:

	void setNodes( Node nodes[]); // (2013.08.28 �߰�) 

	void setRtn (Rtn* rtn); // (2013.09.09) ����� ��ü �ּ� ����
	void setKmr( Kmr* kmr); // Ű ������ ��ü �ּ� ����

	// �̺�Ʈ ���� ó�� ����

	virtual void setMAC (struct _tagEvntMsg* msg) = 0; // MAC ����

	virtual void setMAC (struct _tagEvntMsg* msg, int clstrID) = 0; // MAC ���� (Ŭ������ ���)

	virtual void updateMAC(struct _tagEvntMsg* msg, int clstrID) = 0; // (2013.11.19) MAC ���� �Լ�

	virtual bool verMAC (struct _tagEvntMsg* msg) = 0; // MAC ���� (���� �� true, ���� �� false ��ȯ)

	virtual bool detect (struct _tagEvntMsg* msg) = 0; // ���� ���� (���� ���� �� true, �ƴϸ� false ��ȯ)

	virtual void doEventMsgProc(struct _tagEvntMsg* msg) = 0; // �̺�Ʈ ���� ó�� �Լ� (2013.09.30 �߰�)

	virtual int getEventMsgLen(struct _tagEvntMsg* msg) = 0; // ���� ���� ��ȯ �Լ� (2013.09.30 �߰�)

	virtual int getCntrlMsgLen(struct _tagCntrlMsg* cm) = 0; // ���� �޽��� ���� ��ȯ �Լ� (2014.01.10 �߰�)

	// ���� �޽��� ���� (2013.08.21 �߰�)
	virtual void genCntrlMsg(enum ctrlMsgType type, Node* cNode)=0; // ���� �޽��� ����

	virtual void testCntrlMsg(); // ���� �޽��� �׽�Ʈ (2013.09.05 �߰�)

	CntrlMsg* getCntrlMsg(); // ���� �޽��� ȹ�� (2013.09.05 �߰�)

	void setCntrlMsg(int clstrID); // ���� �޽��� �� cluster ID ���� (2013.09.16) �߰�

	// ���� �޽��� ó�� ���� (2013.08.16 �߰�)
	virtual void doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) = 0; // ���� �޽��� ���ſ� ���� ó��

	
	int getNumDtctn(); // ���� Ž�� Ƚ�� ��ȯ
	int getNumVer(); // ���� ���� Ƚ�� ��ȯ

	virtual void testControl() = 0;// �ӽ� �׽�Ʈ �Լ�

	Sec(void);
	~Sec(void);

public:

	// ��� ���� 
	Node* nodes; // ��� ���� �迭�� ���� �ּ�

	// BS ����
	BS* bs; // ���� ��� ��ü �ּ� (2014.03.24 �߰�)

	Rtn* rtn; // ����� ��ü
	Kmr* kmr; // Ű ������ ��ü	

	struct _tagCntrlMsg cntlMsg;// ���� �޽��� (2013.08.21)

	bool atkDtctd; // ���� Ž�� ���� (��: ���� ���� ����)
	int numDtctn; // // ���� Ž�� Ƚ�� (��: ���� ���� ����)	

	int numVer; // ���� ���� Ƚ��

	std::list<struct _tagMAC*> tmplst;// (2013.11.19) �ӽ� MAC ��� (���� �� MAC ���� �������� ���)

	/*
	IDLIST upperNodes[NUM_NODES][IHA_T+1]; //  (2013.11.22 ����) ���� ��� ��� (array of list)
	IDLIST lowerNodes[NUM_NODES][IHA_T+1]; //  (2013.11.22 ����) ���� ��� ���	
	*/

//};

#endif