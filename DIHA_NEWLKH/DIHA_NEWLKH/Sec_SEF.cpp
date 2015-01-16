// 2014.05.27
// ������
// Sec_SEF.cpp
// Sec_SEF Ŭ���� ����


#include "Sec_SEF.h"
#include <iostream>

using namespace std;

void Sec_SEF::setMAC (EvntMsg* msg)
{
	// ���� �ʿ� - ��� iD�κ��� Ŭ������ ID ���ϴ� ����

	/*
	int clstrID = (msg->CoS->id) / NODES_PER_CLSTR; // (2012.11.26 ����) Ŭ�������� ID�� ��� id�� Ŭ������ �� ��� ���� ���� ��

	setMAC(msg, clstrID);
	*/
}

void Sec_SEF::setMAC (EvntMsg* msg, int clstrID) // (2014.08.20)
{
	// ������ ���� �ִ� MAC ���� (�޸� ��ȯ)

	list<MAC*>::iterator itMAC = msg->macs.begin();

	for ( itMAC; itMAC!= msg->macs.end(); itMAC++)
	{
		MAC* mac = *itMAC;
		delete mac;
	}

	msg->macs.clear(); 

	// hop count�� 0���� Ȯ���ϰ� �׷��� �ʴٸ� �Լ� ��ȯ

	if (msg->hop != 0)
	{
		return;
	}

	// ���� ������ �����ϴ� ������ ID Ȯ�� ( ��Ȳ�� ���� t+1 ���� ���� ���� ���� �� ����)

	list<int> sourceNodes; // ���� ������ �����ϴ� ������ ID ���	

	set<int> clstrNodes = rtn->getClstr(clstrID);  // �Ҽ� ������ ID �߰� (�ִ� T��)

	set<int>::iterator it_clstrNodes;

	int idx = 1; // ���� ������ �����ϴ� �Ϲ� (�� CH) ������ �ε���

	for (it_clstrNodes = clstrNodes.begin();  it_clstrNodes != clstrNodes.end(); it_clstrNodes++) // Ŭ������ �Ҽ� ���� �� ���� ������ �����ϴ� ������ ��Ͽ� ����
	{
		int nodeID = *it_clstrNodes;

		if ( nodeID == clstrID)
		{
			continue;
		}

		sourceNodes.push_back(nodeID);

		if (idx >= INTERLEAVING_T) // ���� ������ �����ϴ� ����� ���� T+1���� �Ǹ� �ݺ��� Ż��
		{
			break;
		}

	} // for�� - ���� ������ �����ϴ� ������ ��Ͽ� ����

	sourceNodes.push_back(clstrID); // CH�� id �߰�

	msg->prevNodes = sourceNodes;// �̺�Ʈ ���� ���� ���� ��� ���� ����

	if (msg->receiver == NULL) // �޽��� ���� ��尡 �������� ������ �Լ� ����
	{
		return;
	}


	// ���� ������ �����ϴ� ���� ������ ���� �ݺ�

	list<int>::iterator it_sourceNode;

	for (it_sourceNode = sourceNodes.begin(); it_sourceNode != sourceNodes.end(); it_sourceNode++) // ���� ���� ���� ��� ��ȸ - MAC ���� �� ����
	{
		int sourceNodeID = *it_sourceNode;

		// ���� ����� ���� ���� ��� ��� ȹ��
		set<int> upNodes = upperNodes[sourceNodeID][INTERLEAVING_T +1];

		set<int>::iterator it_upNode;

		for (it_upNode = upNodes.begin(); it_upNode != upNodes.end(); it_upNode++) // ���� ����� �� ���� ���� ��忡 ���� �ݺ�
		{

			int upNodeID = *it_upNode;

			PairKey* pk = kmr->getPairKey(sourceNodeID, upNodeID); // pairwise key ȹ��

			MAC* mac = new MAC; // pairwise MAC ���� �� �߰�

			mac->origin = nodes[sourceNodeID]; // MAC ���� ��� ����
			mac->pKey = pk; // MAC�� pairwise key ����
			mac->code = pk->kVal + msg->val; // �޽��� ���� �ڵ� ����

			msg->macs.push_back(mac); // ���� �� MAC ��Ͽ� �ű� MAC �߰�

			// ���� ����� MAC �������� ���� ������ �Ҹ� ����

				nodes[sourceNodeID]->enrgyDiff = hashEnrg; // ����� ���� ������ �Ҹ�
				nodes[sourceNodeID]->eCnsmptn += nodes[sourceNodeID]->enrgyDiff; // (2013.10.04) MAC ��꿡 ���� ������ �Ҹ� ����
				Node::totEnrgCnsmptn += nodes[sourceNodeID]->enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)

				nodes[upNodeID]->remEnrgy -= nodes[upNodeID]->enrgyDiff; // (2013.10.04) MAC ��꿡 ���� �ܿ� ������ ����

		} // for�� - ���� ����� MAC ���� �� ����	



	} // for�� - ���� ���� ���� ��� ��ȸ - MAC ���� �� ���� �Ϸ� 


	/*
	// ������ ���� �ִ� MAC ��� ����
	
	list<MAC*>::iterator itMAC = msg->macs.begin();

	for ( itMAC; itMAC!= msg->macs.end(); itMAC++)
	{
		MAC* mac = *itMAC;
		delete mac;
	}

	msg->macs.clear(); 

	// Ŭ������ �� �ּ� ID, �ִ� ID ���
	if (msg->hop == 0) // // hop count�� 0�� ���� ���� �� MAC ����
	{
		int minID = clstrID * NODES_PER_CLSTR; // Ŭ�������� ��� �� �ּ� ID
		int maxID = minID + NODES_PER_CLSTR - 1; // Ŭ�������� ��� �� �ִ� ID

		// �ּ� id ���� t+1���� ���鿡 ���� ���� ���� �ݺ�
		for (int i = 0; i < IHA_T+1; i++)
		{
			int cNodeID = minID + i; // ���� ����� ID

			IDLIST upNodes = upperNodes[cNodeID][0]; // ���� ����� ���� ���� ��� ��� ȹ��

			IDLIST::iterator it = upNodes.begin();

			for (it; it !=upNodes.end(); it++) // ���� ����� �� ���� ���� ��忡 ���� �ݺ�
			{
				int upNodeID = *it; // ���� ���� ��� ID

				PairKey* pk = kmr->getPairKey(cNodeID, upNodeID); // pairwise key ȹ��
				MAC* mac = new MAC; // pairwise MAC ���� �� �߰�

				mac->origin = (nodes + cNodeID); // (2013.09.02) MAC ���� ��� ����
				mac->pKey = pk; // MAC�� pairwise key ����
				mac->code = pk->kVal + msg->val; // �޽��� ���� �ڵ� ����

				msg->macs.push_back(mac); // ���� �� MAC ��Ͽ� �ű� MAC �߰�


				// (2013.10.31) �׽�Ʈ�� - �ӽ÷� �ּ� ó�� 
				nodes[cNodeID].enrgyDiff = hashEnrg; // ����� ���� ������ �Ҹ�
				nodes[cNodeID].eCnsmptn += nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC ��꿡 ���� ������ �Ҹ� ����
				Node::totEnrgCnsmptn += nodes[cNodeID].eCnsmptn; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)

				nodes[cNodeID].remEnrgy -= nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC ��꿡 ���� �ܿ� ������ ����
				
			}

			if (cNodeID == rtn->getCH(clstrID)->id ) // ���� ��尡 CH�̸� BLOOM filter ��뿡 ���� �߰����� ������ �Ҹ� ����
			{
				int numMACs = msg->macs.size();

				// (2013.10.31) �׽�Ʈ�� - �ӽ÷� �ּ� ó��
				nodes[cNodeID].enrgyDiff = (numMACs * bloom_filter_k) * hashEnrg; //(2013.10.04) MAC ���� * bloom_filter_k Ƚ����ŭ �ؽ� ���� ����
				nodes[cNodeID].eCnsmptn += nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC ������ ���� ������ �Ҹ� ����
				Node::totEnrgCnsmptn += nodes[cNodeID].eCnsmptn; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)

				nodes[cNodeID].remEnrgy -= nodes[cNodeID].enrgyDiff; // (2013.10.04) MAC ������ ���� �ܿ� ������ ����
				
			}

		}
	}	
	*/
}


