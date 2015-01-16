
// 2014.4.15
// Atk.cpp
// 설명: 공격 프로토콜 중 위조 보고서 주입 공격 구현
// 문수영

#include "Atk.h"

void Atk::setNodes(map<int, Node*> nodes)
{
	this->nodes = nodes;
}


/* (2012.11.20) 구현 부분을 파생 클래스로 옮김
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
