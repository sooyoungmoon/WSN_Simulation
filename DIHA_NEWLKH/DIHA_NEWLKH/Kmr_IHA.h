

// 2014.06.13
// ������
// Kmr_IHA.h
// IHA�� Ű �Ŵ��� Ŭ���� ����

#ifndef _KMR_IHA_H
#define _KMR_IHA_H

#include "Kmr.h"

class Kmr_IHA : public Kmr // key manager
{
public:
	
	// �׷� Ű ��й� ���� �Լ�
	void updateKey(struct _tagKNode* kNode); // �׷� Ű ���� �Լ�
	//void resetUpdatedKeys(struct _tagKNode* kNode); // (2014.07.31) key tree�� �� k-node ���� ���� �� �ʱ�ȭ 

	void genKeyMsg(); // Ű ��й� �޽��� ����
	void testKeyMsg(); // Ű ��й� �޽��� �׽�Ʈ
	int getKeyMsgLen(struct _tagKeyMsg* keyMsg); // Ű �޽��� ���� ��ȯ
	bool handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode);

	//void genKeyMsg();  // Ű ��й� �޽��� ����	
	//KeyMsg* getKeyMsg(); // Ű ��й� �޽��� ��ȯ

	// ���͸� Ű ����
	//void addPairKey (int nID, int nID2);
	// (2013.08.29) �� ����� iD�� �Է����� pairwise key�� �߰� // pair key �߰�

	// key tree ����
	int cnstrctKTree(); // key tree ����
	int cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes); // (2014.06.05) key tree (��)���� - Ű Ʈ���� ���� ��ȯ
	int cnstrctKTree(struct _tagKNode* root, set<int> nodes); // (2014.06.08) key tree (��)���� - Ű Ʈ���� ���� ��ȯ

	int updateKTree(IDSet addedNodes, IDSet deletedNodes); // (2014.05.29) Ű Ʈ�� ���� �Լ� - �Է�: �߰� ��� ��� (addedNodes), ���� ��� ��� (deletedNodes)
	void dstrctKTree(); //  key tree ����

	// ������ ������ �׷� ����
	void cnstrctLBHG(); // ������ �׷� ����
	void cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp); //  // (2014.06.13) ������ �׷� ����
	void updateLBHG(); // // (2014.06.13) ������ �׷� ���� (��� �� ����ȭ ������ ���� �ش� �׷� ����)

	Kmr_IHA();
	~Kmr_IHA();

};
/*
	void addPairKey (int nID, int nID2);// (2013.08.29) �� ����� iD�� �Է����� pairwise key�� �߰�
	void setPairKeys(); // (2013.08.29) pairwise key ���� 
	//int getNumPairwiseKeys(Node* candi) const const; // �ĺ� ��尡 ���� pairwise key ���� ��ȯ
	//PairKey* getPairKey (int nID1, int nID2); // (2013.08.19) �� ��尡 �����ϴ� pairwise key ��� �� ��ȯ

	// test �Լ�
	void testKeyInfo(); // Ű ���� ���

	Kmr_IHA(void);
	~Kmr_IHA(void);
};
*/


#endif