bool Sec_SEF::verMAC(EvntMsg* msg) // (2013.08.20)
{
	bool vResult = true;

	// ���� �� MAC�� �ϳ��� �˻�	

	std::list<MAC*>::iterator it = msg->macs.begin();

	for (it; it != msg->macs.end(); it++)
	{
		MAC* mac = *it; // ���� �˻� ���� MAC

		PairKey* pKey = mac->pKey; // pairwise key

		// MAC ���� ���� ���� ��尡 ���� ���迡 �ִ��� �˻�

		Node* origin = mac->origin;
		Node* receiver = msg->receiver;

		if ((pKey->nID == origin->id && pKey->nID2 == receiver->id) || (pKey->nID == receiver->id && pKey->nID2 == origin->id))
		{
			// MAC ���� ���� ���� ���� ���� ���迡 �ش���

			// MAC �ڵ� ����
			msg->numVer++; // (2013.10.29) ���� Ƚ�� ����

			int cNodeID = msg->receiver->id;			
			
			nodes[cNodeID]->enrgyDiff = hashEnrg; // MAC ��� 1ȸ

			nodes[cNodeID]->eCnsmptn += nodes[cNodeID]->enrgyDiff;
			Node::totEnrgCnsmptn += nodes[cNodeID]->enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)
			nodes[cNodeID]->remEnrgy -= nodes[cNodeID]->enrgyDiff;			
			

			int correctCode = pKey->kVal + msg->val; // ���� �ڵ�

			if (mac->code == correctCode) // ���� ����
			{
				// �ƹ��͵� �� ��
			}
			else // ���� ���� (���� ���� Ž��)
			{
				vResult = false;
				break;
			}
		}
	}

	return vResult; // ���� ��� ��ȯ
	
}


void Sec_SEF::updateMAC(struct _tagEvntMsg* msg) // (2014.08.20)
{
	int cNodeID = msg->sender->id;	
	
	// ���� �� id ��� ����
	msg->prevNodes.pop_front(); // T+1 ȩ ���� ����� id ����
	msg->prevNodes.push_back(msg->sender->id); // �۽� ����� ID �߰�


	// �ڽŰ� ���� ���� ��尡 �����ϴ� Ű�� MAC�� �����ϰ� ���� �� mac ��Ͽ� �߰�
	
	// (2014.04.25 ���� - ����/���� ���� ��� �ڷ��� ���� - array of list -> map 

	std::set<int> upNodes = upperNodes[cNodeID][INTERLEAVING_T+1]; // �ش� ����� (T+1) ȩ ���� ��� ���

	std::set<int>::iterator itID2 = upNodes.begin();
	

	for (itID2; itID2 != upNodes.end(); itID2++)
	{
		int upNodeID = *itID2; // ���� ���� ��� ID
		PairKey* pk = kmr->getPairKey(cNodeID, upNodeID); // pairwise key ȹ��
		MAC* mac = new MAC; // pairwise MAC ���� �� �߰�

		mac->origin = nodes[cNodeID]; // (2013.09.02) MAC ���� ��� ����
		mac->pKey = pk; // MAC�� pairwise key ����
		mac->code = pk->kVal + msg->val; // �޽��� ���� �ڵ� ����

		msg->macs.push_back(mac); // ���� �� MAC ��Ͽ� �ű� MAC �߰�
		//tmplst.push_back(mac); // �ӽ� ��Ͽ� �ű� MAC �߰�
		
	}

	/*
	msg->macs.clear(); // ���� �� MAC ��� ���� ����

	//�ӽ� ����� ��ҵ��� ���� �� MAC ��Ͽ� �߰�
	std::list<MAC*>::iterator itMAC2 = tmplst.begin();

	for ( itMAC2; itMAC2 != tmplst.end(); itMAC2++)
	{
		MAC* mac = *itMAC2;
		msg->macs.push_back (mac);
	}
	
	tmplst.clear();// �ӽ� ��� �� �ʱ�ȭ
	*/

	/*  MAC ���� ���������� ������ �Ҹ� (���� �ʿ� �� ���) */
	
	nodes[cNodeID]->enrgyDiff = hashEnrg; //(2014.08.21)  �ؽ� ���� ����
	nodes[cNodeID]->eCnsmptn += nodes[cNodeID]->enrgyDiff; // (2013.10.04) MAC ������ ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += nodes[cNodeID]->enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)

	nodes[cNodeID]->remEnrgy -= nodes[cNodeID]->enrgyDiff; // (2013.10.04) MAC ������ ���� �ܿ� ������ ����
	
}


