// 2014.07.31
// ������
// Kmr.h
// NEW_LKH ��� Ű ���� Ŭ���� ����

#include "Kmr_NEWLKH.h"
#include "SrchNode_KTS.h"

void Kmr_NEWLKH::updateKey(struct _tagKNode* kNode) // (2014.07.08) �׷� Ű ���� �Լ�
{
	ofstream out("tst_KeyRecipentSet.txt");

	/*
	if (kNode->level == 1 && kNode->index == 0) // �ӽ� - ����� ��
	{
		kNode->updated_eviction = false;
	}
	*/

	 // Ű Ʈ�� �󿡼� update_addition �Ǵ� update_eviction �÷��װ� ��(true)���� ������ kNode�� Ű ���� (����� ȣ��)

	//if (kNode->updated_addition == true || kNode->updated_eviction == true) // ���� �ش� Ű�� ������ �ʿ��� ���, Ű �� ����
	if ( (kNode->updated_addition == true) || (kNode->updated_eviction == true) ||  // (2014.07.31) ���� �ش� Ű�� ������ �ʿ��� ���, Ű �� ����
		( (kNode->created == true) && (kNode->unode==NULL) ) ) // 1) ��� �߰��� ���� Ű ����, 2) ��� ���ſ� ���� Ű ����, 3) �ű� kNode �������� ���� Ű ����
	{
		GroupKey* gk;
		gk = kNode->groupKey; // �׷� Ű ȹ��

		gk->oldVal = gk->newVal;
		gk->newVal = gen1.genRandInt(Max_Key_Value); // �׷� Ű ����

		set<int> recipents;

		if (kNode->updated_eviction == true)
		{
			// �ڽ� ������ Ű�� ��ȣȭ -> ������ �ڽ� ��� Ű�� ������ ���� ��� �׷쿡 ����

			struct _tagKNode* tkNode;

			tkNode = kNode->leftChild;

			while (tkNode != NULL) // ������ �ڽ� ��忡 ���� �ݺ�
			{			
				recipents = this->getNdsInGrp(tkNode);

				if (!recipents.empty())
				{
					keyRecipentSet.insert ( (make_pair) (kNode->groupKey, recipents));
				}
				tkNode = tkNode->rightSibling;
			} // Ű - ���� ��� pair �߰�
		} // if �� - updated_eviction == true �� ��� ó�� �Ϸ�
		else if (kNode->updated_addition == true)
		{
			// ���� Ű�� ��ȣȭ -> ���� ���鿡�� ����
			// �ű� ������ Ű�� ��ȣȭ -> �ű� ���鿡�� ����
			
			recipents = this->getNdsInGrp(kNode);

			set<int> existingNodes; // ���� ��� �� ���� ������� �����ϴ� ���
			set<int> newNodes; // ���� ��� �� ���� ���忡�� �߰��� ���

			set<int>::iterator it_recipent;

			for( it_recipent = recipents.begin(); it_recipent != recipents.end(); it_recipent++) // ���� ��� ��� ��ȸ - ���� ���� �ű� ��� ����
			{
				int nodeID;
				Node* node;
				nodeID = *it_recipent;
				node = nodes[nodeID];

				if (node->newbee == true)
				{
					newNodes.insert(nodeID);
				}
				else
				{
					existingNodes.insert(nodeID);
				}
			} // ���� ��� ��ϰ� �ű� ��� ��� ���� �Ϸ�

			keyRecipentSet.insert(  (make_pair) (kNode->groupKey, existingNodes)); // ���� ��带 ���� <���� Ű, {���� ��� ID}> pair �߰�

			set<int>::iterator it_newRecipent;

			for(it_newRecipent = newNodes.begin(); it_newRecipent != newNodes.end(); it_newRecipent++) // ���� ��� �� �ű� ��� ��� ��ȸ
			{
				int newNodeID;
				newNodeID = *it_newRecipent;

				set<int> newRecipent;
				newRecipent.insert(newNodeID);

				keyRecipentSet.insert( (make_pair) (kNode->groupKey,  newRecipent) );

			} // �ű� ��带 ���� <���� Ű, {���� ��� ID}> ���� �Ϸ�


		} // else-if �� - updated_eviction == false�̰� updated_addition == true�� ��� ó�� �Ϸ�

		else if (kNode->created == true && kNode->unode == NULL ) // (2014.07.31) ���� ������ �׷� Ű�� �����ϴ� kNode�� ���
		{
			// �ش� �׷쿡 ���Ե� ��� ���� ������ Ű�� ��ȣȭ

			recipents = this->getNdsInGrp(kNode); // �ش� �׷쿡 ���� ���� ������ ���� ȹ��

			set<int>::iterator it_recipent;

			for( it_recipent = recipents.begin(); it_recipent != recipents.end(); it_recipent++) // ���� ��� ��� ��ȸ
			{
				int nodeID = *it_recipent;
				set<int> newRecipent;

				newRecipent.insert(nodeID);

				keyRecipentSet.insert ( (make_pair) (kNode->groupKey,  newRecipent) );

			} // for �� ���� - ���� ������ kNode�� �����Ǵ� �׷쿡 ���� ������ ���� <���� Ű, {���� ��� ID} ���� �Ϸ�


		} // else-if �� - created == true �̰� kNode->uNode == NULL �� ��� (��, ���� ������ group key�� ���) ó�� �Ϸ�

	}

	  // ��� ȣ�� ���� ���� - �ش� kNode�� ����� individual key�� �ش�
	if (kNode->unode != NULL)
	{
		return;
	}
	
	 // Ű Ʈ�� ��ȸ

	if (kNode->leftChild != NULL)
	{
		updateKey(kNode->leftChild); // �ش� kNode�� leftChild�� ���� ��� ȣ��
	}

	if ( kNode != root && kNode->rightSibling != NULL)
	{
		updateKey(kNode->rightSibling); // �ش� kNode�� rightChild�� ���� ��� ȣ��
	}


	// { < ���� Ű - {���� ��� ���}>  } Ȯ��
	 set <pair <GroupKey*, set<int>> >::iterator it;

	 for (it = keyRecipentSet.begin(); it != keyRecipentSet.end(); it++) // ���  < ���� Ű - {���� ��� ���}> pair�� ���� �ݺ�
	 {
		 GroupKey* gk;
		 gk = (*it).first;

		 out << "groupKey (" << gk->oldVal << ", " << gk->newVal << ")";

		 set<int> recipents;
		 recipents = (*it).second;

		 set<int>::iterator it_recipent;

		 out << ", recipents: {";

		 for (it_recipent = recipents.begin(); it_recipent != recipents.end(); it_recipent++) // ���� ��� ���
		 {
			 int nodeID;
			 nodeID = *it_recipent;
			 out << nodeID << ", ";

		 } // for�� ���� - �־��� ���� Ű�� ���� ��� ��� ��� �Ϸ�

		 out << " }" << endl;
	 }


	out.close();
}
/*
void Kmr_NEWLKH::resetUpdatedKeys(struct _tagKNode* kNode)
{
	// ���� kNode�� updated, updated_addition, updated_eviction �÷��׸� ���� (false)�� ����
	kNode->updated = false;
	kNode->updated_addition = false;
	kNode->updated_eviction = false;


	// ��� ȣ�� ���� ����
	if (kNode->unode != NULL)
	{
		return;
	}


	// ����� ȣ�� - left child

	if (kNode->leftChild != NULL)
	{
		resetUpdatedKeys(kNode->leftChild); // �ش� kNode�� leftChild�� ���� ��� ȣ��
	}

	// ����� ȣ�� - right sibling

	if ( kNode != root && kNode->rightSibling != NULL)
	{
		resetUpdatedKeys(kNode->rightSibling); // �ش� kNode�� rightChild�� ���� ��� ȣ��
	}
}
*/
void Kmr_NEWLKH::genKeyMsg() // Ű ��й� �޽��� ����
{
	/// Ű ���� - keyRecipentSet ( {<���� Ű - ���� ��� ���> } ) ����

	int idx_keyMsg = 0;
	int idx_protokeyMsg = 0;
	this->keyRecipentSet.clear(); // Ű - ���� ��� map ����
	//this->delKeyMsg(); // Ű ��й� �޽��� ���� (�޸� ��ȯ)
	updateKey(root); // Ű ����

	map<int, struct _tagProtoKeyMsg*> protoKeyMsgs; // Ű ��й� �޽��� ���� ��

	// Ű ��й� �޽��� ������Ÿ�� (prototype) ���� set <pair <GroupKey*, set<int>> >::iterator it;

	cout << " sizeof key msg: " << sizeof(KeyMsg) << endl;
	cout << " sizeof proto key msg: " << sizeof(ProtoKeyMsg) << endl;

	 set <pair <GroupKey*, set<int>> >::iterator it;

	 for (it = keyRecipentSet.begin(); it != keyRecipentSet.end(); it++) // ���  < ���� Ű - {���� ��� ���}> pair�� ���� �ݺ�
	 {
		 GroupKey* gk;
		 gk = (*it).first;
		 
		 set<int> recipents;
		 recipents = (*it).second;
		 
		 ProtoKeyMsg* protokeyMsg = new ProtoKeyMsg(); // Ű �޽��� ���� ����
		

		 protokeyMsg->newKeys.push_back(*gk); // Ű ��й� �޽����� �׷� Ű ���� ���� 
		 protokeyMsg->recipents = recipents; // Ű ��й� �޽����� ���� ��� ID ���� ����		 

		 protoKeyMsgs[idx_protokeyMsg++] = protokeyMsg;

		
	 } // for�� ���� - Ű ��й� �޽��� ���� ����
	
	// Ű ��й� �޽��� ������Ÿ���� ����: {���ŵ� (����) �׷� Ű ���, ���� ��� ����

	 
	 this->keyRecipentSet.clear(); // Ű - ���� ��� map ����


	 map<int, ProtoKeyMsg*>::iterator it_protokm1;
	 map<int, ProtoKeyMsg*>::iterator it_protokm2;

	 set<int> emptyProtoKeyMsgs; // ������ Ű ��й� �޽��� ������ map �󿡼��� key

	 ///***
	 for ( it_protokm1 = protoKeyMsgs.begin(); it_protokm1 != protoKeyMsgs.end(); it_protokm1++) // Ű ��й� �޽��� ������ ���� ������
	 {
		 int i, j;
		 i = (*it_protokm1).first;

		 if (protoKeyMsgs[i]->recipents.empty() ) // (2014.10.22) Ű ��й� �޽����� ���� ��� ������ ��� �ִ� ��� ���� �޽����� �̵�
		 {
			 continue;
		 }

		 for ( it_protokm2 = protoKeyMsgs.begin(); it_protokm2 != protoKeyMsgs.end(); it_protokm2++) // (2014.08.05)		
		 {
			 j = (*it_protokm2).first;

			  if (protoKeyMsgs[j]->recipents.empty() ) // (2014.10.22) Ű ��й� �޽����� ���� ��� ������ ��� �ִ� ��� ���� �޽����� �̵�
			 {
				 continue;
			 }

			 if (i == j)
			 {
				 continue;
			 }

			 bool isSubset = false;

			 isSubset = this->isSubset( protoKeyMsgs[j]->recipents, protoKeyMsgs[i]->recipents);

			if (isSubset == true) // (j��°) Ű ��й� �޽����� ���Ե� �׷� Ű ������ (i��°) Ű ��й� �޽����� ���Ե� �׷�Ű ������ �κ������� ���
			{
				IDSet::iterator it;

				for (it = protoKeyMsgs[j]->recipents.begin(); it != protoKeyMsgs[j]->recipents.end(); it++) // (j��°) Ű ��й� �޽����� ���Ե� ���� ��� ID�� (i��°) Ű ��й� �޽������� ����
				{
					int nodeID;
					nodeID = *it;

					protoKeyMsgs[i]->recipents.erase(nodeID);
				} // for �� ���� - (j��°) Ű ��й� �޽����� ���Ե� ��� ID�� (i��°) Ű ��й� �޽������� ����

				list<GroupKey>::iterator it_newKeys;

				for ( it_newKeys = protoKeyMsgs[i]->newKeys.begin(); it_newKeys != protoKeyMsgs[i]->newKeys.end(); it_newKeys++) // (i��°) Ű ��й� �޽����� ���Ե� �׷� Ű�� (j��°) Ű ��й� �޽����� ����
				{
					GroupKey gk = *it_newKeys;
					protoKeyMsgs[j]->newKeys.push_back(gk);
				}// for�� ���� - i��° Ű ��й� �޽����� ����� �׷� Ű�� j��° Ű ��й� �޽���

				if (protoKeyMsgs[i]->recipents.empty() == true) // 2014.10.22 ���ŵ� Ű ��й� �޽��� ���� ���� (��ġ �̵�)
				{
					emptyProtoKeyMsgs.insert(i); // ���ŵ� Ű ��й� �޽����� key ����
					break; // inner for-loop Ż��
				}

 			} // if �� - (i��°&j��°) Ű ��й� �޽��� ����

			else
			{
				
			}
		 } // inner for-loops- (i��°) Ű ��й� �޽����� (j��°) Ű ��й� �޽����� �� �Ϸ�
		 /*
		 if (protoKeyMsgs[i]->recipents.empty() == true)
		 {
			 emptyProtoKeyMsgs.insert(i); // ���ŵ� Ű ��й� �޽����� key ����
		 }
		 */

	 } // outer for-loops - (i��°) Ű ��й� �޽����� ������ Ű ��й� �޽����� ���� �� �Ϸ�  

	 // ���� ��� ����� ��� �ִ� Ű ��й� �޽��� ������ ����

	 set<int>::iterator it_protoKeyMsg;

	 for (it_protoKeyMsg = emptyProtoKeyMsgs.begin(); it_protoKeyMsg !=emptyProtoKeyMsgs.end(); it_protoKeyMsg++) // ���� ��� ����� ��� �ִ� Ű ��й� �޽��� ���� ����
	 {
		 int key = *it_protoKeyMsg;

		 delete (protoKeyMsgs[key]);
		 protoKeyMsgs.erase(key);
	 }	// ���� ��� ����� ��� �ִ� Ű ��й� �޽��� ���� ���� �Ϸ�

	 ///***

	 /// Ű ��й� �޽��� ����



	 for ( it_protokm1 = protoKeyMsgs.begin(); it_protokm1 != protoKeyMsgs.end(); it_protokm1++) // Ű ��й� �޽��� �������κ��� Ű ��й� �޽��� ����
	 {
		 ProtoKeyMsg* protoKeyMsg = (*it_protokm1).second; 

		 KeyMsg* keyMsg = new KeyMsg();
		 this->initKeyMsg(keyMsg);

		 keyMsg->newKeys = protoKeyMsg->newKeys;
		 keyMsg->recipents = protoKeyMsg->recipents;

		 keyMsgs[idx_keyMsg++] = keyMsg;
	 } // for�� - Ű ��й� �޽��� �ʱ� ���� �Ϸ�

	 /// Ű ��й� �޽��� ���� ����

	 for ( it_protokm1 = protoKeyMsgs.begin(); it_protokm1 != protoKeyMsgs.end(); it_protokm1++) // Ű ��й� �޽��� �������κ��� Ű ��й� �޽��� ����
	 {

		  ProtoKeyMsg* protoKeyMsg = (*it_protokm1).second; 
		  delete(protoKeyMsg);

	 } // Ű ��й� �޽��� ���� ���� �Ϸ�

	 protoKeyMsgs.clear();

	  /// �� ����� ����� Ŭ������ ������ Ű ��й� �޽����� ����

	   // Ŭ�����Ͱ� ����� ��� ��� ����

	set<int> nodesWithNewCH;	

	map<int, set<int>> clusters = rtn->getClusters(); // Ŭ������ ���� ȹ��

	map<int, set<int>>::iterator it_clstr; 

	for (it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++) // Ŭ������ �� ��ȸ - �� Ŭ�����Ϳ� ���� ���鿡 ���� Ŭ������ ���� ���� �˻�
	{
		set<int> nodesInCluster = (*it_clstr).second;	
		 set<int>::iterator it_ndID;

		 // (2014.09.21) ���� - 
		int CHID = (*it_clstr).first;

		if ( nodes[CHID] == NULL) // CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
		{
			continue;
		}
		// (2014.09.21 ���� �Ϸ�) CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�

		for (it_ndID = nodesInCluster.begin(); it_ndID != nodesInCluster.end(); it_ndID++) // Ŭ������ �� ��� ���� ��ȸ
		{
			bool clstrChange;

			int nodeID;
			nodeID = *it_ndID;

			if (nodes[nodeID] == NULL) // (2014.09.29) �̹� ���ŵ� ����̸� �ǳ� �ٱ� 
			{
				continue;
			}


			if (nodes[nodeID]->newbee == true) // �ű� ����� ���
			{
				nodesWithNewCH.insert(nodeID);
			}

			clstrChange = rtn->hasUpdatedCH(nodeID);

			if (clstrChange == true)
			{
				nodesWithNewCH.insert(nodeID);
			}
		} // for�� ���� - ���� Ŭ������ �� ��� ��� �˻� �Ϸ� - Ŭ������ ���� ��� Ž��

	} // for�� ���� - CH�� ����� ������ ��� ���� �Ϸ�

	

	 // (2014.07.15) ������� Ȯ��

	 // 'Ŭ������ ���� ���' ��� ��ȸ -> Ű ��й� �޽��� ����

	 set<int>::iterator it_ndWithNewCH;

	 for ( it_ndWithNewCH = nodesWithNewCH.begin(); it_ndWithNewCH != nodesWithNewCH.end(); it_ndWithNewCH++) // CH�� ����� ������ ��� ��ȸ
	 {
		 int nodeID = *it_ndWithNewCH;

		  map<int, KeyMsg*>::iterator it_km3;

		  for ( it_km3 = keyMsgs.begin(); it_km3 != keyMsgs.end(); it_km3++) // Ű ��й� �޽��� �� ��ȸ
		  {
			  KeyMsg* km = (*it_km3).second;

			  if (km->recipents.find ( nodeID) != km->recipents.end()) // ���� Ŭ������ ���� ����� ID�� ���� ��� ��Ͽ� ���Ե� ���
			  {
				   int clusterID = rtn->getClstrID(nodeID); // �ش� ����� ����� CH' id�� Ű ��й� �޽����� ���

				  if ( km->recipents.size() == 1) // Ű ��й� �޽����� ���� ��尡 1���� ���
				  {				

					  km->CHID = clusterID; 
				  } // if ��

				  else // Ű ��й� �޽����� ���� ��尡 2�� �̻��� ���
				  {
					    KeyMsg* keyMsg = new KeyMsg();
						this->initKeyMsg(keyMsg);

						keyMsg->newKeys = km->newKeys; // �׷� Ű ��� ����
						keyMsg->CHID = clusterID; // �ű� Ű ��й� �޽����� Ŭ������ ���� ����
						keyMsg->recipents.insert(clusterID); // �ű� Ű ��й� �޽����� ���� ��� ���� �̵�
						km->recipents.erase(clusterID); // ���� Ű ��й� �޽������� ���� ��� ���� ����

						keyMsgs[idx_keyMsg++] = keyMsg; // Ű ��й� �޽��� �ʿ� �߰�
					  
				  } // else ��

			  } // if�� ���� - ���� 'Ŭ������ ���� ���'�� ���� Ű ��й� �޽��� ���� 
		  } // for�� ���� - // Ű ��й� �޽��� �� ��ȸ

	 } // for�� ���� - CH�� ����� ������ ��� ��ȸ - Ű ��й� �޽����� Ŭ������ ���� ����

	 // (2014.07.16) ������� Ȯ��!!

	 /// �� ����� ����� ���� ��� ������ Ű ��й� �޽����� ����

	   // '���� ��尡 ����� ��� ���' ����

	set<int> nodesWithNewAssoc; // ���� ��尡 ����� ��� ���	

	for (it_clstr = clusters.begin(); it_clstr != clusters.end(); it_clstr++) // Ŭ������ �� ��ȸ - �� Ŭ�����Ϳ� ���� ���鿡 ���� ���� ��� ���� ���� �˻�
	{
		set<int> nodesInCluster = (*it_clstr).second;		

		set<int>::iterator it_ndID;

		// (2014.09.21) ���� - 
		int CHID = (*it_clstr).first;

		if ( nodes[CHID] == NULL) // CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
		{
			continue;
		}
		// (2014.09.21 ���� �Ϸ�) CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�

		for (it_ndID = nodesInCluster.begin(); it_ndID != nodesInCluster.end(); it_ndID++) // Ŭ������ �� ��� ���� ��ȸ
		{
			int nodeID = *it_ndID;

			bool assocChange; // ���� ��� ���� ����

			if (nodes[nodeID] == NULL) // (2014.09.29) �̹� ���ŵ� ����̸� �ǳ� �ٱ� 
			{
				continue;
			}
			

			Sec_IHA* sec_IHA = dynamic_cast <Sec_IHA*> (sec); 

			assocChange = sec_IHA->hasUpdatedAssoc(nodeID);

			if (assocChange == true) // ���� ��尡 ����� ���, ���� ��� ���� ��� ��Ͽ� �߰�
			{
				nodesWithNewAssoc.insert(nodeID);
			}// if �� ���� - ���� ��� ���� ��� ��Ͽ� �߰�

		} // inner for loop ���� - Ŭ������ �� ��� ���� �˻� �Ϸ�

	} // outer for loop - ��� Ŭ�������� ��� �˻� �Ϸ�
	
	// (2014.07.22) ������� Ȯ��!!


	  // Ű ��й� �޽����� ���� ��� ���� ����

	set<int>::iterator it_ndWithNewAssoc; // ���� ��尡 ����� ��� ����� �ݺ��� (iterator)

	for ( it_ndWithNewAssoc = nodesWithNewAssoc.begin(); it_ndWithNewAssoc != nodesWithNewAssoc.end(); it_ndWithNewAssoc++) // ���� ��尡 ����� ��� ��� ��ȸ
	{

		int nodeID = *it_ndWithNewAssoc;

		map<int, KeyMsg*>::iterator it_km4;

		for ( it_km4 = keyMsgs.begin(); it_km4 != keyMsgs.end(); it_km4++) // Ű ��й� �޽��� �� ��ȸ
		{
			 KeyMsg* km = (*it_km4).second;

			 if (km->recipents.find ( nodeID) != km->recipents.end()) // ���� ���� ��� ���� ����� ID�� ���� ��� ��Ͽ� ���Ե� ���
			 {
				 Sec_IHA* sec_IHA = dynamic_cast <Sec_IHA*> (sec); 

				 std::set<int> upperNodes = sec_IHA->upperNodes[nodeID][INTERLEAVING_T+1];
				 std::set<int> lowerNodes = sec_IHA->lowerNodes[nodeID][INTERLEAVING_T+1];

				  if ( km->recipents.size() == 1) // Ű ��й� �޽����� ���� ��尡 1���� ���
				  {	
					  km->upAssocNds = upperNodes; // Ű ��й� �޽����� ���� ���� ��� ���� ����
					  km->downAssocNds = lowerNodes; //		"			����		"
					 

				  } // if�� �Ϸ�

				  else // Ű ��й� �޽����� ���� ��尡 2�� �̻��� ���
				  {
					    KeyMsg* keyMsg = new KeyMsg();
						this->initKeyMsg(keyMsg);

						keyMsg->newKeys = km->newKeys; // �׷� Ű ��� ����
						keyMsg->CHID = km->CHID; // �ű� Ű ��й� �޽����� Ŭ������ ���� ����

						keyMsg->recipents.insert (nodeID); // �ű� Ű ��й� �޽����� ���� ��� ID ����
						km->recipents.erase (nodeID); // ���� Ű ��й� �޽������� ���� ��� id ����

						keyMsgs[idx_keyMsg++] = keyMsg; // Ű ��й� �޽��� �ʿ� �߰�

				  } // else �� ���� - �ű� Ű ��й� �޽��� ���� �� ���� ��� ���� ����
				  break;
			 } // if �� �Ϸ� - ���� Ű ��й� �޽����� ���� ��� ���� ���� �Ϸ�

		} // for�� �Ϸ� - Ű ��й� �޽����� ���� ��� ���� ���� �Ϸ�

	} // for�� �Ϸ� - Ű ��й� �޽����� ���� ��� ���� �߰� �Ϸ�

	 testKeyMsg(); // Ű ��й� �޽��� �׽�Ʈ

}

