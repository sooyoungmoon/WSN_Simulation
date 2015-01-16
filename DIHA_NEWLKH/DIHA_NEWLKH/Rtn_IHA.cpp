// 2014.05.21
// ������


#include "Rtn_IHA.h"
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

void Rtn_IHA::findNbrs()
{
	/*
	// �̿� ��� Ž�� ����

	int nbrIdx = 0; // �̿� ��� �ε���	

	int i = 0, j = 0;

	for(i=0; i < NUM_NODES;i++) 
	{
		for(j=0; j<NUM_CHS;j++)
		{
			if ( nodes[i].id != clstr_hds[j]->id)
			{
				//�̿� ��尡 ���� ���� ��� ������ �Ÿ��� �����ϴ� ��� �̿� ��� ��Ͽ� �߰� 
				if(  pow((double)((clstr_hds[j])->x-(nodes+i)->x),2) + pow((double)((clstr_hds[j])->y-(nodes+i)->y),2) < pow((double)TRANS_RANGE,2))
				{
					if (nbrIdx < NBRS_PER_NODE)
					{
						nbrs[i][nbrIdx++] = (clstr_hds[j]); // �̿� ��� ���� ����

					}
				}
			}
		}

		numNbrs[i] = nbrIdx; // (2013.08.13) �� ����� �̿� ��� �� ����
		nbrIdx = 0;
	}

	setCandis(); // �ĺ� �θ� ��� ����
	this->testCandis(); // (2012.11.23 �߰�) �ĺ� ��� �׽�Ʈ �Լ� ȣ��
	*/
}

void Rtn_IHA::setCHs()
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
		double minDist;
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

list<Node*> Rtn_IHA::selNext (struct _tagCntrlMsg* cntrlMsg, BS* bs) // // ���� �޽����� ���� ���� ��� ����
{
	// BS���� ���� ���� �̳��� ������ ��Ͽ� �߰��Ͽ� ��ȯ

	list<Node*> nextNdLst;

	if (cntrlMsg->cmType == BS_HELLO)
	{
		map<int, Node*>::iterator it_node;
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

	this->nxtNds = nextNdLst;

	return this->nxtNds;	
}

list<Node*> Rtn_IHA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)  // ���� �޽����� ���� ���� ��� ����
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
		// �� ���� �ڽ��� �ִ� ��� CH ��忡�� �޽��� ����

		Node* spParent = NULL;

		spParent = getSPParentCH(cNode);
		//spParent = getSPParent(cNode);

		if ( spParent != NULL )
		{
			nextNdLst.push_back(spParent);
		}
	}
	else
	{
		cout << "type is invalid";
		exit(-1);
	}
	return nextNdLst;	
}

Node* Rtn_IHA::selNext(struct _tagEvntMsg* evntMsg, Node* cNode) // (2014.08.13) �̺�Ʈ ������ ���� ���� ���� ��� ����
{
	// �ִ� ��� ��� �����

	Node* next = NULL;

	Node* spParent = this->getSPParentCH(cNode); // ���� ����� �ִ� ��� CH ��� ��ȯ
	//Node* spParent = this->getSPParent(cNode); // ���� ����� �ִ� ��� �θ� ��� ��ȯ

	if (spParent !=NULL) // �ִ� ��� �θ� ��带 ���� ���� ����
	{
		next = spParent;
	}
		
	return next; // ���� ��� ��ȯ
}

list<Node*> Rtn_IHA::selNext (struct _tagKeyMsg* keyMsg, BS* bs)
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
			
			//this->eval_result[cnt] = eval(recipent, candi);
			//this->eval_result[candiID] = eval(recipent, candi);
			cnt++;
		}
		
		/* ���� */
		quicksort (0, candis.size()-1);
		//this->quicksort (candis, 0, candis.size()-1);

		/* ���� ��� ���� �߰� */
		nextNdSet.insert(candis[0]);

		// (���� ���, {���� ���}) map ����
		this->next_recipent_Map[ candis[0]->id].insert (recipent->id);
	}

	set<Node*>::iterator it3;

	for (it3 = nextNdSet.begin(); it3 != nextNdSet.end(); it3++) // ������ ���� ��� ������ ������ ���� ��� ��Ͽ� ����
	{
		Node* candi = (*it3);
		nextNdLst.push_back(candi);
	}

	return nextNdLst; // ���� ��� ��� ��ȯ
}

