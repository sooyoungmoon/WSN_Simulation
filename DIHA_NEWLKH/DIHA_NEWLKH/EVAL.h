// 2014.04.10
// ������
// EVAL.h
// ����/���� ��� ���� �� Ŭ���� ����

#ifndef _EVAL_H
#define _EVAL_H

#include "Node.h"
#include "Atk.h"
#include "Sec.h"
#include "typedef.h"

class EVAL
{
public:	
	
	// ��� ���� ����
	void setNodes(map<int, Node*> nodes); // ��� ��� ����
	void setEvctdNodes(map<int, Node*> evctdNodes); // ��� ��� ����

	void setAtk(Atk* atk); // ���� ��� ��ü ����
	void setSec(Sec* sec); // ���� ��� ��ü ����

	// ���� ��
	void evalSec(); // ���ȼ� ��
	void evalEnergy();// ������ ȿ���� ��
	void evalLifetime(); // ��Ʈ��ũ ���� ��	

	EVAL();
	~EVAL();

private:

	map<int, Node*> nodes; // ��� ��� ( {node ID, node pointer) }
	map<int, Node*> evctdNodes; // ��Ʈ��ũ���� ���ŵ� ������ ��� 

	Atk* atk; // ���� ��� ��ü
	Sec* sec; // ���� ��� ��ü

	double avgEnrgCnpmtn; // ������ ��� ������ �Ҹ�

};

#endif