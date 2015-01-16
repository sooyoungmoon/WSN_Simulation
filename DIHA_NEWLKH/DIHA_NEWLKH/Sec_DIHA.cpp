// 2014.06.24
// ������
// Sec_DIHA
// DIHA�� ���� Ŭ���� ����


#include "Sec_DIHA.h"

// �̺�Ʈ ���� ó�� ����	

void Sec_DIHA::setMAC (EvntMsg* msg) // MAC ����
{

}

void Sec_DIHA::setMAC (struct _tagEvntMsg* msg, int clstrID) // (2014.08.19) MAC ���� 
{
	// ������ ���� �ִ� MAC ��� ����

	list<MAC*>::iterator itMAC = msg->macs.begin();

	for ( itMAC; itMAC!= msg->macs.end(); itMAC++)
	{
		MAC* mac = *itMAC;
		delete mac;
	}

	msg->macs.clear(); 	

	if (msg->hop != 0) // hop count�� 0�� ���� ���� �� MAC ����
	{
		return;
	}

	// ���� ������ �����ϴ� ������ ID Ȯ�� ( ��Ȳ�� ���� t+1 ���� ���� ���� ���� �� ����)

	// (2014.10.10) �̹� ���ŵ� ������ id�� Ŭ������ ������ ���Ե� ��� �̸� �����ϴ� ���� �ʿ�

	set<int> deledtedNds; // ���ŵ� ��� ����



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

		if (nodes[nodeID] == NULL)
		{
			deledtedNds.insert(nodeID); // (2014.10.10) ���ŵ� ��� ��Ͽ� �߰�
			continue;
		}


		sourceNodes.push_back(nodeID);

		if (idx >= INTERLEAVING_T) // ���� ������ �����ϴ� �Ϲ� ��� (ch ����)�� ���� T���� �Ǹ� �ݺ��� Ż��
		{
			break;
		}

		idx++; // (2014.10.10)

	} // for�� - ���� ������ �����ϴ� ������ ��Ͽ� ����


	set<int>::iterator it_del;

	for (it_del = deledtedNds.begin(); it_del != deledtedNds.end(); it_del++) // (2014.10.10) ���ŵ� ��� ������ Ŭ�����Ϳ��� ����
	{
		int delNodeID = *it_del;
		
		rtn->removeFromCluster(delNodeID);
	} // for�� �Ϸ� - ���ŵ� ��� ������ Ŭ�����Ϳ��� ����

	msg->prevNodes = sourceNodes;// �̺�Ʈ ���� ���� ���� ��� ���� ����
	msg->prevNodes.push_back(clstrID);

	//sourceNodes.push_back(clstrID); // CH�� id �߰�

	// �ҽ� ��� �� T���� �Ϲ� ��� (�� CH)�� �����ϸ� ch�� ���� ���� ��带 ����Ͽ� MAC ����	(�ҽ� ���: id�� ���� ������� ù ��°�� �ش��ϴ� �Ϲ� ���)	

	if (msg->receiver == NULL) // �޽��� ���� ��尡 �������� ������ �Լ� ����
	{
		return;
	}


	int upNodeID = msg->receiver->id;


	int MACGeneratNodeID;

	PairKey* pk = NULL;

	MAC* mac = NULL; 


	if ( !sourceNodes.empty() ) // CH�� �ƴ� �ҽ� ��尡 �����ϸ� ù ��° �ҽ� ���� ���� ���� ������ pair key�� MAC ���� �� ÷��
	//if (sourceNodes.size() == INTERLEAVING_T) // CH�� �ƴ� �ҽ� ����� ���� T�̸� ù ��° �ҽ� ���� ���� ���� ������ pair key�� MAC ���� �� ÷��
	{
		MACGeneratNodeID = sourceNodes.front();

		pk = kmr->getPairKey(MACGeneratNodeID, upNodeID); // pairwise key ȹ��

		if (pk == NULL)
		{
			cout << " pk == NULL " << endl;
			
			kmr->addPairKey(MACGeneratNodeID, upNodeID); // 
			pk = kmr->getPairKey(MACGeneratNodeID, upNodeID); // pairwise key ȹ��
						
		}

		mac = new MAC; // pairwise MAC ���� �� �߰�

		mac->origin = nodes[MACGeneratNodeID]; // MAC ���� ��� ����
		mac->pKey = pk; // MAC�� pairwise key ����
		mac->code = pk->kVal + msg->val; // �޽��� ���� �ڵ� ����

		msg->macs.push_back(mac); // ���� �� MAC ��Ͽ� �ű� MAC �߰�

		// ���� ����� MAC �������� ���� ������ �Ҹ� ����

		nodes[MACGeneratNodeID]->enrgyDiff = hashEnrg; // ����� ���� ������ �Ҹ�
		nodes[MACGeneratNodeID]->eCnsmptn += nodes[MACGeneratNodeID]->enrgyDiff; // (2013.10.04) MAC ��꿡 ���� ������ �Ҹ� ����
		Node::totEnrgCnsmptn += nodes[MACGeneratNodeID]->enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)

		nodes[MACGeneratNodeID]->remEnrgy -= nodes[MACGeneratNodeID]->enrgyDiff; // (2013.10.04) MAC ��꿡 ���� �ܿ� ������ ����
	}

}