list<Node*> Rtn_IHA::selNext (struct _tagKeyMsg* keyMsg, Node* cNode)
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
		//candis.clear();
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

}

list<Node*> Rtn_IHA::selNext (struct _tagTREQ* treq, Node* cNode) // (2014.05.10) TREQ �޽��� ������ ���� ���� ��� ����
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

list<Node*> Rtn_IHA::selNext (struct _tagTRES* tres, Node* cNode) // (2014.05.10) TRES �޽��� ������ ���� ���� ��� ����
{
	// ���� ��

	list<Node*> nextNdLst;

	Node* spParent = this->getSPParentNode(cNode); // ���� ����� �ִ� ��� CH ��� ��ȯ

	if (spParent !=NULL)
	{
		nextNdLst.push_back(spParent);
	}

	// ...

	return nextNdLst;
}

/*
double Rtn_IHA::eval (struct _tagKeyMsg* keyMsg, Node* cNode, Node* candi) // Ű ��й� �޽��� ������ ���� �ĺ� ��� ��
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

double Rtn_IHA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi)
{
	double res = 0; // �ĺ� ����� �� ���

	return res;
}

double Rtn_IHA::eval (struct _tagEvntMsg* evntMsg, Node* candi)
{
	double res = 0; // �ĺ� ����� �� ���

	return res;
}


double Rtn_IHA::eval(Node* recipent, Node* candi) // Ű ��й� �޽��� ������ ���� �ĺ� ��� ��
{
	double res = 0;

	res = sqrt ( pow((double) candi->x- recipent->x, 2) + pow ( (double) candi->y - recipent->y, 2) );	

	return res;
}

double Rtn_IHA::testConect()
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

Rtn_IHA::Rtn_IHA()
{

}

Rtn_IHA::~Rtn_IHA()
{

}


/*
void Rtn_IHA::findNbrs()
{
	// �̿� ��� Ž�� ����

	int nbrIdx = 0; // �̿� ��� �ε���	

	int i = 0, j = 0;

	for(i=0; i < NUM_NODES;i++) 
	{
		for(j=0; j<NUM_CHS;j++)
		{
			if ( nodes[i].id != clstr_hds[j]->id)
			{
				//�̿� ��尡 ���� ���� ��� ������ �Ÿ��� �����ϴ� ��� �̿� ��� ��Ͽ� �߰� 
				if(  pow((double)((clstr_hds[j])->x-(nodes+i)->x),2) + pow((double)((clstr_hds[j])->y-(nodes+i)->y),2) < pow((double)TRANS_RANGE,2))
				{
					if (nbrIdx < NBRS_PER_NODE)
					{
						nbrs[i][nbrIdx++] = (clstr_hds[j]); // �̿� ��� ���� ����

					}
				}
			}
		}

		numNbrs[i] = nbrIdx; // (2013.08.13) �� ����� �̿� ��� �� ����
		nbrIdx = 0;
	}

	setCandis(); // �ĺ� �θ� ��� ����
	this->testCandis(); // (2012.11.23 �߰�) �ĺ� ��� �׽�Ʈ �Լ� ȣ��
}
*/

/*
void Rtn_IHA::setCHs()
{

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
}
*/

