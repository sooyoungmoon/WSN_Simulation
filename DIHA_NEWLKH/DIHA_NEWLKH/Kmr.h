

// 2014.07.31
// ������
// Kmr.h
// Ű �Ŵ��� Ŭ���� ����

#ifndef _KMR_H
#define _KMR_H

#include "Node.h"
#include "params.h"
#include "typedef.h"
#include "Rtn.h"
#include "Sec.h"
#include "RAND.h"
#include <iostream>

#include "AStar.h"
#include "GA.h"

class Rtn;
class Sec;

class AStar;
class GA;

class Kmr // key manager
{
public:

	// ��� ���� �� ����
	void setNodes(map<int, Node*> nodes); // ��� ����Ʈ �ʱ�ȭ �Լ�
	void setRtn (Rtn* rtn); // rtn ��ü �ּ� ����
	void setSec (Sec* sec); // sec ��ü �ּ� ����	

	// �׷� Ű ��й� ���� �Լ�  (2014.05.08)		

	//virtual void updateKey()=0; // Ű Ʈ�� ���� �׷� Ű ����
	virtual void updateKey(struct _tagKNode* kNode)=0; // Ű Ʈ�� ���� �׷� Ű ����
	void resetUpdatedKeys(); // (2014.07.31) key tree�� �� k-node ���� ���� (���� ���� ����) �� �ʱ�ȭ 
	void resetUpdatedKeys(struct _tagKNode* kNode); // (2014.07.31) key tree�� �� k-node ���� ���� �� �ʱ�ȭ - ��� ȣ��
	void printReceivedNodes(); // (2014.08.07) Ű ��й� �޽��� ���� ��� ���� ���
	void resetReceivedNodes(); // (2014.08.07) Ű ��й� �޽��� ���� ��� ���� ����

	virtual void genKeyMsg()=0; // Ű ��й� �޽��� ����
	//void testKeyMsg(); // Ű ��й� �޽��� �׽�Ʈ

	bool isSubset (set<int> a, set<int> b); // (2014.07.10) IDSet a�� IDSet b�� �κ������̸� true, �׷��� ������ false ��ȯ