void Sec_SEF::doEventMsgProc(struct _tagEvntMsg* msg) // (2014.08.21) ���ŵ� �̺�Ʈ ���� ó��
{	
	
	/// ���� ��忡 ���� ������ MAC�� �̺�Ʈ �������� ���� */
		

	int cNodeID = msg->receiver->id;
	
	std::list<MAC*>::iterator itMAC = msg->macs.begin();


	//list<MAC*> tmplst;

	for (itMAC; itMAC != msg->macs.end(); itMAC++) // �̺�Ʈ ������ ���Ե� mac�� �ϳ��� �˻�	
	{
		MAC* mac = *itMAC; // ���� �˻����� MAC

		//IDSet::iterator itID = lowerNodes[cNodeID][INTERLEAVING_T].begin(); // ���� ���� ����� ���� ���� ��� ����� iterator		

		bool matched = false; // MAC ���� ��尡 ���� ���� ���� ������ ��� true�� ����

		PairKey* pKey = mac->pKey; // pairwise key

		// MAC ���� ���� ���� ��尡 ���� ���迡 �ִ��� �˻�

		Node* origin = mac->origin;
		Node* receiver = msg->receiver;

		if ((pKey->nID == origin->id && pKey->nID2 == receiver->id) || (pKey->nID == receiver->id && pKey->nID2 == origin->id))
		{
			// MAC ���� ���� ���� ���� ���� ���迡 �ش���. �ش� MAC�� ���� �� ��Ͽ��� ����

			/*
			matched = true;
			delete (mac);// �ش� MAC ��ȯ	
			*/

			tmplst.push_back(mac); // (2014.08.25) ������ MAC�� �ӽ� ��Ͽ� �߰��Ͽ��ٰ� �̺�Ʈ ���� �� mac ��Ͽ��� �����ϰ� �޸� ��ȯ
		}	

		else
		{
			//tmplst.push_back(mac); // ������ MAC�� ������ ������ MAC�� �ӽ� ��Ͽ� �߰� (updateMAC() �Լ����� ���)
		}		
	}	

	// (2014.08.25 ����) ������ MAC�� �޸� ��ȯ, ��Ͽ��� ����

	
	if ( !tmplst.empty() ) // ������ MAC�� ���� - �̺�Ʈ ���� �� MAC ��Ͽ��� ���� �տ� ��ġ�� mac
	{
		MAC* mac = * (msg->macs.begin()); // ���� �տ� ��ġ�� MAC�� �޸𸮸� ��ȯ�ϰ� ��Ͽ��� ����

		delete mac;

		msg->macs.pop_front();
	}
	
	tmplst.clear(); // �ӽ� MAC ��� ����
	
}

void Sec_SEF::writeEventMsg(ofstream& out, int event_no, struct _tagEvntMsg* msg) // (2014.08.11) ������ �̺�Ʈ ���� �׽�Ʈ (���Ͽ� ���)
{
	out << "\nevent_no: " << event_no << endl;
	
	out << "\t (x, y) = (" << msg->x << ", " << msg->y << "), "; // ��ǥ

	out << " value = " << msg->val << endl; // �̺�Ʈ ��

	out << "\t ���� ȩ �� = " << msg->hop << endl; // ���� ȩ ��

	out << "\tsender: " << msg->sender->id ; // �۽� ���

	if (msg->receiver != NULL) // ���� ���
	{
		out << ", receiver: " << msg->receiver->id ;
	}

	out << "\n\n\tprev nodes: { " ; // ���� ������ ID

	std::list<int>::iterator it_prevNodes;

	for (it_prevNodes = msg->prevNodes.begin(); it_prevNodes != msg->prevNodes.end(); it_prevNodes++) // ���� ��� ���� ���
	{
		int nodeID = *it_prevNodes;

		out << nodeID << ", ";

	} // for �� ���� - ���� ��� ���� ��� �Ϸ�

	out << "}" << endl; 

	std::list<MAC*>::iterator it_MAC;

	out << "\n\t MACs: " << endl;

	int mac_cnt = 0;

	for ( it_MAC = msg->macs.begin(); it_MAC != msg->macs.end(); it_MAC++) // MAC ���� ���
	{
		MAC* mac = *it_MAC;

		out << "\n\t(" << mac_cnt << ")";

		Node* origin = mac->origin;

		out << "\n\torigin = " << origin->id;

		PairKey* pk = mac->pKey;

		out << "\n\tnID = " << pk->nID;
		out << "\n\tnID2 = " << pk->nID2;		
		out << "\n\tcode = " << mac->code;

		// MAC ���� ���
	} //for �� ���� - MAC ���� ��� �Ϸ�
	
	out << "\n\tnumVer: " << msg->numVer;

	out << endl;		// ���� �� MAC ���� Ƚ��
		
}

void Sec_SEF::genCntrlMsg(ctrlMsgType type, BS* bs) // (2014.05.20 �߰�) BS���� ���� �޽��� ����
{
	if (bs == NULL)
	{
		cout << "(bs == NULL)" << endl;
		exit(-1);
	}	

	// ���� �޽��� Ÿ�� ����

	cntlMsg->cmType = type; 
	cntlMsg->sender = NULL;
	cntlMsg->receiver = NULL;

	// ���� �޽����� ����� ID ��� ����
	for (int i = 1; i <= INTERLEAVING_T+1; i++)
	{
		//IDSet idSet = cntlMsg->IDSets[i];

		//if ( !idSet.empty() )
		{
			cntlMsg->IDSets[i].clear(); 
		}
	}

	// ���� ��� ����� �߰� ��忡�� �߰��� - BS������ �� �ʱ�ȭ�� ����
}

