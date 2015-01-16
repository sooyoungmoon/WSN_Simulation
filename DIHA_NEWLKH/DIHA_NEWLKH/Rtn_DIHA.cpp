// 2014.05.21
// ������
// Rtn_DIHA.h
// DIHA�� ����� Ŭ���� ����

#include "Rtn_DIHA.h"

void Rtn_DIHA::findNbrs()
{

}

void Rtn_DIHA::setCHs()
{
	// �� cell ���� CH ����

	std::map <int, std::set<int>> clusterSet; // ������ Ŭ������ ������ �з��ϱ� ���� set	
	std::map <int, int> numNodesInCell; // <cell ��ȣ, cell�� ���� ��� ��>	
	std::map <int, int> chs; // <cell ��ȣ, CH's ID>

	std::map < int, pair<double, double> > COA; // <Ŭ������ ID, <��� x��ǥ, ��� y��ǥ> >
	
	ndsClstrMap.clear(); // ���� Ŭ������ ���� ����
	clusters.clear();

	for (int c = 0; c < NUM_CELL; c++) // cell ���� ���� �߽� �ʱ�ȭ
	{
		COA[c] = make_pair(0, 0); // (0, 0)���� �ʱ�ȭ -> ��� ��쿡 ���ŵǴ���? ->yes, �� cell �� �ּ� 1�� �̻� ��� ����
		numNodesInCell[c] = 0; // 0���� �ʱ�ȭ
	}



	// ��带 �� cell ���� �з�, �� cell �� ������ x ��ǥ / y ��ǥ �հ� ����

	map<int, Node*>::iterator it_node;
	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++) // ��� ��� ��ȸ, cell ����, x,y ���� ���� -> ���� �߽� ���ϱ�
	{
		Node* pNode = (*it_node).second;
		//Node* pNode = nodes[n];

		int cellID = pNode->cell;

		clusterSet[cellID].insert(pNode->id); // ��尡 ��ġ�ϴ� cell�� ��� ���տ� �ش� ��� id �߰�

		int x = pNode->x;
		int y = pNode->y;

		COA[cellID].first += x;
		COA[cellID].second += y;

		numNodesInCell[cellID]++; // cell�� ��� ���տ� ���ԵǴ� ��� �� +1����
	}

	// �� cell�� ���� �߽� ���ϱ� (��� x, ��� y)
	for (int c = 0; c < NUM_CELL; c++) // x ��ǥ / y ��ǥ�� ������ ���� ��� ���� ����
	{
		COA[c].first /= numNodesInCell[c];
		COA[c].second /= numNodesInCell[c];
	}

	for (int c = 0; c < NUM_CELL; c++) // cell�� ���� �� ��忡�� �����߽ɱ����� �Ÿ� ���ϱ�
	{
		double minDist=0;
		double curDist=0;

		for ( set<int>::iterator it = clusterSet[c].begin(); it != clusterSet[c].end(); it++)
		{
			int nodeID = *it;
			Node* pNode = nodes[nodeID]; 

			curDist = sqrt ( pow((double)pNode->x - COA[c].first, 2) + pow ((double)pNode->y - COA[c].second, 2) );

			if ( it == clusterSet[c].begin() ) // ù ��° ������ ���, �ּ� �Ÿ� ������ ���� �ش� ���κ��� ���� �߽ɱ����� �Ÿ��� ����
			{
				minDist = curDist;
				chs[c] = nodeID;
			}

			else if ( curDist < minDist)
			{
				minDist = curDist;
				chs[c] = nodeID;
			}
		} // �� cell�� ��ǥ�ϴ� cluster head ���� �Ϸ�		

		for ( set<int>::iterator it = clusterSet[c].begin(); it != clusterSet[c].end(); it++) // Ŭ������ ���� ����
		{
			int nodeID = *it;
			int clstrhdID = chs[c]; 

			ndsClstrMap[nodeID] = clstrhdID; 

			clusters[clstrhdID].insert (nodeID);
		}
		// Ŭ������ ���� ���� �Ϸ�
	}

	/*
	// CH ���� (���� ����� ����)

	int clsID = 0;

	int mberIdx[NUM_CHS] = {0}; // Ŭ������ ����� �ε���

	for (int i = 0; i < NUM_CHS; i++)
	{
		int nIDMin = NODES_PER_CLSTR * i; // �ش� Ŭ������ ���� ��� ID�� �ּҰ��� �ִ밪
		int nIDMax = nIDMin + NODES_PER_CLSTR - 1; 

		int ch = gen1.uniformI(nIDMin, nIDMax);

		clstr_hds[i] = &nodes[ch];
	}
	*/
}