void Sec_DIHA::updateMAC(struct _tagEvntMsg* msg) // (2014.08.20) MAC ����
{
	// ���� �� id ��� ����
	//msg->prevNodes.pop_front(); // T+1 ȩ ���� ����� id ����
	//msg->prevNodes.push_back(msg->sender->id); // �۽� ����� ID �߰�	

	// IHA_T ȩ ���� ���� ���� ���� ��尡 �����ϴ� Ű�� MAC�� �����ϰ� ���� �� mac ��Ͽ� �߰�

	std::list<int> pNodes = msg->prevNodes;

	int lowerNodeID = -1; // T ȩ ���� ����� id

	if (msg->prevNodes.size() > INTERLEAVING_T)
	{
		msg->prevNodes.pop_front(); // T+1 ȩ ���� ����� id ����
		lowerNodeID = msg->prevNodes.front(); // IHA_T ȩ ���� ����� ID ȹ��
	}
	else if ( msg->prevNodes.size() == INTERLEAVING_T)
	{
		// ���� prevNodes�� ù ��°�� ��ġ�� ��尡 CoS�̸� �ش� ��尡 Tȩ ���� �����
		int tempID = pNodes.front();

		if (tempID == msg->CoS->id)
		{
			lowerNodeID = tempID; // IHA_T ȩ ���� ����� ID ȹ��
		}
	}
	
	msg->prevNodes.push_back(msg->sender->id); // �۽� ����� ID �߰�

	if (lowerNodeID < 0) // ���� Tȩ ���� ��尡 �������� ������ �Լ� ����
	{
		return;
	}

	//int lowerNodeID = pNodes.front(); // IHA_T ȩ ���� ����� ID ȹ��
	int nextNodeID = msg->receiver->id;
	PairKey* pk = kmr->getPairKey(lowerNodeID, nextNodeID); // pairwise key ȹ��

	if (pk == NULL) // ������ �� ��尡 ���� ���踦 ���� ���� ���
	{
		kmr->addPairKey (lowerNodeID,  nextNodeID); // (2013.11.27) pair key �߰� (���� ���� ���� �����ϴ� Ű)								
		pk = kmr->getPairKey(lowerNodeID,  nextNodeID);
	}

	// pairwise MAC ���� �� �߰�
	MAC* mac = new MAC; // pairwise MAC ���� �� �߰�

	mac->origin = nodes[lowerNodeID]; // (2013.09.02) MAC ���� ��� ����
	mac->pKey = pk; // MAC�� pairwise key ����
		
	mac->code = pk->kVal + msg->val; // �޽��� ���� �ڵ� ����
	
	///** (2014.08.22 ����) ���� ������ ���, ���� t ȩ ���� ��尡 source ����̰� �Ѽ� ��尡 �ƴϸ� �߸��� �ڵ� ��(-1)�� ����ǵ��� ����
	if (msg->falseReport == true) // ���� ������ ���, T ȩ ���� ��尡 SOURCE ������� Ȯ��,
	{
		if ( msg->CoS->id == rtn->getClstrID(lowerNodeID)) // Tȩ ���� ��尡 SOURCE ����̸� �Ѽ� ������� Ȯ��
		{
			
			if ( nodes[lowerNodeID]->cmprmsd == false) // (2014.08.31) T ȩ ���� ��尡 �Ѽ� ��尡 �ƴϸ� �߸��� �ڵ� �� ����
			//if (msg->CoS->cmprmsd == false) // �Ѽ� ��尡 �ƴϸ� �߸��� �ڵ� �� ����
			{
				mac->code = -1;
			}
		}
	} // IF��  - ���� ������ ���, T ȩ ���� ��尡 SOURCE ������� Ȯ��,
	///** (2014.08.22 ���� �Ϸ�)


	msg->macs.push_back(mac); // ���� �� MAC ��Ͽ� �ű� MAC �߰�
	
	
	// ������ �Ҹ� ����

	nodes[lowerNodeID]->enrgyDiff = hashEnrg; // ����� ���� ������ �Ҹ�
	nodes[lowerNodeID]->eCnsmptn += nodes[lowerNodeID]->enrgyDiff; // (2013.10.04) MAC ��꿡 ���� ������ �Ҹ� ����
	Node::totEnrgCnsmptn += nodes[lowerNodeID]->enrgyDiff; // (2013.10.31) ��ü ������ �� ������ �Ҹ� ���� (���� ����)

	nodes[lowerNodeID]->remEnrgy -= nodes[lowerNodeID]->enrgyDiff; // (2013.10.04) MAC ��꿡 ���� �ܿ� ������ ����
}

