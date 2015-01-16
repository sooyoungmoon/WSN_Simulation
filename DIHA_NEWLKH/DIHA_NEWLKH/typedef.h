// 2014.07.31
// ������
// typedef.h
// ���迡 �ʿ��� �ڷ��� ����

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

#include <iostream>
#include <list>
#include <map>
#include <set>
#include "Node.h"
#include "params.h"

using namespace std;

class Node;

typedef map<int, Node*> NodesMap; // <node ID, node pointer>

typedef list<int> NodeIDLst;
typedef list<Node*> NodesLst;

typedef std::set<int> IDSet; // ID ����
typedef std::list<int> IDLIST; // ID ���

// filtering key

/*
typedef struct _tagPairKey // (2013.08.19 �߰�)
{
	int nID; // ù ��° ��� ID
	int nID2; //  �� ��° ��� ID 
	int kVal; // Ű ��

} PairKey;
*/
struct _tagPairKey // (2013.08.19 �߰�)
{
	int nID; // ù ��° ��� ID
	int nID2; //  �� ��° ��� ID 
	int kVal; // Ű ��

};

typedef struct _tagPairKey PairKey;

// group key

typedef struct _tagGroupKey // (2014.05.14 ����)
{
	int oldVal; // ���� ��
	int newVal; // ���� ��

	//int kVal; // Ű ��

} GroupKey;

typedef list<GroupKey> GroupKeyLst;

typedef struct _tagKNode // key tree�� �����ϴ� k node
{
	struct _tagGroupKey* groupKey; // (2014.04.22) �׷� Ű

	struct _tagKNode* parent;
	struct _tagKNode* leftChild;
	//struct _tagKNode* rightChild;
	struct _tagKNode* leftSibling;
	struct _tagKNode* rightSibling;
	
	bool isCluster; // (2014.06.08) Ŭ�����Ϳ� �����Ǵ� k-node�� ��� true, �׷��� ������ false
	double COG_X; // (2014.06.19) Ŭ�����Ϳ� ���� ������ ���� �߽��� X ��ǥ
	double COG_Y; // (2014.06.19) Ŭ�����Ϳ� ���� ������ ���� �߽��� Y ��ǥ

	int CHID; // (2014.06.04) (Ŭ�����Ϳ� �����Ǵ� k-node�� ���) CH�� ID

	struct _tagUNode* unode; 

	int level; // key tree �������� level ����
	int index; // ���� level �������� �ε��� (�������� 0, 1, ...)

	bool updated; // (2014.05.13) Ű ���� ����
	bool updated_addition; // (2014.05.08) k-node�� ���ŵǾ�����, ����� �߰�/���ŷ� ���� ���ŵǾ������� T/F�� ǥ��
	bool updated_eviction; 

	bool created; // (2014.06.08) k-node�� �̹� ���忡 �����Ǿ��ٸ� true, �׷��� �ʴٸ� false - (Ű ���� ����)

	int numSensorNodes; // (2014.09.03) Ŭ�������� kNode�� ���, �ش� Ŭ�����Ϳ� ���� ���� ��� ��

	int outDegree; // (2014.09.04) �ش� kNode�� �ڽ� kNode�� ���� (Ŭ�����Ϳ� �����Ǵ� �׷� Ű�� ���, ������ ����)

} KNode;

typedef struct _tagUNode // key tree�� �����ϴ� u node
{
	Node* sensorNode; // u node�� �����Ǵ� ���� ��� 
	bool isCH; // CH ����
	KNode* parent;	
	
} UNode;

/*
typedef struct _tagKTreeNode // key tree node
{
	KTreeNode* parent;	
	KTreeNode* leftSibling;
	KTreeNode* rightSibling;

} KTreeNode;
*/

// MAC

typedef struct _tagMAC
{
	Node* origin; // (2013.09.02) MAC ���� ��� (MAC ���� �������� ���� �ʿ�)
	PairKey* pKey; // (2013.08.19 ����) pairwise Ű 
	int code; // ���� �ڵ�
} MAC;

