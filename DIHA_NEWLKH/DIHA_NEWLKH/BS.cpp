

// 2014.05.20
// ������
// BS.cpp
// ���̽� �����̼� Ŭ���� ����

#include "BS.h"

void BS::reconstNewLKH () // NewLKH (��)����
{

}

void BS::updateGroupKey() // �׷� Ű ����
{

}

void BS::setKmr(Kmr* kmr) // Ű ������ ��ü ����
{
	this->kmr = kmr;
}

void BS::setRtn(Rtn* rtn) // ����� ��ü ����
{
	this->rtn = rtn;
}

void BS::txMsg(struct _tagCntrlMsg* cm, Node* next)
{
	//...
}

void BS::rxMsg(struct _tagCntrlMsg* cm)
{
	//...
}

void BS::txMsg(struct _tagKeyMsg* km, Node* next)
//void BS::txMsg(struct _tagKeyMsg* km)
{	
	rtn->setRecipent(km, next); // Ű �޽����� ���� ��� ����
}

void BS::addNbr(Node* nbr) // �̿� ��� �߰�
{
	nbrLst.push_back(nbr);
}

list<Node*> BS::getNbrLst() // �̿� ��� ��� ��ȯ
{
	return this->nbrLst;
}

void BS::resetNbrLst() // (2014.08.07) BS�� �̿� ��� ��� ����
{
	this->nbrLst.clear();
}

BS::BS()
{

}

BS::~BS()
{

}