void Sec_DIHA::doEventMsgProc(struct _tagEvntMsg* msg) // (2014.08.21) �̺�Ʈ ���� ó�� (MAC ����)
{
	int cNodeID = msg->receiver->id;

	std::list<MAC*>::iterator itMAC = msg->macs.begin();

	list<MAC*> tmplst;

	for (itMAC; itMAC != msg->macs.end(); itMAC++) // �̺�Ʈ ������ ���Ե� mac�� �ϳ��� �˻�	
	{
		MAC* mac = *itMAC; // ���� �˻����� MAC

		//IDSet::iterator itID = lowerNodes[cNodeID][INTERLEAVING_T].begin(); // ���� ���� ����� ���� ���� ��� ����� iterator

		//bool matched = false; // MAC ���� ��尡 ���� ���� ���� ������ ��� true�� ����

		PairKey* pKey = mac->pKey; // pairwise key

		// MAC ���� ���� ���� ��尡 ���� ���迡 �ִ��� �˻�

		Node* origin = mac->origin;
		Node* receiver = msg->receiver;

		if ((pKey->nID == origin->id && pKey->nID2 == receiver->id) || (pKey->nID == receiver->id && pKey->nID2 == origin->id))
		{
			// MAC ���� ���� ���� ���� ���� ���迡 �ش���. �ش� MAC�� ���� �� ��Ͽ��� ����
			//matched = true;
			//delete (mac);// �ش� MAC ��ȯ	

			tmplst.push_back(mac); // (2014.08.25) ������ MAC�� �ӽ� ��Ͽ� �߰��Ͽ��ٰ� �̺�Ʈ ���� �� mac ��Ͽ��� �����ϰ� �޸� ��ȯ
		}	

		/*
		if (matched == false)
		{			
			tmplst.push_back(mac); // ���� ���� ��尡 ������ MAC�� ������ ������ mac���� �ӽ� ��Ͽ� �߰�
		}
		*/
	}

	// (2014.08.25 ����) ������ MAC�� �޸� ��ȯ, ��Ͽ��� ����

	
	if ( !tmplst.empty() ) // ������ MAC�� ���� - �̺�Ʈ ���� �� MAC ��Ͽ��� ���� �տ� ��ġ�� mac
	{
		MAC* mac = * (msg->macs.begin()); // ���� �տ� ��ġ�� MAC�� �޸𸮸� ��ȯ�ϰ� ��Ͽ��� ����

		delete mac;

		msg->macs.pop_front();
	}
	
	tmplst.clear(); // �ӽ� MAC ��� ����


	//msg->macs.clear(); // ���� �� MAC ��� ���� ����
}

// ���� �޽��� ó�� ����
void Sec_DIHA::testControl() //
{

}

