

// 2014.05.14
// ������
// BS.h
// ���̽� �����̼� Ŭ���� ����

#ifndef _BS_H
#define _BS_H

#include "Kmr.h"
#include "Rtn.h"
#include <list>

using std::list;

class Kmr;
class Rtn;
class Node;

class BS 
{
public:

	void reconstNewLKH (); // NewLKH (��)����
	void updateGroupKey(); // �׷� Ű ����
	void setKmr(Kmr* kmr); // Ű ������ ��ü ����
	void setRtn(Rtn* rtn); // ����� ��ü ����

	void txMsg(struct _tagCntrlMsg* cm, Node* next); // ���� �޽��� �۽� �Լ� (cm: ���� �޽���, next: ���� ���� ���)
	void rxMsg(struct _tagCntrlMsg* cm); // ���� �޽��� ���� �Լ� (cm: ���� �޽���) 

	void txMsg(struct _tagKeyMsg* km, Node* next); // Ű �޽��� �۽� �Լ� (km: key message, next: ���� ���� ���)
	
	void addNbr(Node* nbr); // �̿� ��� �߰�

	list<Node*> getNbrLst(); // �̿� ��� ��� ��ȯ
	
	void resetNbrLst(); // (2014.08.07) BS�� �̿� ��� ��� ����


	BS();
	~BS();

private:

	Kmr* kmr;
	Rtn* rtn;

	std::list<Node*> nbrLst;

};

#endif