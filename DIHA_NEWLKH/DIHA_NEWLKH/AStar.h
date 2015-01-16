

// 2014.04.22
// 문수영
// AStar.h
// A* 탐색 알고리즘 클래스 선언

#ifndef _ASTAR_H
#define _ASTAR_H

#include <list>
#include <map>
#include "SrchGrph.h"

using std::list;
using std::map;

class AStar
{
public:

	SrchNode* findSolution(); // 문제의 해 발견 (i, i-th degree)
	//std::list<SrchNode*> findSolution(); // 문제의 해 발견 (i, i-th degree)
	SrchNode* getSolution(); // 문제의 해 반환
	AStar(void);
	~AStar(void);

private:
	SrchGrph* graph; // 탐색 그래프
	//std::list<SrchNode*> solution; // 문제의 해
	SrchNode* solution; // 문제의 해
};

#endif