void Kmr_NEWLKH::testKeyMsg() // Ű ��й� �޽��� �׽�Ʈ
{
	ofstream out("tst_keyMsg.txt");

	 map<int, KeyMsg*>::iterator it_km1;
	

	for ( it_km1 = keyMsgs.begin(); it_km1 != keyMsgs.end(); it_km1++)
	//for (int i = 0; i < keyMsgs.size(); i++) // ������ Ű ��й� �޽��� ��ȸ �� ���Ͽ� ���
	{
		int i = (*it_km1).first;

		if (keyMsgs[i]->recipents.empty() == true)
		{
			continue;
		}

		list<GroupKey>::iterator it_newKeys;

		for(it_newKeys = keyMsgs[i]->newKeys.begin(); it_newKeys != keyMsgs[i]->newKeys.end(); it_newKeys++) // i��° Ű ��й� �޽����� �׷� Ű ��� ���
		{
			GroupKey gk = *it_newKeys;
			out << "(old key: " << gk.oldVal << " new key: " << gk.newVal << "), ";
		} // for �� ���� - �׷� Ű ��� ��� ���


		IDSet::iterator it_node;

		out << "recipents: {";

		for (it_node = keyMsgs[i]->recipents.begin(); it_node != keyMsgs[i]->recipents.end(); it_node++)  // i��° Ű ��й� �޽����� ���� ��� ��� ���
		{
			int nodeID;
			nodeID = *it_node;

			out << nodeID << ", ";
		} // for �� ���� - ���� ��� ��� ���

		out << " } " << endl;

		if ( keyMsgs[i]->CHID != -1)
		{
			out << "cluster Info. : "; // Ŭ������ ���� ���
			out << keyMsgs[i]->CHID;
			out << endl;
		}
		// ���� ��� ���� ���

		if ( !keyMsgs[i]->upAssocNds.empty())
		{
			IDSet::iterator it_upNd;

			out << "upper assoc. nodes: {"; // ���� ���� ��� ���� ���

			for ( it_upNd = keyMsgs[i]->upAssocNds.begin(); it_upNd != keyMsgs[i]->upAssocNds.end(); it_upNd++) // ���� ���� ��� ���� ���
			{
				int nodeID = *it_upNd;

				out << nodeID << ", ";
			}// for�� ���� - i ��° �޽����� ���� ���� ��� ���� ��� �Ϸ�

			out << " } " << endl;

		}// 

		if (! keyMsgs[i]->downAssocNds.empty())
		{
			IDSet::iterator it_downNd;

			out << "lower assoc. nodes: {"; // ���� ���� ��� ���� ���

			for ( it_downNd = keyMsgs[i]->downAssocNds.begin(); it_downNd != keyMsgs[i]->downAssocNds.end(); it_downNd++) // ���� ���� ��� ���� ���
			{
				int nodeID = *it_downNd;

				out << nodeID << ", ";
			}// for�� ���� - i ��° �޽����� ���� ���� ��� ���� ��� �Ϸ�

			out << " } " << endl;
		} // 

	} // for �� ����- Ű ��й� �޽��� ��� 

	out.close();

	return;	
	
}

