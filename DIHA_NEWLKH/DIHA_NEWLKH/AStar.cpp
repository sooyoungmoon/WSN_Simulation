

// 2014.04.21
// 문수영
// AStar.cpp
// A* 탐색 알고리즘 클래스 구현

#include "AStar.h"
#include "SrchGrph.h"
#include "SrchNode_KTS.h"

SrchNode* AStar::findSolution()
{
	graph = new SrchGrph(); // 그래프 객체 생성	


	// 탐색 트리의 root 노드 생성, 설정
	SrchNode* root = new SrchNode_KTS(); 
	root->setH();
	root->setF();
	graph->setRoot(root);

	solution = graph->doSearch(); // 탐색 명령
	
	delete graph; // 그래프 객체의 소멸자에서 탐색 트리 상의 노드들에 대한 메모리 해제	
	return solution; 
}

SrchNode* AStar::getSolution() // (2014.09.02) 키 트리의 해 반환
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
