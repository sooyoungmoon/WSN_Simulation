// 2014.04.15
// 문수영
// Atk.h
// Attack 클래스 선언

#ifndef _ATK_H
#define _ATK_H

#include "Node.h"
#include "Atk.h"
#include "Sec.h"
#include "typedef.h"
#include "RAND.h"

class Node;

class Atk
{
public:

	void setNodes(map<int, Node*> nodes); // 노드 목록 설정

	virtual void forgeEvntMsg(struct _tagEvntMsg* msg) = 0;

	virtual void initCompNodes (Node nodes[]) = 0;

	int getNumAtk(); // 공격 횟수 반환

	// test 함수

	virtual void testCompNodes (Node nodes[]) = 0;

	Atk(void);
	~Atk(void);

protected:

	map<int, Node*> nodes; // 노드 목록 ( {node ID, node pointer) }

	int numAtk; // 공격 횟수

	RAND gen1; // 훼손 노드 결정을 위한 난수 생성기

	unsigned int seed1; // seed

	/*
	virtual void init(Node nodes[]); // 공격 초기화
	virtual void attack(EvntMsg* rep, Node nodes[], Sec* sec); // 공격 발생

	random_gen gen1; // 훼손 노드 결정을 위한 난수 생성기

	unsigned int seed1; // seed

	int numAtk; // 공격 1 발생 횟수 (예: 허위 보고서 공격)
	int numAtk2; // 공격 2 발생 횟수 (예: 웜홀 공격)
	*/

	
};

#endif