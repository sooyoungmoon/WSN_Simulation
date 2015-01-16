// 2014.07.14
// Rtn.h
// routing Ŭ���� ����

#ifndef _RTN_H
#define _RTN_H

#include "Node.h"
#include "BS.h"

#include "Atk.h"
#include "Sec.h"
#include "Kmr.h"
#include "Fuzzy.h"
#include "params.h"
#include "typedef.h"
#include <stdio.h>
#include <iostream>
#include <map>

using std::list;
using std::map;

class Sec;
class BS;

class Rtn
{
public:
	// Ŭ������ ���� ����
	void setNodes(map<int, Node*> nodes); // ��� ����Ʈ �ʱ�ȭ �Լ�
	void setBS( BS* bs); // ���� ��� ��ü �ּ� ����
	virtual void findNbrs(); // �̿� ��� Ž�� �Լ�
	virtual void setCHs() = 0; // CH ���� �Լ�
	Node* getCH (int clstrID); // CH ȹ�� �Լ�
	set<int> getClstr (int clstrID); // (2014.05.21) �Է�: Ŭ������ ID (ch's ID), ���: Ŭ������ �Ҽ� ������ iD ����
	map<int, set<int>> getClusters(); // Ŭ������ ���� ��ȯ
	int getClstrID (int nodeID); // (2014.04.10) �Է�: ��� id, ���: Ŭ������ ID (= CH's ID)
	void changeCluster (int nodeID, int clstrID); // Ŭ������ ���� �Լ�
	void changeCluster (int nodeID, int clstrID, int CH_X, int CH_Y); // Ŭ������ ���� �Լ� - CH_X: �ű� CH�� X ��ǥ, CH_Y: �ű� CH�� Y ��ǥ
	void removeFromCluster (int nodeID); // (2014.07.14) Ŭ�����Ϳ��� �ش� ��� ����
	void removeCH (int CHID); //(2014.09.19) ���ŵ� ��尡 CH�� ��� �ش� Ŭ������ ���� ����

	bool hasUpdatedCH (int nodeID);// (2014.05.19) �־��� nodeID�� �����Ǵ� ����� Ŭ������ (CH' ID)�� ����Ǿ����� �˻�

	void arrangeClusters(); // (2014.08.11) Ŭ������ ��� �� ũ�Ⱑ 0�� Ŭ������ ������ ���� 

	// ���� ��� ���� ����
	//void addParent (Node* cNode, Node* prevNode); // (2013.10.11 �߰�) (�Ϲ�) �θ� ��� �߰� (�� ��尡 ���� ���� �θ� ��带 ���� �� ����)	
	
	
	///*** �θ� (�Ϲ�/CH) ��� �߰�
	void addParentNode (Node* cNode, Node* prevNode); // (2014.08.17 �߰�) (�Ϲ�) �θ� ��� �߰� (�� ��尡 ���� ���� �θ� ��带 ���� �� ����)	
	void addParentCH (Node* cNode, Node* parentCH); // (2014.08.14) �θ� CH ��� �߰�
	///***
	
	//std::list<Node*> getParents(Node* cNode); // // (2013.10.11 �߰�) �θ� ��� ����Ʈ ��ȯ
	
	///** �ִܰ�� �θ� (�Ϲ�/CH) ��� ��� ȹ��
	std::list<Node*> getParentNodes(Node* cNode); // // (2014.08.17 �߰�) �θ� ��� ����Ʈ ��ȯ
	std::list<Node*> getParentCHs(Node* cNode); // // (2014.08.14 �߰�) �θ� CH ��� ����Ʈ ��ȯ
	///**


	Node* getSPParent (Node* cNode); // (2013.09.09) �θ� ��� ��ȯ
	
	///*** �ִ� ��� �θ� (�Ϲ�/CH) ��� ��� ȹ��
	Node* getSPParentNode (Node* cNode); // (2014.08.17 �߰�) �θ� ��� ��ȯ
	Node* getSPParentCH (Node* cNode); // (2014.08.17 �߰�) �θ� CH ��� ��ȯ
	///***


	void setKmr( Kmr* kmr); // (2012.11.10 �߰�) kmr ��ü ������� ����	
	void setFuzzy (Fuzzy* fuzzy); // fuzzy ��ü ������� ����

