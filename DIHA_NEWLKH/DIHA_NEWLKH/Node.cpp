// 2014.05.14
// Node.cpp
// ������

#include "Node.h"
#include <iostream>
#include <fstream>

using namespace std;

double Node::totEnrgCnsmptn; // ���� ��� ���� ��� �� �̸� ����ϴ� �ҽ� ���Ͽ��� �̿� ���� ������ �߰��� �־�� ��

void Node::txMsg(struct _tagEvntMsg* evntMsg)
{
	(evntMsg->hop)++; // (2013.12.27 ��ġ �̵�) �޽��� ���� ȩ �� ����

	// ������ �Ҹ� ���

	// �̺�Ʈ ���� �޽��� ���� ȹ�� (���������� ������ ���� �޶���)

	int msgLen = sec->getEventMsgLen(evntMsg);

	enrgyDiff = msgLen * txEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; // (2013.10.07) ���� �۽ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; // (2013.10.07) ���� �۽ſ� ���� �ܿ� ������ ����

	if ( remEnrgy < 0)
	{
		remEnrgy = 0;
	}
	// ��� ������� ��� 	
}

bool Node::rxMsg(struct _tagEvntMsg* evntMsg)
{
	bool res = true;	
	//(evntMsg->hop)++; // (2013.12.27 �ּ�ó��) �޽��� ���� ȩ �� ����
	
	int msgLen = sec->getEventMsgLen(evntMsg); // �̺�Ʈ ���� �޽��� ���� ȹ��

	enrgyDiff = msgLen * rxEnrg; // ���� ������ �Ҹ�
	
	eCnsmptn += enrgyDiff; // (2013.10.07) ���� ���ſ� ���� ���� ����� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; // (2013.10.07) ���� ���ſ� ���� ���� ����� �ܿ� ������ ����

	if ( remEnrgy < 0)
	{
		remEnrgy = 0;
	}	

	// (2012.11.23) ���� ���� �Լ� ȣ��
	
	if ( sec->detect(evntMsg) == true) // ������ ����� ��� ���� ������ ���� false ��ȯ
	{
		res = false;
	}
	if ( distance < TRANS_RANGE) // (2012.11.24) ���� ��尡 BS���� ���� ���� �̳��̸� �� �̻� ������ �ʿ� ����
	{
		res = false; // (2013.10.08) �ּ�ó��
	}
	// ���� �������ݿ� ���� �̺�Ʈ ������ ���� ó�� ���� ����
	sec->doEventMsgProc(evntMsg); // (2013.09.30)

	return res;
}

void Node::txMsg(struct _tagKeyMsg* km, Node* nextNode) // Ű �޽��� �۽� �Լ�
{
	// ������ ����� Ű ��й� �޽��� ����


	// ������ �Ҹ� ���
	int msgLen = kmr->getKeyMsgLen(km);
	

	enrgyDiff = msgLen * txEnrg; // ���� ������ �Ҹ�
	eCnsmptn += enrgyDiff; // (2013.10.07) ���� �۽ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; // (2013.10.07) ���� �۽ſ� ���� �ܿ� ������ ����

	if ( remEnrgy < 0)
	{
		remEnrgy = 0;
	}

	rtn->setRecipent(km, nextNode); // ���� ��� ����

}


bool Node::rxMsg(struct _tagKeyMsg* km) // Ű �޽��� ���� �Լ�	
{
	bool res = true;	

	// ������ �Ҹ� ���
	int msgLen = kmr->getKeyMsgLen(km);
	
	enrgyDiff = msgLen * rxEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; // (2013.10.07) ���� ���ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; // (2013.10.07) ���� ���ſ� ���� �ܿ� ������ ����


	// Ű �޽��� ó�� �Լ� ȣ��

	res = kmr->handleKeyMsg(km, this);

	return res; // Ű ��й� �޽��� ó�� ����� ��ȯ (true: ��� ����, false: ���� �Ϸ�)
}

void Node::txMsg(struct _tagCntrlMsg* cm)
{
	// ������ �Ҹ� ���
	//this->eCnsmptn += CNTL_MSG_LEN * txEnrg; 

	/* (2014.01.10 ����) ���� ��� ���� �ܰ迡���� ������� ��� */

	int msgLen = sec->getCntrlMsgLen(cm);

	enrgyDiff = msgLen * txEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; // (2013.10.07) ���� �޽��� �۽ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; // (2013.10.07) ���� �޽��� �۽ſ� ���� �ܿ� ������ ����

	/* (2014.01.10 �����Ϸ�) */

}

bool Node::rxMsg(struct _tagCntrlMsg* cm)
{
	bool res = true;

	// ������ �Ҹ� ���
	//this->eCnsmptn += CNTL_MSG_LEN * txEnrg; 
	
	/* (2014.01.10 ����) ���� ��� ���� �ܰ迡���� ������� ��� */

	int msgLen = sec->getCntrlMsgLen(cm);

	enrgyDiff = msgLen * rxEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; // (2013.10.07) ���� �޽��� ���ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; // (2013.10.07) ���� �޽��� ���ſ� ���� �ܿ� ������ ����

	/* (2014.01.10 �����Ϸ�) */

	sec->doCntlOp(cm, this);// ���� �޽��� ���ſ� ���� ó�� �Լ� ȣ�� (�θ� ��� ���� ����)


	return res;
}

