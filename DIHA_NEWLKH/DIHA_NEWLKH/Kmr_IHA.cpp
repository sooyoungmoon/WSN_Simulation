
// 2014.07.31
// ������
// Kmr_IHA.cpp
// IHA�� Ű �Ŵ��� Ŭ���� ����


#include "Kmr_IHA.h"
#include "Sec_IHA.h"
#include <fstream>

using namespace std;

//int Node::numNodesEvctd;

void Kmr_IHA::updateKey(struct _tagKNode* kNode) // �׷� Ű ����
{
	GroupKey* gk;

	gk = root->groupKey;

	gk->oldVal = gk->newVal;
	gk->newVal = gen1.genRandInt(Max_Key_Value); // �׷� Ű ����
	
	//... root Ű ����
}
/*
void Kmr_IHA::resetUpdatedKeys(struct _tagKNode* kNode) // (2014.07.31) key tree�� �� k-node ���� ���� �� �ʱ�ȭ 
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
void Kmr_IHA::genKeyMsg() // Ű ��й� �޽��� ����
{
	/* 1. key tree ���� root ��忡 ����� �׷� Ű ���� */
	//GroupKey oldKey;
	//GroupKey newKey;

	if (root == NULL)
	{
		cout << "root == NULL" << endl;
		exit(-1);
	}

	GroupKey* gk = root->groupKey;
	
	if (gk == NULL)
	{
		cout << "gk == NULL" << endl;
		exit(-1);
	}

	gk->oldVal = gk->newVal;
	gk->newVal = gen1.genRandInt(Max_Key_Value); // �׷� Ű ����
	

	/* 2. �� ��帶�� �ش� ����� ���� Ű (individual key)�� ���ο� �׷� Ű�� ��ȣȭ�Ͽ� Ű ���� �޽��� ���� */

	KNode* knode = root->leftChild;

	if (knode == NULL)
	{
		cout << "kNode == NULL" << endl;
		exit(-1);
	}

	int cnt = 0;

	while (knode != NULL) // key tree ���� ��� individual key ��ȸ, ������ individual key�� ���ο� �׷� Ű ��ȣȭ
	{
		UNode* unode = knode->unode;

		Node* pnode = unode->sensorNode;

		int nodeID = pnode->id;

		KeyMsg* keyMsg = new KeyMsg;	

		this->initKeyMsg(keyMsg);

		keyMsg->newKeys.push_back ( *gk);

		keyMsg->recipents.insert(nodeID);

		this->keyMsgs[cnt] = keyMsg;

		knode = knode->rightSibling;

		cnt++;
	}

	/*
	for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
	{
		KeyMsg* keyMsg = new KeyMsg;

	}
	*/
}

void Kmr_IHA::testKeyMsg() // Ű ��й� �޽��� �׽�Ʈ
{
	ofstream out("tst_keyMsg_IHA.txt");

	out << " rekey message " << endl;

	for (int m = 0; m < keyMsgs.size(); m++)
	{
		KeyMsg* keyMsg = keyMsgs[m];

		out << "\nmsg. " << m << " : " << endl;

		list<GroupKey>::iterator it;

		for (it = keyMsg->newKeys.begin(); it != keyMsg->newKeys.end(); it ++)
		{
			int oldVal = (*it).oldVal;
			int newVal = (*it).newVal;

			out << " old key: " << oldVal <<" -> " ;
			out << " new key: " << newVal;
			out << endl;
		}

		out << endl;		
	}

	out.close();
}

int Kmr_IHA::getKeyMsgLen(struct _tagKeyMsg* keyMsg) // Ű �޽��� ���� ��ȯ
{
	int len = 0;

	len += 2 +2; // sender / receiver ID size;

	// Ű ���� ���
	
	len += keyMsg->newKeys.size() * Group_Key_Size;

	// ���� ��� ID ũ��

	len += keyMsg->recipents.size() * ID_LEN;	
	
	return len;
}

