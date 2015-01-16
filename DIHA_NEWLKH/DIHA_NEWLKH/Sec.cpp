// 2014.05.22
// ������
// Sec.cpp
// security Ŭ���� ����


#include "Sec.h"
#include <fstream>

using namespace std;

// ��� ���� �� ����

void Sec::setNodes(map<int, Node*> nodes) // ��� ��� ����
{
	this->nodes = nodes;
}


void Sec::setRtn (Rtn* rtn) //  ����� ��ü �ּ� ����
{
	this->rtn = rtn;
}


void Sec::setKmr( Kmr* kmr) // Ű ������ ��ü �ּ� ����
{
	this->kmr = kmr;
}

void Sec::initEventMsg(struct _tagEvntMsg* evntMsg, Node* CoS) // �̺�Ʈ ���� �ʱ�ȭ
{
	evntMsg->x = CoS->x;
	evntMsg->y = CoS->y;
	evntMsg->val = 0;
	evntMsg->hop = 0;
	evntMsg->CoS = CoS; // CoS ��� ����
	evntMsg->sender = CoS; // �ۼ��� ��� ����		
	evntMsg->receiver = NULL; 	
	evntMsg->numVer = 0;
	evntMsg->falseReport = false;
	evntMsg->detected = false; // (2014.09.23) 
}

// ���� �޽��� ���� ��� ����

void Sec::setReceiver(struct _tagCntrlMsg* msg, Node* nextNode) // (2014.05.22)
{
	msg->receiver = nextNode;
}

// ���� �޽��� ó�� ����

CntrlMsg* Sec::getCntrlMsg() // ���� �޽��� ȹ��
{
	return cntlMsg;
}


void Sec::setCntrlMsg(int clstrID) // ���� �޽��� �� Ŭ������ ID ����
{
	cntlMsg->clstrID = clstrID; 
}

int Sec::getNumDtctn() // ���� Ž�� Ƚ�� ��ȯ
{
	return this->numDtctn;
}

int Sec::getNumVer() // MAC ���� Ƚ�� ��ȯ	 
{
	return this->numVer;
}

bool Sec::isAtkDtctd() // (2014.08.21) ���� ���� ���� ��ȯ
{
	return this->atkDtctd;
}

void Sec::resetTmpLst() // (2014.08.25) �ӽ� MAC ��� ����
{
	this->tmplst.clear();
}

Sec::Sec()
{
	// ���� �޽��� ��ü ����

	this->cntlMsg = new CntrlMsg;
	
	atkDtctd = 0;
	numDtctn = 0;
	numVer = 0;
}

Sec::~Sec()
{
	// ���� �޽��� ��ü �޸� ��ȯ
	delete cntlMsg;


	// �ӽ� MAC ��Ͽ� ���� �ִ� MAC ���� �޸� ��ȯ
}

/*

void Sec::setNodes(Node nodes[]) // (2013.08.28 �߰�)
{
	this->nodes = nodes; // �迭�� ���� �ּ� ����
}

void Sec::setRtn(Rtn* rtn) // (2013.09.09 �߰�)
{
	this->rtn = rtn; // ����� ��ü ����
}

void Sec::setKmr(Kmr* kmr)
{
	this->kmr = kmr;
}

void Sec::testCntrlMsg() // (2013.09.05) ���� �޽��� �׽�Ʈ
{
	ofstream out("test_Sec_CntrlMsg.txt", std::ofstream::app);

	out << endl;

	if (this->cntlMsg->cmType == BS_HELLO)
	{
		out << "cmType = BS_HELLO\n";
	}
	else if (this->cntlMsg->cmType == CLSTR_ACK)
	{
		out << "cmType = CLSTR_ACK\n";
	}
	else
		out << "wrong message type!\n";

	if (cntlMsg->sender != NULL)
	{
		out << "sender: " << cntlMsg->sender->id << endl;
	}

	if (cntlMsg->receiver != NULL)
	{
		out << "receiver: " << cntlMsg->receiver->id << endl;
	}

	out << "ID lists: " << endl;

	for (int i = 0; i < IHA_T +1; i++)
	{
		IDLIST lst = cntlMsg->IDLists[i]; // i ��° IDLIST ȹ��

		out << " [ " << i << " ] "; 

		IDLIST::iterator it = lst.begin();

		for (it; it != lst.end(); it++) // i��° IDLIST ���
		{
			int id = *it;
			out << id << ", ";
		}

		out << endl;
	}

	out.close();
}

CntrlMsg* Sec::getCntrlMsg()
{
	return &cntlMsg;
}

void Sec::setCntrlMsg(int clstrID)
{
	cntlMsg->clstrID = clstrID;
}

int Sec::getNumDtctn()
{
	return this->numDtctn;
}

int Sec::getNumVer()
{
	return this->numVer;
}

Sec::Sec()
{
	this->numDtctn = 0;

	numVer = 0; 

	atkDtctd = false; 

	// ���� �޽��� ����ü�� ��� ���� �ʱ�ȭ

	cntlMsg->cmType = BS_HELLO;
	cntlMsg->sender = NULL;
	cntlMsg->receiver = NULL;
}

Sec::~Sec()
{

}
*/