	virtual bool handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode) = 0; // Ű ��й� �޽��� ó�� �Լ� (��� ���� ��: true ��ȯ, ���� �Ϸ� ��: false ��ȯ)
	virtual void testKeyMsg()=0; // Ű ��й� �޽��� �׽�Ʈ
	virtual int getKeyMsgLen(struct _tagKeyMsg* keyMsg) = 0; // Ű ��й� �޽��� ���� ��ȯ
	
	void initKeyMsg( struct _tagKeyMsg* kMsg); // Ű ��й� �޽��� �ʱ�ȭ
	map<int, struct _tagKeyMsg*> getKeyMsgs( ); // Ű ��й� �޽��� ȹ�� 
	void delKeyMsg(); // Ű ��й� �޽��� ����
	void delRecvdKMsg(Node* cNode); // (2014.05.15)���ŵ� Ű ��й� �޽��� ����	
	void storeKMsg(struct _tagKeyMsg* keyMsg, Node* cNode); // (2014.05.15) ���ŵ� Ű ��й� �޽��� ����
	void restoreKMsg(struct _tagKeyMsg* keyMsg, Node* cNode); // (2014.05.15) ���ŵ� Ű ��й� �޽��� ����		
	
	list<struct _tagGroupKey> getKeySet(list<Node*> nodes); // �־��� ���(��)�� �׷� Ű ��� ��ȯ
	list<Node*> getUserSet(list<struct _tagGroupKey> keys); // �־��� Ű(��)�� ��� ��� ��ȯ
	set<int> getUserSet( struct _tagKNode* kNode); // (2014.07.08) �־��� kNode�� Ű�� ������ ��� ��� ��ȯ
	
	// key tree ����

	virtual int cnstrctKTree() = 0; // key tree ���� - Ű Ʈ���� ���� (h) ��ȯ
	virtual int cnstrctKTree(struct _tagKNode* root, set<int> nodes) = 0; // (2014.06.08)  key tree �籸�� - Ű Ʈ���� ���� ��ȯ 
	virtual int updateKTree(set<int> addedNodes, set<int> deletedNodes) = 0; // (2014.05.08) key tree ���� - ����� �߰�/���� �ݿ�  - Ű Ʈ���� ���� (h) ��ȯ
	virtual void dstrctKTree() = 0; // key tree ����
	
	int renumberKNodes(); // (2014.06.12) Ű Ʈ�� ���� ��� knode���� level, index �缳�� - ���: key tree�� ���� (height)
	bool renumberKNodes(int level); // (2014.06.12) Ư�� level�� ���� kNode���� level�� index �缳�� - ���: ���� level�� ��尡 �����ϴ� �� ����		

	set<int> getNdsInGrp(struct _tagKNode* root); // (2014.06.08) �־��� �׷쿡 ���Ե� ���� ��� ID ���� ��ȯ
	int getNumNdsInGrp(struct _tagKNode* root); // (2014.05.30) �־��� �׷쿡 ���Ե� ���� ��� �� ��ȯ
	int getNumClstrsInGrp(struct _tagKNode* root); // (2014.05.30) �־��� �׷쿡 ���Ե� Ŭ������ �� ��ȯ

	void initKNode (struct _tagKNode* kNode); // (2014.04.24) k-node �ʱ�ȭ
	void initUNode (struct _tagUNode* uNode); // (2014.06.02) u-node �ʱ�ȭ
	
	void printKTree(); // (2014.04.22) key tree ���
	void printKTree(struct _tagKNode* kNode); // (2014.04.22) key tree ���
	void printKTree(ofstream& out, struct _tagKNode* kNode); // (2014.08.31) ���Ͽ� key tree ���

	void addKTreeNode( struct _tagKNode* joinPoint, struct _tagKNode* newNode);  // key tree node �߰�
	void deleteKTreeNode( struct _tagKNode* leavePoint, struct _tagKNode* delNode);  // key tree node ����

	int getKTreeHeight(); // key tree height ��ȯ
	int getNumNodesAtLevel (int level); // Ư�� level�� ��� �� ��ȯ

	bool isCluster (struct _tagKNode* kNode); // �־��� k-node�� Ŭ�����Ϳ� �����Ǵ� �� Ȯ�� - True: Ŭ������, False: Ŭ������ �ƴ�


	// ������ �׷� ���� ����

	virtual void cnstrctLBHG()=0; // �ʱ� ������ �׷� ����
	virtual void cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp) = 0; // (2014.06.13) �־��� �� ����ȭ ������ ���� �ش� �׷� ����
	virtual void updateLBHG() = 0; // // (2014.06.13) ������ �׷� ���� (��� �� ����ȭ ������ ���� �ش� �׷� ����)
	set<struct _tagKNode*> getKNodesAtLevel(int level);// (2014.06.17) Ư�� level�� ���� kNOde ��� ��ȯ
	struct _tagKNode* getKNode( int level, int index); // (2014.04.28) key tree�� �־��� level�� index�� �����Ǵ� knode ��ȯ
	struct _tagUNode* getUNode (struct _tagKNode* clstrKey, int snID); // (2014.05.19) Ŭ�����Ϳ� �����Ǵ� kNode�� ���� ��� ID�κ��� �ش� uNode �ּ� ��ȯ	
	int getNumKNodes(struct _tagKNode* root); // (2014.06.13) (����) Ű Ʈ���� ���� k-node�� ���� ��ȯ 

	// ���͸� Ű ����
	void addPairKey (int nID, int nID2); // (2013.08.29) �� ����� iD�� �Է����� pairwise key�� �߰� // pair key �߰�	
	struct _tagPairKey* getPairKey (int nID1, int nID2);  // (2013.08.19) �� ��尡 �����ϴ� pairwise key ��� �� ��ȯ // pair key ��ȯ

	// pair key test
	void testPairKey(); // (2014.08.19) ���� ��� �������� ���� pair key ������ ���Ͽ� ���

	set<struct _tagPairKey*> getPKSet (int nID); // (2014.08.19) �־��� ����� pair key set ��ȯ

	// �̺�Ʈ ���� ���� ����

	int getNumPairKeys(Node* candi); // �ĺ� ����� pairwirse key ���� ��ȯ
 
	Kmr();
	~Kmr();

protected:
		
	map<int, Node*> nodes; // ��� ��� ( {node ID, node pointer) }
	map<int, Node*> CHs; // CH ���  ( {cluster ID, CH pointer) }	
	Rtn* rtn; // ����� ��ü
	Sec* sec; // ���� ��ü

	// Ű ���� ����
	map<int, set<struct _tagPairKey*> > pKeys; // (2014.05.23) <node ID, {pair key*} > ����� pairwise key ��ü �ּ�

	// Ű ��й� ����
	map<int, struct _tagKeyMsg*> keyMsgs; // Ű 


	set <pair <struct _tagGroupKey*, set<int>> > keyRecipentSet; // <���� Ű - {���� ��� ID}>  ����

	set<int> receivedNodes; // (2014.08.07)  {Ű ��й� �޽��� ���� ��� ID}

	// Ű Ʈ�� ����
	struct _tagKNode* root;// Ű Ʈ���� root
	int kTreeHeight; // Ű Ʈ���� ����
	map<int, int> numNodesAtLevel; // �� level�� ��� �� ���� (key tree ���� �� ����)
	
	set<struct _tagKNode*> cKNodes; // (2014.06.10) Ŭ�����Ϳ� �����Ǵ� k-node���� ����
	map<struct _tagKNode*, set<int> > restruct_pts; // (2014.06.08)  <�籸��ȭ ����, �Ҽ� ������ id ����> - Ű Ʈ�� �籸��ȭ ������ �ش� �׷� ���� ���� ��� ID ����
	list<struct _tagKNode*> cur_level_knodes; // (2014.04.18) ���� ���� key tree�� ���� level�� �����ϴ� knode���� ���

	

	// �˰���
	AStar* astar;
	GA* ga;	
	RAND gen1; // // Ű ������ ���� ���� ������	
};

#endif