bool Kmr_IHA::handleKeyMsg(struct _tagKeyMsg* keyMsg, Node* cNode)
{	
	bool res = true; // ���� ��� ���� (true: ��� ����, false: ���� �Ϸ�)
	bool isRecipent = false; // ���� ���� ��尡 Ű ��й� �޽����� ���� ��� ��Ͽ� �ִ��� ����
	// ���� ����� ID�� ���� ��� ��Ͽ� �ִ� �� �˻�
	

	IDSet::iterator it;

	for ( it = keyMsg->recipents.begin(); it != keyMsg->recipents.end(); it++)
	{
		int recipentID = (*it);

		if ( cNode->id == recipentID)
		{
			isRecipent = true;
		}
	}

	if (isRecipent == true)
	{
		keyMsg->recipents.erase(cNode->id); // ���� ��尡 ���� ��� ��Ͽ� ���ԵǾ� ������ �ش� ��Ͽ��� ���� ��� ID ����

		this->receivedNodes.insert(cNode->id); // (2014.08.07) Ű ��й� �޽��� ���� ��� ���� ���� (debugging ��)

		if ( keyMsg->recipents.empty())
		{
			res = false;
		}
	} // IF�� ���� - ���� ��尡 ���� ����� ����� ó�� �Ϸ�

	//NodeIDLst recipents = keyMsg->recipents;
	/*
	for ( it = keyMsg->recipents.begin(); it != keyMsg->recipents.end(); it++)
	{
		int recipentID = (*it);

		if ( cNode->id == recipentID)
		{
			keyMsg->recipents.erase(it);
			//recipents.erase(it);

			if ( keyMsg->recipents.empty())
			{
				res = false;
				break;
			}
		}
	}
	
	if (res == false)
	{
		return res;
	}
	*/

	// �׷� Ű ���� (Ű Ʈ���� �ִ� �׷� Ű�� ������ ����Ǿ��ٰ� ����)


	/// Ŭ������ ���� ���� (rtn)
	/*
	//NodeIDLst::iterator it_node;
	IDSet::iterator it_node;

	int cNodeID = cNode->id;
	int clusterID = keyMsg->CHID;

	rtn->changeCluster(cNodeID, clusterID); 	

	std::set<int> upperAssocnodeset;

	for (it_node = keyMsg->upAssocNds.begin(); it_node != keyMsg->upAssocNds.end(); it_node++)
	{
		int nodeID = (*it_node);

		upperAssocnodeset.insert(nodeID);
	}

	sec->updateAssocNodes(cNodeID, 0, INTERLEAVING_T+1, upperAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)
	//sec->updateAssocNodes(cNodeID, 0, upperAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)

	std::set<int> lowerAssocnodeset;

	for (it_node = keyMsg->downAssocNds.begin(); it_node != keyMsg->upAssocNds.end(); it_node++)
	{
		int nodeID = (*it_node);

		lowerAssocnodeset.insert(nodeID);
	}

	sec->updateAssocNodes(cNodeID, 1, INTERLEAVING_T+1, lowerAssocnodeset); // ���� ���� ��� ���� ���� (iha, diha)
	*/
	return res;
}
/*
void Kmr_IHA::genKeyMsg()
{

}
*/
/*
KeyMsg* Kmr_IHA::getKeyMsg() // Ű ��й� �޽��� ��ȯ
{

}
*/
/*
void Kmr_IHA::addPairKey (int nID, int nID2)
{

}
*/
int Kmr_IHA::cnstrctKTree() // key tree  �ʱ� ����
{
	int height = 2; // key tree�� ����	

	this->root = new KNode;

	// root ��� �ʱ�ȭ
	this->initKNode (root);
	root->isCluster = true; // �׷� ���� ������ iD ����� ���ϱ� ���� ��ü ��Ʈ��ũ�� �ϳ��� Ŭ�����ͷ� ������ -  getNdsInGrp() �Լ�

	//root->groupKey->newVal = gen1.genRandInt(Max_Key_Value);

	numNodesAtLevel[0] = 1; // level 0�� ��� �� ����

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
	// ���� ���� ��� �� ȹ��
	int numCurNodes = 0;
	numCurNodes = this->nodes.size();

	numNodesAtLevel[1] = numCurNodes; // level 1�� ��� �� ����
	numNodesAtLevel[2] = numCurNodes; // level 2�� ��� �� ����

	KNode* prev = NULL;
	KNode* current = NULL;

	int kNodeIdx = 0;

	map< int, Node*>::iterator it_node;
	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int i = Node::numNodesEvctd; i < Node::totNumNodes; i++) // ���Ӱ� �߰��Ǵ� ���鿡 ���� �ݺ�
	//for (int i = 0; i< numCurNodes; i++)
	{
		KNode* kNode = new KNode; // k-node ����
		this->initKNode(kNode);
		kNode->parent = root;
		kNode->level = 1;
		kNode->index = kNodeIdx;
		
		//kNode->index = i;
	/*
		kNode->groupKey = new GroupKey;
		kNode->groupKey->newVal = gen1.genRandInt(Max_Key_Value); // (2014.05.08) ������ �����Ͽ� �׷� Ű ������ ����
		kNode->parent = root;
		kNode->leftChild = NULL;
		kNode->rightSibling = NULL;
		kNode->unode = NULL;
		kNode->level = 1;
		kNode->index = i;
		*/
		UNode* uNode = new UNode; // u-node ����	  
		
		uNode->sensorNode = (*it_node).second;
		//uNode->sensorNode = nodes[i] ;
		uNode->isCH = false;
		
		  current = kNode; // ���� k-node

		  kNode->unode = uNode; // k-node, u-node �� ������ ����
		  uNode->parent = kNode;

		  if (prev != NULL)// left/right sibling ����
		  {
			  prev->rightSibling = current;
			  current->leftSibling = prev;
		  }

		  kNode->parent = root; // k-node, root �� ������ ����

		  if (kNodeIdx ==0)
		  {
			  root->leftChild = kNode; // left child ����
		  }

		  kNodeIdx++;

		  prev = kNode; // ���� k-node ����
	}  	
	// k-node, u-node ��ü ����

	//root->updated = true; // (2014.07.31) update �÷��׸� ������ ����
	//root->updated_addition = true;

	this->kTreeHeight = height;

	return height;
}

