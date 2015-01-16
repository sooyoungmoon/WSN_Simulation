
// 2014.04.22
// ������
// SrchNode_KTS.h
// KEY TREE STRUCTURE ���� �������� A* Ž���� ���� Ž�� ��� ����

#ifndef _SRCHNODE_KTS_H
#define _SRCHNODE_KTS_H

#include "SrchNode.h"
#include <map>

using std::map;



class SrchNode_KTS : public SrchNode
{
public:

	// ��� ���� ����
	static void setNumNodes(int numNodes); // (2014.04.22)
	static void setNumClusters(int numClusters);

	// void setNumNodes(int numNodes);
	// void setNumClusters(int numClusters);

	// �޸���ƽ �Լ�
	void setH(); // ������ �������� ���� ��� ���
	int log2(int n); // ���� n�� ���� log (base =2) �Լ� ��� ���

	// Ž�� �Լ�
	double getLinkCost (SrchNode* parent); // �θ� ������ ���� ��� ���
	bool isSameState( SrchNode* n); // �ٸ� ���¿� ��

	// ���� ���� ���� ��ȯ (key tree �󿡼� �־��� level�� degree)
	int getDegree( int level); // (2014.04.18) 
	map <int, int> getDegreeSeq(); // 

	// ���� ���� ���
	void calNxtStates (); // ������ ���� ���� ��� ���

	// ���� ����
	bool isGoalNode(); // ��ǥ ���¿� ��ġ�ϸ� true, �ƴϸ� false ��ȯ

	// test
	void prntStat(); // ���� ���� ���	

	SrchNode_KTS();
	~SrchNode_KTS();

private:
	
	// ���¸� �����ϴ� ����
	int currentLevel; // ������� ����� KTS�� ���� (��, ���� Ž�� ����� ����- 0, 1, 2, ..., c )
	map <int, int> degreeSeq; // KTS ������ ���� �� level�� degree ��
	int numLeafs; // ������� ����� KTS�� leaf node�� ��

	// ���� ������ �ʿ��� ����

	static int numNodes;// ���� ���� ��� ��
	static int numClusters; // �䱸�Ǵ� Ŭ������ ��

	//int numNodes;// ���� ���� ��� ��
	//int numClusters; // �䱸�Ǵ� Ŭ������ ��

	//int numClusters; // ���� Ŭ������ ��

};

#endif