// 2014.07.31
// ������
// Kmr.h
// NEW_LKH ��� Ű ���� Ŭ���� ����

#ifndef _KMR_Kmr_NEWLKH_H
#define _KMR_Kmr_NEWLKH_H

#include "Kmr.h"
#include "Sec_IHA.h"

class Kmr_NEWLKH : public Kmr// key manager
{
public:

	// �׷� Ű ��й� ���� �Լ�
	void updateKey(struct _tagKNode* kNode); // �׷� Ű ���� �Լ�
	//void resetUpdatedKeys(struct _tagKNode* kNode); // (2014.07.31) key tree�� �� k-node ���� ���� �� �ʱ�ȭ 

	void genKeyMsg(); // Ű ��й� �޽��� ����
	void testKeyMsg(); // Ű ��й� �޽��� �׽�Ʈ
	int getKeyMsgLen(struct _tagKeyMsg* keyMsg); // Ű �޽��� ���� ��ȯ
	bool handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode); // Ű �޽��� ó�� �Լ�

	//void genKeyMsg();  // Ű ��й� �޽��� ����	
	//KeyMsg* getKeyMsg(); // Ű ��й� �޽��� ��ȯ

	// ���͸� Ű ����
	//void addPairKey (int nID, int nID2);
	// (2013.08.29) �� ����� iD�� �Է����� pairwise key�� �߰� // pair key �߰�

	// key tree ����
	int cnstrctKTree(); // key tree ����
	int cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes); // (2014.06.05) key tree (��)���� - Ű Ʈ���� ���� ��ȯ
	int cnstrctKTree(struct _tagKNode* root, set<int> nodes); // (2014.06.08) key tree (��)���� - Ű Ʈ���� ���� ��ȯ


public:
	int updateKTree(IDSet addedNodes, IDSet deletedNodes); // (2014.05.29) Ű Ʈ�� ���� �Լ� - �Է�: �߰� ��� ��� (addedNodes), ���� ��� ��� (deletedNodes)
public:
	void dstrctKTree(); //  key tree ����
	void dstrctKTree(struct _tagKNode*); // key tree ���� (��� �Լ�)

	// ������ ������ �׷� ����
	void cnstrctLBHG(); // ������ �׷� ����
	void cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp);  // (2014.06.13) ������ �׷� ����
	void updateLBHG(); // // (2014.06.13) ������ �׷� ���� (��� �� ����ȭ ������ ���� �ش� �׷� ����)

	void updateClstr(); // (2014.10.11) �� ��带 ���� Ŭ�����Ϳ� �߰�, �� Ŭ�������� �����߽� & CH �絵��, ������ Ŭ������ ����

	//void updateClstr (Chromosome solution); // (2014.05.19) GA�κ��� ����� �ظ� ������� Ŭ������ ���� ����

	//void updateClstr (Chromosome solution, list<int> isolatedNodes); // (2014.09.03) GA�κ��� ����� �ظ� ������� Ŭ������ ���� ����, �� ��带 ���� Ŭ�����Ϳ� �߰�

	void updateAssoc ( ); // (2014.05.27) ���ŵ� Ŭ������ ������ ������� ���� ��� ���� ����

	void expandLevel(int level, int degree); // (2014.04.24) key tree�� ���� level Ȯ��
	void expandLevel(int degree); // (2014.04.18) key tree�� ���� level Ȯ�� (���� level�� �� ��帶�� degree ��ŭ�� �ڽ� ��� ����)

	void printIsolatedNodes(); // (2014.09.02) ���� ��� ��� ��� {node ID}

	Kmr_NEWLKH(void);
	~Kmr_NEWLKH(void);

private:

	//list<struct _tagKNode*> cur_level_knodes; // (2014.04.18) ���� ���� key tree�� ���� level�� �����ϴ� knode���� ���
	list<int> isolatedNodes; // (2014.09.02) GA �˰��� ���� ��� Ŭ�����Ϳ� ������ ���� ����
};

#endif