list<Node*> Rtn_DIHA::selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs) // // ���� �޽����� ���� ���� ��� ����
{
	// BS���� ���� ���� �̳��� ������ ��Ͽ� �߰��Ͽ� ��ȯ

	list<Node*> nextNdLst;

	map<int, Node*>::iterator it_node;

	if (cntrlMsg->cmType == BS_HELLO)
	{

		for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
		//for (int nID = Node::numNodesEvctd; nID < Node::totNumNodes; nID++)
		{
			Node* node = (*it_node).second;
			int nID = node->id;
			//Node* node = nodes[nID];
			
			if (node == NULL)
			{
				cout << " (node == NULL)";
				exit(-1);
			}

			if ( node->distance < TRANS_RANGE)
			{
				if ( ndsClstrMap[nID] == nID) // ��尡 CH�� ��� ���� ��� ��Ͽ� �߰�
				{
					nextNdLst.push_back(node);
				}
			}
		}
	}

	// ...
	return nextNdLst;	
}

list<Node*> Rtn_DIHA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)
{
	list<Node*> nextNdLst;

	// ���� �޽��� Ÿ�Կ� ���� ó��
	if (cntrlMsg->cmType == BS_HELLO)
	{
		int cNodeID = cNode->id;
		double curDist=0;
		double dist = 0;
		curDist = cNode->distance;

		list<Node*>::iterator it;
		list<Node*> nbrLst = nbrLsts[cNodeID];


		// �ڽ��� �̿� ������ �ϳ��� �˻�

		for (it = nbrLst.begin(); it != nbrLst.end(); it++)
		{
			Node* nbr = *it;
			int nbrID = nbr->id;

			dist = nbr->distance;

			if ( dist > curDist) // �� ���� �ڽź��� BS�� �� �̿� CH�鿡�� �޽��� ����
			{
				if ( ndsClstrMap[nbrID] == nbrID) // ��尡 CH�� ��� ���� ��� ��Ͽ� �߰�
				{
					nextNdLst.push_back(nbr); // ��� �ּ� ����
				}
			}
		}
		
	}
	else if (cntrlMsg->cmType == CLSTR_ACK)
	{
		// �� ���� �ڽ��� �θ� ��� ��Ͽ� ���Ե� ���鿡�� �޽��� ����

		//list<Node*> parents = this->getParents(cNode);
		list<Node*> parents = this->getParentCHs(cNode); // (2014.08.17 ����) �θ� CH ��� ȹ��

		list<Node*>::iterator it;
		
		
		for (it= parents.begin(); it!= parents.end(); it++)
		{
			Node* p = *it;

			if (p!= NULL)
			{
				nextNdLst.push_back(p);
			}
		}	
	}
	else
	{
		cout << "type is invalid";
		exit(-1);
	}
	return nextNdLst;			
}

