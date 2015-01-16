// 2014.08.31
// ������
// main.cpp
// main �Լ� ���� ����


#include <iostream>
#include <fstream>
#include <map>
#include "params.h"

#include "Node.h"
#include "BS.h"
#include <list>

#include "Atk.h"
#include "Atk_FRA.h"

#include "Kmr.h"
#include "Kmr_IHA.h"
#include "Kmr_NEWLKH.h"

#include "Rtn.h"
#include "Rtn_IHA.h"
#include "Rtn_DIHA.h"
#include "Rtn_SEF.h"

#include "Sec.h"
#include "Sec_IHA.h"
#include "Sec_DIHA.h"
#include "Sec_SEF.h"

#include "RAND.h"
// #include "RAND.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::ofstream;

Fuzzy fuzzy; // fuzzy �߷��� ���� Fuzzy Ŭ���� ��ü

RAND gen0; // ��尡 ��ġ�� cell ������ ���� ���� ����
int seed0=1;

RAND gen1; // ��� ��ġ�� ���� ���� ����
int seed1=1;	

RAND gen2; // CoS ������ ���� ���� ����
int seed2=1;	

RAND gen3; // ���� ���� �߻��� ���� ���� ����
int seed3=1;	

RAND gen4; // ��� �Ѽ��� ���� ���� ����

int seed4=1; 

 // Node Ŭ������ ���� ���� ����
unsigned int Node::totNumNodes; // ������� ������ �� ��� ��
unsigned int Node::numNodesEvctd; // ������� ���ŵ� ��� ��

void deployNode ( Node* pNode, int xMin, int xMax, int yMin, int yMax); // �ű� ��带 �ʵ忡 ��ġ (��ġ �� BS������ �Ÿ� ����)
void testNode(map<int, Node*> nodes); // ��� ���� �׽�Ʈ
map<int, Node*> arrangeNodes( map<int, Node*> nodes); // ��� ���� ���� (value = NULL�� ��� ����)

