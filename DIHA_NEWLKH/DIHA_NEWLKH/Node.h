// 2014.05.15
// ������
// Node.h
// node Ŭ���� ����

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
	int cell; // �ʵ尡 �׸��� ������ ���� �� ��尡 ��ġ�ϴ� cell�� ��ȣ

	double distance; // sink �������� �Ÿ�
	double eCnsmptn; // ����� ������ �Ҹ�
	double remEnrgy; // ����� �ܿ� ��������
	double enrgyDiff; // ������ ��ȭ��
	bool cmprmsd; // ��尡 �Ѽյ� ��� 1, ������ ��� 0

	static double totEnrgCnsmptn; // (2013.10.31) ������ ��ü ������ �Ҹ�
	unsigned static int totNumNodes; // ������� ������ (�̹� ���ŵ� ���鵵 ����) �� ��� �� 
							// ������ ������ ����� ID

	unsigned static int numNodesEvctd; // ������� ���ŵ� ��� �� (=������ ���ŵ� ����� ID) - First created, first evicted

	Rtn* rtn; // ����� ��ü
	Sec* sec; // ���� ��ü
	Kmr* kmr; // Ű ���� ��ü

	bool newbee; // (2014.05.08) �̹� ���忡 ���� �߰��� ��� true, �׷��� �ʴٸ� false

	int round; // (2014.05.10) ���� �����ϰ� �ִ� ���� ��ȣ - topology request/response �ܰ迡�� ������- (�ʱⰪ = -1)

	struct _tagKeyMsg* receivedKMsg; // ���ŵ� Ű ��й� �޽���
	//struct _tagKeyMsg receivedKMsg; // ���ŵ� Ű ��й� �޽���
	//static std::ofstream out; // (2013.09.03) ���� ��� ��Ʈ��
	
public:
	void txMsg(struct _tagEvntMsg* evntMsg); // ������ �޽��� �۽� �Լ�
	bool rxMsg(struct _tagEvntMsg* evntMsg); // (2012.11.23 ����) ������ �޽��� ���� �Լ� (��� ������ �ʿ��� ��� true, ������ �����ؾ� �� ���� false ��ȯ)

	void txMsg(struct _tagKeyMsg* km, Node* nextNode); // Ű �޽��� �۽� �Լ�
	bool rxMsg(struct _tagKeyMsg* km); // Ű �޽��� ���� �Լ�	

	void txMsg(struct _tagCntrlMsg* cm); // (2013.08.13) ���� �޽��� �۽� �Լ�
	bool rxMsg(struct _tagCntrlMsg* cm); // (2013.08.13) ���� �޽��� ���� �Լ�	

	void txMsg(struct _tagTREQ* treq); // (2014.05.10) TREQ �۽� �Լ�
	int rxMsg(struct _tagTREQ* treq); // (2014.05.10) TREQ ���� �Լ�	

	void txMsg(struct _tagTRES* tres); // (2014.05.10) TRES �۽� �Լ�
	int rxMsg(struct _tagTRES* tres); // (2014.05.10) TRES ���� �Լ�	

	void setRtn(Rtn* rtn); // ����� ��ü ����
	void setKmr(Kmr* kmr); // Ű ���� ��ü ����
	void setSec(Sec* sec); // ���� ��ü ����

	void printNode(std::ofstream &out); // (2013.09.03 �߰�) ��� ���� ���

	bool depleted(void); // (2013.09.23)

	Node(void);
	~Node(void);
};

#endif