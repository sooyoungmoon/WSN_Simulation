// 2014.07.31
// 문수영
// typedef.h
// 실험에 필요한 자료형 선언

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

#include <iostream>
#include <list>
#include <map>
#include <set>
#include "Node.h"
#include "params.h"

using namespace std;

class Node;

typedef map<int, Node*> NodesMap; // <node ID, node pointer>

typedef list<int> NodeIDLst;
typedef list<Node*> NodesLst;

typedef std::set<int> IDSet; // ID 집합
typedef std::list<int> IDLIST; // ID 목록

// filtering key

/*
typedef struct _tagPairKey // (2013.08.19 추가)
{
	int nID; // 첫 번째 노드 ID
	int nID2; //  두 번째 노드 ID 
	int kVal; // 키 값

} PairKey;
*/
struct _tagPairKey // (2013.08.19 추가)
{
	int nID; // 첫 번째 노드 ID
	int nID2; //  두 번째 노드 ID 
	int kVal; // 키 값

};

typedef struct _tagPairKey PairKey;

// group key

typedef struct _tagGroupKey // (2014.05.14 수정)
{
	int oldVal; // 이전 값
	int newVal; // 현재 값

	//int kVal; // 키 값

} GroupKey;

typedef list<GroupKey> GroupKeyLst;

typedef struct _tagKNode // key tree를 구성하는 k node
{
	struct _tagGroupKey* groupKey; // (2014.04.22) 그룹 키

	struct _tagKNode* parent;
	struct _tagKNode* leftChild;
	//struct _tagKNode* rightChild;
	struct _tagKNode* leftSibling;
	struct _tagKNode* rightSibling;
	
	bool isCluster; // (2014.06.08) 클러스터에 대응되는 k-node의 경우 true, 그렇지 않으면 false
	double COG_X; // (2014.06.19) 클러스터에 속한 노드들의 무게 중심의 X 좌표
	double COG_Y; // (2014.06.19) 클러스터에 속한 노드들의 무게 중심의 Y 좌표

	int CHID; // (2014.06.04) (클러스터에 대응되는 k-node의 경우) CH의 ID

	struct _tagUNode* unode; 

	int level; // key tree 내에서의 level 정보
	int index; // 동일 level 내에서의 인덱스 (좌측부터 0, 1, ...)

	bool updated; // (2014.05.13) 키 갱신 여부
	bool updated_addition; // (2014.05.08) k-node가 갱신되었는지, 노드의 추가/제거로 인해 갱신되었는지를 T/F로 표현
	bool updated_eviction; 

	bool created; // (2014.06.08) k-node가 이번 라운드에 생성되었다면 true, 그렇지 않다면 false - (키 배포 목적)

	int numSensorNodes; // (2014.09.03) 클러스터의 kNode의 경우, 해당 클러스터에 속한 센서 노드 수

	int outDegree; // (2014.09.04) 해당 kNode의 자식 kNode의 개수 (클러스터에 대응되는 그룹 키의 경우, 슬롯의 개수)

} KNode;

typedef struct _tagUNode // key tree를 구성하는 u node
{
	Node* sensorNode; // u node에 대응되는 센서 노드 
	bool isCH; // CH 여부
	KNode* parent;	
	
} UNode;

/*
typedef struct _tagKTreeNode // key tree node
{
	KTreeNode* parent;	
	KTreeNode* leftSibling;
	KTreeNode* rightSibling;

} KTreeNode;
*/

// MAC

typedef struct _tagMAC
{
	Node* origin; // (2013.09.02) MAC 생성 노드 (MAC 생성 과정에서 설정 필요)
	PairKey* pKey; // (2013.08.19 수정) pairwise 키 
	int code; // 인증 코드
} MAC;

typedef struct _tagTREQ
{
	int senderID;// 송신 노드
	int round; // 현재 라운드 (0, 1, 2, ...)

	
} TREQ; // topology request message

typedef struct _tagTRES
{
	int srcID; // 소스 노드 
	std::list<Node*> nbrLst; // 이웃 노드 목록

} TRES; // topology response message


typedef struct _tagProtoKeyMsg
{
	list < GroupKey> newKeys; // 		
	IDSet recipents;// 수신 노드들의 ID 집합

} ProtoKeyMsg; // 키 갱신 메시지 프로토타입


typedef struct _tagKeyMsg
{
	list < GroupKey> newKeys; // 	
	//list < pair<GroupKey, GroupKey>> newKeys; // {(기존 키, 신규 키)}
	//std::map<GroupKey, GroupKey> newKeys; // {(기존 키, 신규 키)}
	//NodeIDLst recipents;// 수신 노드들의 목록
	IDSet recipents;// 수신 노드들의 ID 집합

	int CHID; // 소속 클러스터의 CH의 ID

	IDSet upAssocNds; // 상위 연관 노드 목록
	IDSet downAssocNds; // 하위 연관 노드 목록

	//NodeIDLst upAssocNds; // 상위 연관 노드 목록
	//NodeIDLst downAssocNds; // 하위 연관 노드 목록

} KeyMsg; // 키 갱신 메시지

// EVENT REPORT
typedef struct _tagEvntMsg
{
	int x;
	int y;
	int val;
	int hop; // hop count
	Node* CoS;	
	Node* sender; // 송신 노드 (매 홉마다 변경)
	Node* receiver; // 수신 노드 ( " )	

	std::list<int> prevNodes; // (2014.08.11) 이전 노드들의 목록

	std::list<MAC*> macs; // (2013.08.19) 보고서 내 MAC 목록

	int numVer; // (2013.10.29) 보고서 내 MAC 검증 횟수

	bool falseReport; // 위조 보고서 여부 (T: 위조 보고서, F: 정상 보고서)

	bool detected; // 위조 보고서 탐지 여부 (T: 탐지됨, F: 미 탐지됨)

} EvntMsg;

// CONTROL MESSAGE

enum ctrlMsgType {BS_HELLO =1, CLSTR_ACK=2, MAC_REQ=3, MAC_RES=4 }; // 제어 메시지 타입 ( BS_HELLO = 1, CLSTR_ACK = 2, MAC_REQ=3, MAC_RES=4}

typedef struct _tagCntrlMsg // control 메시지 (BS HELLO, CLUSTER ACK 메시지)
{
	ctrlMsgType cmType; // (2013.08.13 추가) 제어 메시지 타입 변수
	Node* sender;
	Node* receiver;

	int clstrID; // (2013.09.16추가) 클러스터 ID

	//map<int, set<int>> IDSets; // <hop count, {upper/lower node ID}>

	IDSet IDSets[INTERLEAVING_T+2];// 노드들의 ID 목록들로 이루어진 목록 

	//IDSet IDSets[INTERLEAVING_T+1];// 노드들의 ID 목록들로 이루어진 목록 (t+1개의 행)

	int nextNodeID; // 다음 노드의 ID - MAC_REQ 메시지의 경우
	MAC mac; // 다음 노드를 위한 MAC - MAC_RES 메시지의 경우

} CntrlMsg;

// 유전 알고리즘 관련

typedef pair<int, int> Gene; // 유전자 자료형 정의- first: 해당 노드의 키 트리 내 level, 
								// second: 1) k-node의 경우, 동일 level 내 인덱스, 2) u-node의 경우, 노드 ID


typedef map<int, Gene> Chromosome; // 염색체 자료형 정의 { (i, i-th gene) }

typedef map<int, char> GeneType; // 유전자의 종류 ( 'G': group, 'N': node)

#endif 