void Sec_SEF::genCntrlMsg(ctrlMsgType type, Node* cNode) // (2013.08.23 �߰�) ��忡�� ���� �޽��� ����
{
	if (cNode == NULL)
	{
		cout << "(cNode == NULL)" << endl;
		exit(-1);
	}

	/* Ű �޽��� ���� �Լ� ���� */

	// ���� �޽��� Ÿ�� ����
	this->cntlMsg->cmType = type; 
	cntlMsg->sender = NULL;
	cntlMsg->receiver = NULL;

	// ���� �޽����� ����� ID ��� ����
	for (int i = 0; i <= INTERLEAVING_T+1; i++)
	{
		cntlMsg->IDSets[i].clear(); 
	}

	cntlMsg->sender = cNode; // �۽� ��� �ּ� ����

	int idx = cNode->id; 
	// 14:57 ������� Ȯ��
	if ( type == BS_HELLO) // BS_HELLO �޽����� ���
	{
		// ��� ID ��� ����
	
		for (int i = 2; i <= INTERLEAVING_T+1; i++)
		{
			IDSet upNdsSet = upperNodes[idx][i-1]; // i-hop �Ÿ��� ��ġ�� ���� ��� ����

			// ����� ��� �ִ� ��� ���� loop�� �̵�
			if (upNdsSet.empty())
			{
				continue;
			}

			IDSet::iterator it = upNdsSet.begin(); // ù ��° ��ҿ� ���� iterator 

			cntlMsg->IDSets[i] = upNdsSet; // i-hop �Ÿ��� ���� ��� ������ ���� �޽����� ����

			// ����� 'i��° ���� ��� ���'�� �޽����� 'i-1��° ���� ��� ���'�� ���� (1<= i <= IHA_T)	 				
		}
		//������ ��Ͽ� �ڽ��� ID �߰�		
		
		cntlMsg->IDSets[1].insert (cNode->id);
		//cntlMsg->IDLists[INTERLEAVING_T].push_front ( cNode->id );
			
	}	
	else if (type == CLSTR_ACK) // CLSTR_ACK �޽����� ���
	{
		int cNodeID = 0;
		cNodeID = cNode->id;
				
		//cntlMsg->IDSets[1].insert( cNode->id );	 		
		//cntlMsg->clstrID = cNodeID;
		
		// ��� ID ��� ����

		int clstrID = cntlMsg->clstrID; // main �Լ��� ���� Src Ŭ�������� ID �ʵ尡 �̸� ������						

		if ( cNodeID == clstrID) // Src CH �� ���
		{
			// Src Ŭ�����Ϳ� ���� ������ ID�� ����Ͽ� ���� ��� ��� ����

			cntlMsg->IDSets[1].insert(clstrID);// CH�� id�� �켱 ����

			int cnt = 1; // ���� �޽����� ����� ��� ID ����
			//int hopCnt = 2; // Src CH�� ���� ��带 �������� ���� �� ȩ �Ÿ� (����) 
			int hopCnt = INTERLEAVING_T+1; // Src CH�� ���� ��带 �������� ���� �� ȩ �Ÿ� (����) 


			IDSet::iterator it;

			IDSet cluster = rtn->getClstr(clstrID);

			for (it = cluster.begin(); it != cluster.end(); it++)
			{
				int nID = *it;
				if (nID == clstrID) // CH�� id�� �̹� ���������Ƿ� skip
				{
					continue;
				}

				cntlMsg->IDSets[hopCnt--].insert(nID);
				//cntlMsg->IDSets[hopCnt++].insert(nID);
				cnt++;
				
				if (cnt > INTERLEAVING_T) // ���� �޽����� t+1���� id�� ����Ǹ� �ݺ��� Ż��
				{
					break;
				}					
			}
		}
		else // Src CH�� �ƴ� ���
		{
			// ������ ����� ���� ��� ����� ����Ͽ� ���� �޽����� ���� ��� ��� ����				
			cntlMsg->IDSets[1].insert(cNodeID);// CH�� id�� �켱 ����

			for (int hopCnt = 2; hopCnt <= INTERLEAVING_T+1; hopCnt++)
			{
				cntlMsg->IDSets[hopCnt] = lowerNodes[cNodeID][hopCnt-1]; // ���� ��� ��� ����
			}
		}			
	}
	
}

void Sec_SEF::testCntrlMsg() // ���� �޽��� �׽�Ʈ �Լ� ������ (2013.09.09 �߰�)
{
	ofstream out("test_Sec_CntrlMsg.txt", std::ofstream::app);

	out << endl;

	if (this->cntlMsg->cmType == BS_HELLO)
	{
		out << "cmType = BS_HELLO\n";
	}
	else if (this->cntlMsg->cmType == CLSTR_ACK)
	{
		out << "cmType = CLSTR_ACK" << cntlMsg->clstrID << endl;
	}
	else
		out << "wrong message type!\n";

	if (cntlMsg->sender != NULL)
	{
		out << "sender: " << cntlMsg->sender->id;
	
		out << " (" << cntlMsg->sender->x << ", " << cntlMsg->sender->y << ") ";	
		// �۽� ����� id list ���

		out << "{ " ;

		for (int i = 0; i < INTERLEAVING_T+1; i++)
		{
			out << "{ " ;

			IDSet set;
			//IDLIST lst;

			if (this->cntlMsg->cmType == BS_HELLO)
			{
				set = upperNodes[cntlMsg->sender->id][i];
			}
			else if (this->cntlMsg->cmType == CLSTR_ACK)
			{
				set = lowerNodes[cntlMsg->sender->id][i];
			}

			if ( ! set.empty() ) // �۽� ����� i��° ���� (�Ǵ� ����) ���� ��� ����� ���
			{
				IDSet::iterator it = set.begin();

				for (it; it != set.end(); it++)
				{
					out << *it << ", ";
				}
			}

			out << "} " ;
		}

		out << "} " << endl; // �۽� ����� ���� ��� ��� (IHA_T +1��) ��� �Ϸ�
	}
	if (cntlMsg->receiver != NULL)
	{
		out << "receiver: " << cntlMsg->receiver->id;
		out << " (" << cntlMsg->receiver->x << ", " << cntlMsg->receiver->y << ") " << endl;	
	}

	out << "ID sets: " << endl;

	for (int i = 0; i < INTERLEAVING_T +1; i++)
	{
		IDSet set = cntlMsg->IDSets[i]; // i ��° IDLIST ȹ��
		//IDLIST lst = cntlMsg->IDLists[i]; // i ��° IDLIST ȹ��

		out << " [ " << i << " ] "; 

		IDSet::iterator it = set.begin();

		for (it; it != set.end(); it++) // i��° IDLIST ���
		{
			int id = *it;
			out << id << ", ";

			// (2013/09.23) ID ��Ͽ� ���Ե� ������ ���� (���� ) ���� ��� ���� ���

			out << "{ " ;

			IDSet tmpSet;

			//if (this->cntlMsg->cmType == BS_HELLO)
			{
				tmpSet = upperNodes[*it][0];
			}
			//else if (this->cntlMsg->cmType == CLSTR_ACK)
			{
				//tmpLst = lowerNodes[*it][i];
			}

			if ( ! tmpSet.empty() ) // �۽� ����� i��° ���� (�Ǵ� ����) ���� ��� ����� ���
			{
				IDSet::iterator tmpIt = tmpSet.begin();

				for (tmpIt; tmpIt != tmpSet.end(); tmpIt++)
				{
					out << *tmpIt << ", ";
				}
			}

			out << "} " ;

			
		}

		out << endl;
	}

	out.close();
}