/*
void Rtn_IHA::setCandis()
{
	// �ƹ� ó���� ����
}
*/
/*
Node** Rtn_IHA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)  // ���� �޽����� ���� ���� ��� ����
{
	// ���� ��� ��� ���
	this->resetNxtNds(); //  ���� ���� ��� �迭 reset
	
	int idx = cNode->id; // ����� �迭 �� �ε���

	int numNxtNds = 0; // ����� ���� ���� ��� ��

	int nxtNdsIdx = 0; // ���� ��� �迭 �� �ε���

	// ���� �޽��� Ÿ�Կ� ���� ó��

	if (cntrlMsg->cmType == BS_HELLO)
	{
		// �� ���� �ڽź��� BS�� �� �̿� ���鿡�� �޽��� ����

		for (int i =0; i < numNbrs[idx]; i++) // ���� ����� �̿� ��带 �ϳ��� �˻�
		{
			// ���� �̿� ��尡 �ڽź��� BS�κ��� �� �� ���, 
			// ���� ��� �迭�� �ش� �̿� ��� ���� ����

			Node * neighbor = nbrs[idx][i];

			if (cNode->distance < neighbor->distance) // ���� ��庸�� �̿� ����� BS������ �Ÿ��� �� �� ���
			{

				int clstr1 = cNode->id / NODES_PER_CLSTR;
				int clstr2 = neighbor->id / NODES_PER_CLSTR;

				if (clstr1 != clstr2) // �� ����� Ŭ�����Ͱ� �ٸ� ��� ���� ���� ���� ����
				{
					nxtNds[nxtNdsIdx++] = neighbor;
					numNxtNds++;

					if (numNxtNds > MAX_NXT_NODES)
					{
						break;
					}
				}
			}
		}
	}
	else if (cntrlMsg->cmType == CLSTR_ACK)
	{
		// �� ���� �ڽ��� �θ� ��忡�� CLSTR_ACK �޽��� ����
		
		if (!parents[idx].empty())
		{
			list<Node*>::iterator it= parents[idx].begin();
			Node* parent = *it;

			nxtNds[0] = parent;	
			numNxtNds = 1;
		}	
		
	}
	else
	{
		cout << "error - Rtn_PRM::selNext()- �߸��� ����޽��� Ÿ��" << endl;
	}	

	// ���� ��� ��� ��ȯ
	
	return this->nxtNds;	

}

Node* Rtn_IHA::selNext (struct _tagEvntMsg* evntMsg, Node* cNode)
{
	Node* next = NULL;

	// �̸� ����� �θ� ��� ��ȯ

	int cNodeID = cNode->id; // ���� ����� ID

	if (!parents[cNodeID].empty())
	{
		list<Node*>::iterator it= parents[cNodeID].begin();
		Node* parent = *it;

		next = parent;
	}


	//next = parents[cNodeID]; // ���� ����� �θ� ��� ��ȯ

	return next;
}

double Rtn_IHA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) 
{
	// ����� ��� �� ��
	double res = 0; // �ĺ� ����� �� ���

	return res;
}

double Rtn_IHA::eval (struct _tagEvntMsg* evntMsg, Node* candi)
{
	// ����� ��� �� ��
	double res = 0; // �ĺ� ����� �� ���

	return res;
}

Rtn_IHA::Rtn_IHA(void)
{
	cout << "Rtn_IHA()::Rtn_IHA() " << endl;
}

Rtn_IHA::~Rtn_IHA(void)
{

}
*/
/*
void Rtn_GEA::setCHs()
{
	// CH ���� 

	int clsID = 0;

	int mberIdx[NUM_CHS] = {0}; // Ŭ������ ����� �ε���

	for (int i = 0; i < NUM_CHS; i++)
	{
		int nIDMin = NODES_PER_CLSTR * i; // �ش� Ŭ������ ���� ��� ID�� �ּҰ��� �ִ밪
		int nIDMax = nIDMin + NODES_PER_CLSTR - 1; 

		int ch = gen1.uniformI(nIDMin, nIDMax);

		clstr_hds[i] = (nodes+ch);
	}
}

void Rtn_GEA::setCandis()
{
	// (2012.11.18)���� ����� GEAR�� �ĺ� ��� ���� ������ ���ٰ� ������
	// ���� 1 - ���� ����� �̿� ���
	// ���� 2 - ���� ��庸�� ��ũ ��忡 �����
	// ���� 3 - ���� ���� �ٸ� Ŭ�����Ϳ� ����

	int i = 0, j = 0; 
	int candiIdx = 0; // �ĺ� �θ� ����� �ε���

	for (i = 0; i < NUM_NODES; i++)
	{
		for ( j = 0; j < NBRS_PER_NODE; j++)
		{
			Node* cNode = this->nbrs[i][j]; // ���� �˻� ���� �̿� ���

			if (cNode == NULL) continue;

			if (cNode->distance < (nodes+i)->distance) // ���� ��庸�� �̿� ����� ��ũ �������� �Ÿ��� �� ����� ���
			{
				int clstr1 = (nodes+i)->id / NODES_PER_CLSTR; 
				int clstr2 = cNode->id / NODES_PER_CLSTR; 

				//if (clstr1 != clstr2) // (2012.11.24 ����) ���� ���� �̿� ����� Ŭ�����Ͱ� �ٸ� ���
				{
					this->candis[i][candiIdx++] = cNode; // �ĺ� ��� ���� ����

					if ( candiIdx >= CANDIS_PER_NODE) // �ĺ� ��� �迭�� ���� �� ��� ���� for�� Ż��
						break; 
				}
			}

		}

		numCandis[i] = candiIdx; // ����� �ĺ� �θ� ��� �� ����

		candiIdx = 0;
	}

}

Node** Rtn_GEA::selNext (KeyMsg* keyMsg, Node* cNode)
{
	// ���� ����� ���� ��� �� ������ ����������
	// Ű ���� ���� ������ ������� �ʰ�,
	// �ٸ� �� �Լ��� ��� (�Ÿ��� ������ �Ҹ𷮸� ���)

	// (2012.11.24 ����) ���� ���� ��� �迭 reset
	this->resetNxtNds();


	// �ĺ� ��� ��
	int idx = cNode->id;

	// �ĺ� ����� DI, EC �� ���ϰ� 0~1 ������ ������ ����ȭ
	
	double maxDI = 0; // �ĺ� ��� �� ��ũ �������� �ִ� �Ÿ�
	double maxEC = 0; // �ĺ� ��� �� �ִ� ������ �Ҹ�

	for (int i = 0; i < numCandis[idx]; i++) // normalizing factor ���
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{			
			double dis = candi->distance; // �ĺ� ����� ��ũ �������� �Ÿ�
			double eCnsmptn = candi->eCnsmptn; // �ĺ� ����� ������ �Ҹ�					

			if ( dis > maxDI) maxDI = dis;

			if ( eCnsmptn > maxEC) maxEC = eCnsmptn;
		}
	}
	
	for (int i = 0; i < numCandis[idx]; i++)
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{		
			double normDI = (candi->distance) / maxDI; // �ĺ� ����� ��ũ �������� �Ÿ� (����ȭ)
			double normEC = candi->eCnsmptn; // // �ĺ� ����� ������ �Ҹ� (����ȭ)

			if (maxEC ==0) normEC = 0;
			else
				normEC /= maxEC;

			eval_result[i] = eval ( normDI, normEC); // �� ��� ����, ����
		}
	}
	
	// �ĺ� ��� ��� ����
	this->quicksort( candis[idx], 0, numCandis[idx]-1); // �� ����� ���� �ĺ� ��� ��� ���� (���� �ĺ� ��� ���� �°� ���� ���� �ʿ�)

	// ���� ��� ��� ����

	int numNxtNds = 0;

	if (numCandis[idx] < BRANCH_FACTOR)
	{
		numNxtNds = numCandis[idx];
	}
	else 
		numNxtNds = BRANCH_FACTOR;


		for (int i = 0; i < numNxtNds; i++) // ���� ���� ��� �迭�� ��� ���� ����
		{
			nxtNds[i] = candis[idx][numCandis[idx] -1-i];

			// (2012.11.12) �ӽ� test
			/*
			if (cNode->id == 999)
			{
				cout << "nxtNds[i] " << nxtNds[i]->id << " " ;
				cout << "nxtNds[i+1] " << nxtNds[i+1]->id << " " ;
			}
			
		}	

	// ���� ��� ��� ��ȯ
	
	return this->nxtNds;	
}

Node** Rtn_GEA::selNext (struct _tagCntrlMsg* cntrlMsg, Node* cNode)  // ��� �� ��
{
	return NULL; 
}


Node* Rtn_GEA::selNext (EvntMsg* evntMsg, Node* cNode)
{
	// �̺�Ʈ �޽��� ���� ��� ���� Ű ���� �޽����� ���� ���� ��� ���� ����� ����
	// ���� ���� ��尡 �ƴ� �� 1���� ���� ��� �ּ� ��ȯ

	// (2012.11.24 ����) ���� ���� ��� �迭 reset
	this->resetNxtNds();

	// �ĺ� ��� ��
	int idx = cNode->id;

	// �ĺ� ����� DI, EC �� ���ϰ� 0~1 ������ ������ ����ȭ
	
	double maxDI = 0; // �ĺ� ��� �� ��ũ �������� �ִ� �Ÿ�
	double maxEC = 0; // �ĺ� ��� �� �ִ� ������ �Ҹ�

	for (int i = 0; i < numCandis[idx]; i++) // normalizing factor ���
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{			
			double dis = candi->distance; // �ĺ� ����� ��ũ �������� �Ÿ�
			double eCnsmptn = candi->eCnsmptn; // �ĺ� ����� ������ �Ҹ�					

			if ( dis > maxDI) maxDI = dis;

			if ( eCnsmptn > maxEC) maxEC = eCnsmptn;
		}
	}
	
	for (int i = 0; i < numCandis[idx]; i++)
	{
		Node* candi = candis[idx][i];

		if (candi != NULL)
		{		
			double normDI = (candi->distance) / maxDI; // �ĺ� ����� ��ũ �������� �Ÿ� (����ȭ)
			double normEC = candi->eCnsmptn; // // �ĺ� ����� ������ �Ҹ� (����ȭ)

			if (maxEC ==0) normEC = 0;
			else
				normEC /= maxEC;

			eval_result[i] = eval ( normDI, normEC); // �� ��� ����, ����
		}
	}
	
	// �ĺ� ��� ��� ����
	this->quicksort( candis[idx], 0, numCandis[idx]-1); // �� ����� ���� �ĺ� ��� ��� ���� (���� �ĺ� ��� ���� �°� ���� ���� �ʿ�)

	// ���� ��� ��� ����

	int numNxtNds = 0;

	if (numCandis[idx] < BRANCH_FACTOR)
	{
		numNxtNds = numCandis[idx];
	}
	else 
		numNxtNds = BRANCH_FACTOR;


		for (int i = 0; i < numNxtNds; i++) // ���� ���� ��� �迭�� ��� ���� ����
		{
			nxtNds[i] = candis[idx][numCandis[idx] -1-i];			
		}	

	// ���� ��� ��� ��ȯ	
	return this->nxtNds[0];	
}




double Rtn_GEA::eval(KeyMsg* keyMsg, Node* candi)
{
	double res = 0;

	// ...

	return res;
}

double Rtn_GEA::eval (struct _tagCntrlMsg* cntrlMsg, Node* candi) // ��� �� ��
{
	double res = 0;

	// ...

	return res;
}

double Rtn_GEA::eval (struct _tagEvntMsg* evntMsg, Node* candi) // ��� �� ��
{
	double res = 0;

	// ...

	return res;
}

double Rtn_GEA::eval (double dist, double eCnpmtn ) // �� factor�� 0~1 ������ ������ ����ȭ
{
	double res = 0;

	res = (-1) * alpha_GEA * dist + (-1) * beta_GEA * eCnpmtn; // (2012.11.29 ����) �� �Լ� �� ����

	return res;
}
*/