Node* Rtn_DIHA::selNext (struct _tagEvntMsg* evntMsg, Node* cNode) // (2014.08.13) �̺�Ʈ ���� ���� ���� ��� ���� �Լ�
{
	// �ִ� ��� ��� �����
	/*
	Node* next = NULL;

	Node* spParent = this->getSPParentCH(cNode); // ���� ����� �ִ� ��� CH ��� ��ȯ
	//Node* spParent = this->getSPParent(cNode); // ���� ����� �ִ� ��� �θ� ��� ��ȯ

	if (spParent !=NULL) // �ִ� ��� �θ� ��带 ���� ���� ����
	{
		next = spParent;
	}
		
	return next; // ���� ��� ��ȯ
	*/

	
	
	Node* next = NULL;
	int cNodeID = cNode->id; 

	this->resetNxtNds(); // ���� ���� ��� �迭 reset
	this->eval_result.clear(); // ���� �� ��� ����
	candis.clear(); // ���� �ĺ� ��� ��� ����


	kmr->testPairKey(); // (debugging) pair key ���� �׽�Ʈ

	// �ĺ� ��忡 ���� ��

	// �ĺ� ����� DI, EC, NK�� ���ϰ� 0~1 ������ ������ ����ȭ

	double maxDI = 0; // �ĺ� ��� �� ��ũ �������� �ִ� �Ÿ�
	double maxEC = 0; // �ĺ� ��� �� �ִ� ������ �Ҹ�
	int maxNK = 0; // �ĺ� ��尡 ���� pairwise Ű�� ������ �ִ밪
	

	/// normalizing factor ���	

	// �ĺ� ��带 �ϳ��� �˻�

	list<Node*>::iterator it;

	for ( it =  parentCHs[cNodeID].begin(); it != parentCHs[cNodeID].end(); it++) // �ĺ� ��带 �ϳ��� �˻�
	//for ( it =  parentsLsts[cNodeID].begin(); it != parentsLsts[cNodeID].end(); it++) // �ĺ� ��带 �ϳ��� �˻�
	{
		Node* candi = *it;

		if (candi != NULL)
		{	
			int numPairKeys = kmr->getNumPairKeys(candi); // �ĺ� ��尡 ���� pairwise Ű�� ���� ��ȯ			

			double dis = candi->distance; // �ĺ� ����� ��ũ �������� �Ÿ�
			double eCnsmptn = candi->eCnsmptn; // �ĺ� ����� ������ �Ҹ�

			if ( numPairKeys > maxNK) maxNK = numPairKeys;

			if ( dis > maxDI) maxDI = dis;

			if ( eCnsmptn > maxEC) maxEC = eCnsmptn;
		}

	} // for�� ���� - �ĺ� ��带 �˻��Ͽ� normalizing factor ���
		
		
	// �� �Է� ��� �� ����ȭ (0~1)

	int cnt = 0;

	for ( it =  parentCHs[cNodeID].begin(); it != parentCHs[cNodeID].end(); it++) // �ĺ� ��带 �ϳ��� �˻�
	//for ( it =  parentsLsts[cNodeID].begin(); it != parentsLsts[cNodeID].end(); it++) // �ĺ� ��带 �ϳ��� �˻�
	{
		Node* candi = *it;

		if (candi != NULL)
		{		
			double eval_res;
			double normNK = kmr->getNumPairKeys(candi); // �ĺ� ��尡 src cluster �κ��� �̹� ���� ���� ���� Ű�� ���� (����ȭ)
			double normDI = (candi->distance) / maxDI; // �ĺ� ����� ��ũ �������� �Ÿ� (����ȭ)
			double normEC = candi->eCnsmptn; // // �ĺ� ����� ������ �Ҹ� (����ȭ)

			if (maxNK == 0) normNK = 1;
			else
				normNK /= maxNK;			

			if (maxEC == 0) normEC = 1;
			else
				normEC /= maxEC;
			
			
			eval_res = normEC + normDI - normNK; // (2014.12.30) ���� ��

			//eval_res = eval_fuzzy (normDI, normEC, normNK); // �� ��� ����	(PCM)		

			//eval_res = normDI; // (2014.10.01) �� ��� ����		(Greedy routing)

			//eval_res = normDI + normEC; // (2014.10.01) �� ��� ����		(GEAR)


			//eval_result[cnt] = make_pair (candi->id, eval_res); // �� ��� ���� 
			//eval_result[cnt] = make_pair (cNodeID, eval_res); // �� ��� ���� 

			candis[cnt] = candi; // �ĺ� ��� ��Ͽ� ����

			cnt++;
			
		}// if�� ����- �ĺ� ��� �� ��� ���� �� ����

	} // for�� ���� - �ĺ� ������ 1) ���� Ű ����, 2) BS������ �Ÿ�, 3) ������ �Ҹ��� (0~1) ������ ������ ����ȭ�ϰ� Fuzzy logic�� ���� �� ��� ����
	

	// �ĺ� ��� ��� ����

	quicksort (0, candis.size()-1); // �� ����� ���� �ĺ� ��� ��� ����	

	// ���� ���� ��� �迭 �� ����

	if ( candis.size() < 1 ) // �ĺ� ��尡 1���� ���� ��� NULL ��ȯ, �׷��� �ʴٸ� ���� ��� ��ȯ
		return NULL;

	else
	{
		next = candis[0];
	}	

	return next;
	
}

