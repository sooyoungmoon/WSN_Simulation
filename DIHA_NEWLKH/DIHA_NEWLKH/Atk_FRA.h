// 2012.10.15
// 문수영
// Atk_FRA.h
// false report attack 클래스 선언

#ifndef _ATK_FRA_H
#define _ATK_FRA_H

#include "Node.h"
#include "Atk.h"
#include "Sec.h"
#include "typedef.h"

class Node;

class Atk_FRA : public Atk
{
public:	

	void forgeEvntMsg(EvntMsg* msg);

	void initCompNodes (Node nodes[]);

	void testCompNodes (Node nodes[]);

	Atk_FRA(void);
	~Atk_FRA(void);
};

#endif