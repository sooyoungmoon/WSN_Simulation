
// 2014.4.15
// Atk.cpp
// ����: ���� �������� �� ���� ���� ���� ���� ����
// ������

#include "Atk.h"

void Atk::setNodes(map<int, Node*> nodes)
{
	this->nodes = nodes;
}


/* (2012.11.20) ���� �κ��� �Ļ� Ŭ������ �ű�
void Atk::forgeEvntMsg(EvntMsg* msg)
{
}
*/
/*
void Atk::initCompNodes(Node nodes[])
{
}
*/
int Atk::getNumAtk()
{
	return numAtk;
}


Atk::Atk()
{
	seed1 = 1;

	gen1.setSeed(seed1);
	//gen1.randomize( &seed1);

	numAtk = 0;
}

Atk::~Atk()
{
}
