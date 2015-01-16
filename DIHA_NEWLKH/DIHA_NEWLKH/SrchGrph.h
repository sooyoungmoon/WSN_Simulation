
// SrchGrph.h
// 2014.04.18
// ������
// A* Ž���� ���� Ž�� �׷��� Ŭ���� ����

#ifndef SRCHGRPH_H
#define SRCHGRPH_H

#include <iostream>
#include <list>
#include "SrchNode.h"

class SrchGrph
{
public:

	// ��� ���� �� ���� �� ��ȯ
	void setRoot (SrchNode* root);
	SrchNode* getRoot ();

	// Ž��


	SrchNode* doSearch(); // ������ �� (���� ����) ��ȯ
	//std::list<SrchNode*> doSearch(); // // �ʱ� ���¿��� ������ ��带 ���� Ž�� ����
	std::list<SrchNode*> getPath(SrchNode* n); // (2014.04.18) (����: ������ �ؿ� �ش��ϴ� ��� ȹ��) �־��� ���·κ��� �ʱ� ���±����� ��� ��ȯ
	//void doSearch(); // root ��忡�� ������ ��带 ���� Ž�� ����

	// Ž���� �ʿ��� ���� ����
	void expand(SrchNode* n); // expand the given node
	void updateCost (SrchNode* newParent, SrchNode* current); // update the cost of current node and (possibly) change its parent node 
	void updateCostRec (SrchNode* newParent, SrchNode* n); // (Recuresive version for its descendants) update the cost of current node 
													// and (possibly) change its parent node 
	bool isInOpenLst (SrchNode* s); // ��� s�� open list�� �����ϴ� �� ���� ��ȯ
	bool isInClsdLst (SrchNode* s); // ��� s�� closed list�� �����ϴ� �� ���� ��ȯ
	void sortOpenLst(); // OPEN ����� ������ f ���� �������� �������� ����

	// �׽�Ʈ �Լ�
	void printPath(SrchNode* n); // ���� ��忡�� root �������� ��θ� ���
	void prntOpenLst(); // open list ������ ȭ�鿡 ���
	void prntClsdLst(); // close list ������ ȭ�鿡 ���

	SrchGrph();
	~SrchGrph();

private:
	
	SrchNode* solution; // ���� ���� (������ ��)
	//std::list<SrchNode*> solutionPath; // �ʱ� ���¿��� ���� ���±����� ���
	SrchNode* root;
	list<SrchNode*> openList; // open node list (nodes not yet expanded)
	list<SrchNode*> closedList; // closed node list (nodes already expanded) 	
};

#endif