void Sec_SEF::doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) // (2014.08.18 ����)
{
	// ���� �޽��� ó�� ���� ����
	int cNodeID = cNode->id;
	
	IDSet srcSet; // ���� �޽��� �� ��� Id ��� (����)	
	Node* sender = msg->sender;

	/*
	if (sender == NULL) // BS�κ��� ���ŵ� ��쿡�� sender == NULL - ���� ���� �ƴ�
	{
		//cout << "(sender == NULL) " << endl;
		//exit(-1);
	}
	*/

	list<Node*> parentCHs = rtn->getParentCHs(cNode);

	if ( msg->cmType == BS_HELLO) // BS_HELLO �޽����� ���
	{
		if ( !parentCHs.empty() || (cNode->distance < TRANS_RANGE) )// BS���� ���� ���� �̳��̰ų� �̹� �θ� CH�� ����Ǿ� ������ �ƹ� ó���� ���� ����
		{
			return;
		}

		/*  �۽� ����� iD�� ���� �ĺ� CH ��� ��Ͽ� ������ ��Ͽ� �߰� */

		//list<Node*> parents = rtn->getParentCHs(cNode);
		//list<Node*> parents = rtn->getParents(cNode);

		bool exist = false;

		for ( list<Node*>::iterator it = parentCHs.begin(); it != parentCHs.end(); it++) // ���� �θ� ��� ��Ͽ� �۽� ��� id�� �ִ��� �˻�
		{
			Node* existingParent = *it;
			int parentID = existingParent->id;

			if ( parentID == sender->id)
			{
				exist = true;
				break;
			}
		}

		if (exist == false) // ���� �θ� ��� ��Ͽ� ������ �۽� ��� ID�� �θ� CH��� ��Ͽ� �߰�
		{
			//rtn->addParent(cNode, sender);
			rtn->addParentCH(cNode, sender);
		}

		// �޽����� ���Ե� ���� ��� ���� ���� (ID �ߺ� ���� ���� ����)

		int hopCnt;

		for (hopCnt = 1; hopCnt <= INTERLEAVING_T+1; hopCnt++) // ���� �޽����� ����� �Ÿ� (ȩ ��) �� iD ����� �˻�
		{
			
			IDSet srcSet = cntlMsg->IDSets[hopCnt];

			IDSet::iterator it;

			for (it = srcSet.begin(); it != srcSet.end(); it++)  // hopCnt �Ÿ��� �ش��ϴ� iD ����� ��ҵ��� �˻�
			{
				int upperNodeID = *it;

				// �ߺ� ���� �˻� �κ� �߰��� ��

				IDSet::iterator find_it;

				find_it = upperNodes[cNodeID][hopCnt].find(upperNodeID);

				if (find_it == upperNodes[cNodeID][hopCnt].end()) // ������ ���� ��� ��Ͽ� ������� ���� ���, ��Ͽ� ����
				{
					upperNodes[cNodeID][hopCnt].insert(upperNodeID);

					if (hopCnt == INTERLEAVING_T+1) // ���� ���� ��� ����
					{
						kmr->addPairKey( cNodeID, upperNodeID); // pair key �߰�
					}
				}			
			}
		}// CH�� ���� ��� ��� ���� �Ϸ�

		/* �Ҽ� Ŭ�������� �� CH ������ ���� ���� ��� ���� */

		IDSet clstrNds; // �Ҽ� Ŭ�������� �� CH ��� ��� ȹ��
		int clstrID = cNodeID; // CH �鸸 ���� �޽��� ���� �� �����Ѵٰ� ���� - ���� �޽��� ���� ���� �׻� CH��
		clstrNds = rtn->getClstr(clstrID);

		clstrNds.erase(clstrID); // Ŭ������ �Ҽ� ��� ��Ͽ��� CH�� id ����

		/* ������ �� CH ��忡 ���� ���� ���� ��� ���� ���� */

		IDSet::iterator it_clstrNds;

		hopCnt = 1;

		for ( it_clstrNds = clstrNds.begin(); it_clstrNds != clstrNds.end(); it_clstrNds++) // ������ �� CH ��帶�� ���� ���� ��� ����
		{
			int clstrNodeID;
			clstrNodeID = *it_clstrNds;

			IDSet srcSet = cntlMsg->IDSets[hopCnt];

			IDSet::iterator it = srcSet.begin();

			for (it = srcSet.begin(); it != srcSet.end(); it++) // ���� ��� ��� �˻�
			{
				int upperNodeID = *it;

				IDSet::iterator find_it = upperNodes[clstrNodeID][INTERLEAVING_T+1].find(upperNodeID);

				if (find_it == upperNodes[clstrNodeID][INTERLEAVING_T+1].end())
				{
					upperNodes[clstrNodeID][INTERLEAVING_T+1].insert(upperNodeID);// ���� ���� ��� �߰� �� pair key ����
					kmr->addPairKey(clstrNodeID, upperNodeID);
				}
			}

			hopCnt++;

			if ( hopCnt > INTERLEAVING_T) // 1~t ȩ �Ÿ��� ����� ���� ���踦 ������ ��� �ݺ��� Ż��
			{
				break;
			}
		} // �� CH ������ ���� ���� ��� ���� �Ϸ�

	}

	else if (msg->cmType == CLSTR_ACK) // CLSTR_ACK �޽����� ���
	{
		// �޽����� ���Ե� ���� ��� id ���� (�ߺ� ������� �ʵ��� ����)

		int hopCnt = 0;

		for (hopCnt = 1; hopCnt <= INTERLEAVING_T+1; hopCnt++)
		{
			IDSet srcSet = cntlMsg->IDSets[hopCnt]; // hopCnt �Ÿ��� ���� ��� ���
			
			IDSet::iterator it_lowerNodes;
			
			for (it_lowerNodes = srcSet.begin(); it_lowerNodes != srcSet.end(); it_lowerNodes++) // ���� �޽��� �� ���� ��� ��Ͽ� �ִ� ��� ID �ϳ��� �˻�
			{
				int lowerNodeID = *it_lowerNodes;						

				IDSet::iterator it_find;
			
				it_find = lowerNodes[cNodeID][hopCnt].find(lowerNodeID); // ���� ���� ��� ��Ͽ� ����� ���, �ߺ� �������� ����

				if (it_find == lowerNodes[cNodeID][hopCnt].end())
				{
					lowerNodes[cNodeID][hopCnt].insert(lowerNodeID);
						
					if (hopCnt == INTERLEAVING_T+1) // ���� ���� ��� ����
					{
						kmr->addPairKey(cNodeID, lowerNodeID);
					}
				}
			} // 			
		}
	} // ���� ���� ��� ���� �Ϸ�

	else
	{
		cout << "cmType is invalid" << endl;
		exit(-1);
	}
}