int Kmr_IHA::cnstrctKTree(struct _tagKNode* root, map<int, Node*> nodes) // (2014.06.05)
{
	int height;

	// ...

	return height;
}

int Kmr_IHA::cnstrctKTree(struct _tagKNode* root, set<int> nodes)
{
	int height;

	// ...

	return height;
}

int Kmr_IHA::updateKTree(IDSet addedNodes, IDSet deletedNodes) // (2014.05.29)
{
	// root ��� ���� �� �ʱ�ȭ -> Kmr ��ü�� �����ڿ��� ó��

	int height = 2;

	numNodesAtLevel[0] = 1; // level 0�� ��� �� ����
	
	this->kTreeHeight = height;	

	/* Ű Ʈ�� �󿡼� ��� ���� */
	
	struct _tagKNode* cKNode; // ���� Ž������ knode
	struct _tagKNode* oKNode; // �� knode
	cKNode = root->leftChild;

	 // root ����� leftChild�������� �ϳ��� �˻�, ���� ��� ��Ͽ� ������ �ش� k-node�� u-node ����, root ����� update_eviction �÷��� = true�� ����

	while ( cKNode != NULL) // ��� individual key�� ���� �ݺ�
	{
		// ...

		struct _tagUNode* uNode = NULL;
		uNode = cKNode->unode; // u-Node ȹ��
		Node* node = uNode->sensorNode;

		if (node ==NULL)
		{
			cKNode= cKNode->rightSibling; // ���� ���� �̵�
			continue;
		}

		IDSet::iterator it_find;
		it_find = deletedNodes.find( node->id);

		/* ���� ��� ��Ͽ� ������ k-node�� u-node ���� */
		if (it_find != deletedNodes.end() )
		{
			oKNode = cKNode;

			struct _tagKNode* tKNode; // eviction-point���� root������ ��ο� ��ġ�ϴ� k-node�� update_eviction �÷��� ����

			tKNode = oKNode->parent; // kNode �����͸� eviction-point�� �ʱ�ȭ

			while (tKNode != NULL) // root �������� ��ο� �����ϴ� kNode�鿡 ���� �ݺ�
			{
				tKNode->updated = true;
				tKNode->updated_eviction = true; // update_eviction �÷��� ����
				tKNode = tKNode->parent;
			}

			delete (uNode); // u-node �޸� ��ȯ

			if (cKNode->leftSibling == NULL)
			{
				cKNode->parent->leftChild = cKNode->rightSibling; // �θ� ����� left child ����
			}

			if (cKNode->leftSibling != NULL)
			{
				cKNode->leftSibling->rightSibling = cKNode->rightSibling; // left sibling -> right sibling ���� ����
			}

			if (cKNode->rightSibling != NULL)
			{
				cKNode->rightSibling->leftSibling = cKNode->leftSibling; // left sibling <- right sibling ���� ����
			}

			cKNode = cKNode->rightSibling;

			delete (oKNode); // k-node ��ȯ

			continue;
		} // k-node�� u-node ����

		cKNode= cKNode->rightSibling; // ���� ���� �̵�
	
	} // ��� ���� �Ϸ�

	numNodesAtLevel[1] -= deletedNodes.size(); // level 1�� ��� �� ����
	numNodesAtLevel[2] -= deletedNodes.size(); // level 2�� ��� �� ����

	/* Ű Ʈ�� �󿡼� ��� �߰� */

	IDSet::iterator it_add;

	for (it_add = addedNodes.begin(); it_add != addedNodes.end(); it_add++) // �߰��� ���鿡 ���� �ݺ�
	{
		int nodeID;

		nodeID = *it_add; // ��� ID ȹ��

		struct _tagKNode* kNode = new struct _tagKNode; // k-node ����
		this->initKNode(kNode); // k-node �ʱ�ȭ


		struct _tagUNode* uNode = new struct _tagUNode; // u-node ����
		this->initUNode(uNode);

		kNode->unode = uNode;
		kNode->parent = root;

		uNode->parent = kNode;
		uNode->sensorNode = nodes[nodeID];

		struct _tagKNode* addPoint;

		addPoint = root; // addition_point ����		
		kNode->level = addPoint->level+1;

		/* ������ ���� */

		if (addPoint->leftChild != NULL)
		{
			kNode->rightSibling = addPoint->leftChild;	// left - right sibling ��� �� ���� ����
			kNode->rightSibling->leftSibling = kNode; 
		}

		addPoint->leftChild = kNode; // addition point�� left child ���� ����

		struct _tagKNode* tKNode; // addition-point���� root������ ��ο� ��ġ�ϴ� k-node�� update_addition �÷��� ����

		tKNode = addPoint;

		while (tKNode != NULL) // root �������� ��ο� �����ϴ� kNode�鿡 ���� �ݺ�
		{
			tKNode->updated = true;
			tKNode->updated_addition = true; // updated_addition �÷��� ����
			tKNode = tKNode->parent; // ���� k-node�� �̵�
		}
	}

	numNodesAtLevel[1] += addedNodes.size(); // level 1�� ��� �� ����
	numNodesAtLevel[2] += addedNodes.size(); // level 2�� ��� �� ����


	/* �� ����ȭ (restructuring) point ���� */	

	this->restruct_pts.clear(); // �� ����ȭ point ��� ����

	set<int> nodesInGrp;

	nodesInGrp = this->getNdsInGrp(root);

	restruct_pts[root] = nodesInGrp;

	//restruct_pts.insert(root); // �� ����ȭ point�� root ���
	/*
	struct _tagKNode* restruct_pt = NULL;
	struct _tagKNode* clstrKey = NULL; // Ŭ������ Ű
	struct _tagKNode* groupKey = NULL; // �׷� Ű

	// (h-2) level�� ������ knode�鿡 ���� �ݺ� (Ŭ�����Ϳ� �����Ǵ� kNode)

	int clusterLevel = height-2; // cluster level ���

	int numCluster;

	numCluster = this->getNumNodesAtLevel(clusterLevel); // Ŭ������ ���� ��ȯ

	for (int cl = 0; cl < numCluster; cl++) // ������ Ŭ�����Ϳ� ���� �ݺ�
	{
		clstrKey = getKNode(clusterLevel, cl); // Ŭ������ Ű ȹ��

		groupKey = clstrKey;

		int numNodesinGrp; // �׷쿡 ���� ��� ��
		int numClstrsinGrp; // �׷쿡 ���� Ŭ������ ��
		numClstrsinGrp = 1; 

		bool reconstruct_needed = false; // �籸��ȭ �ʿ伺 ����


		while ( groupKey != root) // ���� ��� ���� �����ϴ� ������ �׷��� Ž�� - groupKey�� root�� �����ϸ� �ݺ��� Ż��
		{
			bool condition = false; // ���� ��� �� ���� ���� (T: ����, F: �Ҹ���)
			int numNodesInGrp = this->getNumNdsInGrp(groupKey); // �ش� �׷쿡 ���ϴ� ��� �� ȹ��
			int numClstrsInGrp = this->getNumClstrsInGrp(groupKey); // �ش� �׷쿡 ���ϴ� Ŭ������ �� ȹ��

			int nodesPerCluster;
			nodesPerCluster = (numNodesInGrp / numClstrsInGrp); // Ŭ������ �� ��� ��� �� ���

			if ( (nodesPerCluster >= INTERLEAVING_T+1) && (nodesPerCluster <= 2* (INTERLEAVING_T+1) ) )
			{				
				restruct_pt = groupKey;
				condition = true;
				break; // ���� ��� ��� ���� �����Ǹ� loop Ż��
			}
			else
			{
				reconstruct_needed = true;
			}
		}

		if (reconstruct_needed == true)
		{
			restruct_pts.push_back(restruct_pt);
		}

		// �ش� Ŭ�����Ͱ� ���� ��� ���� �����ϸ� �籸��ȭ ����Ʈ ���� ���ʿ�
		// �ش� Ŭ�����Ͱ� ���� ��� ���� �������� ���� ��� �籸��ȭ ����Ʈ ����

	}
	*/
	/* ������ �� ����ȭ point�� ���� Ű Ʈ�� �κ��� ���� */ 

	/* level, index �缳�� */

	int cnt_kNode;
	cnt_kNode = 0;

	cKNode = root->leftChild;

	while (cKNode != NULL)
	{
		cKNode->index = cnt_kNode++;
		cKNode = cKNode->rightSibling;
	}



	return kTreeHeight;
}

