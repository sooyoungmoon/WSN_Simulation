// 2014.05.22
// 문수영
// Sec.cpp
// security 클래스 정의


#include "Sec.h"
#include <fstream>

using namespace std;

// 멤버 변수 값 설정

void Sec::setNodes(map<int, Node*> nodes) // 노드 목록 설정
{
	this->nodes = nodes;
}


void Sec::setRtn (Rtn* rtn) //  라우팅 객체 주소 설정
{
	this->rtn = rtn;
}


void Sec::setKmr( Kmr* kmr) // 키 관리자 객체 주소 설정
{
	this->kmr = kmr;
}

void Sec::initEventMsg(struct _tagEvntMsg* evntMsg, Node* CoS) // 이벤트 보고서 초기화
{
	evntMsg->x = CoS->x;
	evntMsg->y = CoS->y;
	evntMsg->val = 0;
	evntMsg->hop = 0;
	evntMsg->CoS = CoS; // CoS 노드 설정
	evntMsg->sender = CoS; // 송수신 노드 설정		
	evntMsg->receiver = NULL; 	
	evntMsg->numVer = 0;
	evntMsg->falseReport = false;
	evntMsg->detected = false; // (2014.09.23) 
}

// 제어 메시지 수신 노드 설정

void Sec::setReceiver(struct _tagCntrlMsg* msg, Node* nextNode) // (2014.05.22)
{
	msg->receiver = nextNode;
}

// 제어 메시지 처리 관련

CntrlMsg* Sec::getCntrlMsg() // 제어 메시지 획득
{
	return cntlMsg;
}


void Sec::setCntrlMsg(int clstrID) // 제어 메시지 내 클러스터 ID 설정
{
	cntlMsg->clstrID = clstrID; 
}

int Sec::getNumDtctn() // 공격 탐지 횟수 반환
{
	return this->numDtctn;
}

int Sec::getNumVer() // MAC 검증 횟수 반환	 
{
	return this->numVer;
}

bool Sec::isAtkDtctd() // (2014.08.21) 공격 검출 여부 반환
{
	return this->atkDtctd;
}

void Sec::resetTmpLst() // (2014.08.25) 임시 MAC 목록 리셋
{
	this->tmplst.clear();
}

Sec::Sec()
{
	// 제어 메시지 객체 생성

	this->cntlMsg = new CntrlMsg;
	
	atkDtctd = 0;
	numDtctn = 0;
	numVer = 0;
}

Sec::~Sec()
{
	// 제어 메시지 객체 메모리 반환
	delete cntlMsg;


	// 임시 MAC 목록에 남아 있는 MAC 들의 메모리 반환
}

/*

void Sec::setNodes(Node nodes[]) // (2013.08.28 추가)
{
	this->nodes = nodes; // 배열의 시작 주소 저장
}

void Sec::setRtn(Rtn* rtn) // (2013.09.09 추가)
{
	this->rtn = rtn; // 라우팅 객체 저장
}

void Sec::setKmr(Kmr* kmr)
{
	this->kmr = kmr;
}

void Sec::testCntrlMsg() // (2013.09.05) 제어 메시지 테스트
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
		IDLIST lst = cntlMsg->IDLists[i]; // i 번째 IDLIST 획득

		out << " [ " << i << " ] "; 

		IDLIST::iterator it = lst.begin();

		for (it; it != lst.end(); it++) // i번째 IDLIST 출력
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

	// 제어 메시지 구조체의 멤버 변수 초기화

	cntlMsg->cmType = BS_HELLO;
	cntlMsg->sender = NULL;
	cntlMsg->receiver = NULL;
}

Sec::~Sec()
{

}
*/