void Sec_SEF::testControl()
{
	ofstream out("assoc_test.txt");

	// �� ����� ���� ���� ��� ���, ���� ���� ��� ��� ���	

	for (int j = 0; j < NUM_NODES; j++)
	{	
		IDSet set;

		//if ( j == 129)
			//cout << " j == 129" << endl;

		out << "\n node " << j << endl ;

		for (int k = 0; k <= INTERLEAVING_T; k++)
		{
			out << "\t upper nodes [" << k << "]:" << " { " ;			

			set = upperNodes[j][k];					

			if ( ! set.empty() ) // �۽� ����� ���� ���� ��� ����� ���
			{
				IDSet::iterator it = set.begin();

				for (it; it != set.end(); it++)
				{
					out << *it << ", ";
				}
			}

			out << "} " << endl; // 
		}

		out << endl ; // �۽� ����� ���� ���� ��� ��� ��� �Ϸ�
		
		for (int k = 0; k <= INTERLEAVING_T; k++)
		{
			set = lowerNodes[j][k];

			out << "\t lower nodes [" << k << "]:" << " { " ;		

			if ( ! set.empty() ) // �۽� ����� ���� ���� ��� ����� ���
			{
				IDSet::iterator it = set.begin();

				for (it; it != set.end(); it++)
				{
					out << *it << ", ";
				}
			}

			out << "} " << endl; // 

		}		
			
		out << endl ; // �۽� ����� ���� ���� ��� ��� ��� �Ϸ�
	}

	out.close();
}




int Sec_SEF::getEventMsgLen(struct _tagEvntMsg* msg)
{
	int eventMsgLen = 0;

	eventMsgLen = DAT_MSG_LEN + (INTERLEAVING_T+1) * PMAC_LEN; // �⺻ ������ �޽��� + (T+1) ���� MAC

	return eventMsgLen; // SEF�� �̺�Ʈ ���� ���� ��ȯ
}

int Sec_SEF::getCntrlMsgLen(struct _tagCntrlMsg* cm) // (2014.01.10) ���� �޽��� ���� ��ȯ
{
	int cntrlMsgLen = 0;
	
	/// // ���� �޽��� Ÿ�Կ� ���� �⺻ ���� ���

	if (cm->cmType == BS_HELLO) // BS_HELLO �޽����� �⺻ ���� ���
	{
		int senderIDSize = 2;		
		cntrlMsgLen = senderIDSize; 
	}
	else // Clstr_ACK �޽����� �⺻ ���� ���
	{
		int senderIDSize = 2;
		int receiverIDSize = 2;
		int clusterIDSize = 2;
		cntrlMsgLen = senderIDSize + receiverIDSize + clusterIDSize;
	}
	
	int numIDs = 0; // ���� �޽����� ���Ե� ID�� ����

	for (int i = 1; i <= INTERLEAVING_T+1; i++)
	{
		numIDs += cm->IDSets[i].size(); // ���� �޽��� �� ID�� ���� ����
	}

	cntrlMsgLen += numIDs * ID_LEN; // ���� �޽��� ���� = �⺻ ���� + id�� ���� * ID ����


	return cntrlMsgLen; // ���� �޽��� ���� ��ȯ
}


bool Sec_SEF::detect(EvntMsg* msg) // (2014.10.20)������ ����Ǹ� TRUE, �ƴϸ� false ��ȯ
{
	bool res = false;

	if (msg->falseReport == true)
	{

		if ( gen0.genRandInt(100) < FILTERING_PROB_SEF ) // ���� ���� üũ!!!
		{
			this->numDtctn++; // ���� ���� Ƚ�� +1 ����
			//cout << " ���� �޽��� ����!!" ;
			res = true; //
			atkDtctd = true;
			msg->detected = true; // (2014.09.23) // ���� ���� ���� ���� ���� ���
		}
		
	}


	return res;



	/*
	// 
	bool res = false;
	atkDtctd = false; // (2014.08.21) ���� ������ �����ϱ� ������ ������ �߻����� ���� ������ ���� -> ���� ����� ���� ���� ���� �� true�� ����
	// ...

	// �ӽ÷� �ּ� ó��
	if (verMAC(msg) == false) // MAC ���� ����� ������ ��� ���� ����
	{
		this->numDtctn++; // ���� ���� Ƚ�� +1 ����
		//cout << " ���� �޽��� ����!!" ;
		res = true; //
		atkDtctd = true;
		msg->detected = true; // (2014.09.23) // ���� ���� ���� ���� ���� ���
	}
	
	return res;	
	*/
}

