

// 2014.04.22
// ������
// AStar.h
// A* Ž�� �˰��� Ŭ���� ����

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

	SrchNode* findSolution(); // ������ �� �߰� (i, i-th degree)
	//std::list<SrchNode*> findSolution(); // ������ �� �߰� (i, i-th degree)
	SrchNode* getSolution(); // ������ �� ��ȯ
	AStar(void);
	~AStar(void);

private:
	SrchGrph* graph; // Ž�� �׷���
	//std::list<SrchNode*> solution; // ������ ��
	SrchNode* solution; // ������ ��
};

#endif