int Kmr_NEWLKH::getKeyMsgLen(struct _tagKeyMsg* keyMsg) // Ű �޽��� ���� ��ȯ
{
	int len = 0;
	
	len += 2 + 2; // sender / receiver ID size;

	// Ű ���� ���	
	len += keyMsg->newKeys.size() * Group_Key_Size;

	// ���� ��� ID ũ��
	len += keyMsg->recipents.size() * ID_LEN;

	// CH ID, ���� ��� id�� ��Ȳ�� ���� ���Ե� ����, ���� �� �� ���� ����

	// CH ID ũ��
	if (keyMsg->CHID > -1)
	{
		len += ID_LEN;
	}
	// ���� ��� ID ũ��
	len += keyMsg->upAssocNds.size() * ID_LEN;
	len += keyMsg->downAssocNds.size() * ID_LEN;
	
	return len;
}

bool Kmr_NEWLKH::handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode)
{
	
	bool res = true; // ���� ��� ���� (true: ��� ����, false: ���� �Ϸ�)

	bool thisIsRecipent = false; // ���� Ű ��й� �޽����� ������ ��尡 �ش� Ű ��й� �޽����� ���� ��� �� �ϳ����� ���� (T/F)

	// ���� ����� ID�� ���� ��� ��Ͽ� �ִ� �� �˻�

	IDSet::iterator it;	

	for ( it = keyMsg->recipents.begin(); it != keyMsg->recipents.end(); it++) // ���� ���� ��尡 ���� ��� ��Ͽ� ���ԵǾ� �ִ��� Ȯ��
	{
		int recipentID = (*it);

		if ( cNode->id == recipentID)
		{
			this->receivedNodes.insert(cNode->id); // (2014.08.07) Ű ��й� �޽��� ���� ��� ���� ���� (debugging ��)
			thisIsRecipent = true;		
		}
	} // for�� ���� - ���� ��� ��� Ȯ�� �Ϸ�

	if (thisIsRecipent == true)
	{
		keyMsg->recipents.erase(cNode->id); // ���� ��尡 ���� ��� ��Ͽ� ���ԵǾ� ������ �ش� ��Ͽ��� ���� ��� ID ����

		if ( keyMsg->recipents.empty())
		{
			res = false;
		}
	} // IF�� ���� - ���� ��尡 ���� ����� ����� ó�� �Ϸ�

	/*
	if (thisIsRecipent == false) // ���� ��尡 ���� ��尡 �ƴϸ� ��ȯ
	{
		return res;
	}
	*/


	// �׷� Ű ���� (Ű Ʈ���� �ִ� �׷� Ű�� ������ ����Ǿ��ٰ� ����)


	// Ŭ������ ���� ���� (rtn)
	/*
	int cNodeID = cNode->id;
	int clusterID = keyMsg->CHID;

	if (clusterID > -1)
	{
		rtn->changeCluster(cNodeID, clusterID); 
	}

	// ���� ��� ���� ����

	NodeIDLst::iterator NodeIDLst_it;

	IDSet::iterator NodeIDSet_it;

	if (!keyMsg->upAssocNds.empty())
	{

		std::set<int> upperAssocnodeset;

		for (NodeIDSet_it = keyMsg->upAssocNds.begin(); NodeIDSet_it != keyMsg->upAssocNds.end(); NodeIDSet_it++)
		//for (NodeIDLst_it = keyMsg->upAssocNds.begin(); NodeIDLst_it != keyMsg->upAssocNds.end(); NodeIDLst_it++)
		{
			int nodeID = (*NodeIDSet_it);

			upperAssocnodeset.insert(nodeID);
		}

		sec->updateAssocNodes(cNodeID, 0, INTERLEAVING_T+1, upperAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)
		//sec->updateAssocNodes(cNodeID, 0, upperAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)
	}

	if (!keyMsg->downAssocNds.empty())
	{

		std::set<int> lowerAssocnodeset;

		for (NodeIDSet_it = keyMsg->downAssocNds.begin(); NodeIDSet_it != keyMsg->downAssocNds.end(); NodeIDSet_it++)
		//for (NodeIDLst_it = keyMsg->downAssocNds.begin(); NodeIDLst_it != keyMsg->downAssocNds.end(); NodeIDLst_it++)
		{
			int nodeID = (*NodeIDSet_it);

			lowerAssocnodeset.insert(nodeID);
		}

		sec->updateAssocNodes(cNodeID, 1, INTERLEAVING_T+1, lowerAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)
		//sec->updateAssocNodes(cNodeID, 1, lowerAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)
	}
	*/
	return res;
}

/*
void Kmr_NEWLKH::genKeyMsg()
{

}
*/
/*
KeyMsg* Kmr_NEWLKH::getKeyMsg() // Ű ��й� �޽��� ��ȯ
{

}
*/
/*
void Kmr_NEWLKH::addPairKey (int nID, int nID2)
{

}
*/
int Kmr_NEWLKH::cnstrctKTree() // Ű Ʈ�� �ʱ� ����
{
	int height = 0;

	this->root = new KNode;


	// (2014.04.23) root ��� �ʱ�ȭ �ʿ�!!
	// root ��� �ʱ�ȭ
	this->initKNode (root);
	//root->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

	/*
	root->groupKey = new GroupKey;
	root->parent = NULL;
	root->leftChild = NULL;
	root->leftSibling = NULL;
	root->rightSibling = NULL;
	root->unode = NULL;
	root->level = 0;
	root->index = 0;
	*/
	int numNodes = nodes.size();

	SrchNode_KTS::setNumNodes(numNodes); // (2014.04.22) ���� ������ �ʿ��� ���� (���� ��� ����) �� ����

	SrchNode* solution = astar->findSolution(); // key tree ������ �ʿ��� degree ���� ȹ��

	//std::list<SrchNode*> degreeSeq = astar->findSolution(); // key tree ������ �ʿ��� degree ���� ȹ��

	// key tree ����

	//list<SrchNode*>::iterator it;

	int cur_level = 0;	

	map <int, int> degreeSeq;

	try 
	{

		SrchNode_KTS* srchNode_KTS = dynamic_cast<SrchNode_KTS*>(solution);

		degreeSeq = srchNode_KTS->getDegreeSeq();  // �߰ߵ� �ؿ� �����Ǵ� degree ���� ȹ��

	} catch (exception& e) {cout << "Exception: " << e.what();}

	height = degreeSeq.size() +1; // (2014.06.10) Ű Ʈ���� ���� = degree seq.�� ũ�� +1 
	this->kTreeHeight = height;

	

	map <int, int>::iterator it;

	int currentLevel = 0;
	numNodesAtLevel[0] = 1; // level 0�� ��� �� ����

	cur_level_knodes.clear();
	cur_level_knodes.push_back(root); // ���� level ��� ��� ���� (level 0: root)

	for(it = degreeSeq.begin(); it != degreeSeq.end(); it++)
	{
		currentLevel = it->first + 1;
		int degree = it->second;
		numNodesAtLevel[currentLevel] = numNodesAtLevel[currentLevel-1] * degreeSeq[currentLevel-1]; // ���� level�� ��� �� ����
		expandLevel(currentLevel, degree);
		//this->expandLevel(degree);
	} // level 0 ~ (h-1) ���� ����


	// /
	list<struct _tagKNode*>::iterator it2;

	for (it2 = cur_level_knodes.begin(); it2!= cur_level_knodes.end(); it2++)
	{
		struct _tagKNode* kNode = *it2;

		UNode* uNode = new UNode; // u-node ����	 
		uNode->sensorNode = NULL;
		uNode->isCH = false;
		kNode->unode = uNode; // k-node, u-node �� ������ ����
		uNode->parent = kNode;

		if (kNode->parent != NULL)// (2014.06.10) cluster key ��� ��� ����, isCluster �÷��� ����
		{
			kNode->parent->isCluster = true; // 
			this->cKNodes.insert(kNode->parent);
		}
	}
	// level h (u-node) ����

	//height = currentLevel + 1; // height = cluster key level +2

	numNodesAtLevel[height] = numNodesAtLevel[height-1]; // level-h�� ��� �� (u-node)
		

	// ...
	//this->kTreeHeight = height;
	return height; // key tree�� ���� ��ȯ
}

/*
int Kmr_NEWLKH::cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes) // (2014.06.05) // Ű Ʈ�� �κ��� �籸��
{
	int height = 0;

	// A*�� ���� degree seq. ���� 

	int numNodes;
	numNodes = nodes.size();

	SrchNode_KTS::setNumNodes(numNodes); // (2014.04.22) ���� ������ �ʿ��� ���� (���� ��� ����) �� ����

	SrchNode* solution = astar->findSolution(); // key tree ������ �ʿ��� degree ���� ȹ��

	int cur_level = 0;	

	map <int, int> degreeSeq;

	try 
	{

		SrchNode_KTS* srchNode_KTS = dynamic_cast<SrchNode_KTS*>(solution);

		degreeSeq = srchNode_KTS->getDegreeSeq();  // �߰ߵ� �ؿ� �����Ǵ� degree ���� ȹ��

	} catch (exception& e) {cout << "Exception: " << e.what();
	}

	// �κ��� key tree ���� 

	// ������ root �� ���� �ִ� ���� Ʈ���� ����
	struct _tagKNode* prevKNode;
	struct _tagKNode* curKNode;
	prevKNode = curKNode = NULL;

	curKNode = root->leftChild;

	while (curKNode != NULL) // root�� �� �ڽ� knode�� root�� �ϴ� subtree ����
	{
		prevKNode = curKNode;
		curKNode = curKNode;
		delete (prevKNode);
	} // root ����� ���� Ʈ�� ���� �Ϸ�

	cur_level_knodes.clear();
	cur_level_knodes.push_back(root); // ���� level ��� ��� ���� (level 0: root)

	map <int, int>::iterator it;

	for(it = degreeSeq.begin(); it != degreeSeq.end(); it++)
	{
		cur_level = it->first + 1;
		int degree = it->second;
		//numNodesAtLevel[currentLevel] = numNodesAtLevel[currentLevel-1] * degreeSeq[currentLevel-1]; // ���� level�� ��� �� ����
		expandLevel(cur_level, degree);
		
	} // level 0 ~ (h-1) ���� ����

	list<struct _tagKNode*>::iterator it2;

	for (it2 = cur_level_knodes.begin(); it2!= cur_level_knodes.end(); it2++) // (h-1)-level�� k-node�鿡 ���� �ݺ�
	{
		struct _tagKNode* kNode = *it2;

		UNode* uNode = new UNode; // u-node ����	 
		this->initUNode(uNode); // u-node �ʱ�ȭ
		
		kNode->unode = uNode; // k-node, u-node �� ������ ����
		uNode->parent = kNode;
	}
	// level h (u-node) ����

	height = cur_level + 1; // height = individual key level +1

	return height;
}*/

