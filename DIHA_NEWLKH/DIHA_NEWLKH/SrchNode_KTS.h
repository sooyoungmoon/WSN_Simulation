
// 2014.04.22
// 문수영
// SrchNode_KTS.h
// KEY TREE STRUCTURE 구성 문제에서 A* 탐색을 위한 탐색 노드 선언

#ifndef _SRCHNODE_KTS_H
#define _SRCHNODE_KTS_H

#include "SrchNode.h"
#include <map>

using std::map;



class SrchNode_KTS : public SrchNode
{
public:

	// 멤버 변수 설정
	static void setNumNodes(int numNodes); // (2014.04.22)
	static void setNumClusters(int numClusters);

	// void setNumNodes(int numNodes);
	// void setNumClusters(int numClusters);

	// 휴리스틱 함수
	void setH(); // 목적지 노드까지의 예상 비용 계산
	int log2(int n); // 정수 n에 대한 log (base =2) 함수 결과 출력

	// 탐색 함수
	double getLinkCost (SrchNode* parent); // 부모 노드와의 연결 비용 계산
	bool isSameState( SrchNode* n); // 다른 상태와 비교

	// 현재 상태 정보 반환 (key tree 상에서 주어진 level의 degree)
	int getDegree( int level); // (2014.04.18) 
	map <int, int> getDegreeSeq(); // 

	// 다음 상태 계산
	void calNxtStates (); // 가능한 다음 상태 목록 계산

	// 종료 조건
	bool isGoalNode(); // 목표 상태와 일치하면 true, 아니면 false 반환

	// test
	void prntStat(); // 현재 상태 출력	

	SrchNode_KTS();
	~SrchNode_KTS();

private:
	
	// 상태를 구성하는 변수
	int currentLevel; // 현재까지 구축된 KTS의 높이 (즉, 현재 탐색 노드의 수준- 0, 1, 2, ..., c )
	map <int, int> degreeSeq; // KTS 구축을 위한 각 level의 degree 열
	int numLeafs; // 현재까지 구축된 KTS의 leaf node의 수

	// 상태 구성에 필요한 변수

	static int numNodes;// 현재 센서 노드 수
	static int numClusters; // 요구되는 클러스터 수

	//int numNodes;// 현재 센서 노드 수
	//int numClusters; // 요구되는 클러스터 수

	//int numClusters; // 현재 클러스터 수

};

#endif