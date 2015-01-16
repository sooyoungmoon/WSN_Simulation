

// 2014.04.21
// ������
// AStar.cpp
// A* Ž�� �˰��� Ŭ���� ����

#include "AStar.h"
#include "SrchGrph.h"
#include "SrchNode_KTS.h"

SrchNode* AStar::findSolution()
{
	graph = new SrchGrph(); // �׷��� ��ü ����	


	// Ž�� Ʈ���� root ��� ����, ����
	SrchNode* root = new SrchNode_KTS(); 
	root->setH();
	root->setF();
	graph->setRoot(root);

	solution = graph->doSearch(); // Ž�� ���
	
	delete graph; // �׷��� ��ü�� �Ҹ��ڿ��� Ž�� Ʈ�� ���� ���鿡 ���� �޸� ����	
	return solution; 
}

SrchNode* AStar::getSolution() // (2014.09.02) Ű Ʈ���� �� ��ȯ
{
	return solution;
} 

/*
std::list<SrchNode*> AStar::findSolution()
{
	graph = new SrchGrph();

	SrchNode* root = new SrchNode_KTS();
	root->setH();
	root->setF();

	graph->setRoot(root);

	solution = graph->doSearch();

	delete root;
	delete graph;
	return solution; 
}
*/

AStar::AStar(void)
{

}


AStar::~AStar(void)
{

}
