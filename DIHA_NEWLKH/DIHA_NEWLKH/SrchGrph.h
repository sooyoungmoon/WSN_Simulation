
// SrchGrph.h
// 2014.04.18
// 문수영
// A* 탐색을 위한 탐색 그래프 클래스 선언

#ifndef SRCHGRPH_H
#define SRCHGRPH_H

#include <iostream>
#include <list>
#include "SrchNode.h"

class SrchGrph
{
public:

	// 멤버 변수 값 설정 및 반환
	void setRoot (SrchNode* root);
	SrchNode* getRoot ();

	// 탐색


	SrchNode* doSearch(); // 문제의 해 (최종 상태) 반환
	//std::list<SrchNode*> doSearch(); // // 초기 상태에서 목적지 노드를 향한 탐색 시작
	std::list<SrchNode*> getPath(SrchNode* n); // (2014.04.18) (목적: 문제의 해에 해당하는 경로 획득) 주어진 상태로부터 초기 상태까지의 경로 반환
	//void doSearch(); // root 노드에서 목적지 노드를 향한 탐색 시작

	// 탐색에 필요한 내부 연산
	void expand(SrchNode* n); // expand the given node
	void updateCost (SrchNode* newParent, SrchNode* current); // update the cost of current node and (possibly) change its parent node 
	void updateCostRec (SrchNode* newParent, SrchNode* n); // (Recuresive version for its descendants) update the cost of current node 
													// and (possibly) change its parent node 
	bool isInOpenLst (SrchNode* s); // 노드 s가 open list에 존재하는 지 여부 반환
	bool isInClsdLst (SrchNode* s); // 노드 s가 closed list에 존재하는 지 여부 반환
	void sortOpenLst(); // OPEN 목록의 노드들을 f 값을 기준으로 오름차순 정렬

	// 테스트 함수
	void printPath(SrchNode* n); // 현재 노드에서 root 노드까지의 경로를 출력
	void prntOpenLst(); // open list 내용을 화면에 출력
	void prntClsdLst(); // close list 내용을 화면에 출력

	SrchGrph();
	~SrchGrph();

private:
	
	SrchNode* solution; // 최종 상태 (문제의 해)
	//std::list<SrchNode*> solutionPath; // 초기 상태에서 최종 상태까지의 경로
	SrchNode* root;
	list<SrchNode*> openList; // open node list (nodes not yet expanded)
	list<SrchNode*> closedList; // closed node list (nodes already expanded) 	
};

#endif