int Kmr_NEWLKH::cnstrctKTree(struct _tagKNode* root, set<int> nodes) // (2014.06.08) // Ű Ʈ�� �κ��� �籸��
{
	int height = 0;

	root->CHID = -1; // (2014.09.27) CH ID ����

	/* A*�� ���� degree seq. ���� */

	int numNodes;
	numNodes = nodes.size();

	SrchNode_KTS::setNumNodes(numNodes); // (2014.04.22) ���� ������ �ʿ��� ���� (���� ��� ����) �� ����

	SrchNode* solution = astar->findSolution(); // key tree ������ �ʿ��� degree ���� ȹ��

	int cur_level = 0;	

	map <int, int> degreeSeq;

	try 
	{

		SrchNode_KTS* srchNode_KTS = dynamic_cast<SrchNode_KTS*>(solution);

		degreeSeq = srchNode_KTS->getDegreeSeq();  // �߰ߵ� �ؿ� �����Ǵ� degree ���� ȹ��

	} catch (exception& e) {cout << "Exception: " << e.what();
	}

	/* �κ��� key tree ���� */

	// ������ root �� ���� �ִ� ���� Ʈ���� ����
	struct _tagKNode* prevKNode;
	struct _tagKNode* curKNode;
	prevKNode = curKNode = NULL;

	curKNode = root->leftChild;

	if (curKNode != NULL)
	{
		dstrctKTree(curKNode); 
	}
	root->leftChild = NULL; // �ڽ� kNode����� ���� ����
	/*
	while (curKNode != NULL) // root�� �� �ڽ� knode�� root�� �ϴ� subtree ����
	{
		prevKNode = curKNode;
		curKNode = curKNode->rightSibling;
		dstrctKTree(prevKNode); 
		//delete (prevKNode);
	} // root ����� ���� Ʈ�� ���� �Ϸ�
	*/
	cur_level_knodes.clear();
	cur_level_knodes.push_back(root); // ���� level ��� ��� ���� (level 0: root)

	map <int, int>::iterator it;

	for(it = degreeSeq.begin(); it != degreeSeq.end(); it++)
	{
		cur_level = it->first + 1;
		int degree = it->second;
		//numNodesAtLevel[currentLevel] = numNodesAtLevel[currentLevel-1] * degreeSeq[currentLevel-1]; // ���� level�� ��� �� ����
		expandLevel(cur_level, degree);
		
	} // level 0 ~ (h-1) ���� ����

	list<struct _tagKNode*>::iterator it2;

	for (it2 = cur_level_knodes.begin(); it2!= cur_level_knodes.end(); it2++) // (h-1)-level�� k-node�鿡 ���� �ݺ�
	{
		struct _tagKNode* kNode = *it2;

		UNode* uNode = new UNode; // u-node ����	 
		this->initUNode(uNode); // u-node �ʱ�ȭ
		
		kNode->unode = uNode; // k-node, u-node �� ������ ����
		uNode->parent = kNode;

		if (kNode->parent != NULL)// (2014.06.10) cluster key ��� ��� ����, isCluster �÷��� ����
		{
			kNode->parent->isCluster = true; // 
			this->cKNodes.insert(kNode->parent);
		}
	}
	// level h (u-node) ����

	height = cur_level + 1; // height = individual key level +1

	return height;
}

int Kmr_NEWLKH::updateKTree(IDSet addedNodes, IDSet deletedNodes) // (2014.05.29) Ű Ʈ�� ���� �Լ� - �Է�: �߰� ��� ��� (addedNodes), ���� ��� ��� (deletedNodes), ���: Ű Ʈ���� ����
{

	int height;		

	set<struct _tagKNode*> modiClstrKey; // ��� �߰�/���ŷ� ���� ����� Ŭ������ Ű ���

	this->restruct_pts.clear(); // �� ����ȭ point ��� ����

	// root ��� ���� �� �ʱ�ȭ -> Kmr ��ü�� �����ڿ��� ó��

	/* Ű Ʈ�� �󿡼� ��� ���� */
	
	//if (nodes.empty() == false)// ��� ����� ��� ���� ���� ��쿡�� ����
	{
		struct _tagKNode* clstrKey;
		clstrKey = NULL;

		struct _tagKNode* indiKey;
		indiKey = NULL;

	//	int clusterLevel = this->kTreeHeight - 2;
	//	int numClusters;

	//	numClusters = getNumNodesAtLevel(clusterLevel); // Ŭ�������� ���� ȹ��

		set<struct _tagKNode*>::iterator it_ckNodes;

		for (it_ckNodes = this->cKNodes.begin(); it_ckNodes != cKNodes.end(); it_ckNodes++) // Ŭ������ Ű (kNode) ��ȸ
		//for (int cl = 0; cl < numClusters; cl++) // // �� Ŭ�����Ϳ� ���� �ݺ�
		{
			clstrKey = *it_ckNodes; // (2014.06.10) Ŭ������ Ű ȹ��

			//clstrKey = this->getKNode(clusterLevel, cl); // Ŭ������ Ű ȹ��

			indiKey = clstrKey->leftChild;

			struct _tagKNode* oldKey;
			oldKey = NULL;

			while (indiKey != NULL) //   // Ŭ������ �� ������ individual key�� ���� �ݺ�
			{

				struct _tagUNode* uNode = NULL;
				uNode = indiKey->unode; // u-Node ȹ��
				Node* node = uNode->sensorNode;

				if (node == NULL)
				{
					indiKey= indiKey->rightSibling; // ���� individual key �� �̵�
					continue;
				}

				IDSet::iterator it_find;
				it_find = deletedNodes.find( node->id); // ���� ��� ��Ͽ� �ִ� �� �˻�

				if (it_find != deletedNodes.end()) // ���� indi_key �� ������ ��尡 ���� ��� ��Ͽ� �ִ� ���
				{
					modiClstrKey.insert(clstrKey); // ����� Ŭ������ Ű ��� �߰�

					if (clstrKey->CHID == node->id)// (2014.07.03)
					{
						set<int> nodesInGrp;
						
						nodesInGrp = this->getNdsInGrp(clstrKey);

						restruct_pts[clstrKey] = nodesInGrp; // ���� ���� CH�� ���ŵ� ��쿡�� �ش� Ŭ�������� Ű�� ����� kNode�� �籸��ȭ ��Ͽ� �߰�
					
						rtn->removeCH(node->id);
					
						clstrKey->CHID = -1; // (2014.09.27) �ش� kNode�� CH ������ ����
					}

					oldKey = indiKey;

					struct _tagKNode* tKNode; // eviction-point���� root������ ��ο� ��ġ�ϴ� k-node�� update_eviction �÷��� ����

					tKNode = oldKey->parent; // kNode �����͸� eviction-point�� �ʱ�ȭ

					while (tKNode != NULL) // root �������� ��ο� �����ϴ� kNode�鿡 ���� �ݺ�
					{
						tKNode->updated = true; 
						tKNode->updated_eviction = true; // update_eviction �÷��� ����
						tKNode = tKNode->parent;
					}

					delete (uNode); // u-node ����

					// (2014.07.14) Ŭ������ ���� ����

					rtn->removeFromCluster(node->id); // ��Ʈ��ũ���� ���ŵ� ���� ��带 Ŭ������ ��Ͽ��� ����

					// ������ ����

					if (indiKey->leftSibling == NULL) // ���� indiKey�� cluster Key�� leftChild �� ���
					{
						clstrKey->leftChild = indiKey->rightSibling;
					}
					else // indiKey->leftSibling != NULL /// ���� indiKey�� cluster Key�� leftChild�� �ƴ� ���
					{
						indiKey->leftSibling->rightSibling = indiKey->rightSibling; // left sibling -> right sibling ���� ����
					}

					if (indiKey->rightSibling != NULL)
					{
						indiKey->rightSibling->leftSibling = indiKey->leftSibling; // left sibling <- right sibling ���� ����
					}

					indiKey = indiKey->rightSibling;

					delete (oldKey); 

					clstrKey->outDegree--; // (2014.09.04) eviction point�� outDegree -1����

					continue;
				} // ���� ��� ���� �Ϸ�
			
				indiKey = indiKey->rightSibling;

			} // while �� ���� - ���� Ŭ������ �� ��� ���� �Ϸ�
		} // for �� ���� - ��� Ŭ�����͵��� ��� ���� �Ϸ�
	} // ��� ���� �۾� �Ϸ�
	// (2014.06.10) ������� Ȯ�� �Ϸ�!

	//this->printKTree(); // debugging ��
	//cout << endl << endl;
	/* Ű Ʈ�� �󿡼� ��� �߰� */

	IDSet::iterator it_add;

	//if (nodes.empty() != true)
	{
		//int clusterLevel = this->kTreeHeight - 2;
		//int numClusters;

		//numClusters = getNumNodesAtLevel(clusterLevel); // Ŭ�������� ���� ȹ��



		for (it_add = addedNodes.begin(); it_add != addedNodes.end(); it_add++) // �߰��� ���鿡 ���� �ݺ�
		{
			set<struct _tagKNode*> adjClstrKey; // ���� Ŭ�����͵��� kNode ����

			int nodeID;

			nodeID = *it_add; // ��� ID ȹ��

			Node* newNode;
		
			newNode = nodes[nodeID]; 

			// �ű� ��尡 �߰��� Ŭ������ ���� 

			struct _tagKNode* clusterKey; // cluster�� �����Ǵ� ������ k-node�鿡 ���� �ݺ�						

			set<struct _tagKNode*>::iterator it_ckNodes;

			for (it_ckNodes = this->cKNodes.begin(); it_ckNodes != cKNodes.end(); it_ckNodes++) // Ŭ������ Ű (kNode) ��ȸ
			//for (int cl = 0; cl < numClusters; cl++) // // �� Ŭ�����Ϳ� ���� �ݺ�
			{
				clusterKey = *it_ckNodes; // (2014.06.10) Ŭ������ Ű ȹ��

				//clusterKey = this->getKNode(clusterLevel, cl); // Ŭ������ Ű ȹ��
				
				// (2014.06.20) ���� Ŭ������ ��� ��� ���� 

				double dist; // �ű� ���� ���� Ŭ�������� ���� �߽ɰ��� �Ÿ� ���

				dist = sqrt( pow( (double) newNode->x - clusterKey->COG_X, 2) + pow ( (double) newNode->y - clusterKey->COG_Y, 2 ));

				if (dist < TRANS_RANGE) // �ű� ���� ���� CH ���� �Ÿ��� ���� ���� �̳��̸� ���� Ŭ������ Ű ��Ͽ� KNODE �߰�
				{
					adjClstrKey.insert(clusterKey); 
				}

				/*
				int CH_ID ;
				CH_ID = clusterKey->CHID;

				if (CH_ID < 0)
				{
					cerr << "(CH_ID < 0)" << endl;
					exit(-1);
				}

				double dist; // �ű� ���� ���� CH���� �Ÿ� ���

				dist = sqrt( pow( (double) newNode->x - nodes[CH_ID]->x, 2) + pow ( (double) newNode->y - nodes[CH_ID]->y, 2 ));

				if (dist < TRANS_RANGE) // �ű� ���� ���� CH ���� �Ÿ��� ���� ���� �̳��̸� ���� Ŭ������ Ű ��Ͽ� KNODE �߰�
				{
					adjClstrKey.insert(clusterKey); 
				}
				*/

			} // ���� Ŭ������ ��� ���� // 
			// (2014.06.10) ������� Ȯ�� �Ϸ�!
			int minNumNdsInClstr = -1;
			struct _tagKNode* clstrWithMinNumNds = NULL;

			set<struct _tagKNode*>::iterator it_clstr;

			for (it_clstr = adjClstrKey.begin(); it_clstr != adjClstrKey.end(); it_clstr++)
			{
				struct _tagKNode* clstrKey;
				clstrKey = *it_clstr;

				int numNodes = this->getNumNdsInGrp(clstrKey); // Ŭ�����Ϳ� ���� ��� �� ���

				if ( minNumNdsInClstr < 0) // �ּ� ��� ���� ���� �ʱ�ȭ ���� ���� ���
				{
					minNumNdsInClstr = numNodes;
					clstrWithMinNumNds = clstrKey;
				}
				else if ( numNodes < minNumNdsInClstr) // ��� ���� ���� �ּ� ��� ������ ������ ��� ���� Ŭ������ Ű�� ����
				{
					minNumNdsInClstr = numNodes;
					clstrWithMinNumNds = clstrKey;
				}
			}	// �ű� ��尡 �߰��� Ŭ������ ����

			// �ű� ��带 ���õ� Ŭ�����Ϳ� �߰�

			struct _tagKNode* addPoint;
			addPoint = clstrWithMinNumNds; // addition_point ����

			if (addPoint == NULL) // ������ Ŭ�����Ͱ� ���� ��� ���� �ű� ��� skip, ���� �ű� ���� ����
			{
				continue;
			}

			struct _tagKNode* kNode = new struct _tagKNode; // k-node ����
			this->initKNode(kNode); // k-node �ʱ�ȭ

			struct _tagUNode* uNode = new struct _tagUNode; // u-node ����
			this->initUNode(uNode);  // u-node �ʱ�ȭ	

			
			kNode->unode = uNode;
			kNode->parent = addPoint;

			uNode->parent = kNode;
			uNode->sensorNode = newNode;

			if (addPoint->leftChild != NULL)
			{
				kNode->rightSibling = addPoint->leftChild;	// left - right sibling ��� �� ���� ����
				kNode->rightSibling->leftSibling = kNode; 
			}

			addPoint->leftChild = kNode; // addition point�� left child ���� ����			

			rtn->changeCluster(newNode->id, clstrWithMinNumNds->CHID); // �ű� ����� Ŭ������ ����

			modiClstrKey.insert(clstrWithMinNumNds); // ����� Ŭ������ Ű ��� �߰�

			addPoint->outDegree++; // (2014.09.04) addition point�� out degree +1����


			// �ű� ��� �߰� �Ϸ�

			// add_point���� root������ ��ο� �����ϴ� k-node���� update_addition �÷��� ����

			struct _tagKNode* tKNode; // addition-point���� root������ ��ο� ��ġ�ϴ� k-node�� update_addition �÷��� ����

			tKNode = addPoint;

			while (tKNode != NULL) // root �������� ��ο� �����ϴ� kNode�鿡 ���� �ݺ�
			{
				tKNode->updated = true; 
				tKNode->updated_addition = true; // updated_addition �÷��� ����
				tKNode = tKNode->parent; // ���� k-node�� �̵�
			}
						
			//this->printKTree(); // debugging ��
			//cout << endl << endl;
		} // for �� - �ű� ��� �߰� �Ϸ�
	}

	// (2014.06.11) ������� Ȯ��

	//this->printKTree(); // debugging ��

	/* �� ����ȭ ���� Ž�� */
	//this->restruct_pts.clear(); // �� ����ȭ point ��� ����
	struct _tagKNode* restruct_pt = NULL; // �� ����ȭ point

	struct _tagKNode* groupKey = NULL; // �׷� Ű

	set<struct _tagKNode*>::iterator it_modiClstr;

	for (it_modiClstr = modiClstrKey.begin(); it_modiClstr != modiClstrKey.end(); it_modiClstr++) // ����� Ŭ�����͵鿡 ���� �ݺ�
	{
		struct _tagKNode* modiCK; // ����� Ŭ�������� Ŭ������ Ű
		modiCK = *it_modiClstr;

		groupKey = modiCK; // ���� �˻����� �׷� Ű

		int numNodesinGrp; // �׷쿡 ���� ��� ��
		int numClstrsinGrp; // �׷쿡 ���� Ŭ������ ��
		set<int> nodesInGrp; // �׷쿡 ���� ��� ���

		numClstrsinGrp = 1; 

		bool reconstruct_needed = false; // �籸��ȭ �ʿ伺 ���� (T: �ʿ�, F: ���ʿ�)

		while (groupKey != root) // 'Ŭ������ �� ��� ��'�� ���� ������ ���ϴ� �ּ� �׷��� root ���
		{
			nodesInGrp = this->getNdsInGrp(groupKey);// �׷쿡 ���� ��� id ���� ȹ��

			

			numNodesinGrp = nodesInGrp.size();// �׷쿡 ���� ��� �� ���

			//numNodesinGrp = this->getNumNdsInGrp(groupKey); // �׷쿡 ���� ��� �� ���
			numClstrsinGrp = this->getNumClstrsInGrp(groupKey); // �׷쿡 ���� Ŭ������ �� ���

			int nodesPerCluster;
			nodesPerCluster = (numNodesinGrp / numClstrsinGrp); // Ŭ������ �� ��� ��� �� ���
			
			if ( (nodesPerCluster >= MIN_NNC) && (nodesPerCluster <= MAX_NNC )) // ���� ��� ���� �����ϴ� ��� �ݺ��� Ż��
			//if ( (nodesPerCluster >= INTERLEAVING_T+1) && (nodesPerCluster <= 2* (INTERLEAVING_T+1) ) ) // ���� ��� ���� �����ϴ� ��� �ݺ��� Ż��
			{
				if (reconstruct_needed == true) // �� ����ȭ�� �ʿ��� ��� �籸��ȭ ���� ����
				{
					restruct_pt = groupKey;
				}			
				break;
			}
			else
			{
				reconstruct_needed = true;
				groupKey = groupKey->parent;
			}

		} // 'Ŭ������ �� ��� ��'�� ���� ������ ���ϴ� �ּ� �׷��� root ���, �ʿ� �� �� ����ȭ ���� ����

		if (reconstruct_needed == true)
		{
			//restruct_pts.insert(groupKey); 

			if (groupKey == root)
			{
				nodesInGrp = this->getNdsInGrp(groupKey);// �� ����ȭ ������ root�� ��� ��� ���� �ٽ� ���
			}
			
			restruct_pts[groupKey] = nodesInGrp; // �� ����ȭ ������ �ش� �׷��� �Ҽ� ��� ID ���� ����
			
		}
	}
	// �� ����ȭ ���� Ž�� �Ϸ�
	// (2014.08.01) ������� Ȯ��

	/// ���� �ٸ� �� ����ȭ ���� ���̿� subtree ���谡 ������ ���, �� ����ȭ ���� ���� ���� �ʿ�
	// (2014.08.01 ��������)

	map<struct _tagKNode*, set<int> >::iterator it_restrct_pt;
	map<struct _tagKNode*, set<int> >::iterator it_restrct_pt2;

	set<struct _tagKNode*> deleted_restrct_pts;

	for (it_restrct_pt = restruct_pts.begin(); it_restrct_pt != restruct_pts.end(); it_restrct_pt++) // ���ŵ� �� ����ȭ ���� ��� ����
	{
		struct _tagKNode* restrct_point = (*it_restrct_pt).first;

		for ( it_restrct_pt2 = restruct_pts.begin(); it_restrct_pt2 != restruct_pts.end(); it_restrct_pt2++) // ���� �� ����ȭ ������ ������ �� ����ȭ ������ �� ��
		{
			struct _tagKNode* restrct_point2 = (*it_restrct_pt2).first;
			struct _tagKNode* temp = restrct_point2;

			bool isDescendent = false; // 2��° �� ����ȭ ������ 1��° �� ����ȭ ������ �ļ�

			if (restrct_point == restrct_point2)
			{
				continue;
			}

			while ( 1) // 2��° �� ����ȭ ������ 1��° �� ����ȭ ������ �ļ����� Ȯ��
			{

				if (temp == restrct_point)
				{
					isDescendent = true;
					break;
				}
				else if ( temp == root)
				{
					break;
				}

				temp = temp->parent;
			} // while�� �Ϸ� - 	2��° �� ����ȭ ������ 1��° �� ����ȭ ������ �ļ����� Ȯ�� �Ϸ�		

			if (isDescendent == true) // ���� �� ����ȭ ������ �������� �ϴ� �ٸ� �� ����ȭ ������ ���ŵ� �� ����ȭ ���� ��Ͽ� �߰�
			{
				deleted_restrct_pts.insert(restrct_point2);
			} // if�� �Ϸ�

		} // for �� �Ϸ� - ���� �� ����ȭ ������ �������� �ϴ� �ٸ� �� ����ȭ ������ ���ŵ� �� ����ȭ ���� ��Ͽ� �߰�

	} // for �� �Ϸ� - ���ŵ� �� ����ȭ ���� ��� ����


	// �� ����ȭ ���� �� ������ - ������ �� ����ȭ �������� root �������� ��� �� �ٸ� �� ����ȭ ������ �����ϴ� ���

	set<struct _tagKNode*>::iterator it_del_restrct_pt2;

	for (it_del_restrct_pt2 = deleted_restrct_pts.begin(); it_del_restrct_pt2 !=deleted_restrct_pts.end(); it_del_restrct_pt2++) // 
	{
		struct _tagKNode* del_restrct_pt = *it_del_restrct_pt2;
		restruct_pts.erase (del_restrct_pt);
	} // for�� ���� - ���ʿ��� �� ����ȭ ���� ����

	// (2014.08.01 �����Ϸ�)

	/* ������ �� ����ȭ ������ ���� Ű Ʈ�� �κ��� �籸�� */

	//map<struct _tagKNode*, set<int> >::iterator it_restrct_pt;
	//set<struct _tagKNode*>::iterator it_restrct_pt;

	for (it_restrct_pt = restruct_pts.begin(); it_restrct_pt != restruct_pts.end(); it_restrct_pt++) // �� ����ȭ �����鿡 ����, �ش� �׷� ���� ��� ����� �ٽ� ����
	{
		struct _tagKNode* restrct_point;
		restrct_point = (*it_restrct_pt).first;

		set<int> nodesInGrp;
		nodesInGrp = this->getNdsInGrp(restrct_point);

		(*it_restrct_pt).second = nodesInGrp;

	}



	for (it_restrct_pt = restruct_pts.begin(); it_restrct_pt != restruct_pts.end(); it_restrct_pt++)
	{
		struct _tagKNode* restrct_point;
		restrct_point = (*it_restrct_pt).first;
		
		set<int> nodesInGrp;
		nodesInGrp = (*it_restrct_pt).second;

		this->cnstrctKTree(restrct_point, nodesInGrp); // Ű Ʈ�� �籸��ȭ
		//this->cnstrctKTree(restrct_point, ); // node map �ʿ�
	}

	// (2014.06.12) ������� Ȯ��!

	/* level, index �缳�� */

	height = this->renumberKNodes();
	this->kTreeHeight = height; // (2014.10.09) Ű Ʈ�� ���� ����
	//this->printKTree(); // debugging

	return height;
}

