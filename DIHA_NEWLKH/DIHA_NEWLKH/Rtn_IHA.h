// 2014.05.12
// ������
// Rtn.h
// routing Ŭ���� ����



/*
#include "Node.h"
#include "Atk.h"
#include "Sec.h"
*/

#ifndef _RTN_IHA_H
#define _RTN_IHA_H



#include "params.h"
#include "typedef.h"
#include "Rtn.h"
#include <stdio.h>

class Rtn_IHA : public Rtn
{
public:
	// Ŭ������ ���� ����	
	void findNbrs(); // (2013.09.09 �߰�) �̿� ��� ���� �Լ� ������	
	void setCHs(); // CH ���� �Լ�

	/* ���� ȩ ���� ��� ���� ���� */

	list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs);

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

	double eval(Node* recipent, Node* candi);
	// Ű ��й� �޽��� ������ ���� �ĺ� ��� ��

	//double eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi);
	// Ű ��й� �޽��� ������ ���� �ĺ� ��� ��

	
	double testConect();

	Rtn_IHA(void);
	~Rtn_IHA(void);

protected:

};

/*
public:
	// Ŭ������ ���� ����	

	void findNbrs(); // (2013.09.09 �߰�) �̿� ��� ���� �Լ� ������

	
	void setCHs(); // CH ���� �Լ�
	void setCandis(); // �ĺ� ��� ���� �Լ�

	// ���� ȩ ���� ��� ���� ����
	Node** selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode); 
	// ���� �޽��� ���� ��� ���� ���� ��� ����

	Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode); 
	// ������ �޽��� ���� ��� ���� ���� ��� ����
	
	double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi);
	// ���� �޽��� ������ ���� �ĺ� ��� ��

	double eval (struct _tagEvntMsg* evntMsg, Node* candi); 
	// ������ �޽��� ������ ���� �ĺ� ��� ��

	Rtn_IHA(void);
	~Rtn_IHA(void);

protected:

};
*/

/*

class Sec;

class Rtn
{
public:

	// Ŭ������ ���� ����
	void setNodes( Node nodes[]); // ��� ��ü ������ �迭 �ʱ�ȭ �Լ�
	void findNbrs(); // �̿� ��� Ž�� �Լ�
	virtual void setCHs() = 0; // CH ���� �Լ�
	Node* getCH (int clstrID); // CH ȹ�� �Լ�
	
	// ���� ��� ���� ����
	void setKmr( Kmr* kmr); // (2012.11.10 �߰�) kmr ��ü ������� ����
	virtual void setCandis() = 0; // �ĺ� ��� ���� �Լ�

	
	virtual Node** selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode) = 0; // (2013.08.06) ���� �޽��� ���� ��� ���� ���� ��� ����
	virtual Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode) = 0; // ������ �޽��� ���� ��� ���� ���� ��� ����
	

	
	virtual double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) = 0; // (2013.08.06) ���� �޽��� ������ ���� �ĺ� ��� ��
	virtual double eval (struct _tagEvntMsg* evntMsg, Node* candi) = 0; // ������ �޽��� ������ ���� �ĺ� ��� ��
	



	void resetNxtNds(); // ���� ��� ��� �ʱ�ȭ

	// sorting ����
	void quicksort(Node* candis[], int start, int finish);
	int partition(Node* candis[], int start, int finish);

	// test �Լ���
	void testNbrs(); // �� ����� �̿� ��� ���� ���Ͽ� ���
	void testCHs(); // �� Ŭ������ CH ���� ���Ͽ� ���
	void testCandis(); // �ĺ� �θ� ��� ���� �׽�Ʈ

	Rtn(void);
	~Rtn(void);

protected:


	// ��� ���� 
	Node* nodes; // ��� ���� �迭�� ���� �ּ�	

	Node* nbrs[NUM_NODES][NBRS_PER_NODE]; // �̿� ��� ����
	int numNbrs[NUM_NODES];// (2013.08.13 �߰�) �� ����� �̿� ��� ��

	Node* clstr_hds[NUM_CHS]; // ch ����

	// �ĺ� ��� ����

	Node* candis[NUM_NODES][CANDIS_PER_NODE]; // �ĺ� �θ� ��� ����
	int numCandis[NUM_NODES];
	double eval_result[CANDIS_PER_NODE]; // ���� ����� �ĺ� �θ� ���鿡 ���� �� �Լ��� ��� �� ����

	// ���� ���� ��� ����
	Node* nxtNds[MAX_NXT_NODES]; // ���� ���� ��� (Ű ���� ��) �迭
	
	// Ű ����
	Kmr* kmr; // Ű ������ ��ü (2012.11.09 �߰�)

	// ���� ���� ����
	random_gen gen1; // CH ������ ���� ���� ������
	unsigned int seed1; // CH ������ ���� seed

};
*/
#endif