void Sec_DIHA::doCntlOp (struct _tagCntrlMsg* msg, Node* cNode) // (2014.08.18 �߰�) ���� �޽��� ���ſ� ���� ó��
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

	if ( msg->cmType == BS_HELLO) // BS_HELLO �޽����� ���
	{
		if (cNode->distance < TRANS_RANGE) // BS���� ���� ���� �̳��̸� �ƹ� ó���� ���� ����
		{
			return;
		}
	

		/*  �۽� ����� iD�� ���� �ĺ� CH ��� ��Ͽ� ������ ��Ͽ� �߰� */

		list<Node*> parents = rtn->getParentCHs(cNode);
		
		// (2014.09.11 ����)
		if ( parents.size() >= MAX_NUM_PARENT_CHS) // �θ� ch ������ �ִ� �ѵ��� �����ϸ� ��� ��ȯ
		{
			return;
		}

		bool exist = false;

		for ( list<Node*>::iterator it = parents.begin(); it != parents.end(); it++) // ���� �θ� ��� ��Ͽ� �۽� ��� id�� �ִ��� �˻�
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

// ������ �Ҹ� ��� ����
int Sec_DIHA::getEventMsgLen(struct _tagEvntMsg* msg)
{
	int len = DAT_MSG_LEN + (INTERLEAVING_T+1) * PMAC_LEN;	

	int commOvrhd = 0;


	/* (2013.10.21) ���� ������� �߰����� PAIRWISE MAC���� ���� �������� (T+1)���� BLOOM FILTER�� ��������ν� ����� �� �ִٰ� ���� */
	/*
	// �߰����� pairwise MAC���� ���� ��� ������� (in bytes)
	int numAddPairMAC = 0; // ���� ��  t+1���� MAC �ܿ� �߰����� pairMAC ����

	numAddPairMAC = msg->macs.size() - IHA_T;

	commOvrhd += numAddPairMAC * PMAC_LEN; // �߰����� pairMAC���� ���� ��� �������

	// commOvrhd /= 5; // bloom filter ����
	*/


	// ���� ���� �������� �� ���� T-ȩ ���� ��忡�� ���� ���� ����� id ���� - �޽��� ������� �߻� (in bytes)
	commOvrhd += ceil((INTERLEAVING_T) * REDUCED_ID_LEN); 

	commOvrhd += ID_LEN;// (2013.11.27) ������ MAC ���� ����� id ���� (���� ���� ��忡�� MAC ������ ���� �ʿ�)
	
	len += commOvrhd; // �⺻ �޽��� ���� + �߰����� ��� ��������� �հ� ��ȯ

	return len;

}

int Sec_DIHA::getCntrlMsgLen(struct _tagCntrlMsg* cm)
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

	/*
	int cntrlMsgLen = 0;

	int numIDs = 0; // ���� �޽����� ���Ե� ID�� ����

	for (int i = 1; i <= INTERLEAVING_T+1; i++)
	{
		numIDs += cm->IDSets[i].size(); // ���� �޽��� �� ID�� ���� ����
	}

	cntrlMsgLen = numIDs * ID_LEN; // ���� �޽��� ���� = id�� ���� * ID ����

	return cntrlMsgLen; // ���� �޽��� ���� ��ȯ
	*/

}

