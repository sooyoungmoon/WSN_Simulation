// 2014.05.20
// ������
// Rtn_DIHA.h
// DIHA�� ����� Ŭ���� ����

#ifndef _RTN_DIHA_H
#define _RTN_DIHA_H

#include "Rtn.h"
#include "params.h"
#include "typedef.h"
#include "Fuzzy.h"

class Node;

class Rtn_DIHA : public Rtn// ����� ��� Ŭ����
{
public:

	// Ŭ������ ���� ����	
	void findNbrs(); // (2013.09.09 �߰�) �̿� ��� ���� �Լ� ������	
	void setCHs(); // CH ���� �Լ�

	/* ���� ȩ ���� ��� ���� ����*/

	list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs); // // ���� �޽����� ���� ���� ��� ����

	list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode); 
	// ���� �޽��� ���� ��� ���� ���� ��� ����

	Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode); 
	// ������ �޽��� ���� ��� ���� ���� ��� ����

	list<Node*> selNext (struct _tagKeyMsg* keyMsg, BS* bs);
	//  Ű ��й� �޽��� ���� ��� ���� ���� ��� ����

	list<Node*> selNext (struct _tagKeyMsg* keyMsg, Node* cNode);
	// Ű ��й� �޽��� ���� ��� ���� ���� ��� ����

	list<Node*> selNext (struct _tagTREQ* treq, Node* cNode);
	// (2014.05.10) TREQ �޽��� ������ ���� ���� ��� ����

	list<Node*> selNext (struct _tagTRES* tres, Node* cNode);
	// (2014.05.10) TREQ �޽��� ������ ���� ���� ��� ����
	
	double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi);
	// ���� �޽��� ������ ���� �ĺ� ��� ��

	double eval (struct _tagEvntMsg* evntMsg, Node* candi); 
	// ������ �޽��� ������ ���� �ĺ� ��� ��

	double eval_fuzzy (double dist, double eCnpmtn, double numKeys); // fuzzy logic�� ���� �ĺ� ��� ��

	double eval(Node* recipent, Node* candi);
	// Ű ��й� �޽��� ������ ���� �ĺ� ��� ��

	//double eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi);
	// Ű ��й� �޽��� ������ ���� �ĺ� ��� ��

	double testConect();

	Rtn_DIHA(void);
	~Rtn_DIHA(void);

private:
	
	Fuzzy fuzzy; //  ���� ���� �߷� ��ü

};

#endif