void Kmr_IHA::dstrctKTree() // Ű Ʈ�� ����
{
	KNode* current = root->leftChild;
	KNode* prev = current;

	while(current !=NULL)
	{		
		current = current->rightSibling;
		
		delete (prev->groupKey);
		delete (prev->unode);
		delete prev;

		prev = current;
	}

	delete root;

}

void Kmr_IHA::cnstrctLBHG() // (2014.04.25) ������ �׷� ���� - LEAF ��忡 ���� ���� ��� ���� ����
{
	// ���� �� 
	// key tree�� leaf node�� ���� ���� ��� ���� ����

	map<int, Node*>::iterator it_node;
	it_node = nodes.begin();

	//int i = Node::numNodesEvctd;

	KNode* current = NULL;

	current = root->leftChild; // ���� Ž�� ���� k-node (u-node�� �θ� ���)

	while (current != NULL) // key tree�� leaf node (u-node) ���� �׿� �����Ǵ� ���� ��� ���� ����
	{
		UNode* unode = current->unode;

		if (unode == NULL)
		{
			cout << " unode is NULL " ; // unode is NULL, ���� ��� �� ����
			exit(-1);
		}

		unode->sensorNode = (*it_node).second; // nodes[i]; // ���� ��� ���� ����
		it_node++;
		
		// ���� ��� ���� ��� (�׽�Ʈ��)
		/* 
		cout << "unode " << i << "- sensor node info. " << endl;

		// ID ���
		int nodeID = unode->sensorNode->id;
		cout << "\t node ID " << nodeID << endl;

		// ��ǥ ���
		cout << "\t (x, y) = ( " << unode->sensorNode->x << ", " << unode->sensorNode->y << " )" << endl;

		// Ŭ������ ���� ��� (CH ���� ����)
		cout << "\t cluster ID (CH's ID) = " << rtn->getClstrID(nodeID) << endl;


		// ���� ��� ���� ���
		try 
		{
			Sec_IHA* sec_IHA = dynamic_cast<Sec_IHA*>(this->sec);

			//std::map <int, std::set<int> > upperNodes = sec_IHA->upperNodes;
			//std::map <int, std::set<int> > lowerNodes = sec_IHA->lowerNodes;

			std::set<int> upperNodes = sec_IHA->upperNodes[nodeID][INTERLEAVING_T+1]; // �ش� ����� (T+1) ȩ ���� ��� ���
			std::set<int> lowerNodes = sec_IHA->lowerNodes[nodeID][INTERLEAVING_T+1]; // �ش� ����� (T+1) ȩ ���� ��� ���

			std::set<int>::iterator it;

			cout << "\t uppper assoc nodes: { " ;

			for (it = upperNodes.begin(); it != upperNodes.end(); it++)
			{
				int nodeID = *it;
				cout << nodeID << ", ";
			}

			cout << " } " << endl;

			cout << "\t lower nodes: { " ;

			for (it = lowerNodes.begin(); it != lowerNodes.end(); it++)
			{
				int nodeID = *it;
				cout << nodeID << ", ";
			}

			cout << " } " << endl;

		} catch (exception& e) {cout << "Exception: " << e.what();}
		
		// �θ� ��� ���
		Node* parent = rtn->getSPParentNode(unode->sensorNode);
		
		if (parent != NULL)
		{
			cout << " parent: " << parent->id << endl;
		}
		//*/

		current = current->rightSibling; // ���� k-node�� �̵�
		//i++; // ��� �ε��� +1 ����
	}	

}