void Sec_DIHA::updateAssocNodes(int nodeID, int assocType, int hopDist, set<int> assocNodes) // (2014.06.24) ������ ���� ��� ����
{
	
	switch (assocType)
	{
	case 0: // ���� ���� ��� ��� ����
		{
			set<int> newUpperNodes; // ���ο� ���� ��� ���		

			set<int>::iterator it_upperNode;

			for (it_upperNode = assocNodes.begin(); it_upperNode != assocNodes.end(); it_upperNode++) // �ӽ� ���� ��� ��� ��ȸ (x hop �̳��� ���� ������ ��� ���)
			{
				int upperNodeID;
				upperNodeID = *it_upperNode;

				double distBetween;
				int hopCnt;

				distBetween = sqrt ( pow((double) nodes[nodeID]->x - nodes[upperNodeID]->x, 2) + pow((double) nodes[nodeID]->y - nodes[upperNodeID]->y, 2) ); // ���� CH�κ��� �ӽ� ���� �������� �Ÿ� ���

				hopCnt =  (int) ceil (distBetween / TRANS_RANGE);

				if (hopCnt != hopDist)
				{
					continue; // ���� CH�κ��� �ӽ� ���� �������� ȩ �� ���, h-hop�� �ٸ��� �ű� ���� ��� ��Ͽ��� ����
				}

				newUpperNodes.insert(upperNodeID);

				/// �θ� ch ��� ���� ����

				if (hopCnt == 1)
				{
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

			} //  x-hop �̳� ��� ��� ��ȸ �Ϸ� - x-hop ���� ��� ����Ϸ�


			if ( this->compareAssoc ( upperNodes[nodeID][hopDist], newUpperNodes) == false) // CH�� ���� ��� ��� ����
			{
				assocUpdated[nodeID] =true;
				upperNodes[nodeID][hopDist].clear();
				upperNodes[nodeID][hopDist] = newUpperNodes;


				// ���� hop == T+1 �̸� ���� ��� ��Ͽ� ���� ����� ���� CH�� ���� PAIR key �߰�

				set<int>::iterator it_UN;

				if (hopDist == INTERLEAVING_T+1) 
				{
					for ( it_UN = upperNodes[nodeID][hopDist].begin(); it_UN != upperNodes[nodeID][hopDist].end(); it_UN++) // ���� ��� ��� ��ȸ - pair key �߰�
					{
						int UN_ID = *it_UN;

						kmr->addPairKey(nodeID, UN_ID); // pair key �߰�

					} 

				} // ���� CH�� ���� ���� ��� ������ PAIR KEY �߰� �Ϸ�


			}	// IF�� - CH�� ���� ��� ��� ���� �Ϸ�		
			

			/// �ش� Ŭ������ �Ҽ� �� CH ������ ���� ���� ��� ����

			if (upperNodes[nodeID][hopDist].empty()==true) // CH�� h-hop ���� ��� ����� ��� ������ �Լ� ����
			{
				return;
			}



			set<int> cluster;
			set<int>::iterator it_memberNode;
			int memberNodeID;

			int node_idx; // Ŭ������ �Ҽ� ����� �ε���
			node_idx = 1;

			cluster = rtn->getClstr(nodeID); // Ŭ������ �Ҽ� ��� ��� ȹ��


			for (it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // Ŭ������ �Ҽ� ��� ��� ��ȸ
			{
				memberNodeID = *it_memberNode; // �Ҽ� ��� ID ȹ��

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

					set<int>::iterator it;

					for (it = upperNodes[nodeID][hopDist].begin(); it != upperNodes[nodeID][hopDist].end(); it++) // CH �� h-hop ���� ��� ��� ��ȸ - h-hop ���� ����� ���� ���� ��� ��� ����
					{
						int hHopUpNodeID;
						hHopUpNodeID = *it;

						if ( lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].find(memberNodeID) == lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].end()) // h-hop ���� ����� ���� ���� ��� ��Ͽ� �Ҽ� ��尡 �������� ���� ���,
						{
							assocUpdated[hHopUpNodeID] = true;
							lowerNodes[hHopUpNodeID][INTERLEAVING_T+1].insert(memberNodeID); // h-hop ���� ����� ���� ���� ��� ��Ͽ� ���� �Ҽ� ��� ID�� ����
						
							// addPairKey

							kmr->addPairKey(memberNodeID, hHopUpNodeID);

						}
					} // CH�� H-HOP ���� ������ ���� ���� ��� ��Ͽ� ���� �Ҽ� ��� id ����
					break;
				} // �Ҽ� ����� ���� ���� ��� ���� �� ���� ����� ���� ���� ��� ����

				node_idx++;

			} // CH�� h-��° ���� ��带 h��° �Ҽ� ����� ���� ���� ���� ����			


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

					if (lowerNodes[nodeID][hopDist].find(lowerNodeID) == lowerNodes[nodeID][hopDist].end()) // ���� ����� h-hop ���� ��� ��Ͽ� �ű� ���� ��尡 ����Ǿ� ���� ���� ���
					{
						assocUpdated[nodeID] = true;	
						lowerNodes[nodeID][hopDist].insert(lowerNodeID);

						kmr->addPairKey(nodeID, lowerNodeID); // (2014.08.27) ���� ���� �ű� ���� ��� ���̿� pair key �߰�
					}

				} // if�� - �ӽ� ���� ����� h - hop ���� ��� ��Ͽ� ���� ��尡 ����Ǿ� ������ �ش� �ӽ� ���� ��带 ���� ����� ���� ��� ��Ͽ� �߰�

			} // for�� ���� - �ӽ� ���� ��� ��� ��ȸ �Ϸ�


			// �ű� ���� ��� ��� ��ȸ

			/// (2014.08.05) �� CH �Ҽ� ������ ���� ���� ��� ��� ���� �ʿ� (Sec_IHA::updateAssocNodes() �� ��������)

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

			//if (newLowerNodes != lowerNodes[nodeID][hopDist])
			//{
				//assocUpdated[nodeID] = true;		
			//}

			//lowerNodes[nodeID][hopDist] = newLowerNodes; // ���� ����� ���� ��� ��� ����

		break;
		} // ���� ���� ��� ��� ���� �Ϸ�
	}
}


Sec_DIHA::Sec_DIHA()
{

}

Sec_DIHA::~Sec_DIHA()
{

}