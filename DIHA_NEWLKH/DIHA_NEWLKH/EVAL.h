// 2014.04.10
// 문수영
// EVAL.h
// 제안/기존 기법 성능 평가 클래스 선언

#ifndef _EVAL_H
#define _EVAL_H

#include "Node.h"
#include "Atk.h"
#include "Sec.h"
#include "typedef.h"

class EVAL
{
public:	
	
	// 멤버 변수 설정
	void setNodes(map<int, Node*> nodes); // 노드 목록 설정
	void setEvctdNodes(map<int, Node*> evctdNodes); // 노드 목록 설정

	void setAtk(Atk* atk); // 공격 기법 객체 설정
	void setSec(Sec* sec); // 보안 기법 객체 설정

	// 성능 평가
	void evalSec(); // 보안성 평가
	void evalEnergy();// 에너지 효율성 평가
	void evalLifetime(); // 네트워크 수명 평가	

	EVAL();
	~EVAL();

private:

	map<int, Node*> nodes; // 노드 목록 ( {node ID, node pointer) }
	map<int, Node*> evctdNodes; // 네트워크에서 제거된 노드들의 목록 

	Atk* atk; // 공격 기법 객체
	Sec* sec; // 보안 기법 객체

	double avgEnrgCnpmtn; // 노드들의 평균 에너지 소모량

};

#endif