void Kmr_IHA::cnstrctLBHG(struct _tagKNode* restrct_pt, set<int> nodesInGrp) // (2014.06.13)
{
	// ���� Ű ���� ����� ��� Ű Ʈ���� ���ŵǴ� �������� leaf ���� ���� ���� ��� ���� ���� ���谡 �����ǹǷ� 
	// ���� ������ �׷� ������ �ʿ����� ����. ���� Ŭ�������� ���� �Լ��� ����Ǿ����Ƿ� ���� �θ� ����
}

void Kmr_IHA::updateLBHG() // // (2014.06.13) ������ �׷� ���� (��� �� ����ȭ ������ ���� �ش� �׷� ����)
{
	// ���� ������ ������ ó�� ���� ����
}

Kmr_IHA::Kmr_IHA() 
{


}

Kmr_IHA::~Kmr_IHA()
{
	
}

/*
void Kmr_IHA::addPairKey (int nID, int nID2)// (2013.08.29) �� ����� iD�� �Է����� pairwise key�� �߰�
{
	// pairwise key ��ü ����

	PairKey* pk = NULL;
	PairKey* pk2 = NULL;

	pk = new PairKey;
	pk2 = new PairKey;

	if (nID <= nID2) // �� ����� iD �� ���� ���� nID��, ū ���� nID2�� ����
	{
		pk->nID = nID;
		pk->nID2 = nID2;

		pk2->nID = nID;
		pk2->nID2 = nID2;
	}

	else
	{
		pk->nID = nID2;
		pk->nID2 = nID;

		pk2->nID = nID2;
		pk2->nID2 = nID;
	}

	// ���� ������ Ű ��ü�� �� ����
	pk->kVal = nID + nID2; // �� ����� ID�� ���� ������ ����
	pk2->kVal = nID + nID2; // �� ����� ID�� ���� ������ ����

	// �� ����� pairwise key ��Ͽ� ���� �߰�
	pKeys[nID].push_back(pk);
	pKeys[nID2].push_back(pk2);
}

void Kmr_IHA::setPairKeys() // (2013.08.28 �߰�) pair key ���� �Լ�
{
	// �̹� ������ ���� ���谡 �����Ǿ� �ִٰ� ����

	// ��� ���鿡 ����, ���� ��� ��� �˻�

	for (int i = 0; i < NUM_NODES; i++)
	{
		// ���� ���� ���� ���� ����� �����ϴ� pair key ����



		// ������ pair key�� �ش� ����� pair key ��Ͽ� �߰�


		// ���� ���� ���� ���� ����� �����ϴ� pair key ����

		// ������ pair key�� �ش� ����� pair key ��Ͽ� �߰�
	}
}

void Kmr_IHA::testKeyInfo()
{
	ofstream out("tst_Kmr_IHA_Key.txt");

	for (int i = 0; i < NUM_NODES; i++)
	{
		
		out << "node ID: " << i << endl; 

		// ���� Ű ���� ���	
		
		

		// ��� Ű ���� ���

	}
	out.close();
}

Kmr_IHA::Kmr_IHA() 
{


}

Kmr_IHA::~Kmr_IHA()
{

}
*/