list<Node*> Rtn_DIHA::selNext (struct _tagKeyMsg* keyMsg, BS* bs)
{
	list<Node*> nextNdLst;
	set<Node*> nextNdSet;

	this->eval_result.clear();
	nxtNds.clear();
	next_recipent_Map.clear(); // (���� ���, {���� ���}) �� ����
	candis.clear();

	set<int> recipents = keyMsg->recipents;

	set<int>::iterator it;

	for (it = recipents.begin(); it != recipents.end(); it++) // �� ���� ��忡 ���� �ĺ� ��� ��
	{
		int recipentID = (*it);
		Node* recipent = nodes[recipentID];		

		list<Node*> nbrLst = bs->getNbrLst();
		list<Node*>::iterator it2 = nbrLst.begin();
		
		/* �� */

		//map<int, Node*> candis;

		int cnt = 0;

		for ( it2 = nbrLst.begin(); it2 != nbrLst.end(); it2++)
		{
			Node* candi = (*it2);
			int candiID = candi->id;
			candis[cnt] = candi;

			double evaluation_res = eval(recipent, candi);

			this->eval_result[cnt] = make_pair(candiID, evaluation_res);

			//this->eval_result[candiID] = eval(recipent, candi);
			cnt++;
		}
		
		/* ���� */
		quicksort (0, candis.size()-1);
		//this->quicksort (candis, 0, candis.size()-1);

		/* ���� ��� ���� �߰� */
		nextNdSet.insert(candis[0]);

		// (���� ���, {���� ���}) map ����

		if (candis[0] != NULL)
		{
			this->next_recipent_Map[candis[0]->id].insert (recipent->id);
		}
	}

	set<Node*>::iterator it3;

	for (it3 = nextNdSet.begin(); it3 != nextNdSet.end(); it3++) // ������ ���� ��� ������ ������ ���� ��� ��Ͽ� ����
	{
		Node* candi = (*it3);
		nextNdLst.push_back(candi);
	}

	return nextNdLst; // ���� ��� ��� ��ȯ
}

list<Node*> Rtn_DIHA::selNext (struct _tagKeyMsg* keyMsg, Node* cNode)
{
	list<Node*> nextNdLst;
	set<Node*> nextNdSet;

	this->eval_result.clear();
	nxtNds.clear();
	next_recipent_Map.clear(); // (���� ���, {���� ���}) �� ����
	candis.clear();

	set<int> recipents = keyMsg->recipents;

	set<int>::iterator it;

	for (it = recipents.begin(); it != recipents.end(); it++) // �� ���� ��忡 ���� �ĺ� ��� ��
	{
		int recipentID = (*it);
		Node* recipent = nodes[recipentID];

		int cNodeID = cNode->id;

		list<Node*>::iterator it2 = this->nbrLsts[cNodeID].begin();
		
		/* �� */

		//map<int, Node*> candis;

		int cnt = 0;

		for ( it2 = nbrLsts[cNodeID].begin(); it2 != nbrLsts[cNodeID].end(); it2++)
		{
			Node* candi = (*it2);
			int candiID = candi->id;
			candis[cnt] = candi;

			double evaluation_res = eval(recipent, candi);
			this->eval_result[cnt] = make_pair(candiID, evaluation_res);

			//this->eval_result[candiID] = eval(recipent, candi);
			cnt++;
		}
		
		/* ���� */
		quicksort (0, candis.size()-1);
		//this->quicksort (candis, 0, candis.size()-1);

		/* ���� ��� ���� �߰� */
		nextNdSet.insert(candis[0]);
		
		if (candis[0] != NULL)
		{
			this->next_recipent_Map[ candis[0]->id].insert (recipent->id); // (���� ���, {���� ���}) map ����
		}
	}

	set<Node*>::iterator it3;

	for (it3 = nextNdSet.begin(); it3 != nextNdSet.end(); it3++) // ������ ���� ��� ������ ������ ���� ��� ��Ͽ� ����
	{
		Node* candi = (*it3);
		nextNdLst.push_back(candi);
	}

	return nextNdLst; // ���� ��� ��� ��ȯ

	// ���� ��
	/*
	list<Node*> nextNdLst;


	// ���� ����� �̿� ��� ��Ͽ� ���� ���� �˻�
	list<int> recipents = keyMsg->recipents;
	int recipentID = recipents.front();
	Node* recipent = nodes[recipentID];

	int cNodeID = cNode->id;
	list<Node*>::iterator it = this->nbrLsts[cNodeID].begin();
	*/
	/* �� */
	/*
	for (it; it !=  nbrLsts[cNodeID].end(); it++)
	{
		Node* nbr = (*it);
		int nbrID = nbr->id;

		this->eval_result[nbrID] = this->eval(keyMsg, cNode, nbr);
	}
	*/
	/* ���� */

	// Ű ��й� �޽��� ���� ��� (����� 1����� ����) �� ���� ����� ��带 ���� ���� ��� ��Ͽ� �߰�
	// ...

	/* ���� ���� ��� ��� ���� */

	return nextNdLst;
}