void Kmr_NEWLKH::dstrctKTree()
{
	// ���� �� 

	// ��� ������� ����	

	try
	{
		dstrctKTree(root);
	} catch (exception& e) {cout << "Exception: " << e.what();}

}

void Kmr_NEWLKH::dstrctKTree(struct _tagKNode* cur_knode)
{
	struct _tagKNode* leftChild = cur_knode->leftChild;
	struct _tagKNode* rightSibling = cur_knode->rightSibling;

	// (2014.06.05) individual key�� ���, u node ��ȯ
	if (cur_knode->unode != NULL)
	{
		delete (cur_knode->unode);
	}

	if (leftChild != NULL) // left child, right- sibling, �ڱ� �ڽ��� ������ ����
	{
		dstrctKTree(leftChild);
	}

	if (rightSibling != NULL)
	{
		dstrctKTree(rightSibling);
	}

	set<struct _tagKNode*>::iterator it_find;

	it_find = this->cKNodes.find(cur_knode);

	if (it_find != cKNodes.end() ) // (2014.06.10) kNode�� ������ ��, cluster key ��Ͽ� ������ ��Ͽ��� ����
	{
		cKNodes.erase(cur_knode); 
	}

	delete (cur_knode);
}

void Kmr_NEWLKH::cnstrctLBHG() // (2014.09.03 ����) ������ �׷� ����
{
	
	/* 1. GA�� Ȱ���� �� ���� */

	Chromosome solution; // ������ �׷� ������ ���� �� (solution)

	// ��ü�� ���� (��ü �� ������ ����) ���
	
	int indiLen = 0;

	double dist; // �� ��忡�� CH������ �Ÿ�

	for (int i = 0; i < getKTreeHeight(); i++) // Ű Ʈ���� ���� k-node���� ������ ����
	{
		indiLen += getNumNodesAtLevel(i);
	}

	ga->setFullSetNodes(nodes); // (2014.09.19) 
	ga->setNodes(nodes);
	ga->setKmr(this);
	ga->setRoot(root);

	ga->setIndiLen(indiLen); // ��ü ���� ����

	solution = ga->findSolution(); // GA �˰����� ���Ͽ� �� ����
	ga->testSolution();
	
	/* 2. ����� �ظ� �м��Ͽ� key tree�� �ش� leaf node�� ���� ��� ���� ���� */

	this->isolatedNodes.clear(); // ���� ��� ��� ����

	unsigned int geneIdx = 0; // ��ȯ�� ��ü�� ����ü�� scan �ϱ� ���� �ε���

	struct _tagKNode* kNode = root;// ���� Ž�� ���� Ű Ʈ�� ���� k-node

	int level = 0; // ������ �׷� ���� ���� �󿡼��� level
	int index = 0; // ������ �׷� ���� ���� �󿡼��� index

	int ndIdxInClstr = 0; // ���� ������ �׷쿡 ���Ե� ����� �ε���

	map<int, char> solGeneType = ga->getSolGeneType(); // �ؿ� ���Ե� �������� ���� ( 'G': group, 'N': node)

	for (geneIdx = 0 ; geneIdx < solution.size(); geneIdx++) // ����ü�� ���Ե� ������ ��ĵ -> Ű Ʈ���� leaf node�� ���� ���� ��� �ּ� ����
	{
		Gene gene = solution[geneIdx];

		switch ( solGeneType[geneIdx])
		{

		case 'G':



			level = gene.first;// ������ �׷��� level�� index ȹ��
			index = gene.second; 

			kNode = getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			ndIdxInClstr = 0;
			kNode->numSensorNodes = ndIdxInClstr; // ������ �׷� �� ���� ��� �� ���� (reset)

			break;

		case 'N':
			{
				// ���� k-node�� Ŭ������ Ű�� �ش�
				// individual key
				// Ŭ������ �� ndIdxInClstr ��° ���Կ� ���� ��� ���� ����

				struct _tagKNode* tKNode = kNode->leftChild; // Ŭ������ Ű ������ ù ��° individual key�� �̵�

				for (int j = 0; j < ndIdxInClstr; j++)
				{
					tKNode = tKNode->rightSibling; // ���� ��� ������ ������ �������� �̵�
				}

				int nodeID = gene.second; // ���� ��� ID ȹ��				
				
				if ( nodeID >=0) // node ID�� �����ϰ� �ش� ��尡 CH�κ��� ���� ���� �̳��̸� Ű Ʈ���� ���� 
				{
					dist =  sqrt (pow ( (double) nodes[nodeID]->x - nodes[kNode->CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[kNode->CHID]->y, 2));	


					if ( dist < TRANS_RANGE)
					{
						struct _tagUNode* uNode = tKNode->unode; // unode ȹ��
						uNode->sensorNode = nodes[nodeID];
						ndIdxInClstr++;
						kNode->numSensorNodes = ndIdxInClstr; // (2014.09.03) Ŭ�������� �Ҽ� ��� ���� kNode�� ���
					}
					else
					{
						isolatedNodes.push_back(nodeID); // CH�κ��� ���� ���� �ۿ� ��ġ�� ��带 �� ��� ��Ͽ� �߰�
					}
				}				
				break;
				/*
				struct _tagKNode* tKNode = kNode->leftChild; // Ŭ������ Ű ������ ù ��° individual key�� �̵�

				for (int j = 0; j < ndIdxInClstr; j++)
				{
					tKNode = tKNode->rightSibling; // ���� ��� ������ ������ �������� �̵�
				}

				int nodeID = gene.second; // ���� ��� ID ȹ��
				
				if (nodeID >=0) // Node* pNode = unode->sensorNode;
				{
					struct _tagUNode* uNode = tKNode->unode; // unode ȹ��
					uNode->sensorNode = nodes[nodeID];
				}
				ndIdxInClstr++;

				break;
				*/
			}
		default:

			cout << "geneType is illegal: ";
			exit(-1);

			break;
		}		
	} // for �� ���� - ����ü ��ĵ �Ϸ�
	
	//this->printKTree();
	
	this->printIsolatedNodes(); // �� ��� ���

	this->updateClstr(); // 
	//this->updateClstr(solution); // (2014.05.19) Ŭ������ ���� ����
	
	this->updateAssoc(); // (2014.05.27) ���� ��� ����
	
	
}

void Kmr_NEWLKH::cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp) // (2014.06.13) ������ �׷� ����
{
	/* 1. GA�� Ȱ���� �� ���� */

	Chromosome solution; // ������ �׷� ������ ���� �� (solution)

	int indiLen;

	double dist; // �� ��忡�� CH������ �Ÿ�

	indiLen = this->getNumKNodes(restrct_pt); // ��ü�� ���� (��ü �� ������ ����) ���

	  // ��� map ����

	map<int, Node*> nodesInGrpMap;

	set<int>::iterator it_node;	

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // ���� Ʈ���� ���� ������ id �������κ��� node ID map ����
	{
		int id;

		id = *it_node;

		nodesInGrpMap[id] = nodes[id];
	}

	/// ga ��ü�� ��� ���� ���� ( node map, Ű ������ ��ü, Ű Ʈ���� root) 

	ga->setFullSetNodes(nodes); // (2014.09.19) ��ü ��� map ����
	ga->setNodes(nodesInGrpMap); // node map ����
	ga->setKmr(this); // Ű ������ ��ü
	ga->setRoot(restrct_pt); // ���� Ű Ʈ���� root ��� ����

	ga->setIndiLen(indiLen); // ��ü ���� ����
	
	cout << "\n restruct_pt- level= " << restrct_pt->level << ", index = " << restrct_pt->index << endl;

	solution = ga->findSolution(); // GA �˰����� ���Ͽ� �� ����
	ga->testSolution();

	

	//this->updateAssoc(); // (2014.05.27) ���� ��� ���� 
	//this->printKTree();

	/* 2. ����� �ظ� �м��Ͽ� key tree�� �ش� leaf node�� ���� ��� ���� ���� */

	//this->isolatedNodes.clear(); // ���� ��� ��� ����


	  // ����� ���� ������ Ÿ�� ȹ��

	unsigned int geneIdx = 0; // ����ü�� Ž���ϱ� ���� �ε���

	struct _tagKNode* kNode = root;// ���� Ž�� ���� Ű Ʈ�� ���� k-node

	int level = 0; // ������ �׷� ���� ���� �󿡼��� level
	int index = 0; // ������ �׷� ���� ���� �󿡼��� index

	int ndIdxInClstr = 0; // ���� ������ �׷쿡 ���Ե� ����� �ε���

	  // // �ؿ� ���Ե� ������ ��ĵ -> Ű Ʈ���� leaf node�� ���� ���� ��� �ּ� ����
	
	map<int, char> solGeneType = ga->getSolGeneType(); // �ؿ� ���Ե� �������� Ÿ�� ���� ȹ�� ( 'G': group, 'N': node)

	for (geneIdx = 0 ; geneIdx < solution.size(); geneIdx++) // ����ü�� ���Ե� ������ ��ĵ -> Ű Ʈ���� leaf node�� ���� ���� ��� �ּ� ����
	{		
		Gene gene = solution[geneIdx];

		switch ( solGeneType[geneIdx])
		{

		case 'G':

			level = gene.first;// ������ �׷��� level�� index ȹ��
			index = gene.second; 

			kNode = getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			ndIdxInClstr = 0;
			kNode->numSensorNodes = ndIdxInClstr; // ������ �׷� �� ���� ��� �� ���� (reset)

			break;

		case 'N':
			{
				// ���� k-node�� Ŭ������ Ű�� �ش�
				// individual key
				// Ŭ������ �� ndIdxInClstr ��° ���Կ� ���� ��� ���� ����

				struct _tagKNode* tKNode = kNode->leftChild; // Ŭ������ Ű ������ ù ��° individual key�� �̵�

				for (int j = 0; j < ndIdxInClstr; j++)
				{
					tKNode = tKNode->rightSibling; // ���� ��� ������ ������ �������� �̵�
				}

				int nodeID = gene.second; // ���� ��� ID ȹ��
			
				
				if ( nodeID >=0) // node ID�� �����ϰ� �ش� ��尡 CH�κ��� ���� ���� �̳��̸� Ű Ʈ���� ���� 
				{
					dist =  sqrt (pow ( (double) nodes[nodeID]->x - nodes[kNode->CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[kNode->CHID]->y, 2));	


					if ( dist < TRANS_RANGE)
					{
						struct _tagUNode* uNode = tKNode->unode; // unode ȹ��
						uNode->sensorNode = nodes[nodeID];
						ndIdxInClstr++;
						kNode->numSensorNodes = ndIdxInClstr; // (2014.09.03) Ŭ�������� �Ҽ� ��� ���� kNode�� ���
					}
					else
					{
						isolatedNodes.push_back(nodeID); // CH�κ��� ���� ���� �ۿ� ��ġ�� ��带 �� ��� ��Ͽ� �߰�
					}
				}				

				break;
				
			}
		default:

			cout << "geneType is illegal: ";
			exit(-1);

			break;
		}				

	} // for �� ���� - �ؿ� ���Ե� ������ ��ĵ �Ϸ�.

	//this->printKTree();
	
	this->printIsolatedNodes(); // �� ��� ���

	//this->updateClstr(solution); // (2014.05.19) Ŭ������ ���� ����

	
}

void Kmr_NEWLKH::updateLBHG() // // (2014.06.13) ������ �׷� ���� (��� �� ����ȭ ������ ���� �ش� �׷� ����)
{
	// ��� �� ����ȭ �����鿡 ���� ������ �׷� ����
	
	map<struct _tagKNode*, set<int> >::iterator it_restrct_pts;

	this->isolatedNodes.clear(); // (2014.10.11) ���� ��� ��� ����


	for (it_restrct_pts = restruct_pts.begin(); it_restrct_pts != restruct_pts.end(); it_restrct_pts++) // ��� �籸��ȭ ������ ���� ������ �׷� ����
	{
		struct _tagKNode* kNode;
		kNode = (*it_restrct_pts).first;

		set<int> nodesInGrp;
		nodesInGrp = (*it_restrct_pts).second;

		cnstrctLBHG(kNode, nodesInGrp); // �־��� �� ����ȭ ������ ���� �׷� ���� �Լ� ȣ��
	} // for�� ���� - ��� �籸��ȭ ������ ���� ������ �׷� ���� �Ϸ�

	this->updateClstr(); // (2014.10.11) Ŭ������ ���� ����

	this->updateAssoc(); // (2014.05.27) ���� ��� ���� // ��� ���� ��� ����

	
}

void Kmr_NEWLKH::updateClstr() // (2014.10.11) �� ��带 ���� Ŭ�����Ϳ� �߰�, �� Ŭ�������� �����߽� &CH �絵��, ��� ����� Ŭ������ ����
{
	//int level; // key tree �󿡼��� level
	//int index; //	"	index	

	map<int, set<int>> clusters; // Ŭ������ ��

	unsigned int i = 0;
	int nodeID;

	/// (2014.09.02) ���� ������ ���� Ŭ�����Ϳ� �߰�

	list<int>::iterator it_node;

	set<struct _tagKNode*>::iterator it_ckNode;

	int minNumNodes = -1;
	int curNumNodes = -1;

	struct _tagKNode* clstrWithMinNodes = NULL; // ���� Ŭ������ �� ���� ��� ���� ���� Ŭ�������� kNode
	struct _tagKNode* tempKNode = NULL; // ���� �˻� ���� Ŭ�������� kNode
	struct _tagUNode* uNode = NULL; // ���� �˻� ���� UNode

	
	double distance;
	int CHID=-1;

	for (it_node = this->isolatedNodes.begin(); it_node != isolatedNodes.end(); it_node++)
	{
		nodeID = *it_node;

		if ( nodes[nodeID] == NULL) // ���ŵ� ����̸� SKIP
		{
			continue;
		}

		minNumNodes = -1;
		clstrWithMinNodes = NULL;

		for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // Ŭ�����͵��� kNode�鿡 ���� �ݺ�
		{
			tempKNode = *it_ckNode;

			curNumNodes = tempKNode->numSensorNodes;

			if ( curNumNodes == 0 || curNumNodes >= tempKNode->outDegree ) // Ŭ�������� ��� ���� 0�̰ų� �� ������ �������� ������ ���� Ŭ�����ͷ� ����
			//if ( curNumNodes == 0 || curNumNodes >= (INTERLEAVING_T+1) ) // Ŭ�������� ��� ���� 0�̰ų� (t+1) �̻��̸� ���� Ŭ�����ͷ� ����
			{
					continue;
			}

			CHID = tempKNode->CHID;

			if (CHID < 0) // �ش� Ŭ�������� CH�� �������� ������ ���� Ŭ�����ͷ� ����
			{
				continue;
			}

			distance =   sqrt (pow ( (double) nodes[nodeID]->x - nodes[CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[CHID]->y, 2));

			if (distance > TRANS_RANGE) // �� ���� ���� CH ������ �Ÿ��� ���� �������� ũ�� ���� Ŭ������ �˻�
			{
				continue;
			}

			if ( minNumNodes < 0 || curNumNodes < minNumNodes) // �ּ� ��� ���� ���ŵǴ� ��� �ּ� ��带 ���� Ŭ������ ����
			{
				minNumNodes = curNumNodes;
				clstrWithMinNodes = tempKNode;
			}

		} // ���� ����� ���� Ŭ������ �� ���� ��� ���� ���� �� ������ �ִ� Ŭ������ Ž��

		if (clstrWithMinNodes == NULL) // �ش� �� ��带 �߰��� ������ Ŭ�����͸� ã�� �� ���� ���, ���� �� ���� �̵�
		{
			continue;
		}

		tempKNode = clstrWithMinNodes->leftChild;

		int nodeIdx =0;

		for ( nodeIdx = 0; nodeIdx < clstrWithMinNodes->outDegree; nodeIdx++)
		//for ( nodeIdx = 0; nodeIdx < minNumNodes; nodeIdx++) // �ּ� ��� ���� ���� Ŭ�����Ϳ� �����ϴ� kNode�� ���� �� �������� �̵�
		{
			if (tempKNode->unode->sensorNode == NULL)
			{
				break;
			}
				tempKNode = tempKNode->rightSibling;			
		}

		if (tempKNode != NULL) // �ӽ� - ����� �� -> ���� ���� �ʿ�
		{
			tempKNode->unode->sensorNode = nodes[nodeID]; // �� ���Կ� ���� ��� �ּ� ����
			clstrWithMinNodes->numSensorNodes++;
		}
		else
		{
			cout << " tempKNode == NULL " << endl ;
		}
	} // ���� ������ ���� Ŭ������ �� ���� ��� ���� ���� �� ������ �ִ� Ŭ�����Ϳ� �߰�

	printKTree();
	// (2014.09.04) ������� Ȯ��!!


	/// (2014.09.02) �� Ŭ�������� �����߽� & CH �� ����, �� ����� Ŭ������ ����


	struct _tagKNode* ckNode = NULL;
	double avg_X = 0; //				�����߽�			x ��ǥ�� ���
	double avg_Y = 0; //				�����߽�			y ��ǥ�� ���

	set<int> nodesInGrp;
	set<int>::iterator it_clstrNode;	
	int numNodes;
	double minDist; // �Ҽ� ��忡�� ���� �߽ɱ����� �ּ� �Ÿ�


	for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // Ŭ�����͵��� kNode�鿡 ���� �ݺ�
	{
		ckNode = *it_ckNode;

		avg_X = 0;
		avg_Y = 0;
		numNodes = 0;
		minDist = -1;

		// �ش� Ŭ�����Ϳ� ���� ���� ��� ���� ȹ��
		
		nodesInGrp = this->getNdsInGrp(ckNode);		
		

		/// �� Ŭ�������� ���� �߽� ����

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // Ŭ�������� �����߽� ���� (�� ��� �߰��� ����)
		{
			nodeID = *it_clstrNode;

			avg_X += nodes[nodeID]->x;
			avg_Y += nodes[nodeID]->y;

			numNodes++;
		} // �ش� Ŭ�������� ���� �߽� ����

		avg_X /= numNodes;
		avg_Y /= numNodes;

		ckNode->COG_X = avg_X; // ���ŵ� �����߽� ������ kNode�� ����
		ckNode->COG_Y = avg_Y;

		/// �� Ŭ�������� CH ����

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // Ŭ�������� CH ���� (�� ��� �߰��� ����)
		{
			nodeID = *it_clstrNode;

			distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2)); // �Ҽ� ��忡�� ���� �߽ɱ����� �Ÿ� ���

			if (minDist < 0 || distance < minDist) // CH ����
			{
				minDist = distance;
				CHID = nodeID; 
			} // // CH ����

		} // �ش� Ŭ�������� CH ����

		ckNode->CHID = CHID; // ���ŵ� CH ������ kNode�� ����

		/// �� Ŭ�����Ϳ� ���� ������ Ŭ������ ����

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // �� ����� Ŭ������ ����
		{
			nodeID = *it_clstrNode;

			if ( nodes[nodeID] != NULL)
			{
				rtn->changeCluster(nodeID, ckNode->CHID);
			}
		} // �� ����� Ŭ������ ����

	} // �� Ŭ�������� ���� �߽� ����, ch ����
}