void Node::txMsg(struct _tagTREQ* treq)
{
	// ������ �Ҹ� ���

	int msgLen = TREQ_MSG_LEN; // TREQ �޽��� ����

	enrgyDiff = msgLen * txEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; //  TREQ �۽ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; //  ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; //  TREQ �۽ſ� ���� �ܿ� ������ ����

	// ������ �Ҹ� ��� �Ϸ�
}

int Node::rxMsg(struct _tagTREQ* treq) // (2014.05.10) TREQ ���� �Լ�	
{	
	// ������ �Ҹ� ���

	int msgLen = TREQ_MSG_LEN; // TREQ �޽��� ����
	enrgyDiff = msgLen * rxEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; //  TREQ ���ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; //  ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; //  TREQ ���ſ� ���� �ܿ� ������ ����
	// ������ �Ҹ� ��� �Ϸ�
	
	rtn->handleTREQ(treq, this);	// TREQ ó�� �Լ� ȣ��	

	return 0;
}

void Node::txMsg(struct _tagTRES* tres)
{
	// ������ �Ҹ� ���

	int msgLen; // TRES �޽��� ����

	msgLen = rtn->getTRESMsgLen(tres);
	enrgyDiff = msgLen * txEnrg; // ���� ������ �Ҹ�

	eCnsmptn += enrgyDiff; //  TRES �۽ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; //  ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; //  TRES �۽ſ� ���� �ܿ� ������ ����
	// ������ �Ҹ� ��� �Ϸ�
}

int Node::rxMsg(struct _tagTRES* tres) // (2014.05.10) TRES ���� �Լ�	
{	
	// ������ �Ҹ� ���

	int msgLen; // TRES �޽��� ����
	msgLen = rtn->getTRESMsgLen(tres);

	enrgyDiff = msgLen * rxEnrg; // ���� ������ �Ҹ�
	eCnsmptn += enrgyDiff; //  TRES ���ſ� ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += enrgyDiff; //  ��ü ������ �� ������ �Ҹ� ���� (���� ����)
	remEnrgy -= enrgyDiff; //  TRES ���ſ� ���� �ܿ� ������ ����
	// ������ �Ҹ� ��� �Ϸ�	
	
	return 0;
}


void Node::setRtn(Rtn* rtn)
{
	this->rtn = rtn;
}

void Node::setKmr(Kmr* kmr)
{
	this->kmr = kmr;
}

void Node::setSec(Sec* sec)
{
	this->sec = sec;
}

void Node::printNode(std::ofstream &out) // (2013.09.03) ��� ���� ��� (id, ��ǥ, �Ÿ�, ������ �Ҹ�, �Ѽ� ��� ����)
{	
	//int clstrId = id / NODES_PER_CLSTR; // cluster ID
	int clstrId = rtn->getClstrID(this->id); // (2014.04.14) ��� ID -> Ŭ������ ID ���� ��� ����

	out << "\n cluster ID: " << clstrId << ", " ;
	out << "Node ID: " << id << ","; // 
	out << " (x, y) = (" << x << ", " << y << ") ";	
	out << " dist = " << sqrt (distance) << "m,";
	out << " eCnsmptn = " << this->eCnsmptn << ",";

	Node* parent = rtn->getSPParentNode(this); // (2014.04.14)  �θ� ��� ���� ��� ����
	//Node* parent = rtn->getSPParent(this); // (2014.04.14)  �θ� ��� ���� ��� ����
	 
	//Node* parent = rtn->getParent(this);

	if (parent != NULL)
	{
		out << " parent Node= " << parent->id  << ",";
	}

	Node* parentCH = rtn->getSPParentCH(this); // (2014.04.14)  �θ� ��� ���� ��� ����
	//Node* parent = rtn->getSPParent(this); // (2014.04.14)  �θ� ��� ���� ��� ����
	 
	//Node* parent = rtn->getParent(this);

	if (parentCH != NULL)
	{
		out << " parent CH= " << parentCH->id  << ",";
	}

	out << this->cmprmsd? "compromised" : "normal" ; // �Ѽ� ��� ����
	out << endl;

}

bool Node::depleted(void)
{
	bool ret = false;

	if (this->remEnrgy < eTh)
		ret = true;

	return ret;
}

Node::Node()
{
	id = totNumNodes;
	//id = 0;
	x = 0;
	y = 0;

	distance = 0;

	this->remEnrgy = INIT_ENRGY; // (2013.09.24) ����� �ܿ� �������� �ʱ�ȭ
	eCnsmptn = 0;
	enrgyDiff = 0;
	cmprmsd = false;

	rtn = NULL;
	sec = NULL;
	kmr = NULL;	

	newbee = true; 

	round = -1;

	this->receivedKMsg = new KeyMsg();
}

Node::~Node()
{
	delete (receivedKMsg);
	//out.close(); // ���� ��� ��Ʈ�� �ݱ�
}