void Sec_SEF::updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes) // assocType - 0: upper node,1 : lower node
{
	switch (assocType)
	{
	case 0: // ���� ���� ��� ��� ����
		{
			set<int> newUpperNodes; // ���ο� ���� ��� ���		

			{
				double dist;
				double minDist;
				int spUpperNode; // �ִ� ��� ���� ���� ��� ID
					
				dist = 0;
				minDist = -1;
				spUpperNode = -1;

				set<int>::iterator it_upperNode;

				for (it_upperNode = assocNodes.begin(); it_upperNode != assocNodes.end(); it_upperNode++) // �ӽ� ���� ��� ��� ��ȸ (x hop �̳��� ���� ������ ��� ���)
				{
					int upperNodeID;
					upperNodeID = *it_upperNode;

					if ( hopDist == 1) // 1 ȩ �Ÿ��� �ִ� ������ BS������ �Ÿ� ����
					{
						//dist = sqrt ( pow((double) nodes[nodeID]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[nodeID]->y - nodes[upperNodeID]->y, 2) ); // ���� ���� 1-hop ������� �Ÿ� ����
						dist = nodes[upperNodeID]->distance;		


						/// (2014.08.17) ���� �θ� CH ��Ͽ� �߰��Ǿ� ���� ������ ����
						  
						list<Node*> parentCHs = rtn->getParentCHs(nodes[nodeID]);// �θ� CH ��� ȹ��
						list<Node*>::iterator it_pCH;

						bool exist = false;


						for (it_pCH = parentCHs.begin(); it_pCH != parentCHs.end(); it_pCH++) // ���� �θ� CH ��Ͽ� ����Ǿ� �ִ��� ���� �˻�
						{
							Node* pCH = *it_pCH;
							
							if ( pCH->id == upperNodeID) // 1ȩ ���� ��尡 ���� �θ� CH ��Ͽ� ����Ǿ� �ִ��� �˻�
							{
								exist = true;
							} // if�� - ���� �θ� CH ���� ��

						} // for�� �Ϸ� - �˻� �Ϸ�

						if (exist == false)
						{
							rtn->addParentCH(nodes[nodeID], nodes[upperNodeID]); // 1ȩ ���� ��带 �θ� CH ��Ͽ� ����
						}

						/// (2014.08.17) ���� �Ϸ�
					}
					else
					{
						int prevHopSPUpperNode; // (h-1)-hop ���� ��� ID
						set<int>::iterator it; 
						it = upperNodes[nodeID][hopDist-1].begin();

						if (it == upperNodes[nodeID][hopDist-1].end()) // (h-1)-ȩ ���� ��尡 �������� ������ h-hop ���� ��� �߰� �Ұ� -> �ݺ��� Ż��
						{
							break;
						}

						prevHopSPUpperNode = *it;

						double distOfOneHop;
						
						distOfOneHop = sqrt ( pow((double) nodes[prevHopSPUpperNode]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[prevHopSPUpperNode]->y - nodes[upperNodeID]->y, 2) ); // (h-1)-hop ���� h-hop ������� �Ÿ� ����

						if ( (distOfOneHop > TRANS_RANGE) || (nodes[prevHopSPUpperNode]->distance <= nodes[upperNodeID]->distance ) ) // ���� ȩ ���� ��庸�� BS�� ������, ���� ȩ ���� ����� ���� ���� �̳��� �ִ� ��� �߿��� bs�� ���� ����� ��带 h ȩ ���� ���� ����
						{
							continue; // ���� �ĺ� ���� ��� �˻�
						}

						dist = nodes[upperNodeID]->distance;
						//dist = sqrt ( pow((double) nodes[prevHopSPUpperNode]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[prevHopSPUpperNode]->y - nodes[upperNodeID]->y, 2) ); // (h-1)-hop ���� h-hop ������� �Ÿ� ����
					}						
						 
					if (minDist < 0) // �ּ� �Ÿ��� �ʱ�ȭ���� ���� ���
					{
						minDist = dist; // �ӽ� ���� ���κ��� BS������ �ּ� �Ÿ�
						spUpperNode = upperNodeID;
					}
					else if ( dist < minDist) // �ּ� �Ÿ��� ���ŵǴ� ���
					{
						minDist = dist;
						spUpperNode = upperNodeID;
					}
				} // x-hop �̳� ��� ��� ��ȸ �Ϸ� - x-hop ���� ��� ����Ϸ�

				if ( (minDist > 0) )
				{
					newUpperNodes.insert(spUpperNode);					
				}								
				
				// ���� ���� ��� ��ϰ� ���ο� ���� ��� ����� �ٸ��� ���� ���� ���
				
				if ( newUpperNodes != upperNodes[nodeID][hopDist])
				{
					assocUpdated[nodeID] = true;		
				}
				upperNodes[nodeID][hopDist] = newUpperNodes;

				// (2014.06.26) ������� Ȯ��!

				// Ŭ������ �Ҽ� �� CH ����� ���� ���� ��� ��� ����
				
				if (upperNodes[nodeID][hopDist].empty()==true) // CH�� h-hop ���� ��� ����� ��� ������ �Լ� ����
				{
					return;
				}


				set<int> cluster;
				set<int>::iterator it_memberNode;
				int memberNodeID;

				cluster = rtn->getClstr(nodeID); // Ŭ������ �Ҽ� ��� ��� ȹ��

				int node_idx;
				node_idx = 1;

				for( it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // Ŭ������ �Ҽ� ��� ��� ��ȸ
				{
					memberNodeID = *it_memberNode; // Ŭ������ �Ҽ� ID;

					if (memberNodeID == nodeID) // �Ҽ� ��� ID�� ch�� ID�� ������ ���, ���� �Ҽ� ���� ����
					{
						continue;
					}

					if ( node_idx == hopDist) // CH�� h-hop ���� ��带 h ��° �Ҽ� ����� ���� ���� ���� ����
					{
						if ( upperNodes[memberNodeID][INTERLEAVING_T+1] != upperNodes[nodeID][hopDist]) // �Ҽ� ����� ���� ���� ���� ��� ��ϰ� �ٸ� ���, ���� ���� üũ �� �ش� ��� ����
						{
							assocUpdated[memberNodeID] = true;						
							upperNodes[memberNodeID][INTERLEAVING_T+1] = upperNodes[nodeID][hopDist];
						}
					

						// CH �� h-hop ���� ����� ���� ���� ��� ��Ͽ� �ش� �Ҽ� ��� ID�� �������� ���� ���, ���� ���ο� üũ
						/*
						set<int>::iterator it;

						for (it = upperNodes[nodeID][hopDist].begin(); it != upperNodes[nodeID][hopDist].end(); it++) // CH �� h-hop ���� ��� ��� ��ȸ - h-hop ���� ����� ���� ���� ��� ��� ����
						{
							int hHopUpNodeID;
							hHopUpNodeID = *it;

							if ( lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].find(memberNodeID) == lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].end()) // h-hop ���� ����� ���� ���� ��� ��Ͽ� �Ҽ� ��尡 �������� ���� ���,
							{
								assocUpdated[hHopUpNodeID] = true;
								lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].insert(memberNodeID); // h-hop ���� ����� ���� ���� ��� ��Ͽ� ���� �Ҽ� ��� ID�� ����
							}
						} // CH �� h-hop ���� ��� ��� ��ȸ - h-hop ���� ����� ���� ���� ��� ��� ���� �Ϸ�
						*/
						break;
					}

					node_idx++;
				} // CH�� h-��° ���� ��带 h��° �Ҽ� ����� ���� ���� ���� ����
				

			} // h-hop ���� ��� ��� ���� �Ϸ�								
						
		break;
		} // ���� ���� ��� ��� ���� �Ϸ�

	case 1: // ���� ���� ��� ��� ����
		{

			set<int> newLowerNodes; // ���ο� ���� ��� ���

			set<int>::iterator it_lowerNode;

			for (it_lowerNode = assocNodes.begin(); it_lowerNode != assocNodes.end(); it_lowerNode++) // �ӽ� ���� ��� ��� ��ȸ (x hop �̳��� ���� ������ ��� ���)
			{
				int lowerNodeID;
				lowerNodeID = *it_lowerNode;

				if (upperNodes[lowerNodeID][hopDist].find(nodeID) != upperNodes[lowerNodeID][hopDist].end()) // �ӽ� ���� ����� h-hop ���� ��� ��Ͽ� ���� ��尡 �����ϸ� �ش� �ӽ� ���� ��带 ���� ����� ���� ��� ��Ͽ� ����
				{
					newLowerNodes.insert(lowerNodeID);
				}
			} // for�� ���� - �ӽ� ���� ��� ��� ��ȸ �Ϸ�

			if (newLowerNodes != lowerNodes[nodeID][hopDist])
			{
				assocUpdated[nodeID] = true;		
			}

			lowerNodes[nodeID][hopDist] = newLowerNodes; // ���� ����� ���� ��� ��� ����

			/// (2014.08.06) �� CH �Ҽ� ������ ���� ���� ��� ��� ����

			set<int> cluster;
			set<int>::iterator it_memberNode;
			int memberNodeID;

			cluster = rtn->getClstr(nodeID); // Ŭ������ �Ҽ� ��� ��� ȹ��

			for (it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // Ŭ������ �Ҽ� ��� ��� ��ȸ
			{
				memberNodeID = *it_memberNode; // �Ҽ� ��� ID ȹ��

				if (memberNodeID == nodeID) // �Ҽ� ��� ID�� ch�� ID�� ������ ���, ���� �Ҽ� ���� ����
				{
					continue;
				}

				for (int hop = 1; hop <= INTERLEAVING_T+1; hop++)
				{
					lowerNodes[memberNodeID][hop].clear(); // �Ҽ� ����� ���� ���� ��� ��� ����
				}
			} // Ŭ������ �Ҽ� �� CH ������ ���� ���� ��� ��� ����

		
		break;
		} // ���� ���� ��� ��� ���� �Ϸ�
	}
}

bool Sec_SEF::compareAssoc ( set<int> oldAssoc, set<int> newAssoc) // (2014.05.27) �� ���� ��� ����� �����ϸ� true,  �ٸ��� false ��ȯ
{
	bool res = true;

	if (oldAssoc.size() != newAssoc.size() )
	{
		res = false;
		return res;
	}

	set<int>::iterator it;
	set<int>::iterator it2;

	for (it = oldAssoc.begin(), it2= newAssoc.begin(); it != oldAssoc.end(), it2 != newAssoc.end(); it++, it2++)
	{
		int id = *it;
		int id2 = *it2;

		if (id != id2)
		{
			res = false;
			break;
		}
	}

	return res;
}

void Sec_SEF::resetAssoc() // (2014.08.21) ���� ������ ���� ��� ������ ����
{
	upperNodes.clear();
	lowerNodes.clear();
}


bool Sec_SEF::hasUpdatedAssoc (int nodeID) // (2014.07.17) Ư�� ����� ���� ��� ���� ���� ��ȯ
{
	if ( this->assocUpdated[nodeID] == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void Sec_SEF::resetUpdatedAssoc() // (2014.07.22) ������ ���� ��� ���� ���θ� false �� ���� (Ű ��й� ����)
{
	std::map<int, bool>::iterator it;

	int nodeID;	

	for (it = this->assocUpdated.begin(); it != assocUpdated.end(); it++) // ������ ���� ���� ���θ� false �� ���� - Ű ��й��� ����
	{
		nodeID = (*it).first;
		assocUpdated[nodeID] = false;
	} // for�� ���� - ������ ���� ��� ���� ���θ� false �� ����
}

void Sec_SEF::testAssoc() // // (2014.07.17) ������ ���� ��� ��� ���Ͽ� ���
{
	ofstream out("tst_assoc.txt");

	map<int, set<int>> clusters = rtn->getClusters(); // Ŭ������ ���� ȹ��

	map<int, set<int>>::iterator it_clstr; 

	for (it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++) // Ŭ������ �� ��ȸ - �� Ŭ�����Ϳ� ���� ���鿡 ���� ���� ��� ��� ���
	{
		set<int> nodesInCluster = (*it_clstr).second;

		set<int>::iterator it_nd;

		// (2014.09.21) ���� - 
		int CHID = (*it_clstr).first;

		if ( nodes[CHID] == NULL) // CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
		{
			continue;
		}
		// (2014.09.21 ���� �Ϸ�) CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�

		for (it_nd = nodesInCluster.begin(); it_nd != nodesInCluster.end(); it_nd++) // Ŭ������ �� ��� ���� ��ȸ - ���� Ŭ�����Ϳ� ���� ������ ���� ��� ��� ���
		{
			int nodeID = *it_nd;

			/// ���� ����� ���� ��� ��� ���
			out << "node ID: " << nodeID;

			for (int hc = 1; hc <= INTERLEAVING_T+1; hc++) // 1~t+1 ������ ȩ�� �����ϴ� ���� ��� ��� ���
			{
				out << ", hc = " << hc << " {";
				
				std::set<int> upNodes = upperNodes[nodeID][hc];

				set<int>::iterator it_upNodes;

				for (it_upNodes = upNodes.begin(); it_upNodes != upNodes.end(); it_upNodes++) // hc ȩ�� �����Ǵ� ���� ��� ��� ��ȸ
				{
					int upNodeID = *it_upNodes;

					out << upNodeID << ", ";

				} // for�� ���� - ���� ����� hc ȩ ���� ��� ��� ���

				out << " } ";

			} // for�� ���� 

			out << endl << "\t";

			/// ���� ����� ���� ��� ��� ���
			for (int hc = 1; hc <= INTERLEAVING_T+1; hc++) // 1~t+1 ������ ȩ�� �����ϴ� ���� ��� ��� ���
			{
				out << "hc = " << hc << " {";

				std::set<int> lowNodes = lowerNodes[nodeID][hc];

				set<int>::iterator it_lowNodes;

				for (it_lowNodes = lowNodes.begin(); it_lowNodes != lowNodes.end(); it_lowNodes++) // hc ȩ�� �����Ǵ� ���� ��� ��� ��ȸ
				{
					int lowNodeID = *it_lowNodes;

					out << lowNodeID << ", ";
				} // for�� ���� - ���� ����� hc ȩ ���� ��� ��� ���

				out << " } ";

			} // for�� ���� 

			out << endl << endl;

		}// for�� ���� - ���� Ŭ�����Ϳ� ���� ������ ���� ��� ��� ���

		out << endl << endl << endl << endl;

	} // for �� ���� - �� Ŭ�����Ϳ� ���� ���鿡 ���� ���� ��� ��� ���


	out.close();
}

Sec_SEF::Sec_SEF()
{
	seed0 = 1;
	this->gen0.setSeed(seed0);

}



Sec_SEF::~Sec_SEF()
{
	// ����/���� ���� ��� ��Ͽ� ����� ��� ����

	for (int i = 0; i < NUM_NODES; i++) // ��� ��忡 ���� �ݺ�
	{
		for (int j = 0; j < INTERLEAVING_T+1; j++) // �� ����� ��� ����/���� ��� ��Ͽ� ���� �ݺ� (IHA_T +1 ��) 
		{
			IDSet set = upperNodes[i][j];
			IDSet::iterator it = set.begin();

			for (it; it != set.end(); )
			{
				IDSet::iterator tmpIt = it;
				++it;
				set.erase(tmpIt);
			} // ���� ��� ����� ��� ����

			set = lowerNodes[i][j];
			it = set.begin();

			for (it; it != set.end(); )
			{
				IDSet::iterator tmpIt = it;
				++it;
				set.erase(tmpIt);
			} // ���� ��� ����� ��� ����
			
		} // ���� ����� ��/���� ��� ��� ���� �Ϸ�
	} // ��� ����� ��/���� ��� ��� ���� �Ϸ�
}