/*
void Kmr_NEWLKH::updateClstr (Chromosome solution) // (2014.07.03) GA�κ��� ����� �ظ� ������� Ŭ������ ���� ����
{
	/// �־��� �� ��ĵ
	map<int, char> solGeneType;
	solGeneType = ga->getSolGeneType(); // �ؿ� ���Ե� �������� ���� ( 'G': group, 'N': node)

	int level; // key tree �󿡼��� level
	int index; //	"	index

	//struct _tagKNode* clusterKey; // Ŭ������ Ű

	map<int, set<int>> clusters; // Ŭ������ ��

	unsigned int i = 0;
	int nodeID;

	/// (2014.09.02) ���� ������ ���� Ŭ�����Ϳ� �߰�

	list<int>::iterator it_node;

	set<struct _tagKNode*>::iterator it_ckNode;

	int minNumNodes = -1;
	int curNumNodes = -1;

	struct _tagKNode* clstrWithMinNodes = NULL; // ���� Ŭ������ �� ���� ��� ���� ���� Ŭ�������� kNode
	struct _tagKNode* tempKNode = NULL; // ���� �˻� ���� Ŭ�������� kNode
	struct _tagUNode* uNode = NULL; // ���� �˻� ���� UNode

	
	double distance;
	int CHID=-1;

	for (it_node = this->isolatedNodes.begin(); it_node != isolatedNodes.end(); it_node++)
	{
		nodeID = *it_node;

		minNumNodes = -1;
		clstrWithMinNodes = NULL;

		for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // Ŭ�����͵��� kNode�鿡 ���� �ݺ�
		{
			tempKNode = *it_ckNode;

			curNumNodes = tempKNode->numSensorNodes;

			if ( curNumNodes == 0 || curNumNodes >= tempKNode->outDegree ) // Ŭ�������� ��� ���� 0�̰ų� �� ������ �������� ������ ���� Ŭ�����ͷ� ����
			//if ( curNumNodes == 0 || curNumNodes >= (INTERLEAVING_T+1) ) // Ŭ�������� ��� ���� 0�̰ų� (t+1) �̻��̸� ���� Ŭ�����ͷ� ����
			{
					continue;
			}

			CHID = tempKNode->CHID;

			if (CHID < 0) // �ش� Ŭ�������� CH�� �������� ������ ���� Ŭ�����ͷ� ����
			{
				continue;
			}

			distance =   sqrt (pow ( (double) nodes[nodeID]->x - nodes[CHID]->x, 2) + pow ( (double) nodes[nodeID]->y - nodes[CHID]->y, 2));

			if (distance > TRANS_RANGE) // �� ���� ���� CH ������ �Ÿ��� ���� �������� ũ�� ���� Ŭ������ �˻�
			{
				continue;
			}

			if ( minNumNodes < 0 || curNumNodes < minNumNodes) // �ּ� ��� ���� ���ŵǴ� ��� �ּ� ��带 ���� Ŭ������ ����
			{
				minNumNodes = curNumNodes;
				clstrWithMinNodes = tempKNode;
			}

		} // ���� ����� ���� Ŭ������ �� ���� ��� ���� ���� �� ������ �ִ� Ŭ������ Ž��

		if (clstrWithMinNodes == NULL) // �ش� �� ��带 �߰��� ������ Ŭ�����͸� ã�� �� ���� ���, ���� �� ���� �̵�
		{
			continue;
		}

		tempKNode = clstrWithMinNodes->leftChild;

		int nodeIdx =0;

		for ( nodeIdx = 0; nodeIdx < clstrWithMinNodes->outDegree; nodeIdx++)
		//for ( nodeIdx = 0; nodeIdx < minNumNodes; nodeIdx++) // �ּ� ��� ���� ���� Ŭ�����Ϳ� �����ϴ� kNode�� ���� �� �������� �̵�
		{
			if (tempKNode->unode->sensorNode == NULL)
			{
				break;
			}
				tempKNode = tempKNode->rightSibling;			
		}

		if (tempKNode != NULL) // �ӽ� - ����� �� -> ���� ���� �ʿ�
		{
			tempKNode->unode->sensorNode = nodes[nodeID]; // �� ���Կ� ���� ��� �ּ� ����
			clstrWithMinNodes->numSensorNodes++;
		}
		else
		{
			cout << " tempKNode == NULL " << endl ;
		}
	} // ���� ������ ���� Ŭ������ �� ���� ��� ���� ���� �� ������ �ִ� Ŭ�����Ϳ� �߰�

	printKTree();
	// (2014.09.04) ������� Ȯ��!!


	/// (2014.09.02) �� Ŭ�������� �����߽� & CH �� ����, �� ����� Ŭ������ ����


	struct _tagKNode* ckNode = NULL;
	double avg_X = 0; //				�����߽�			x ��ǥ�� ���
	double avg_Y = 0; //				�����߽�			y ��ǥ�� ���

	set<int> nodesInGrp;
	set<int>::iterator it_clstrNode;	
	int numNodes;
	double minDist; // �Ҽ� ��忡�� ���� �߽ɱ����� �ּ� �Ÿ�


	for (it_ckNode = this->cKNodes.begin(); it_ckNode != cKNodes.end(); it_ckNode++) // Ŭ�����͵��� kNode�鿡 ���� �ݺ�
	{
		ckNode = *it_ckNode;

		avg_X = 0;
		avg_Y = 0;
		numNodes = 0;
		minDist = -1;

		// �ش� Ŭ�����Ϳ� ���� ���� ��� ���� ȹ��
		
		nodesInGrp = this->getNdsInGrp(ckNode);		
		

		/// �� Ŭ�������� ���� �߽� ����

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // Ŭ�������� �����߽� ���� (�� ��� �߰��� ����)
		{
			nodeID = *it_clstrNode;

			avg_X += nodes[nodeID]->x;
			avg_Y += nodes[nodeID]->y;

			numNodes++;
		} // �ش� Ŭ�������� ���� �߽� ����

		avg_X /= numNodes;
		avg_Y /= numNodes;

		ckNode->COG_X = avg_X; // ���ŵ� �����߽� ������ kNode�� ����
		ckNode->COG_Y = avg_Y;

		/// �� Ŭ�������� CH ����

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // Ŭ�������� CH ���� (�� ��� �߰��� ����)
		{
			nodeID = *it_clstrNode;

			distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2)); // �Ҽ� ��忡�� ���� �߽ɱ����� �Ÿ� ���

			if (minDist < 0 || distance < minDist) // CH ����
			{
				minDist = distance;
				CHID = nodeID; 
			} // // CH ����

		} // �ش� Ŭ�������� CH ����

		ckNode->CHID = CHID; // ���ŵ� CH ������ kNode�� ����

		/// �� Ŭ�����Ϳ� ���� ������ Ŭ������ ����

		for (it_clstrNode = nodesInGrp.begin(); it_clstrNode != nodesInGrp.end(); it_clstrNode++) // �� ����� Ŭ������ ����
		{
			nodeID = *it_clstrNode;

			rtn->changeCluster(nodeID, ckNode->CHID);
		} // �� ����� Ŭ������ ����

	} // �� Ŭ�������� ���� �߽� ����, ch ����	
}
*/