list<Node*> Rtn_DIHA::selNext (struct _tagTREQ* treq, Node* cNode) // (2014.05.10) TREQ �޽��� ������ ���� ���� ��� ����
{
	// ���� ��

	if (cNode == NULL)
	{
		cout << "cNode == NULL";
		exit(-1);
	}

	list<Node*> nextNdLst;

	// �ʵ� ���� ������ ��� �˻�

	map<int, Node*>::iterator it_node;
	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	//for (int n = Node::numNodesEvctd; n < Node::totNumNodes; n++)
	{
		Node* pNode = (*it_node).second;
		//Node* pNode = nodes[n];

		if ( pNode != cNode)
		{
			double dist = sqrt( pow((double) pNode->x - cNode->x, 2 ) + pow ( (double) pNode->y-cNode->y, 2));// ���� ���� ���� �˻� ���� ��� �� �Ÿ� ���

			if ( dist < TRANS_RANGE)
			{
				nextNdLst.push_back(pNode);
			}
		}
	}
	// ...


	return nextNdLst;
}

list<Node*> Rtn_DIHA::selNext (struct _tagTRES* tres, Node* cNode) // (2014.05.10) TREQ �޽��� ������ ���� ���� ��� ����
{
	// ���� ��

	list<Node*> nextNdLst;

	Node* spParent = this->getSPParentNode(cNode); // ���� ����� �ִ� ��� �θ� ��� ��ȯ

	if (spParent !=NULL)
	{
		nextNdLst.push_back(spParent);
	}

	// ...

	return nextNdLst;
}

double Rtn_DIHA::eval(Node* recipent, Node* candi)
{
	double res = 0;

	res = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	

	return res;
}

double Rtn_DIHA::testConect()
{
	map<int, set<int>>::iterator it;

 	int numNodesInClstr = 0;
	int numIsolatedCHs = 0; // BS���� ������� ���� ch

	int CHID;

	Node* tempNode;

	double res; // �� CH�� �� BS�κ��� ������ CH���� ����


	for (it = clusters.begin(); it != clusters.end(); it++) // Ŭ������ ��ȸ - �� CH�κ��� BS������ ��ΰ� ����Ǿ� �ִ��� �˻�
	{
		tempNode = NULL;

		if ( !(*it).second.empty())
		{
			numNodesInClstr += (*it).second.size();

			if (numNodesInClstr ==0)
			{
				continue;
			}

			CHID = (*it).first;

			if ( nodes[CHID] == NULL) // (2014.09.21) ���� - CH�� �̹� ���ŵ� ����̸� ���� Ŭ�����ͷ� �̵�
			{
				continue; 
			}

			// CH-BS ������ ��� ����

			tempNode = nodes[CHID];

			while (tempNode->distance >= TRANS_RANGE) // �ִ� ��θ� ���� BS �������� �̵�
			{				
				tempNode = this->getSPParentCH(tempNode);		

				if (tempNode == NULL)
				{
					break;
				}

			} // �ִ� ��θ� ���� BS �������� �̵�

			if ( tempNode == NULL) // ���� ��尡 bs�κ��� ���� ���� ����getSPParentCH ���, �ش� CH�� BS�κ��� ����
			{
				numIsolatedCHs++;
			} // �� ��� �� ����
		}

	} // for�� ��� CH�� ���� ���� �˻�

	res =  ((double) numIsolatedCHs)/ clusters.size();

	return res;
}

/*
double Rtn_DIHA::eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi) // Ű ��й� �޽��� ������ ���� �ĺ� ��� ��
{
	double res = 0; // �ĺ� ����� �� ���

	list<int> recipents = keyMsg->recipents;
	int recipentID = recipents.front();
	Node* recipent = nodes[recipentID];

	// Ű �޽��� ���� ���� �ĺ� ��� ���� �Ÿ��� ���

	double dist = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	
	res = dist;
	return res;
}
*/

double Rtn_DIHA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi)
{
	double res = 0; // �ĺ� ����� �� ���

	return res;
}

double Rtn_DIHA::eval (struct _tagEvntMsg* evntMsg, Node* candi)
{
	double res = 0; // �ĺ� ����� �� ���

	return res;
}

double Rtn_DIHA::eval_fuzzy (double dist, double eCnpmtn, double numKeys) // (2014.08.13) fuzzy logic�� ���� (�̺�Ʈ ������ ����) �ĺ� ��� �� 
{
	double res = 0;

	res =  (double) fuzzy.think( eCnpmtn*100, dist*100,  numKeys*100); // fuzzy logic�� ���� �߷�

	return res;
}

Rtn_DIHA::Rtn_DIHA()
{

}

Rtn_DIHA::~Rtn_DIHA()
{

}