	virtual list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs) = 0; // (2014.05.20) ���� �޽��� ���� ��� ���� ���� ��� ����
	virtual list<Node*> selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode) = 0; // (2013.08.06) ���� �޽��� ���� ��� ���� ���� ��� ����
	virtual Node* selNext (struct _tagEvntMsg* evntMsg, Node* cNode) = 0; // ������ �޽��� ���� ��� ���� ���� ��� ����
	virtual list<Node*> selNext (struct _tagTREQ* treq, Node* cNode) = 0; // (2014.05.10) TREQ �޽��� ���� ���� ��� ���� �Լ�
	virtual list<Node*> selNext (struct _tagTRES* tres, Node* cNode) = 0; // (2014.05.10) TRES �޽��� ���� ���� ��� ���� �Լ�	
	
	virtual double eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) = 0; // (2013.08.06) ���� �޽��� ������ ���� �ĺ� ��� ��
	virtual double eval (struct _tagEvntMsg* evntMsg, Node* candi) = 0; // ������ �޽��� ������ ���� �ĺ� ��� ��	
	virtual double eval(Node* recipent, Node* candi) = 0; // Ű ��й� �޽��� ������ ���� �ĺ� ��� ��

	// Ű ��й� ����
	virtual list<Node*> selNext (struct _tagKeyMsg* keyMsg, BS* bs) = 0;
	virtual list<Node*> selNext (struct _tagKeyMsg* keyMsg, Node* cNode) = 0;
	void setRecipent (struct _tagKeyMsg* keyMsg, Node* next); // (2014.05.12) Ű ��й� �޽����� ���� ��� ����
	void resetUpdatdClstr(); // (2014.07.15) ������ Ŭ������ ������Ʈ ���θ� �ǹ��ϴ� �÷��׸� false�� ����

	// topology ����
	void resetTopology(); // topology �� �ʱ�ȭ

	// TREQ, TRES �޽��� ó�� ����
	
	void genTREQMsg (int round); // bs���� TREQ �޽��� ����
	struct _tagTREQ* getTREQMsg(); // TREQ �޽��� ȹ��	
	void updatTREQMsg (Node* cNode); // �߰� ��忡�� TREQ �޽��� �ʵ� ������Ʈ
	void genTRESMsg (Node* cNode);
	struct _tagTRES* getTRESMsg(); // TRES �޽��� ȹ��	
	void handleTREQ(struct _tagTREQ* treq, Node* cNode); // ���ŵ� TREQ �޽��� ó��
	int getTRESMsgLen(struct _tagTRES* tres); // TRES �޽��� ���� ��ȯ 

	void resetNxtNds(); // ���� ��� ��� �ʱ�ȭ

	// sorting ����
	void quicksort( int start, int finish);
	int partition( int start, int finish);	

	// test �Լ���
	void testNbrs(); // �� ����� �̿� ��� ���� ���Ͽ� ���
	void testCHs(); // �� Ŭ������ CH ���� ���Ͽ� ���

	//void testCandis(); // �ĺ� �θ� ��� ���� �׽�Ʈ
	void testParentNodes(); // �ĺ� �θ� ��� ���� �׽�Ʈ
	void testParentCHs(); // �ĺ� CH ��� ���� �׽�Ʈ

	
	virtual double testConect()=0; // (2014.10.31) CH��κ��� BS������ ��ΰ� ����Ǿ� �ִ��� Ȯ�� - ��ȯ ��: BS�κ��� ������ CH�� ����



	Rtn(void);
	~Rtn(void);

public:
	int numEvntRcvd; // (2014.08.21) BS���� ���ŵ� �̺�Ʈ ���� ����

protected:
	map<int, Node*> nodes; // ��� ��� ( {node ID, node pointer) }

	// Ŭ������ ����
	std::map<int, int> ndsClstrMap; // (2014.04.10)  <��� ID - Ŭ������ ID > ( Ŭ������ ID = CH's ID)
	std::map<int, bool> clstrUpdated; // (2014.05.19) <��� iD - Ŭ������ ���� ����(T/F)> - ���� ����� ���Ͽ� �Ҽ� Ŭ�������� CH�� �ٲ������?
	std::map<int, std::set<int> > clusters; // (2014.04.10) <Ŭ������ ID - {��� ID} > (Ŭ�����Ϳ� ���� ������ ID ���� ����)
	
	BS* bs; // ���� ��� ��ü �ּ�
	
	map<int, list<Node*>> nbrLsts; // �� ����� �̿� ��� ���
	//map<int, list<Node*>> parentsLsts; // �� ����� �ĺ� �θ� ��� ��� <id, parent list>- �Ϲ� ���
	
	///***
	map<int, list<Node*>> parentsNodes; // �� ����� �ĺ� �θ� ��� ��� <id, parent list>- �Ϲ� ���
	map<int, list<Node*>> parentCHs; // (2014.08.14)  �� ����� �ĺ� CH ��� ��� <id, parent list>- CH ���
	///***

	map <int, std::pair<int, double> > eval_result; // �ĺ� ��� �� ��� - <index, node iD, eval. result>
	map<int, Node*> candis; // �ĺ� ��� <��� ID, ��� �ּ�> map		

	// ���� ���� ��� ����
	list<Node*> nxtNds; // TREQ/TRES �޽���, ���� �޽���, Ű ��й� �޽��� ������ ���� ���� ���� ��� ��� 
	map<int, set<int> > next_recipent_Map; // < ���� ��� ID, {���� ��� ID} > map	

	Kmr* kmr; // Ű ������ ��ü
	Fuzzy* fuzzy; // fuzzy ��ü
	RAND gen1; // (2014.03.24) CH ������ ���� ���� ������
	unsigned int seed1; // CH ������ ���� seed	

	struct _tagTREQ* treqMsg; // TREQ �޽���
	struct _tagTRES* tresMsg; // TRES �޽���
};


/*
{
public:

	// Ŭ������ ���� ����
	void setNodes( Node nodes[]); // ��� ��ü ������ �迭 �ʱ�ȭ �Լ�
	virtual void findNbrs(); // �̿� ��� Ž�� �Լ�
	virtual void setCHs() = 0; // CH ���� �Լ�
	Node* getCH (int clstrID); // CH ȹ�� �Լ�
	
	// ���� ��� ���� ����
	void addParent (Node* cNode, Node* prevNode); // (2013.10.11 �߰�) �θ� ��� �߰� (�� ��尡 ���� ���� �θ� ��带 ���� �� ����)	
	std::list<Node*> getParents(Node* cNode); // // (2013.10.11 �߰�) �θ� ��� ����Ʈ ��ȯ
	Node* getSPParent (Node* cNode); // (2013.09.09) �θ� ��� ��ȯ
	void setKmr( Kmr* kmr); // (2012.11.10 �߰�) kmr ��ü ������� ����
	void setSec (Sec *sec); // (2013.10.17 �߰�) sec ��ü ������� ����

	//void setParent (Node* cNode, Node* prevNode); // (2013.09.02 �߰�) �θ� ��� ����
	//virtual void setCandis() = 0; // �ĺ� ��� ���� �Լ�
	//virtual void setCandis( std::list<Node*> lst, Node* node) = 0; // (2013.10.17 �߰�) 
		
	
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

public:

	// cutoff Ž�� ����
	int numEvntRcvd; // BS���� ���ŵ� �̺�Ʈ ���� ����

	// ��� ���� 
	//Node* nodes; // ��� ���� �迭�� ���� �ּ�	

	list<Node*> nodes_lst; // ��� ��ü ����Ʈ

	Node* nbrs[NUM_NODES][NBRS_PER_NODE]; // �̿� ��� ����
	int numNbrs[NUM_NODES];// (2013.08.13 �߰�) �� ����� �̿� ��� ��

	Node* clstr_hds[NUM_CHS]; // ch ����

	// �ĺ� ��� ����
	
	std::list<Node*> parents[NUM_NODES]; // (2013.10.11 ����) �θ� ��� ���� (������ ����� ��θ� �̿��ϴ� �������ݿ� ���)
	Node* candis[NUM_NODES][CANDIS_PER_NODE]; // �ĺ� �θ� ��� ����
	int numParents[NUM_NODES] ; // (2013.11.27) �θ� ��� ��
	int numCandis[NUM_NODES]; // �ĺ� ��� ��
	double eval_result[CANDIS_PER_NODE]; // ���� ����� �ĺ� �θ� ���鿡 ���� �� �Լ��� ��� �� ����

	// ���� ���� ��� ����
	Node* nxtNds[MAX_NXT_NODES]; // ���� ���� ��� (Ű ���� ��) �迭
	
	// Ű ����
	Kmr* kmr; // Ű ������ ��ü (2012.11.09 �߰�)
	Sec* sec; // ���� �������� ��ü (2013.10.17 �߰�)

	// ���� ���� ����
	RAND gen1; // (2014.03.24) CH ������ ���� ���� ������
	unsigned int seed1; // CH ������ ���� seed

	//random_gen gen1; // CH ������ ���� ���� ������	

};
*/

#endif