/*
void Kmr_NEWLKH::updateClstr (Chromosome solution, list<int> isolatedNodes)
{

}
*/
void Kmr_NEWLKH::updateAssoc( ) // (2014.05.27) ���ŵ� Ŭ������ ������ ������� ���� ��� ���� ����
{
	/* ���ŵ� Ŭ������ ���� ȹ�� */

	map<int, set<int>> clusters = rtn->getClusters(); // 

	/* ������ Ŭ�����Ϳ� ������ Ŭ������ �� �Ÿ� (ȩ ��) ��� */

	map<int, set<int>>::iterator it_clstr1;
	map<int, set<int>>::iterator it_clstr2;

	std::map<int,  std::map <int, std::set<int>> > temp_upperNodes;  // ���ο� ���� ��� ���
	std::map<int,  std::map <int, std::set<int>> > temp_lowerNodes;  // ���ο� ���� ��� ���

	int hop;

	for ( hop = 1; hop <= INTERLEAVING_T+1; hop++) // CH���� (1~ (t+1)-hop) ����/���� ���� ��� ��� ����
	//for ( hop = 1; hop <= INTERLEAVING_T; hop++) // CH���� (1~ (t+1)-hop) ����/���� ���� ��� ��� ����
	{
		for (it_clstr1 = clusters.begin(); it_clstr1 != clusters.end(); it_clstr1++) // �� CH�� h hop �Ÿ��� ����/���� ���� ��� ��� ���
		{
			set<int> clstr1 = (*it_clstr1).second;		

			if (clstr1.empty() ) // Ŭ�����Ͱ� ��� �ִ� ��� skip
			{
				continue;
			}


			int CH_ID1 = (*it_clstr1).first; // CH ���ϱ�

			if (nodes[CH_ID1] == NULL) // (2014.09.21) �ش� ��尡 �̹� ���ŵ� ����̸� ���� loop�� �̵�
			{
				continue;
			}

			if (hop == 1) // 1 hop ����/���� ���� ��� ���
			{
				for (it_clstr2 = (it_clstr1); it_clstr2 != clusters.end(); it_clstr2++)
				{
					if (it_clstr2 == it_clstr1) // Ŭ�����Ͱ� ��� �ִ� ��� skip
					{
						continue;
					}				

					set<int> clstr2 = (*it_clstr2).second;		

					if (clstr2.empty() ) // Ŭ�����Ͱ� ��� �ִ� ��� skip
					{
						continue;
					}

					int CH_ID2 = (*it_clstr2).first; // CH ���ϱ� 


					if (nodes[CH_ID2] == NULL) // (2014.09.21)  �ش� ��尡 �̹� ���ŵ� ����̸� ���� loop�� �̵�
					{
						continue;
					}

					// �� CH ���� �Ÿ� ���ϱ�

					double dist = sqrt ( pow((double) nodes[CH_ID1]->x - nodes[CH_ID2]->x, 2) + pow((double) nodes[CH_ID1]->y - nodes[CH_ID2]->y, 2) );

					if (dist < TRANS_RANGE) // �� ��� �� �Ÿ��� ���� ���� �̳��� ��� ����/���� ���� ��� ��Ͽ� �߰�
					{
						int upperNodeID;
						int lowerNodeID; 

						if (nodes[CH_ID1]->distance < nodes[CH_ID2]->distance)
						{
							upperNodeID = CH_ID1;
							lowerNodeID = CH_ID2;
						}
						else
						{
							upperNodeID = CH_ID2;
							lowerNodeID = CH_ID1;
						}

						// ������ ��� ���� �߰�
						temp_upperNodes[lowerNodeID][1].insert(upperNodeID);
						temp_lowerNodes[upperNodeID][1].insert(lowerNodeID);

						///** ���� CH�� H ȩ ���� CH�� ���� �ӽ� ���� ��� ��Ͽ� Ŭ������ �Ҽ� �� CH ���� �߰�

						set<int> cluster;
						set<int>::iterator it_memberNode;
						int memberNodeID;

						cluster = rtn->getClstr(lowerNodeID); // Ŭ������ �Ҽ� ��� ��� ȹ��

						int node_idx;
						node_idx = 1;

						for( it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // Ŭ������ �Ҽ� ��� ��� ��ȸ
						{
							memberNodeID = *it_memberNode; // Ŭ������ �Ҽ� ID;

							if (memberNodeID == lowerNodeID) // �Ҽ� ��� ID�� ch�� ID�� ������ ���, ���� �Ҽ� ���� ����
							{
								continue;
							}

							if ( node_idx == hop) // CH�� h-hop ���� ��带 h ��° �Ҽ� ����� ���� ���� ���� ����
							{
									
								temp_lowerNodes[upperNodeID][INTERLEAVING_T+1].insert(memberNodeID);					

								// CH �� h-hop ���� ��� ��� ��ȸ - h-hop ���� ����� ���� ���� ��� ��� ���� �Ϸ�
								break;
							}

							node_idx++;
						} // CH�� h-��° ���� ��带 h��° �Ҽ� ����� ���� ���� ���� ����

					///** ���� CH�� H ȩ ���� CH�� ���� �ӽ� ���� ��� ��Ͽ� Ŭ������ �Ҽ� �� CH ���� �߰� (�Ϸ�)

					} // 1 hop ��/���� ���� ��� �߰�		

					


				} // for �� ���� - ���� CH�� 1 hop ����/���� CH ��� ��� �Ϸ�
			} // hop ==1�� ��� ó�� �Ϸ�

			else
			{
				std::set<int>::iterator it_upperNode; // ���� ��� �ݺ���

				std::set<int>::iterator it_upperupperNode; // ���� ����� ���� ��� �ݺ���

				int CH_ID2;

				for (it_upperNode = temp_upperNodes[CH_ID1][hop-1].begin(); it_upperNode != temp_upperNodes[CH_ID1][hop-1].end(); it_upperNode++) // ���� ����� h-1 ȩ ���� ��� ��� ��ȸ
				{
					CH_ID2 = *it_upperNode;

					for (it_upperupperNode = temp_upperNodes[CH_ID2][1].begin(); it_upperupperNode != temp_upperNodes[CH_ID2][1].end(); it_upperupperNode++) // (h-1) ȩ ���� ����� 1-hop ���� ��� ��ȸ
					{
						int upperNodeID;

						upperNodeID = *it_upperupperNode;

						temp_upperNodes[CH_ID1][hop].insert(upperNodeID); // (h-1) ȩ ���� ����� 1-hop ���� ��� ID�� h-hop ���� ��� ��Ͽ� �߰�


						///** ���� CH�� H ȩ ���� CH�� ���� �ӽ� ���� ��� ��Ͽ� Ŭ������ �Ҽ� �� CH ���� �߰�
						
						set<int> cluster;
						set<int>::iterator it_memberNode;
						int memberNodeID;

						cluster = rtn->getClstr(CH_ID1); // Ŭ������ �Ҽ� ��� ��� ȹ��

						int node_idx;
						node_idx = 1;

						for( it_memberNode = cluster.begin(); it_memberNode != cluster.end(); it_memberNode++) // Ŭ������ �Ҽ� ��� ��� ��ȸ
						{
							memberNodeID = *it_memberNode; // Ŭ������ �Ҽ� ID;

							if (memberNodeID == CH_ID1) // �Ҽ� ��� ID�� ch�� ID�� ������ ���, ���� �Ҽ� ���� ����
							{
								continue;
							}

							if ( node_idx == hop) // CH�� h-hop ���� ��带 h ��° �Ҽ� ����� ���� ���� ���� ����
							{
									
								temp_lowerNodes[upperNodeID][INTERLEAVING_T+1].insert(memberNodeID);					

								// CH �� h-hop ���� ��� ��� ��ȸ - h-hop ���� ����� ���� ���� ��� ��� ���� �Ϸ�
								break;
							}

							node_idx++;
						} // CH�� h-��° ���� ��带 h��° �Ҽ� ����� ���� ���� ���� ����
						
						///** ���� CH�� H ȩ ���� CH�� ���� �ӽ� ���� ��� ��Ͽ� Ŭ������ �Ҽ� �� CH ���� �߰� (�Ϸ�)


					} // (h-1) ȩ ���� ����� 1-hop ���� ��� �ݺ� �Ϸ�


				} // for�� ���� -  ���� ����� h ȩ ���� ��� ��� ���� �Ϸ�

				


				std::set<int>::iterator it_lowerNode;// ���� ��� �ݺ���

				std::set<int>::iterator it_lowerlowerNode; // ���� ����� ���� ��� �ݺ���

				for (it_lowerNode = temp_lowerNodes[CH_ID1][hop-1].begin(); it_lowerNode != temp_lowerNodes[CH_ID1][hop-1].end(); it_lowerNode++) // ���� ����� h-1 ȩ ���� ��� ��� ��ȸ
				{
					CH_ID2 = *it_lowerNode;

					for (it_lowerlowerNode = temp_lowerNodes[CH_ID2][1].begin(); it_lowerlowerNode != temp_lowerNodes[CH_ID2][1].end(); it_lowerlowerNode++) // (h-1) ȩ ���� ����� 1-hop ���� ��� ��ȸ
					{
						int lowerNodeID;

						lowerNodeID = *it_lowerlowerNode;

						


						temp_lowerNodes[CH_ID1][hop].insert(lowerNodeID); // (h-1) ȩ ���� ����� 1-hop ���� ��� ID�� h-hop ���� ��� ��Ͽ� �߰�
					} // (h-1) ȩ ���� ����� 1-hop ���� ��� �ݺ� �Ϸ�
				} // for�� ���� -  ���� ����� h ȩ ���� ��� ��� ���� �Ϸ�
			} // else�� ���� - hop = (2~ (T+1))�� ��� ó�� �Ϸ�

			// (2014.06.23) 15:49 ������� Ȯ��! 

			//sec->updateAssocNodes(CH_ID1, 0, hop, temp_upperNodes[CH_ID1][hop]); // ���� ��� ����
			//sec->updateAssocNodes(CH_ID1, 1, hop, temp_lowerNodes[CH_ID1][hop]);
		}
	} // for �� ���� - CH���� (1~ (t+1)-hop) �ӽ� ����/���� ��� ��� ���� �Ϸ�

	
	/// �� CH�� ���� ���� ��� ����

	set<struct _tagKNode*>::iterator it_cKNode;


	for (it_cKNode = cKNodes.begin(); it_cKNode != cKNodes.end(); it_cKNode++) // Ű Ʈ���� ����� (Ŭ������ Ű) ��� ��ȸ
	{
		int CH_ID;

		CH_ID = (*it_cKNode)->CHID; //  CH ID ȹ��
		
		for ( hop = 1; hop <= INTERLEAVING_T+1; hop++) // 1~(t+1) ȩ ���� ���� ��� ����
		{
			sec->updateAssocNodes(CH_ID, 0, hop, temp_upperNodes[CH_ID][hop]);

		} // for�� ���� - ���� CH�� ���� ���� ��� ��� ���� �Ϸ�

	} // for �� ���� - �� CH�� ���� ���� ��� ��� ���� �Ϸ�

	/// �� CH�� ���� ���� ��� ����
	// (2014.06.26) 20:11 ������� Ȯ��
	for (it_cKNode = cKNodes.begin(); it_cKNode != cKNodes.end(); it_cKNode++)
	{
		int CH_ID;

		CH_ID = (*it_cKNode)->CHID; //  CH ID ȹ��
		
		if (CH_ID == 13)
		{
			cout << " (lowerNodeID == 13) " ;
		}

		for ( hop = 1; hop <= INTERLEAVING_T+1; hop++) // 1~(t+1) ȩ ���� ���� ��� ����
		{
			sec->updateAssocNodes(CH_ID, 1, hop, temp_lowerNodes[CH_ID][hop]);

		} // for�� ���� - ���� CH�� ���� ���� ��� ��� ���� �Ϸ�
	} // for �� ���� - �� CH�� ���� ���� ��� ��� ���� �Ϸ�
	
}



void Kmr_NEWLKH::expandLevel(int level, int degree) // (2014.04.24) key tree�� ���� level Ȯ��
{
	// ���� �� 

	list<struct _tagKNode*>::iterator it;

	list<struct _tagKNode*> next_level_knodes; // ���� level ��� ���

	int index = 0; // ���� level�� ���� kNOde���� index

	for ( it = cur_level_knodes.begin(); it != cur_level_knodes.end(); it=cur_level_knodes.erase(it) ) // ���� level ���鿡 ���� �ݺ�
	{
		struct _tagKNode* parent = *it;		

		KNode* prev = NULL;
		KNode* current = NULL;			

		parent->outDegree = degree; // (2014.09.04) 

		for (int i = 0; i < degree; i++)
		{
			KNode* kNode = new KNode; // k-node ����
			this->initKNode (kNode);
			kNode->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

			next_level_knodes.push_back(kNode);

			//UNode* uNode = new UNode; // u-node ����	  

			current = kNode; // ���� k-node

			current->level = level; // kNode�� level ����
			current->index = index++; // kNode�� �ε��� ����

			//kNode->unode = uNode; // k-node, u-node �� ������ ����
			//uNode->parent = kNode;

			if (prev != NULL)// left/right sibling ����
			{
			  prev->rightSibling = current;
			  current->leftSibling = prev;
			}

			kNode->parent = parent; // k-node, root �� ������ ����

		    if (i ==0)
			{
			  parent->leftChild = current; // left child ����
			}

			prev = current; // ���� k-node ����
		}
	}

	this->cur_level_knodes.clear(); // ������ ���� level ��� ��� ����
	this->cur_level_knodes = next_level_knodes; // ���� level ��� ����� ���� level�� ����
}

void Kmr_NEWLKH::expandLevel(int degree) // (2014.04.18) key tree�� ���� level Ȯ�� (���� level�� �� ��帶�� degree ��ŭ�� �ڽ� ��� ����)
{
	// ���� �� 

	list<struct _tagKNode*>::iterator it;

	list<struct _tagKNode*> next_level_knodes; // ���� level ��� ���

	int index = 0; // ���� level�� ���� kNOde���� index

	for ( it = cur_level_knodes.begin(); it != cur_level_knodes.end(); it=cur_level_knodes.erase(it) ) // ���� level ���鿡 ���� �ݺ�
	{
		struct _tagKNode* parent = *it;		

		KNode* prev = NULL;
		KNode* current = NULL;			

		parent->outDegree = degree; 

		for (int i = 0; i < degree; i++)
		{
			KNode* kNode = new KNode; // k-node ����			

			this->initKNode (kNode);

			kNode->groupKey->newVal =gen1.genRandInt(Max_Key_Value);

			next_level_knodes.push_back(kNode);

			//UNode* uNode = new UNode; // u-node ����	  

			current = kNode; // ���� k-node

			current->index = index++; // kNode�� �ε��� ����

			//kNode->unode = uNode; // k-node, u-node �� ������ ����
			//uNode->parent = kNode;

			if (prev != NULL)// left/right sibling ����
			{
			  prev->rightSibling = current;
			  current->leftSibling = prev;
			}

			kNode->parent = parent; // k-node, root �� ������ ����

		    if (i ==0)
			{
			  parent->leftChild = current; // left child ����
			}

			prev = current; // ���� k-node ����
		}
	}

	this->cur_level_knodes.clear(); // ������ ���� level ��� ��� ����
	this->cur_level_knodes = next_level_knodes; // ���� level ��� ����� ���� level�� ����

}

void Kmr_NEWLKH::printIsolatedNodes() // (2014.09.02) ���� ��� ��� ���
{
	ofstream out("tst_isolatedNodes.txt");

	out << "isolated nodes: " << endl << endl;

	std::list<int>::iterator it;

	for (it = this->isolatedNodes.begin(); it != this->isolatedNodes.end(); it++)
	{
		int nodeID = *it;
		out << nodeID << ", ";
	}

	out.close();
}

Kmr_NEWLKH::Kmr_NEWLKH()
{

}

Kmr_NEWLKH::~Kmr_NEWLKH()
{

}