typedef struct _tagTREQ
{
	int senderID;// �۽� ���
	int round; // ���� ���� (0, 1, 2, ...)

	
} TREQ; // topology request message

typedef struct _tagTRES
{
	int srcID; // �ҽ� ��� 
	std::list<Node*> nbrLst; // �̿� ��� ���

} TRES; // topology response message


typedef struct _tagProtoKeyMsg
{
	list < GroupKey> newKeys; // 		
	IDSet recipents;// ���� ������ ID ����

} ProtoKeyMsg; // Ű ���� �޽��� ������Ÿ��


typedef struct _tagKeyMsg
{
	list < GroupKey> newKeys; // 	
	//list < pair<GroupKey, GroupKey>> newKeys; // {(���� Ű, �ű� Ű)}
	//std::map<GroupKey, GroupKey> newKeys; // {(���� Ű, �ű� Ű)}
	//NodeIDLst recipents;// ���� ������ ���
	IDSet recipents;// ���� ������ ID ����

	int CHID; // �Ҽ� Ŭ�������� CH�� ID

	IDSet upAssocNds; // ���� ���� ��� ���
	IDSet downAssocNds; // ���� ���� ��� ���

	//NodeIDLst upAssocNds; // ���� ���� ��� ���
	//NodeIDLst downAssocNds; // ���� ���� ��� ���

} KeyMsg; // Ű ���� �޽���

// EVENT REPORT
typedef struct _tagEvntMsg
{
	int x;
	int y;
	int val;
	int hop; // hop count
	Node* CoS;	
	Node* sender; // �۽� ��� (�� ȩ���� ����)
	Node* receiver; // ���� ��� ( " )	

	std::list<int> prevNodes; // (2014.08.11) ���� ������ ���

	std::list<MAC*> macs; // (2013.08.19) ���� �� MAC ���

	int numVer; // (2013.10.29) ���� �� MAC ���� Ƚ��

	bool falseReport; // ���� ���� ���� (T: ���� ����, F: ���� ����)

	bool detected; // ���� ���� Ž�� ���� (T: Ž����, F: �� Ž����)

} EvntMsg;

// CONTROL MESSAGE

enum ctrlMsgType {BS_HELLO =1, CLSTR_ACK=2, MAC_REQ=3, MAC_RES=4 }; // ���� �޽��� Ÿ�� ( BS_HELLO = 1, CLSTR_ACK = 2, MAC_REQ=3, MAC_RES=4}

typedef struct _tagCntrlMsg // control �޽��� (BS HELLO, CLUSTER ACK �޽���)
{
	ctrlMsgType cmType; // (2013.08.13 �߰�) ���� �޽��� Ÿ�� ����
	Node* sender;
	Node* receiver;

	int clstrID; // (2013.09.16�߰�) Ŭ������ ID

	//map<int, set<int>> IDSets; // <hop count, {upper/lower node ID}>

	IDSet IDSets[INTERLEAVING_T+2];// ������ ID ��ϵ�� �̷���� ��� 

	//IDSet IDSets[INTERLEAVING_T+1];// ������ ID ��ϵ�� �̷���� ��� (t+1���� ��)

	int nextNodeID; // ���� ����� ID - MAC_REQ �޽����� ���
	MAC mac; // ���� ��带 ���� MAC - MAC_RES �޽����� ���

} CntrlMsg;

// ���� �˰��� ����

typedef pair<int, int> Gene; // ������ �ڷ��� ����- first: �ش� ����� Ű Ʈ�� �� level, 
								// second: 1) k-node�� ���, ���� level �� �ε���, 2) u-node�� ���, ��� ID


typedef map<int, Gene> Chromosome; // ����ü �ڷ��� ���� { (i, i-th gene) }

typedef map<int, char> GeneType; // �������� ���� ( 'G': group, 'N': node)

#endif 