int main(int argc, char* argv[])
{
	/* 1. ���� */
	cout << " /* 1. ���� */ " << endl;

	// ���� ������ seed ����

	gen0.setSeed(seed0);
	gen1.setSeed(seed1);
	gen2.setSeed(seed2);
	gen3.setSeed(seed3);
	gen4.setSeed(seed4);

	
	//gen0.testRand();
	//gen1.testRand();
	//gen2.testRand();
	//gen3.testRand();
	//gen4.testRand();
	

	// ���� ��ǥ ������ ���� �ʿ��� ������ ����

	/// ȩ �� ����
		int totNumEvents = 0; // ������� �߻��� �̺�Ʈ ����
		int numEventsInRound = 0; // (2014.09.24) ���� ���忡�� �߻��� �̺�Ʈ ����

		int countedEvents = 0; // ��� ���� ȩ ���� ���� �� �ݿ��� �̺�Ʈ ������ ���� (bs���� �����ϰų�, �߰��� ������ ����� ������ ����)��

		double avgHopCnt = 0; // ���� ��� ���� ȩ ��
		double avgHopCntDetectedER = 0; // �߰� ��忡 ���� ����� ���� ������ ��� ���� ȩ ��

	/// cutoff ����

		bool cutoffOccur = false;// ���� ���� ���� �� cutoff�� �߻��Ͽ����� ����
		bool noCutoff = true; // �ùķ��̼� ���ۺ��� cutoff�� �߻����� �ʾҴ��� ���� (true: �� ���� �߻� �� ��)

		int numCutoffs = 0; // (2013.11.28) cutoff �߻� Ƚ��
		int numCutoffsCoS = 0; // (2014.09.30) (CoS�� ������ ���� ����) cutoff �߻� Ƚ��
		int numCutoffsFwd = 0; // (2014.09.30) (�߰� ����� ������ ���� ����) cutoff �߻� Ƚ��
		int numCutoffsIsol = 0; // (2014.09.30) (�߰� ����� ������ ����) cutoff �߻� Ƚ��
		int numCutoffsInRound = 0; // (2014.09.24) ���� ������ cutoff �߻� Ƚ��
		double cutoffRatio = 0; // (2014.01.02) (cutoffȽ��/ �̺�Ʈ�߻�Ƚ��) �߻� ����

		map<int, bool> cutoff; // �� Src cluster���� cutoff�� �߻��Ͽ����� ���� <CH's ID, T/F>
		int numCutoffClusters = 0; // cutoff�� �߻��� Ŭ������ ����		

		int firstCutoff = -1; // ù ��° Ŭ�����Ϳ��� cutoff�� �߻��� �������� �ð� (loop Ƚ��)
		//int midCutoff = -1; // �߰� Ŭ�����Ϳ���  ~
		int lastCutoff = -1; // ������ Ŭ�����Ϳ��� ~

		double depletNodeRatio; // ������ �� ����� ����
		
	/// ���� ����	

		int totalNumVer = 0; // �� ���� Ƚ��

	// ���� ��ǥ ������ ���� �ʿ��� ���� ���� �Ϸ�


	/* 2. �������� ���� �� �ʱ�ȭ */

	cout << " /* 2. �������� ���� �� �ʱ�ȭ */ " << endl;

	Atk* atk;
	Sec* sec;
	Kmr* kmr; // Ű ���� ��ü 
	Rtn* rtn;

	map<int, Node*> nodes; // ��� �� (��� id, ��� ������) (���� ��Ʈ��ũ�� �����ϴ� ���)
	map<int, Node*> delNodes; // ��� �� (�ʵ忡�� ���ŵ� ���)

	IDSet addedNodes; // ���� ���忡�� �߰��� ����� ID ����
	IDSet deletedNodes; // ���� ���忡�� ���ŵ� ����� ID ����
	IDSet depletedNodes; // (2014.09.17) ���� ���忡�� �������� ���� ����� ID ����
	IDSet compNodes; // (2014.09.24) �Ѽյ� ����� ID ����

	map<int, set<int>> clusters; // Ŭ������ ��

	map<int, set<int>> srcClusters; // (2014.08.31) src Ŭ������ �� - CoS�� ���õ� �� �ִ� CH���� Ŭ������ (BS�κ��� �Ÿ��� T+1ȩ �̻��� CH)

	BS bs; // BS

	list<Node*> fNodes; // �޽��� ������ ���� ���� ��� ���

	try
	{	

		if ( strcmp(argv[1], "IHA") == 0) // Ű ���� ��� ����
			kmr = new Kmr_IHA();
		else if ( strcmp(argv[1], "PRM") == 0)
			kmr = new Kmr_NEWLKH();			

		if ( strcmp(argv[2], "IHA") == 0) // ����� �������� ����
			rtn = new Rtn_IHA();
		else if ( strcmp(argv[2], "PRM") == 0) 
			rtn = new Rtn_DIHA();		
		else if ( strcmp(argv[2], "SEF") == 0) 
			rtn = new Rtn_SEF();

		if ( strcmp(argv[3], "IHA") == 0) // ���� �������� ����
			sec = new Sec_IHA();
		else if ( strcmp(argv[3], "PRM") == 0)
			sec = new Sec_DIHA();
		else if ( strcmp(argv[3], "SEF") == 0)
			sec = new Sec_SEF();

		if ( strcmp(argv[4], "FRA") == 0) // ���� �������� ����
			atk = new Atk_FRA();
		else 
		{
			cerr << "���� ���������� �߸��� ����" << endl;
		}
	}

	catch (bad_alloc & ba )
	{
		cout << " error �߻�: " << endl;
		cout << ba.what() << endl;
	}
	
	kmr->setRtn(rtn); 
	kmr->setSec(sec);
	sec->setRtn(rtn);
	sec->setKmr(kmr);

	bs.setKmr(kmr);
	bs.setRtn(rtn);

	rtn->setBS(&bs); // (2014.08.07)
	rtn->setKmr(kmr);
	rtn->setFuzzy(&fuzzy);

	Sec_IHA* sec_IHA = dynamic_cast <Sec_IHA*> (sec);

	/* 3. ��� ��ü ���� �� �ʱ�ȭ */

	Node::totNumNodes = 0;

	int round = 0;
	
	ofstream out_event("tst_eventMsg.txt"); // �̺�Ʈ ���� ��� ���� ����

	ofstream out_isolRatio("tst_isolRatio.csv"); // �� ��� (�Ǵ� CH) ���� ��� ���� ����

	while (round < NUM_ROUNDS) // ������ ����� ������ ���� �ݺ�
	{
		/* 3. ��� �߰�/���� */

		cout << "/* 3. ��� �߰�/���� */ " << endl;

		addedNodes.clear(); // ���� ���忡�� �߰��� ����� ID ��� ����
		deletedNodes.clear(); // ���� ���忡�� ���ŵ� ����� ID ��� ����

		  // ù ��° ������ ���, �ʱ� ��� ����ŭ ��� ��ü ����

		if (round ==0 )
		{
			int cnt = 0; // ������ ��ü�� ��
			int numNodesPerCell =  NUM_NODES / NUM_CELL; // virtual grid�� �����ϴ� cell �ϳ� �� ��ġ�� ��� �� ���

			//cout << "numNodesPerCell: " << numNodesPerCell << endl;  // ����� �� �ڵ� 

			while (cnt < NUM_NODES) // �ʱ� ��� �� ��ŭ ��� ��ü�� �����ϰ� ��� �ʿ� �߰�
			{
				Node* pNode = new Node();				

				nodes.insert( make_pair ( Node::totNumNodes++, pNode));		

				addedNodes.insert ( pNode->id); // (2014.06.02) ���� ���忡�� ���� �߰��� ��� ��Ͽ� ����

				// �ű� ����� ��ġ ����				
				
				//int cellID = gen0.genRandInt(NUM_CELL); // ��尡 �߰��� cell ID ���

				// (2014.09.17 ����) ���� ���忡�� �� cell ���� t+1���� ��尡 ��ġ�ǵ��� ��
				int cellID = cnt % NUM_CELL; // ��尡 �߰��� cell ID ���

				//int cellID = cnt / numNodesPerCell; // ��尡 �߰��� cell ID ���
				pNode->cell = cellID;
				
				int xMin = (cellID % CELLS_PER_ROW) * CELL_WIDTH;
				int xMax = xMin + CELL_WIDTH;
				int yMin = (cellID / CELLS_PER_ROW) * CELL_HEIGHT;
				int yMax = yMin + CELL_HEIGHT;

				//cout << "cell iD: " << cellID; // ����� �� �ڵ� 
				//cout << " node ID: " << pNode->id; // ����� �� �ڵ�			

				deployNode(pNode, xMin, xMax, yMin, yMax); // �ش� cell ���� ���� ������ ��ġ�� ��� ��ġ
				/*
				if (gen4.genRandInt(100) < CNR ) // (2014.08.22) ������ Ȯ���� �Ѽ� ���� ����
				{
					pNode->cmprmsd = true;
					compNodes.insert(pNode->id);
				}	
				*/
				pNode->setKmr(kmr);
				pNode->setRtn(rtn);
				pNode->setSec(sec);

				//cout << "\t (" << pNode->x << ", " << pNode->y << ") \n" ; // ����� �� �ڵ� 
				cnt++;
			}

			//testNode(nodes); // ��� ���� �׽�Ʈ
		}
		else // �� ��° ���� ������ ���, ��� �߰�/���� ������ ����Ͽ� ���� �߰� �� ����
		{
			// ��� ����

			set<int>::iterator it_dpltdNode;

			for ( it_dpltdNode = depletedNodes.begin(); it_dpltdNode != depletedNodes.end(); it_dpltdNode++)
			{
				int nodeID = *it_dpltdNode;
				Node* node = nodes[nodeID];

				deletedNodes.insert( nodeID); // ���ŵ� ��� ID ��Ͽ� �� ����� id �߰�
				delNodes.insert( make_pair( nodeID, node) ); // ���ŵ� ��� map�� �� ��� ���� �߰�
				nodes.erase (nodeID); // ��� map���� �� ��� ���� ����

				rtn->removeFromCluster (nodeID); // (2014.10.10)

				Node::numNodesEvctd++; // ���ŵ� ��� �� +1����
			} // �� ��� ����

			// (2014.09.24) �Ѽյ� ��� ���� ���� �߰� ����

			set<int>::iterator it_compNode;

			for ( it_compNode = compNodes.begin(); it_compNode != compNodes.end(); it_compNode++) // �Ѽ� ��� ��� ��ȸ - ���� ��� ��Ͽ� �߰�
			{
				int nodeID = *it_compNode;
				Node* node = nodes[nodeID];

				deletedNodes.insert( nodeID); // ���ŵ� ��� ID ��Ͽ� �� ����� id �߰�
				delNodes.insert( make_pair( nodeID, node) ); // ���ŵ� ��� map�� �� ��� ���� �߰�
				nodes.erase (nodeID); // ��� map���� �� ��� ���� ����
				Node::numNodesEvctd++; // ���ŵ� ��� �� +1����
			} // for�� ���� -  �Ѽ� ��� ����

			compNodes.clear(); // �Ѽ� ��� ��� ����

			// ��� �߰�
			
			int numAddedNodes = static_cast <int> (depletedNodes.size() + NUM_NODES * ((double) FTR/ 100 + 0.1)); // (2014.10.22) �߰��Ǵ� ��� �� = �� ��� �� + �ʱ� ��� �� * ���� ����

			//int numAddedNodes = NUM_NODES; // (2014.10.02) �ʱ� ��� ����ŭ ���ο� ��� �߰�

			//int numAddedNodes = deletedNodes.size(); // ���ŵ� ��� ����ŭ ���ο� ��� �߰�
			//int numAddedNodes = depletedNodes.size(); // ���� ��� ����ŭ ���ο� ��� �߰�

			// (2014.09.24) �Ѽյ� ��� ���� ŭ ���ο� ��� �߰� ����

			int cnt = 0;

			while (cnt < numAddedNodes)
			{
				Node* pNode = new Node();
				nodes.insert( make_pair ( Node::totNumNodes++, pNode));			

				addedNodes.insert ( pNode->id); // (2014.06.02) ���� ���忡�� ���� �߰��� ��� ��Ͽ� ����

				// �ű� ����� ��ġ ����

				int numNodesPerCell =  numAddedNodes / NUM_CELL; // virtual grid�� �����ϴ� cell �ϳ� �� ��ġ�� ��� �� ���				
				
				int cellID = gen0.genRandInt(NUM_CELL); // ��尡 �߰��� cell ID ���
				
				pNode->cell = cellID;

				int xMin = (cellID % CELLS_PER_ROW) * CELL_WIDTH;
				int xMax = xMin + CELL_WIDTH;
				int yMin = (cellID / CELLS_PER_ROW) * CELL_HEIGHT;
				int yMax = yMin + CELL_HEIGHT;

				deployNode(pNode, xMin, xMax, yMin, yMax); // �ش� cell ���� ���� ������ ��ġ�� ��� ��ġ

				/*
				if (gen4.genRandInt(100) < CNR ) // (2014.08.22) ������ Ȯ���� �Ѽ� ���� ����
				{
					pNode->cmprmsd = true;
					compNodes.insert(pNode->id);
				}	
				*/
				pNode->setKmr(kmr);
				pNode->setRtn(rtn);
				pNode->setSec(sec);

				cnt++;
			} // �ű� ��� �߰� �Ϸ�		
			
			depletedNodes.clear(); // �� ��� ��� ����
		
		} // ��� �߰� �� ���� �Ϸ�

		map<int, Node*>::iterator it_node;

		// ��� �Ѽ�

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // (2014.10.02) ��� �Ѽ�
		{
			Node* pNode = (*it_node).second;

			if (gen4.genRandInt(100) < CNR ) // ������ Ȯ���� �Ѽ� ���� ����
			{
				pNode->cmprmsd = true;
				compNodes.insert(pNode->id);
			}	

		}// for�� - ��� �Ѽ� �Ϸ�


		kmr->setNodes(nodes);// ��ü ��� ���� ����		
		sec->setNodes(nodes);
		rtn->setNodes(nodes);
		atk->setNodes(nodes);

		// ��� �� ���
		cout << "����: " << round << endl;
		cout << "\t��� ��: " << nodes.size() << endl << endl;

		testNode(nodes); // ��� ���� �׽�Ʈ

		/* 3-2. Topology Request/Response ���� */

		rtn->resetTopology(); // topology ���� ���� ����

		fNodes.clear(); // ���� ��� ��� ����
		map<int, bool> forwarded ;// �޽��� ���� ���� <node ID, T/F> - �޽����� ������ ���� true�� ����				

		rtn->genTREQMsg(round); // TREQ �޽��� ����
		TREQ* treq = rtn->getTREQMsg(); // TREQ �޽��� ȹ��

		//map<int, Node*>::iterator it_node;

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
		//for (unsigned int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
		{
			Node* pNode = (*it_node).second;

			if (pNode == NULL) // (2014.09.22) 
			{
				continue;
			}

			double dist = pNode->distance;

			if (dist < TRANS_RANGE) // BS���� ���� ���� �̳��̸� ���� ��� ��Ͽ� �߰�
			{
				bs.addNbr(pNode); // BS�� �̿� ��� ��Ͽ� �߰�
				fNodes.push_back(pNode); // ���� ��� ��Ͽ� �߰�
				forwarded[pNode->id] = true; // ���� ���θ� ������ ����
			}	
			else
			{
				forwarded[pNode->id] = false; // ���� ���θ� �������� ����
			}			
		}

		while( !fNodes.empty())
		{
			// ���� ��� ����� ù ��° ��� ȹ��
			Node* cNode = fNodes.front(); 
			fNodes.pop_front(); 

			rtn->updatTREQMsg(cNode); // TREQ �޽��� ������Ʈ			
			
			list<Node*> nxtNds = rtn->selNext(treq, cNode); // ���� ����� ���� ���� ��� ��� ȹ��

			cNode->txMsg(treq); // ���� ����� �۽� �Լ� ȣ��

			if ( !nxtNds.empty()) // ���� ����� ���� ���� ������ ���� �Լ��� ȣ���ϰ� ���� ��� ��Ͽ� �߰�
			{
				list<Node*>::iterator it;

				for (it = nxtNds.begin(); it != nxtNds.end(); it++)
				{
					Node* nextNode = (*it);
					int nextNodeID = nextNode->id;
					nextNode->rxMsg(treq);

					if ( forwarded[nextNodeID] == false) // ���� ��� ��Ͽ� ���Ե� ���� ������ ��Ͽ� �߰�
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;
					}
				}// ���� ���� ����� ���� ���� ���鿡�� �޽��� ���� �Ϸ�
			}

		} // TREQ �޽��� ���� �Ϸ�

		// TRES �޽��� 

		// �� ���鿡 ����, TREQ �޽����� �����ϰ� BS �������� ���� 

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
		//for (unsigned int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
		{
			fNodes.clear();
			forwarded.clear();

			Node* sNode = (*it_node).second;

			if (sNode == NULL) // (2014.09.22) 
			{
				continue;
			}

			//Node* sNode = nodes[n];
			rtn->genTRESMsg(sNode); // source node�� TRES �޽��� ����

			TRES* tres = rtn->getTRESMsg(); // TRES �޽��� ȹ��

			fNodes.push_back(sNode);
			forwarded[sNode->id] = true;

			while( !fNodes.empty())
			{
				// ���� ��� ����� ù ��° ��� ȹ��
				Node* cNode = fNodes.front(); 
				fNodes.pop_front(); 

				list<Node*> nxtNds = rtn->selNext(tres, cNode); // ���� ����� ���� ���� ��� ��� ȹ��

				cNode->txMsg(tres); // ���� ����� �۽� �Լ� ȣ��

				list<Node*>::iterator it;

				for (it = nxtNds.begin(); it!= nxtNds.end(); it++)
				{
					Node* nextNode = (*it);
					int nextNodeID = nextNode->id;
					nextNode->rxMsg(tres);

					if ( forwarded[nextNodeID] == false) // ���� ��� ��Ͽ� ���Ե� ���� ������ ��Ͽ� �߰�
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;
					}
				}

			}
		} // TRES �޽��� ���� �Ϸ�

		/* 3-2. Topology Request/Response �Ϸ� */

		bool fwd = true;

		//rtn->testParentNodes();

		/* 4. Ŭ������ �籸�� */  //  ���� Ű ���� ����� ��쿡�� ����
		/*
		cout << "/ 4. Ŭ������ �籸�� /" << endl; 
		  // ch ����, Ŭ������ ���� ����
		
		rtn->setCHs(); 
		rtn->testCHs(); 
		
		// 5. ���� ��� ���� // //  ���� Ű ���� ���, IHA�� ��쿡�� ����
		
		cout << "/ 5. ���� ��� ���� /" << endl;
		
		sec_IHA->resetAssoc(); // ���� ������ ���� ��� ���� ���� (reset)

		/// BS_HELLO �޽��� ���� �� ���� 
		
		sec->genCntrlMsg( BS_HELLO, &bs); // BS_HELLO �޽��� ����
		struct _tagCntrlMsg* cntlMsg; // ���� �޽��� �ּ�

		cntlMsg = sec->getCntrlMsg(); // BS_HELLO �޽��� ȹ��		

		/// ���� ��� ���� ����
		fNodes.clear();
		forwarded.clear();
		
		list<Node*> nxtNds; // ���� ���� ��� ���
		
		nxtNds = rtn->selNext(cntlMsg, &bs); // �ʱ� ���� ��� ��� ȹ��		

		list<Node*>::iterator it;

		//bool fwd = true;

		for (it = nxtNds.begin(); it!= nxtNds.end(); it++) // �ʱ� ���� ���鿡 ���� �ݺ�
		{
			// BS -> {�ʱ� ���� ���} �޽��� ����

			Node* nextNode = (*it);
			int nextNodeID = nextNode->id;

			/// BS -> ��� ���� 
			sec->setReceiver(cntlMsg, nextNode);
			bs.txMsg(cntlMsg, nextNode); // BS�� �۽� �Լ� ȣ��			
				
			bool fwd = true; // ��� ���� ����
			fwd = nextNode->rxMsg(cntlMsg); // BS�� �̿� ����� ���� �Լ� ȣ��

			//if (fwd == false)
			{
			//	break;
			}

			if ( fwd == true && forwarded[nextNodeID] == false) // ���� ��� ��Ͽ� ���Ե� ���� ������ ��Ͽ� �߰�
			{
				fNodes.push_back(nextNode);
				forwarded[nextNodeID] = true;				
			}
		} // BS �޽��� �ʱ� ����
		
		while ( !fNodes.empty() ) // ���� ��� ����� ��� ���� ���� ���� �ݺ�
		{
			 // ���� ��� ȹ��, ���� ��� ��Ͽ��� ���� 
			Node* cNode = fNodes.front(); 
			fNodes.pop_front();

			sec->genCntrlMsg(BS_HELLO, cNode); // ���� ��忡�� ���� �޽��� �� ����
			cntlMsg = sec->getCntrlMsg();
			
			list<Node*> nxtNds = rtn->selNext(cntlMsg, cNode); // ���� ����� ���� ���� ��� ��� ���� �� ȹ��
			
			list<Node*>::iterator it;

			for (it = nxtNds.begin(); it != nxtNds.end(); it++)  // ���� ����� ���� ���� ���鿡 ���� �ݺ�
			{
				Node* nextNode = *it;
				int nextNodeID = nextNode->id;

				/// �޽��� �ۼ��� 
				sec->setReceiver(cntlMsg, nextNode);
				cNode->txMsg(cntlMsg); // �۽�

				bool fwd = true; // ��� ���� ����
				fwd = nextNode->rxMsg(cntlMsg); // ����

				if (forwarded[nextNodeID]==false && fwd == true) // ���� ��带 ���� ��� ��Ͽ� �߰� (���Ǻ�)
				{
					forwarded[nextNodeID] = true;
					fNodes.push_back(nextNode);
				}		

			}
		}


		/// BS_HELLO �޽��� ���� �� ���� �Ϸ�		

		/// Clstr_ACK �޽��� ���� �� ���� 

		  // Ŭ������ ���� �ݺ�

		map<int, set<int>>::iterator it_clusters;
		clusters = rtn->getClusters();

		for (it_clusters = clusters.begin(); it_clusters != clusters.end(); it_clusters++) // Ŭ�����͸��� �ݺ�
		{
			
			if ( !(*it_clusters).second.empty()) // �ش� Ŭ�����Ϳ� ���� ��尡 1�� �̻� ������ ���
			{
				int CHID = (*it_clusters).first; // CH�� Id

				set<int> clusternodes = (*it_clusters).second;

				// �޽��� ����
				sec->setCntrlMsg(CHID); // cluster ID (= CH's ID) ����
				sec->genCntrlMsg(CLSTR_ACK, nodes[CHID]); // Cluster ACK �޽��� ����
				
				cntlMsg = sec->getCntrlMsg(); // CLSTR_ACK �޽��� ȹ��		

				/// ���� ��� ���� ���� 
				fNodes.clear();
				forwarded.clear();
				nxtNds.clear();
				

				nxtNds = rtn->selNext(cntlMsg, nodes[CHID]); // �ʱ� ���� ��� ��� ȹ��
			
				list<Node*>::iterator it_nxtNds;

				bool fwd = true;

				for(it_nxtNds = nxtNds.begin(); it_nxtNds != nxtNds.end(); it_nxtNds++) // �ʱ� ���鿡�� ����
				{
					// CH -> {�ʱ� ���� ���} �޽��� ����
					Node* nextNode = (*it_nxtNds);
					int nextNodeID = nextNode->id;

					sec->setReceiver(cntlMsg, nextNode); // ���� ��� ����
					
					nodes[CHID]->txMsg(cntlMsg); // �۽�

					bool fwd = true;

					fwd = nextNode->rxMsg(cntlMsg); // ����
					
					if ( fwd == true && forwarded[nextNodeID] == false) // ���� ��� ��Ͽ� ���Ե� ���� ������ ��Ͽ� �߰�
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;				
					}					

				} // �ʱ� ���鿡�� ���� �Ϸ� // (2014.05.26) ������� Ȯ��!!

				while ( !fNodes.empty() ) // ���޳�� ��� �� ��帶�� �ݺ�
				{
					// �߰� ��� ->  {�߰� ���� ���} �޽��� ����

					 // ���� ��� ȹ��, ���� ��� ��Ͽ��� ���� 
					Node* cNode = fNodes.front(); 
					fNodes.pop_front();

					sec->genCntrlMsg(CLSTR_ACK, cNode); // ���� ��忡�� ���� �޽��� �� ����
					cntlMsg = sec->getCntrlMsg();
					// 18:14 ������� Ȯ��
					list<Node*> nxtNds = rtn->selNext(cntlMsg, cNode); // ���� ����� ���� ���� ��� ��� ���� �� ȹ��
					// 20:12 ������� Ȯ��
					list<Node*>::iterator it;

					for (it = nxtNds.begin(); it != nxtNds.end(); it++)  // ���� ����� ���� ���� ���鿡 ���� �ݺ�
					{
						Node* nextNode = *it;
						int nextNodeID = nextNode->id;

						/// �޽��� �ۼ���
						sec->setReceiver(cntlMsg, nextNode);
						cNode->txMsg(cntlMsg); // �۽�

						bool fwd = true; // ��� ���� ����
						fwd = nextNode->rxMsg(cntlMsg); // ����

						if (forwarded[nextNodeID]==false && fwd == true) // ���� ��带 ���� ��� ��Ͽ� �߰� (���Ǻ�)
						{
							forwarded[nextNodeID] = true;
							fNodes.push_back(nextNode);
						}		

					} // ���� Ŭ�������� CLSTR_ACK ���� �Ϸ�

				}
			}
		} // for �� ����
		*/
		// 4, 5�ܰ� �Ϸ� - ���� Ű ���� ����� ��쿡�� ����
		
		// Clstr_ACK �޽��� ���� �� �Ϸ�
		

		//kmr->testPairKey(); // pair key ���� Ȯ��

		/* 6. Ű �׷��� (��) ���� */
		
		cout << "/* 6. Ű �׷��� (��) ���� */" << endl;

		/*
		if (round > 0)
		{		
			kmr->dstrctKTree(); // ���� Ű Ʈ�� ����
		}
		*/

		if (round == 0) // ù ��° ������ ���, �ʱ� Ű Ʈ�� ����
		{
			kmr->cnstrctKTree(); // Ű Ʈ�� ����
		}

		// 
		else
		{
			kmr->updateKTree(addedNodes, deletedNodes); // Ű Ʈ�� ���� (�Ǵ� �籸��)
		}
		

		kmr->printKTree(); // Ű Ʈ�� ���
		//cout << endl;

		// (2014.06.12) ������� �Ϸ�!

		//*

		// ���� �� ��� �� ���
		/*
		cout << endl;

		for (int i = 0; i <= kmr->getKTreeHeight(); i++)
		{
			cout << "Level " << i << ": " ;
			cout << kmr->getNumNodesAtLevel(i) << endl;
		}

		/*/

		/* 7. NEW_LKH ���� ��� ���� ���� */
		
		cout << "/* 7. leaf node - ���� ��� ���� ���� ����*/" << endl;

		if (round == 0) // ù ��° ������ ���, �ʱ� ������ �׷� ����
		{
			kmr->cnstrctLBHG();
		}

		else
		{
			kmr->updateLBHG(); // ���� ������ ���, ������ �׷� ����
		}

		kmr->printKTree();

		rtn->testCHs();

		double res = rtn->testConect();

		cout << "\n round " << round << ": Percentage of isolated CHs: " << res << endl << endl;
		
		out_isolRatio << res << ",";

		///** ������� �� �θ� CH �׽�Ʈ 
		
		//sec_IHA->testAssoc(); // ���� ��� �׽�Ʈ		

		rtn->testParentCHs(); // �θ� CH ��� �׽�Ʈ
		
		///**

		/* 8. �׷� Ű ���� */ 				
		
		cout << " /* 8. �׷� Ű ���� */ " << endl;

		/// Ű ��й� �޽��� ����
		kmr->genKeyMsg(); // Ű ���� �޽��� ����		


		map<int, Node*>::iterator it_nd;

		for (it_nd = nodes.begin(); it_nd != nodes.end(); it_nd++) // Ű ��й� ����, ���� ���� ������ 'newbee' �÷��׸� false�� ���� - ���� ���� Ű ��й� ����
		{
			Node* pNode = it_nd->second;

			if (pNode == NULL) // (2014.09.22) 
			{
				continue;
			}

			pNode->newbee = false;
		}

		

		// (2014.07.31) ������� Ȯ��!!

		/// Ű ��й� �޽��� ����
		
		map<int, struct _tagKeyMsg*> keyMsgs = kmr->getKeyMsgs(); // Ű ��й� �޽��� map ȹ��

		map<int, KeyMsg*>::iterator it_km; // Ű ��й� �޽��� �ݺ���

		for (it_km = keyMsgs.begin(); it_km != keyMsgs.end(); it_km++)
		//for (unsigned int km = 0; km < keyMsgs.size(); km++) // ������ Ű ��й� �޽����� BS���� ���� ���� ���(��)���� ����
		{
			int km = (*it_km).first;

			
			// ���� ��� ���� ���� 
			fNodes.clear();
			forwarded.clear();

			// ���� ���� ��� - {���� ���} map ����

			map<int, list<Node*> > recipentMap;

			// �ʱ� ���� ��� ��� ȹ�� 
			KeyMsg* keyMsg = keyMsgs[km]; // Ű �޽��� ȹ��
			
			// (2014.08.07) debugging�� - Ű ��й� �޽����� ���� ��尡 2�� �̻��� ��� ���� ����
			if (keyMsg->recipents.size() > 1) 
			{
				;//cout << "keyMsg->recipents.size() > 1" << endl ; // do nothing (for debugging)
			}
			
			
			list<Node*> nxtNds = rtn->selNext(keyMsg, &bs); // BS�� ���� ���� ��� ��� ȹ�� 	// �ʱ� ���� ��� ��� ȹ��
			
			list<Node*>::iterator it;

			bool fwd = true;

			for (it = nxtNds.begin(); it!= nxtNds.end(); it++)
			{
				Node* nextNode = (*it);

				if (nextNode == NULL)
				{
					continue;
				}

				int nextNodeID = nextNode->id;

				// BS -> ��� ���� //

				bs.txMsg(keyMsg, nextNode); // BS�� �۽� �Լ� ȣ��
				//bs.txMsg(keyMsg); // BS�� �۽� �Լ� ȣ��
				
				bool fwd = true; // ��� ���� ����

				fwd = nextNode->rxMsg(keyMsg); // BS�� �̿� ����� ���� �Լ� ȣ��

				if (fwd == false)
				{
					//break;
					continue;
				}

				if ( fwd == true && forwarded[nextNodeID] == false) // ���� ��� ��Ͽ� ���Ե� ���� ������ ��Ͽ� �߰�
				{
					fNodes.push_back(nextNode);
					forwarded[nextNodeID] = true;
					kmr->storeKMsg(keyMsg, nextNode); // ���ŵ� Ű ��й� �޽��� ����	
				}
			}		

			// ��� -> ��� ���� 
			while( !fNodes.empty())
			{
				// ���� ��� ����� ù ��° ��� ȹ��
				Node* cNode = fNodes.front(); 
				fNodes.pop_front(); 

				kmr->restoreKMsg(keyMsg, cNode); // ������ ����Ǿ��� Ű ��й� �޽��� ���� (���� ��� ����)

				list<Node*> nxtNds = rtn->selNext(keyMsg, cNode); // ���� ����� ���� ���� ��� ��� ȹ��
				
				//cNode->txMsg(keyMsg); // ���� ����� �۽� �Լ� ȣ��

				list<Node*>::iterator it;

				for (it = nxtNds.begin(); it!= nxtNds.end(); it++)
				{
					Node* nextNode = (*it);

					if (nextNode == NULL) // (2014.09.30) 
					{
						continue;
					}

					int nextNodeID = nextNode->id;

					cNode->txMsg(keyMsg, nextNode); // ���� ����� �۽� �Լ� ȣ��

					bool fwd = true; // ��� ���� ����
					
					fwd = nextNode->rxMsg(keyMsg);				
					
					if (fwd == false)
					{
						break;
					}

					if ( fwd == true && forwarded[nextNodeID] == false) // ���� ��� ��Ͽ� ���Ե� ���� ������ ��Ͽ� �߰�
					{
						fNodes.push_back(nextNode);
						forwarded[nextNodeID] = true;
						kmr->storeKMsg(keyMsg, nextNode); // ���ŵ� Ű ��й� �޽��� ����	
					}
				}
				kmr->delRecvdKMsg(cNode); // ���� ����� ���ŵ� Ű ��й� �޽��� ����
			}
		} // for�� �Ϸ� - Ű ���� �޽��� ���� �Ϸ�
		
		// ... Ű ���� �޽��� ���� �Ϸ�

		rtn->resetUpdatdClstr(); // ������ Ŭ������ ���� ���� ����
		sec_IHA->resetUpdatedAssoc(); // ������ ���� ��� ���� ���� ����

		kmr->delKeyMsg(); // Ű ���� �޽��� ����
		

		kmr->resetUpdatedKeys();// key tree�� �� k-node ���� ���� �� �ʱ�ȭ �ʿ� (updated ��� ����)

		kmr->printReceivedNodes();
		kmr->resetReceivedNodes(); // Ű ��й� �޽��� ���� ��� ���� ����

		/// (2014.09.24) �̺�Ʈ ���� �߻� �� �� ��� ��� ���� 
		// 
		map<int, Node*>::iterator it_dpltdNode;

		for ( it_dpltdNode = nodes.begin(); it_dpltdNode != nodes.end(); it_dpltdNode++) // 
		{
			Node* node = (*it_dpltdNode).second;

			if (node == NULL) // (2014.09.22) 
			{
				continue;
			}

			if (node->depleted() == true) // ���� ����� �������� ���� ���
			{
				depletedNodes.insert(node->id); // �� ��� ��Ͽ� �ش� ����� ID �߰�
			} // �� ��带 ��Ͽ� �߰�

		} // �� ��� ��� ����

		depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();

		///  (2014.09.24) ���� �Ϸ�

		// 9. �̺�Ʈ ���� ���� 

		cout << " /* 9. �̺�Ʈ ���� ����  */ " << endl;

		int event_no = 0;
		
		numCutoffsInRound = 0; // (2014.09.24) ���� ���� �� cutoff Ƚ�� ����
		numEventsInRound = 0; // (2014.09.24) ���� ���� �� �̺�Ʈ �߻� Ƚ�� ����

		//map<int, set<int>> clusters;
		set<int> cluster;

		map<int, set<int>>::iterator it_clstr;

		Node* CoS;

		int chIdx=0;

		EvntMsg evntMsg; // �̺�Ʈ ����
	
		rtn->arrangeClusters(); // Ŭ������ ��� ���� (ũ�Ⱑ 0�� Ŭ������ ���� ����)
		clusters = rtn->getClusters(); // Ŭ������ ��� ȹ��		

		// (2014.08.31 ����) srcCluster ��� ����

		srcClusters.clear();

		for(it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++ ) // cluster ��� �� CH�� BS���� (T+1) ȩ �̻� ������ ��ġ�� Ŭ������ ������ ����
		{
			int CHID = (*it_clstr).first;
			set<int> cluster = (*it_clstr).second;

			// (2014.09.21) ���� - 		

			if ( nodes[CHID] == NULL) // CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
			{
				continue;
			}
		// (2014.09.21 ���� �Ϸ�) CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�

			//if ( nodes[CHID]->distance >= (INTERLEAVING_T+1) * TRANS_RANGE)
			
			if ( nodes[CHID]->x <= 50 ||  nodes[CHID]->x >= (FIELD_WIDTH-50) || nodes[CHID]->y <= 50 || nodes[CHID]->y >= FIELD_HEIGHT-50)			
			{
				srcClusters[CHID] = cluster;
			}

		}
		// (2014.08.31 ���� �Ϸ�) srcCluster ��� ���� �Ϸ�



		//kmr->testPairKey(); // pair key ���� �׽�Ʈ



		cutoffRatio = 0;

		//for (event_no = 0; event_no < 0; event_no++)
		//for (event_no = 0; event_no < 10; event_no++)
		for (event_no = 0; event_no < NUM_EVENTS; event_no++) // �� ���� �� NUM_EVENTS ��ŭ�� �̺�Ʈ �߻�
		{
			// �̺�Ʈ ���� ���� 
			 
			cutoffOccur = false;
			
			  // �̺�Ʈ �߻� - CH���� �ϳ��� �����ϰ� ����

			totNumEvents++; // �̺�Ʈ �߻� Ƚ�� +1 ����
			numEventsInRound++; // (2014.09.24) ���� ���� �� �̺�Ʈ �߻� Ƚ�� +1 ����

			if (totNumEvents % 500 == 0)
			{

				cout << "-periodic report-" << endl;
				cout << "event no. " << totNumEvents << " (" << numEventsInRound << ") " <<  endl;

				cout << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";

				cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;

				cout << "\t cutoff ratio: " << cutoffRatio << endl;

			}
			// Ŭ������ ��� ���� - ũ�Ⱑ 0�� Ŭ������ ������ Ŭ������ ��Ͽ��� ����			

			CoS = NULL;					
			
			//* (2014.08.31) Cos ���� ���� ���� (BS�κ����� �Ÿ��� t+1 �̻��� CH�� ����

			chIdx = gen2.genRandInt(srcClusters.size()) ; // �� ��° CH�� ������ �������� Ȯ�������� ����

			int t_chIdx = 0; // �ӽ� ����

			for(it_clstr = srcClusters.begin(); it_clstr != srcClusters.end(); it_clstr++ ) // �����ϰ� ���õ� CH ���� ȹ��
			{
				if ( (*it_clstr).second.size() ==0) // ũ�Ⱑ 0�̸� skip
				{
					continue;
				}

				if ( t_chIdx == chIdx )
				{		
					int CHID =  (*it_clstr).first;
					
					CoS = nodes[CHID];
					cluster = (*it_clstr).second;
				}

				t_chIdx++;
			}// for�� ���� - CoS ����			


			// CoS�� ������ �� ���� �˻�
			if ( CoS->depleted() == true) // CoS�� �������� ���� ���, cutoff �߻�
			{
				cutoffOccur = true; // CoS�� ������ ���� ���� cutoff �߻� - cutoff �߻� ���θ� '��'���� ����
				numCutoffsCoS++; // CoS�� ������ ���� ���� cutoff �߻�Ƚ�� +1 ����

				if (cutoff[CoS->id] == false) // �ش� Ŭ�������� cutoff �߻� ���θ� '��'���� ����, cutoff �߻� Ŭ������ �� +1 ����
				{
					cutoff[CoS->id] = true;
					numCutoffClusters++;
				}				

				// (2014.09.24 ����)
				depletedNodes.insert(CoS->id); // �� ��� ��Ͽ� �ش� ����� ID �߰�
			

				// ��ü ��忡 ���� �� ��� ���� ���� ���
				depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();
				
			
			} // if�� ���� - CoS�� �������� ���� ��� ó�� �Ϸ�

			Node* cur = CoS; // ���� ��� ����		

			if (cutoffOccur == false) // (2014.09.24) CoS���� cutoff�� �߻����� ���� ��쿡�� ���� ����, ���� ���� ������ ����
			{				

				// ���� ���� 

				sec->initEventMsg(&evntMsg, CoS); // �̺�Ʈ ���� �ʱ�ȭ
				//sec->writeEventMsg(out_event, totNumEvents, &evntMsg); // ���Ͽ� �̺�Ʈ ���� ���� ���

				// ���� MAC ����

				list<MAC*>::iterator itMAC = evntMsg.macs.begin(); 

				for (itMAC; itMAC != evntMsg.macs.end(); itMAC++) // �̺�Ʈ ���� �� ���� MAC�� ���� �޸� ��ȯ
				{
					MAC* mac = *itMAC;

					if (mac != NULL)
					{
						delete (mac);
					}
				} // FOR�� ���� - ���� MAC�� ���� �޸� ��ȯ �Ϸ�

				evntMsg.macs.clear(); // �̺�Ʈ ���� �� ���� MAC ��� ����
				//sec->resetTmpLst(); // ���� �ӽ� MAC ��� ����

				/// ���� ���� ��� ����

				Node* next = rtn->selNext(&evntMsg, cur); // Rtn - ���� ��� ����

				evntMsg.receiver = next; // ���� ��� ����

				/// ���� ���� ��带 ����Ͽ� ���� �� MAC ����
				sec->setMAC(&evntMsg, CoS->id); 

				// ���� ���� ������ ���� �����ϰ� ���� MAC ����

				if ( gen3.genRandInt(100) < FTR) // FTR ������ŭ Ȯ�������� ���� ���� ���� �߻�
				{
					atk->forgeEvntMsg(&evntMsg); // Atk - ���� ���� ���� �߻�	
					//cout << "false report" << endl;
				}
				else
				{
					;//cout << "normal report" << endl;
				}


				sec->writeEventMsg(out_event, totNumEvents, &evntMsg); // ���Ͽ� �̺�Ʈ ���� ���� ���

				// (2014.08.20) ������� Ȯ��!!

				// 10. ���� ���� �� ���͸� 		

				//cout  << "/* 10. ���� ���� �� ���͸� 	*/ " << endl;

				while (next != NULL) // ���� ���� ��尡 NULL�� �ƴϸ� �ݺ�
				{
					//sec->resetTmpLst(); // ���� �ӽ� MAC ��� ����

					

					if (cutoffOccur == true) // �̺�Ʈ ���� ���� �� cutoff �߻��ϸ� while�� Ż��
					{
						break; //  while �� Ż��
					}

					cur->txMsg(&evntMsg); // ���� ����� �۽� �Լ� ȣ��

					if ( next->rxMsg(&evntMsg) == false) // ���� ����� ���� �Լ� ȣ�� - ��ȯ���� ���� ��� ���� ���θ� ����
					{
						fwd = false; // ��� ���� �� ��
					}
					else
					{
						fwd = true; // ��� ����
					}			

					cur = next; // ���� ��� ����					

					if (fwd == false) // ���� �Լ� ��ȯ���� false�̸� ������ �ߴ���
					{
						//evntMsg.macs.clear();
						//sec->resetTmpLst(); // ���� �ӽ� MAC ��� ����
						break;
					}

					evntMsg.sender = cur; // �۽� ��� ����

					next = rtn->selNext(&evntMsg, cur); // ���� ���� ��� ����
					evntMsg.receiver = next; // ���� ��� ����			

					if (next == NULL) // (2014.08.28 ����)
					{
						break;
					}

					sec->updateMAC(&evntMsg); // ���ŵ� ���� ����� MAC ���� �Լ� ȣ��

					sec->writeEventMsg(out_event, totNumEvents, &evntMsg); // ���Ͽ� �̺�Ʈ ���� ���� ���

					if (next->depleted() == true) // ���� ����� �������� ���� ���, ���� ���� ����			
					{					
						cutoffOccur = true; // �߰������ ������ ���� ���� cutoff �߻� - cutoff �߻� ���θ� '��'���� ����
						//numCutoffsFwd++; // (2014.09.30) �߰� ����� ������ ���� ���� cutoff �߻� Ƚ�� +1 ����

						if (cutoff[CoS->id] == false) // �ش� Ŭ�������� cutoff �߻� ���θ� '��'���� ����, cutoff �߻� Ŭ������ �� +1 ����
						{
							cutoff[CoS->id] = true;
							numCutoffClusters++;
						}				

						depletedNodes.insert(next->id); // (2014.09.24) �� ��� ��Ͽ� ���� ��� ID �߰�
						
						break; // ���� ������ ���� �ݺ��� Ż��
					} // ���� ��� ������ �� ��, ���� ���� ����

				} // �̺�Ʈ ���� ���� �Ϸ�

				// ������ ���� �ִ� MAC�� �޸𸮸� ��ȯ�ϰ� ��Ͽ��� ����

				totalNumVer += evntMsg.numVer; // ���� ���� Ƚ�� ����				
			
				/*
				if ( evntMsg.detected == true)
				{
					avgHopCntDetectedER+= evntMsg.hop; // ����� ���� ������ ȩ �� ����
				}

				if ( evntMsg.detected == true || (cur->distance) < TRANS_RANGE ) // ���� ���� �������� ������ ����Ǿ��ų� BS���� ���� �Ǿ��ٸ� ��� ���� ȩ ���� ����
				//if ( sec->isAtkDtctd()== true || (cur->distance) < TRANS_RANGE ) // ���� ���� �������� ������ ����Ǿ��ų� BS���� ���� �Ǿ��ٸ� ��� ���� ȩ ���� ����
				{
					countedEvents++; // ��� ���� ȩ ���� �ݿ��� ���� ���� +1 ����
					avgHopCnt += evntMsg.hop;	// ���� ���� ȩ �� ����
				}

				// ������ ������� �ʰ� ���� ��尡 BS���� ���� ���� �̳��̸� BS���� ����

				if ( evntMsg.detected == false && (cur->distance) < TRANS_RANGE )
				{
					cur->txMsg(&evntMsg);	
					rtn->numEvntRcvd++; // BS���� ���ŵ� ���� ���� +1 ����
				}
				*/


			} // (2014.09.24 ���� �Ϸ�) CoS���� cutoff �� �߻����� ���� ��쿡 ���� ó��
			
				if (evntMsg.detected == true) // ������ ����� ���
				{
					countedEvents++; // ��� ���� ȩ ���� �ݿ��� ���� ���� +1 ����
					avgHopCnt += evntMsg.hop;	// ���� ���� ȩ �� ����
					avgHopCntDetectedER+= evntMsg.hop; // ����� ���� ������ ȩ �� ����
				}
				else // ������ ���� �� �� ���
				{
					if (cur->distance < TRANS_RANGE)
					{
						avgHopCnt += evntMsg.hop;	// ���� ���� ȩ �� ����
						//avgHopCntDetectedER+= evntMsg.hop; // ����� ���� ������ ȩ �� ����

						countedEvents++; // ��� ���� ȩ ���� �ݿ��� ���� ���� +1 ����

						cur->txMsg(&evntMsg); // BS���� ���� ����
						rtn->numEvntRcvd++; // BS���� ���ŵ� ���� ���� +1 ����
					}
					else // ������ ������� �ʾ������� BS���� �������� ���� ����
					{
						numCutoffs++; //
						numCutoffsInRound++;
						numCutoffsFwd++; // (2014.09.30) �߰� ����� ������ ���� ���� cutoff �߻� Ƚ�� +1 ����

						if (cutoffOccur == false)
						{
							numCutoffsIsol++; // (2014.09.30) ����� ������ ���� cutoff �߻� Ƚ�� +1 ����
						}

						depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();

						cutoffRatio = (double) numCutoffsInRound / (double) numEventsInRound; // (2014.09.24) ���� ���� ������ �߻��� �̺�Ʈ �� cutoff�� ���� ���

						if (noCutoff == true)
						{
							noCutoff = false;
							firstCutoff = totNumEvents; // first cutoff ���		
						}
						else if ( cutoffRatio >= Exit_Cond)
						{
							cout << "\n****Current round exit condition****" << endl;

							lastCutoff = totNumEvents; // last cutoff ���

							cout << endl << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";
					
							cout << ", event no.: " << totNumEvents << "( " << numEventsInRound << ")" ;				

							cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;

							cout << "\t cutoff ratio: " << cutoffRatio << endl;
							break;
						}

					} // if-else �� ���� - ������ ������� ���� ���, BS���� ������ ���� �׷��� ���� ��� (cutoff) ó�� �Ϸ�

				} // IF-ELSE �� ���� - ������ ����� ���� �׷��� ���� ��� ó�� �Ϸ�


			
			/*
			if (cutoffOccur == true) // ���� �̺�Ʈ ������ ���� cutoff�� �߻��� ���, cutoff ���� ��ǥ ����
			{
				numCutoffs++;
				numCutoffsInRound++;

				//cout << endl << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";

				//cout << ", event no.: " << totNumEvents << "( " << numEventsInRound << ")" ;

				depletNodeRatio = ( (double) depletedNodes.size()) / nodes.size();

				//cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;
				
				
				cutoffRatio = (double) numCutoffsInRound / (double) numEventsInRound; // (2014.09.24) ���� ���� ������ �߻��� �̺�Ʈ �� cutoff�� ���� ���
				//cutoffRatio = numCutoffs / totNumEvents;

				//cout << "\t cutoff ratio: " << cutoffRatio << endl;

				if (noCutoff == true)
				{
					noCutoff = false;
					firstCutoff = totNumEvents; // first cutoff ���		
				}
				else if ( cutoffRatio >= Exit_Cond)
				{
					lastCutoff = totNumEvents; // last cutoff ���

					cout << endl << numCutoffs << "( " << numCutoffsInRound << ") cutoffs occur: ";
					
					cout << ", event no.: " << totNumEvents << "( " << numEventsInRound << ")" ;				

					cout << "\n\t depleted Node ratio: " << depletNodeRatio<< endl;

					cout << "\t cutoff ratio: " << cutoffRatio << endl;
					break;
				}
				
			} // cutoff ���� ���� ��ǥ ���	, ���� ����� �̵�
			*/			
			

		} // for - �̺�Ʈ �߻� ����

		// 11. ���� ���� ���� Ȯ�� 			

		//break; // (2014.10.20) ���� ���� - �̺�Ʈ �߻� �� �ܰ������ ������ �Ҹ� ���� ����

		if ( (++round > NUM_ROUNDS) )// cutoff ������ ���� ������ �Ѿ�ų� ��� ���带 ������ ���, �ùķ��̼� ����)
		//if ( (lastCutoff > 0) || (++round > NUM_ROUNDS) )// cutoff ������ ���� ������ �Ѿ�ų� ��� ���带 ������ ���, �ùķ��̼� ����)
		{
			break;
		}
		
		else
		{
			// ���� ����� �����ϱ� �� ������ �ű� ���� ���θ� false�� ������ (Ű ��й� ����)

			for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
			//for (unsigned int nd = Node::numNodesEvctd; nd < Node::totNumNodes; nd++) 
			{
				Node* pN = (*it_node).second;

				if (pN == NULL)
				{
					continue;
				}

				//Node* pN = nodes[nd];
				pN->newbee = false; // 
			}
		}
		
	} //  round ����
	
	out_event.close(); // �̺�Ʈ ���� ��� ���� �ݱ�
	out_isolRatio.close(); // (2014.10.31) �� ��� (�Ǵ� CH) ���� ��� ���� �ݱ�

	/* 12. ��� ��ǥ ����, ��� */

	/// ���� ���� ����

	int numAtk = atk->getNumAtk();
	int numDtc = sec->getNumDtctn();

	if (numDtc > 0)
	{
		avgHopCntDetectedER /= (double) numDtc;
	}
	

	/// �̺�Ʈ �߻� Ƚ�� - ��ü, �κ� (BS ����, ����, CUTOFF)

	cout << "\ntotal number of events: " << totNumEvents << endl;	

	cout << "\n 1) BS���� ���ŵ� �̺�Ʈ ���� ����: " << rtn->numEvntRcvd << endl;

	cout << " 2) num detected reports: " << numDtc << endl;

	cout << " 3) num cutoffs: " << numCutoffs << endl;

	cout << " 3-1) cutoff (CoS) " << numCutoffsCoS << endl;

	cout << " 3-2) cutoff (Fwd) " << numCutoffsFwd-numCutoffsCoS << "( cutoff (isol) = " << numCutoffsIsol << " )" << endl;

	cout << "������ ������ �� ��� �� ����" << depletNodeRatio << endl; // (2014.12.12 �߰�)


	/// ��� ���� ȩ ��

	avgHopCnt /= countedEvents; // �̺�Ʈ ���� ��� ���� ȩ �� ��� �� ���
	
	
	cout << "\n countedEvents: " << countedEvents << endl;

	cout << "\n���� ��� ���� ȩ ��" << avgHopCnt;

	/// ������ ������ �Ҹ� ���

	nodes = arrangeNodes(nodes); // node ��� ���� ( Value = NULL �� ��� ����)

	int totalNumNodes = nodes.size() + delNodes.size();

	cout << "\n ��ü ��� ��: " <<  totalNumNodes;
	cout << "\n �� ������ �Ҹ�" << Node::totEnrgCnsmptn << ", ��� ������ �Ҹ�" << Node::totEnrgCnsmptn / totalNumNodes << endl;

	/// ��Ʈ��ũ ����	


	cout << "first cutoff: " << firstCutoff;
	//cout << ", last cutoff: " << lastCutoff;

	
	/// ���� ����

	cout << "\n num. of false reports: " << numAtk << endl;	

	if (numDtc >0)
	{
		cout << " avg hop count of detected false reports: " << avgHopCntDetectedER << endl;
	}
	/* 13. ���� */

	// ��� ��ü �޸� ��ȯ

	map<int, Node*>::iterator it_nodes;

	for (it_nodes = nodes.begin(); it_nodes!= nodes.end(); it_nodes++)
	{
		Node* pNode = it_nodes->second;

		if (pNode != NULL)
		{
			delete (pNode);
		}
	}

	nodes.clear(); // ��� �� ��� ����

	for (it_nodes = delNodes.begin(); it_nodes!= delNodes.end(); it_nodes++)
	{
		Node* pNode = it_nodes->second;
		delete (pNode);
	}
	 
	delNodes.clear(); // ��� �� ��� ����


	char temp_c;
	cin >> temp_c; // ���α׷��� ����� ��忡�� �ٷ� ����Ǵ� ���� ����

	return 0;
	


} // main �Լ� �Ϸ�

