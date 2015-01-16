// 2014.04.15
// ������
// Atk.h
// Attack Ŭ���� ����

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

	void setNodes(map<int, Node*> nodes); // ��� ��� ����

	virtual void forgeEvntMsg(struct _tagEvntMsg* msg) = 0;

	virtual void initCompNodes (Node nodes[]) = 0;

	int getNumAtk(); // ���� Ƚ�� ��ȯ

	// test �Լ�

	virtual void testCompNodes (Node nodes[]) = 0;

	Atk(void);
	~Atk(void);

protected:

	map<int, Node*> nodes; // ��� ��� ( {node ID, node pointer) }

	int numAtk; // ���� Ƚ��

	RAND gen1; // �Ѽ� ��� ������ ���� ���� ������

	unsigned int seed1; // seed

	/*
	virtual void init(Node nodes[]); // ���� �ʱ�ȭ
	virtual void attack(EvntMsg* rep, Node nodes[], Sec* sec); // ���� �߻�

	random_gen gen1; // �Ѽ� ��� ������ ���� ���� ������

	unsigned int seed1; // seed

	int numAtk; // ���� 1 �߻� Ƚ�� (��: ���� ���� ����)
	int numAtk2; // ���� 2 �߻� Ƚ�� (��: ��Ȧ ����)
	*/

	
};

#endif