void deployNode ( Node* pNode, int xMin, int xMax, int yMin, int yMax)
{	
	int rand_x = xMin + gen1.genRandInt(xMax - xMin);
	int rand_y = yMin + gen1.genRandInt(yMax - yMin);

	pNode->x = rand_x;
	pNode->y = rand_y;

	pNode->distance = sqrt( pow( (double) pNode->x - BS_X , 2) + pow ( (double) pNode->y - BS_Y , 2) );
	
}

void testNode(map<int, Node*> nodes)
{
	ofstream out("tst_nodes.txt");

	map<int, Node*>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++)
	{
		Node* pNode = NULL;
		pNode = it->second;

		if (pNode ==NULL) // (2014.09.22) 
		{
			continue;
		}

		out << "node ID: " << it->first;
		//cout << "node ID" << it->first;

		out << "cell ID: " << pNode->cell << endl;

		out << "\t (  " << pNode->x << ", " << pNode->y << ") " ;
		//cout << "\t ( << " << pNode->x << ", " << pNode->y << ") \n" ;



		out << ", distance: " << (int) pNode->distance << endl;

		if ( pNode->cmprmsd == true)
		{
			out << "compromised " << endl;
		}
	}

	out.close();
}

map<int, Node*> arrangeNodes( map<int, Node*> nodes) // nodes map �� ���Ե� ��ҵ� �� Node �����Ͱ� NULL�� ��ҵ� ����
{
	set<int> entitiesToBeDeleted;

	map<int, Node*>::iterator it;
	set<int>::iterator it_set;

	for (it = nodes.begin(); it != nodes.end(); it++) // nodes map�� ��ҵ� �� value = NULL �� ��Ұ� ������ �ش� key�� ���� ��Ͽ� �߰�
	{
		Node* node = (*it).second;

		if (node == NULL)
		{
			entitiesToBeDeleted.insert((*it).first);
		}
	}//  for�� - // nodes map�� ��ҵ� �� value = NULL �� ��Ұ� ������ ���� ��Ͽ� �߰�

	for (it_set = entitiesToBeDeleted.begin(); it_set != entitiesToBeDeleted.end(); it_set++)
	{
		int key = *it_set;

		nodes.erase(key);
	}

	return nodes;
}