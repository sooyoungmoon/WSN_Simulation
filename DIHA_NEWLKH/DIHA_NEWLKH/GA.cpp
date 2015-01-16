

// 2014.07.04
// ������
// GA.cpp
// ���� �˰��� Ŭ���� ����

#include "GA.h"

using std::cout;

void GA::setNodes(map<int, Node*> nodesInput) // (2013.08.28 �߰�)
{
	int count = 0;
	// 

	nodes.clear();

	map<int, Node*>::iterator it;

	for (it = nodesInput.begin(); it != nodesInput.end(); it++) // �Է� ���� node map�� ��ҵ��� ��� ������ nodes map�� �����ϵ� Ű ���� ���� ���� 0~ size-1
	{
		Node* node = (*it).second;

		nodes[count] = node;
		count++;
	}// �Է� ���� node map�� ��ҵ��� �����ϰ� key ���� ���� ����

	//this->nodes= nodes; // 
}

void GA::setFullSetNodes(map<int, Node*> fullSetNodes)
{
	this->fullSetNodes = fullSetNodes; // ��ü ��� map ����
}

void GA::setKmr(Kmr* kmr) // Ű ������ ��ü ����
{
	this->kmr = kmr;
}

void GA::setIndiLen(int indiLen)
{
	this->indiLen = indiLen;
}

void GA::setRoot(KNode* root)
{
	this->root = root;
}

void GA::create_population() // �ʱ� ���� ����
{
	// GA�� ��� ������ nodes map�� key ���� �缳�� ���ش�.

	map<int, Node*>::iterator it;

	/*
	int cnt = 0;

	for (it = nodes.begin(); it != nodes.end(); it++) // node map �� ��ҵ� ��ȸ - key �缳�� 
	{
		(*it).first = cnt;
		cnt++;
	} // node map�� key �缳�� �Ϸ�
	*/


	quicksort(0, nodes.size()-1); // ��� ����� cell id �������� �������� ����
		
	for(int i=0; i < Population_Size; i++)
	{
		/*
		if ( i % 10 ==0)
		{
			cout << "��ü " << i << "~" << i+9 << " ���� " << endl;
		}
		*/
		geneIdx = 0;

		for (int j = 0; j < this->indiLen; j++) 
		{
			Gene gene = std::make_pair(-1, -1); // �������� ù ��°�� �� ��° ���� -1�� �ʱ�ȭ 

			population[i].insert( make_pair (j, gene)); // �� ������ �ش� ��ü�� �� �������� ���� �ʱ�ȭ
			//population[i].insert(std::pair<int, Gene> (j, gene)); // �� ������ �ش� ��ü�� �� �������� ���� �ʱ�ȭ
			temp_population[i].insert( make_pair (j, gene)); // �ӽ� ������ �ش� ��ü�� �������� ���� 0���� �ʱ�ȭ

			this->popGeneTypes[i].insert(std::pair<int, char> (j, '\0' ));
			//this->geneTypes[i].insert(std::pair<int, char> (j, '\0' ));
			//this->geneTypes.insert(std::pair<int, char> (j, '\0' )); // gene type �ʱ�ȭ
		}	
		// ������� Ȯ�� (7/4, 11:30)
		this->writeGroupIDs(root, i, geneIdx);// �׷� id�� �����Ǵ� ������ ��� (key tree parsing)	
		
		//this->printIndividual(population[i]); // i ��° individual ���

		this->writeNodeIDs(i);// i��° individual�� ��� id�� �����Ǵ� ������ ��� (random)

		this->printIndividual(population[i]); // i ��° individual ���

		temp_population[i] = population[i];
		this->tPopGeneTypes[i] = this->popGeneTypes[i];

		this->veriIndividual(temp_population[i], this->tPopGeneTypes[i]); // // i ��° individual ����
		//this->veriIndividual(temp_population[i], geneTypes[i]); // // i ��° individual ����
	}

}

void GA::evaluate() // ���뿡 ���� ��ü���� ��
{	
	bestIndi = -1;
	double best_fitness;

	double eval_res = 0;
	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	double avg_fitness = 0;

	int weight = 0 ; // ���� (level)�� ���� �׷��� ����ġ

	map<int, int> weights; // �� level�� ���� ����ġ �� ( <level, weight> )

	//* ����ġ ���

	int degree = 0; //

	//for (int j = kTreeHeight-1; j >= 1; j--) // ck level ���� ����ġ ����
	for (int j = kTreeHeight-2; j >= 1; j--) //ck level ���� ����ġ ����
	{
		weights[j] = 1; // ���� level�� ����ġ�� 1�� �ʱ�ȭ

		if (j == kTreeHeight-2)
		{
			weights[j] = 1;
		}
		else
		{
			weights[j] = weights[j+1]*Max_Degree;
		}

		/*
		degree = kmr->getNumNodesAtLevel(j+1) / kmr->getNumNodesAtLevel(j); // ���� level�� degree ���

		if (j==kTreeHeight-2)
		{
			weights[j] = degree;
		}
		else
		{
			weights[j] = weights[j+1]*degree;
		}
		*/
	}

	//* ����ġ ��� �Ϸ�


	for (int i =0; i < Population_Size; i++)
	{		

		fitness[i] = 0; // �� ��� ����		
		
		//cout << i << "��° ��ü ��" << endl;
			 
		eval_res = 0;

		// (2014.10.07) i��° ��ü �� ��� ����

		Chromosome indi = population[i]; // i��° ��ü ȹ��

		int locus;



		for (locus = 0; locus < indi.size(); locus++) // ��ü ��ĵ
		{
			if ( this->popGeneTypes[i][locus] == 'G') // ������ Ÿ���� Group�� ���
			{
				int level = indi[locus].first;
				int index = indi[locus].second;

				if (level == 0)
				{
					continue;
				}

				double eval = weights[level] * f (i, level, index); // �򰡰�� �� ���� (i��° ��ü�� �ش� (level, index)�� �׷쿡 ���� ������ �е�)

				if ( this->IsFiniteNumber(eval) == false)
				{
					cout << "\n f() �Լ�- ��ȯ���� ������ �Ǽ��� �ƴ�";
				}

				eval_res += eval;
			}

		} // for�� �Ϸ� - ��ü ��ĵ �� �� �Ϸ�


		//int j = kTreeHeight-2;
		/*
		int numNodesAtLevel = 0;

		int startingLevel = 0;

		if ( root->level == 0)
		{
			startingLevel = 1;
		}
		else
		{
			startingLevel = root->level;
		}
		
		
		for (int j = startingLevel; j < kTreeHeight-1; j++)
		//for (int j = root->level+1; j < kTreeHeight-1; j++)
		//for (int j=1; j <  kTreeHeight-1; j++)  // i ��° ��ü�� 1~ (h-2) level�� �׷� ��
		{			

			// j ��° level�� ��� �� ȹ��
			numNodesAtLevel = kmr->getNumNodesAtLevel(j);

			if (numNodesAtLevel ==0)
			{
				//return;
				continue; // (2014.09.29 ����) - Ư�� level�� kNode ���� 0�� ��� 
			}

			for (int k= 0; k < numNodesAtLevel; k++) //�� ���� �׷쿡 ���� ������ �����߽��� �������� �Ÿ��� ǥ������ ���
			{
				double eval = weights[j] * f (i, j, k); // �򰡰�� �� ���� (i��° ��ü�� j��° level, k��° �׷쿡 ���� ������ �е�)
				
				//double eval =  (1.0 / j) * f(i , j, k); // �򰡰�� �� ���� (i��° ��ü�� j��° level, k��° �׷쿡 ���� ������ �����߽����κ����� �Ÿ��� ��� ���)
				//double eval = j * f(i , j, k); // �򰡰�� �� ���� (i��° ��ü�� j��° level, k��° �׷쿡 ���� ������ �����߽����κ����� �Ÿ��� ��� ���)
				
				if ( this->IsFiniteNumber(eval) == false)
				{
					cout << "\n f() �Լ�- ��ȯ���� ������ �Ǽ��� �ƴ�";
				}
				
				eval_res += eval;

				//eval_res += j * f(i, j, k); // �򰡰�� �� ���� (i��° ��ü�� j��° level, k��° �׷쿡 ���� ������ �����߽����κ����� �Ÿ��� ��� ���)
			}	// j��° level�� �׷� �� �Ϸ�		
		} // ��� level�� �׷� �� �Ϸ�
		*/



		// �� ��� ���
		fitness[i] = eval_res;

		if (eval_res == 0)
		{
			eval_res = 0;
		}

		avg_fitness += fitness[i];

		if ( bestIndi < 0) // ���� ���� �ʱ�ȭ ���� ���� ���
		{
			bestIndi = i;
			best_fitness = fitness[i];
		}
		else if ( fitness[i] > best_fitness) // ���յ��� ���� ���� (����) ��쿡 �ش��ϴ� ��ü ��ȣ ����
		//else if ( fitness[i] < best_fitness) // ���յ��� ���� ���� (����) ��쿡 �ش��ϴ� ��ü ��ȣ ����
		{
			bestIndi = i;
			best_fitness = fitness[i];
		}

		


		//fitness[i] = eval_res / numNodesAtLevel;

		//this->veriIndividual(population[i], this->popGeneTypes[i]);
	} // ���� �� �Ϸ�

	if (bestIndi < 0) // (2014.09.29)
	{
		bestIndi = 0;
		best_fitness = fitness[0];
	}

	avg_fitness /= Population_Size;

	cout << "best_fitness: " << best_fitness << endl;
	cout << "avg_fitness: " << avg_fitness << endl;
	
}

double GA::f(int individual, int level, int index) // (2014.07.04 ����) �־��� level�� index�� �����ϴ� ���� �׷쿡 ���� ������ �� ��� (�����߽����κ����� ��� �Ÿ�) ��ȯ
{
	set<int> nodesInGrp; // �־��� ��ü���� (level, index)�� �ش��ϴ� �׷� �Ҽ� ������ ID ����

	double res = 0;

	Chromosome indi = population[individual];

	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	int groupPos = 0; // ����ü �󿡼� �ش� ���� �׷��� ���� ��ġ
 
	int numNodes = 0; // �ش� ���� �׷쿡 ���� ������ ���� 
	double avg_X = 0; //				"			x ��ǥ�� ���
	double avg_Y = 0; //				"			y ��ǥ�� ���
	double avg_dist = 0; // ���� �߽����κ��� ��������� �Ÿ��� ���

	unsigned int i = 0;

	// �ش� ���� �׷��� �����߽� (COG) ���, ��� ���� ���
	
	struct _tagKNode* kNode = NULL;
	
	for ( i = 0; i < indi.size(); i++) // �ؿ� �ش��ϴ� ��ü ��ĵ
	{
		if ( this->popGeneTypes[individual][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // �־��� level�� index�� �����Ǵ� �׷� ������ �����ϴ� ������ �߰�
			{
				kNode = kmr->getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			}
			else if ( indi[i].first == level && indi[i].second > index) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}	
			
			else if (kNode != NULL && indi[i].first < level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			//else if ( indi[i].first > level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}		
			
		} // ������ Ÿ���� 'G'�� ��� ó��
		
		else if ( popGeneTypes[individual][i] == 'N' && kNode != NULL)  // �ش� ������ �׷� ���� ��� ������ �����ϴ� ������ �߰�		
		{
			if( kNode->level == level && kNode->index == index) // �ش� ������ �׷� ���� ��� ������ �����ϴ� ������ �߰�
			{
				int nodeID; 
				nodeID = indi[i].second;

				if (nodeID >=0)
				{
					numNodes++;

					avg_X += fullSetNodes[nodeID]->x;
					avg_Y += fullSetNodes[nodeID]->y;	
					//avg_X += nodes[nodeID]->x;
					//avg_Y += nodes[nodeID]->y;	
					nodesInGrp.insert(nodeID); // (2014.08.29) �ش� �׷��� ��� ���� ����
				}
			}			
		}
	}  // for �� ���� - ��ĵ �Ϸ� - �ش� ���� �׷��� ���� �߽ɰ� ��� ���� ��� �Ϸ�

	
	if (numNodes ==0)
	{
		return 0;
		
		//cout << "\n numNodes == 0" << endl;
		//this->printIndividual( indi);
		//this->veriIndividual( indi, this->popGeneTypes[individual]);
	}

	//if (numNodes !=0)
	{
		avg_X /= numNodes;
		avg_Y /= numNodes;	
	}

	//* 2014.09.01 �� �Լ� ����

	double totalArea = FIELD_WIDTH * FIELD_HEIGHT; // �� ������ ���� ����
	double expectedArea = 0;// ��Ʈ��ũ�� �� ������ �ش� level�� �׷��� ������ �յ��ϰ� �������� ���� �� ������ ��� ����
	double radius = 0; // expectedArea�� ������ �������� ���� ������

	int numGroups = kmr->getNumNodesAtLevel(level); // �ش� level�� �׷��� ���� ȹ��
	int numNodesWithinArea=0; // �ش� �׷쿡 ���� ���� ��, �����߽����κ��� radius ���� ��ġ�� ������ ����


	if (kNode->isCluster == true)
	{
		radius = TRANS_RANGE;
	}
	else
	{
		expectedArea = totalArea / numGroups;
		radius = sqrt((expectedArea/PI));
	}
	// �ش� ���� �׷��� ���� �� ���� �߽����κ��� radius ���� ��ġ�ϴ� ������ �� ���

	set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // // �ش� ���� �׷��� ���� �� ���� �߽����κ��� radius ���� ��ġ�ϴ� ������ �� ���
	{
		int nodeID = *it_node;
		
		double distance = sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
		//double distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

		if (distance < radius)
		{
			numNodesWithinArea++;
		} 

	} // for�� �Ϸ� - �����߽����κ��� radius ���� ��ġ�ϴ� ������ �� ���

	res = numNodesWithinArea;

	return res;

	//* 2014.09.01 �� �Լ� ���� �Ϸ�

	// �ش� ���� �׷��� ������ �����߽��� �������� �� �������� �Ÿ��� ��� ���
	/*
	set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // �ش� ���� �׷��� ������ ���� �߽����κ����� ��� �Ÿ� ���
	{
		int nodeID = *it_node;
		avg_dist +=  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

	} // for�� �Ϸ�- �ش� ���� �׷��� ������ ���� �߽����κ����� ��� �Ÿ� ���

	if (numNodes !=0)
	{
		avg_dist /= numNodes; 
	}

	// ���� �߽����κ��� �Ÿ��� ǥ������ ���

	double diff = 0; // ���� �߽����κ����� �Ÿ��� ���� (��� �Ÿ� ����, ���밪)
	double min_diff = -1; // ������ �ּҰ�

	double variance = 0; // ���� �߽����κ����� �Ÿ��� �л�
	double std_deviation = 0; // ���� �߽����κ����� �Ÿ��� ǥ������

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++)
	{
		int nodeID = *it_node;

		double distance;
			
		distance =  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));	

		diff = abs (distance -  avg_dist) ;			

		variance += pow ( diff   , 2); // ������ ������ ����

	} // 

	variance /= numNodes; // ������ ������ ���

	std_deviation = sqrt (variance); // ǥ�� ���� ���	

	//res = avg_dist + std_deviation; // (2014.06.18) �� ���� ����
	//res = std_deviation;
	res = avg_dist;

	return res;
	*/
}

double GA::testF(ofstream & out, int level, int index) // (2014.07.04 ����)
{
	set<int> nodesInGrp; // �־��� ��ü���� (level, index)�� �ش��ϴ� �׷� �Ҽ� ������ ID ����

	double res = 0;

	Chromosome indi = population[this->bestIndi];

	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	int groupPos = 0; // ����ü �󿡼� �ش� ���� �׷��� ���� ��ġ
 
	int numNodes = 0; // �ش� ���� �׷쿡 ���� ������ ���� 
	double avg_X = 0; //				"			x ��ǥ�� ���
	double avg_Y = 0; //				"			y ��ǥ�� ���
	double avg_dist = 0; // ���� �߽����κ��� ��������� �Ÿ��� ���

	unsigned int i = 0;

	int CHID = -1; // CH�� ID
	double minDist = -1; // �����߽ɿ��� �������� �ּ� �Ÿ�

	int nodeID;
	double dist;

	// �ش� ���� �׷��� �����߽� (COG) ���, ��� ���� ���

	struct _tagKNode* kNode = NULL;

	out << "level: " << level << ", index = " << index;
	out << " { ";


	for ( i = 0; i < indi.size(); i++) // �ؿ� �ش��ϴ� ��ü ��ĵ
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // �־��� level�� index�� �����Ǵ� �׷� ������ �����ϴ� ������ �߰�
			{
				kNode = kmr->getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			}
			else if ( indi[i].first == level && indi[i].second > index) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}
			else if (kNode != NULL && indi[i].first < level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			//else if ( indi[i].first > level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}		

		} // ������ Ÿ���� 'G'�� ��� ó��

		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  
		{
			if( kNode->level == level && kNode->index == index) // �ش� ������ �׷� ���� ��� ������ �����ϴ� ������ �߰�		
			{
				int nodeID; 
				nodeID = indi[i].second;

				if (nodeID >=0)
				{
					numNodes++;
					avg_X += fullSetNodes[nodeID]->x;
					avg_Y += fullSetNodes[nodeID]->y;	
					//avg_X += nodes[nodeID]->x;
					//avg_Y += nodes[nodeID]->y;	
					nodesInGrp.insert(nodeID); // (2014.08.29) ��� ID �׷쿡 id �߰�
				}
				out << nodeID << "(" << fullSetNodes[nodeID]->x << ", " << fullSetNodes[nodeID]->y << ") " ;
				//out << nodeID << "(" << nodes[nodeID]->x << ", " << nodes[nodeID]->y << ") " ;

			}
		}
	} // for �� ���� - ��ĵ �Ϸ� - �ش� ���� �׷��� ���� �߽ɰ� ��� ���� ��� �Ϸ�

	out << " } " ;

	if (numNodes ==0)
	{
		out << endl;
		return 0;		
	}	
	
	avg_X /= numNodes;
	avg_Y /= numNodes;		
		
	//kNode = kmr->getKNode(level, index);

	kNode->COG_X = avg_X; // (2014.06.19) �׷��� �����߽� ������ �ش� �׷�� ������ kNode�� ���� 
	kNode->COG_Y = avg_Y;

	// �ش� ���� �׷��� ������ �����߽��� �������� �� �������� �Ÿ��� ��� ���

	set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // �ش� ���� �׷��� ������ ���� �߽����κ����� ��� �Ÿ� ���
	{
		int nodeID = *it_node;
		avg_dist +=  sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
		//avg_dist +=  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

	} // for�� �Ϸ�- �ش� ���� �׷��� ������ ���� �߽����κ����� ��� �Ÿ� ���

	if (numNodes !=0)
	{
		avg_dist /= numNodes; // ������ �����߽����κ����� ��� �Ÿ� ���
	}

	/// (2014.09.02) �ش� ���� �׷��� Ŭ�������� ��� CH ���

	if (kNode->isCluster == true) // ���� �׷��� Ŭ�������� ���, CH ���
	{


		 for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // �ش� �׷�(Ŭ������)�� ��� ��� ��ȸ
		 {
			 nodeID = *it_node;
			 dist = sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
			 //dist = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

			 if ( ( minDist < 0) || ( dist < minDist ) ) // ��忡�� �����߽ɱ����� �ּ� �Ÿ��� CH ID ����
			 {
				 minDist = dist;
				 CHID = nodeID;
			 }
		 } // �ش� �׷��� ��� ��� ��ȸ - CH ���

		 if (minDist > -1) // kNode�� CH ID ����
		 {
			 kNode->CHID = CHID;
		 }
		 else
		 {
			 cout << "CHID was not set" << endl;
		 }

	} // if�� - ���� Ŭ�������� CH ��� 

	///


	// ���� �߽����κ��� �Ÿ��� ǥ������ ���

	double diff = 0; // ���� �߽����κ����� �Ÿ��� ���� (��� �Ÿ� ����, ���밪)
	double min_diff = -1; // ������ �ּҰ�

	double variance = 0; // ���� �߽����κ����� �Ÿ��� �л�
	double std_deviation = 0; // ���� �߽����κ����� �Ÿ��� ǥ������

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++)
	{
		int nodeID = *it_node;

		double distance;
			
		distance =  sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));	
		//distance =  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));	

		diff = abs (distance -  avg_dist) ;			

		variance += pow ( diff   , 2); // ������ ������ ����

	} // 

	variance /= numNodes; // ������ ������ ���

	std_deviation = sqrt (variance); // ǥ�� ���� ���	

	res = avg_dist + std_deviation; // (2014.06.18) �� ���� ����

	out << ", COG: ( " << kNode->COG_X << ", " << kNode->COG_Y << "), avg_dist: " << avg_dist << ", std_deviation: " << std_deviation << endl;
	

	if ( CHID > -1)
	{
		out << "CH ID = " << CHID << endl;
	}

	//* 2014.09.01 �� �Լ� ����

	double totalArea = FIELD_WIDTH * FIELD_HEIGHT; // �� ������ ���� ����
	double expectedArea = 0;// ��Ʈ��ũ�� �� ������ �ش� level�� �׷��� ������ �յ��ϰ� �������� ���� �� ������ ��� ����
	double radius = 0; // expectedArea�� ������ �������� ���� ������

	int numGroups = kmr->getNumNodesAtLevel(level); // �ش� level�� �׷��� ���� ȹ��
	int numNodesWithinArea=0; // �ش� �׷쿡 ���� ���� ��, �����߽����κ��� radius ���� ��ġ�� ������ ����

	expectedArea = totalArea / numGroups;

	radius = sqrt((expectedArea/PI));

	// �ش� ���� �׷��� ���� �� ���� �߽����κ��� radius ���� ��ġ�ϴ� ������ �� ���

	//set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // // �ش� ���� �׷��� ���� �� ���� �߽����κ��� radius ���� ��ġ�ϴ� ������ �� ���
	{
		int nodeID = *it_node;
		double distance = sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
		//double distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

		if (distance < radius)
		{
			numNodesWithinArea++;
		} 

	} // for�� �Ϸ� - �����߽����κ��� radius ���� ��ġ�ϴ� ������ �� ���

	out << ",  numNodesWithinArea: " << numNodesWithinArea << endl;
	//* 2014.09.01 �� �Լ� ���� �Ϸ�

	return res;

	/*
	double res = 0;

	Chromosome indi = population[this->bestIndi];

	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	int groupPos = 0; // ����ü �󿡼� �ش� ���� �׷��� ���� ��ġ
 
	int numNodes = 0; // �ش� ���� �׷쿡 ���� ������ ���� 
	double avg_X = 0; //				"			x ��ǥ�� ���
	double avg_Y = 0; //				"			y ��ǥ�� ���
	double avg_dist = 0; // ���� �߽����κ��� ��������� �Ÿ��� ���

	unsigned int i = 0;

	groupPos = getStartingPos (level, index);

	out << "level: " << level << ", index = " << index;
	out << " { ";

	// �ش� ���� �׷��� �����߽� (COG) ���, ��� ���� ���
	
	struct _tagKNode* kNode = NULL;


	for ( i = 0; i < indi.size(); i++) // �ؿ� �ش��ϴ� ��ü ��ĵ
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // �־��� level�� index�� �����Ǵ� �׷� ������ �����ϴ� ������ �߰�
			{
				kNode = kmr->getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			}
			else if ( indi[i].first == level && indi[i].second > index) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}
			else if ( indi[i].first > level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}		

		} // ������ Ÿ���� 'G'�� ��� ó��

		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  
		{
			if( kNode->level == level && kNode->index == index) // �ش� ������ �׷� ���� ��� ������ �����ϴ� ������ �߰�		
			{
				int nodeID; 
				nodeID = indi[i].second;

				if (nodeID >=0)
				{
					numNodes++;
					avg_X += nodes[nodeID]->x;
					avg_Y += nodes[nodeID]->y;		
				}

				out << nodeID << "(" << nodes[nodeID]->x << ", " << nodes[nodeID]->y << ") " ;

			}
		}
	} // for �� ���� - ��ĵ �Ϸ� - �ش� ���� �׷��� ���� �߽ɰ� ��� ���� ��� �Ϸ�

	

	out << " } " ;

	if (numNodes ==0)
	{
		out << endl;
		return 0;
		
		//cout << "\n numNodes == 0" << endl;
		//this->printIndividual( indi);
		//this->veriIndividual( indi, this->popGeneTypes[individual]);
	}

	//if (numNodes !=0)
	
	avg_X /= numNodes;
	avg_Y /= numNodes;	

	//struct _tagKNode* kNode;
		
	kNode = kmr->getKNode(level, index);

	kNode->COG_X = avg_X; // (2014.06.19) �׷��� �����߽� ������ �ش� �׷�� ������ kNode�� ���� 
	kNode->COG_Y = avg_Y;
	

	kNode = NULL; // (2014.08.27) kNode ����

	// �ش� ���� �׷��� ������ �����߽��� �������� �� �������� �Ÿ��� ��� ���

	for ( i = 0; i < indi.size(); i++) 
	//for ( i = groupPos; i < indi.size(); i++) 
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // �־��� level�� index�� �����Ǵ� �׷� ������ �����ϴ� ������ �߰�
			{
				kNode = kmr->getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			}
			else if ( indi[i].first == level && indi[i].second > index) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}
			else if ( indi[i].first > level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}		

		} // ������ Ÿ���� 'G'�� ��� ó��
		
		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  	
		{

			if( kNode->level == level && kNode->index == index) // �ش� ������ �׷� ���� ��� ������ �����ϴ� ������ �߰�		
			{
				int nodeID;
			
				nodeID= indi[i].second;
			
				avg_dist +=  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));
			}
		}		
	}

	//if (numNodes !=0)
	{
		avg_dist /= numNodes; 
	}

//	res = avg_dist;

	// �ش� ���� �׷��� ������ �����߽����κ����� �Ÿ��� ǥ�� ���� ���

	double diff = 0; // ���� �߽����κ����� �Ÿ��� ���� (��� �Ÿ� ����, ���밪)
	double min_diff = -1; // ������ �ּҰ�
	//int idx_CH; // �����߽����κ��� ���� ����� ��忡 �����Ǵ� unode�� key tree������ �ε��� (0, 1, 2, ...) (CH)
	//int CH_ID = 0; // CH�� id

	double variance = 0; // ���� �߽����κ����� �Ÿ��� �л�
	double std_deviation = 0; // ���� �߽����κ����� �Ÿ��� ǥ������

	kNode = NULL;

	for ( i = 0; i < indi.size(); i++) 
	//for ( i = groupPos; i < indi.size(); i++) 
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // �־��� level�� index�� �����Ǵ� �׷� ������ �����ϴ� ������ �߰�
			{
				kNode = kmr->getKNode(level, index); // �ش� ������ �׷쿡 �����Ǵ� kNode�� �̵�
			}
			else if ( indi[i].first == level && indi[i].second > index) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}
			else if ( indi[i].first > level) // �ش� �׷쿡 ���� ���� ��ĵ �Ϸ�
			{
				break;
			}		

		} // ������ Ÿ���� 'G'�� ��� ó��

		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  	
		{

			if( kNode->level == level && kNode->index == index) // �ش� ������ �׷� ���� ��� ������ �����ϴ� ������ �߰�		
			{
		
				int nodeID;
			
				nodeID = indi[i].second;
			
				double distance;
			
				distance =  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));	

				diff = abs (distance -  avg_dist) ;			

				variance += pow ( diff   , 2); // ������ ������ ����

			}
		}		
	}

	variance /= numNodes; // ������ ������ ���

	std_deviation = sqrt (variance); // ǥ�� ���� ���

	//res = std_deviation;

	out << ", COG: ( " << kNode->COG_X << ", " << kNode->COG_Y << "), avg_dist: " << avg_dist << ", std_deviation: " << std_deviation << endl;

	res = avg_dist + std_deviation; // (2014.06.18) �� ���� ����

	
	return res;
	*/
}

int GA::getStartingPos(int level, int index)
{
	int startingPos = 0;

	if (level == 0)
	{
		return 0;
	}

	int degree = kmr->getNumNodesAtLevel(level) / kmr->getNumNodesAtLevel(level-1); // ���� ������ degree ����

	int parentLevelIndex = index/ degree; // ���� ���� �׷��� index ����

	int indexInParentGroup = index % degree; // ���� ���� �׷� �������� index ����

	// ���� �׷쿡 ���� ������ ���� ����

	int numNodesInCurLevel = kmr->getNumNodesAtLevel(level);// ���� level�� ���� ��� ��
	int numGenesInGroup = 0; // ���� �� ���� ������ ����

	for (int i = level; i < kmr->getKTreeHeight(); i++)
	{			
		int numNodesAtLevel = kmr->getNumNodesAtLevel(i); // level (level~ h-1) ���� ��� ��
		numGenesInGroup += numNodesAtLevel/numNodesInCurLevel; // �� level�� ��� ���� L1�� ��� ���� ���� ����� ����
	}

	int parentGroupPos = getStartingPos(level-1, parentLevelIndex); // ���� ���� �׷��� ���� ��ġ ��ȯ

	startingPos = parentGroupPos + 1 + indexInParentGroup * numGenesInGroup;

	return startingPos;
	

}

void GA::select() // (2014.06.17 ����) �򰡰���� ����� ��ü�� ���� - tournament selection ���
{
	int match = 0; // �� ���

	//map<int, pair<int, int>> t_population[Population_Size];// �ӽ� ���� (������ ���� ���)
	/*
	for (int i = 0; i < Population_Size; i++) // ������ ���� ���� ���� ������ ����
	{
		//t_population[i] = temp_population[i];
		this->veriIndividual(temp_population[i], this->tPopGeneTypes[i]);
	}
	*/
	// (2014.05.06) ������� �̻� ����!
	
	//for (int i = 0; i < Population_Size /2 ; i++) // (2014.06.17 ����) ������� ������ ���� ���븦 �����ϴ� ��ü �� ���ݸ� ���� ���뿡�� �����ϰ� �������� ���� ����
	for (int i = 0; i < Population_Size; i++) // �� ������ i ��° ��ü�� ���Ƿ� ���õ� ��ü�� ���Ͽ� �� ����� ��ü�� ����
	{											// �� ����� ���� ��ü�� �� ����� (��� �Ÿ�)

		if ( i==bestIndi || rand()%4  > 0) // 3/4�� Ȯ���� ���� ���뿡�� ���õǵ��� �� 
		//if ( i==bestIndi || rand()%2  > 0) // (2014.06.18)  4/5�� Ȯ���� ���� ���뿡�� ���õǵ��� �� - ���� ���뿡�� �ְ��� ��ü�̾��� ��� ���ܵ�
		{
			temp_population[i].clear();

			//this->temp_population[i] = this->population[i];
			//this->temp_population[i] = this->population[(i+1)%Population_Size];		

			match = gen2.genRandInt(Population_Size);

			bool doMutation = false; 

			int r = 0;

			r = gen5.genRandInt(100);

			if ( r < Pm *100) // Ȯ�������� ���� (mutation)
			{
				doMutation = true;
			}

			if (fitness[i] > fitness[match]) // ���Լ� ���� ���� ���� ��ȣ��
			//if (fitness[i] < fitness[match]) // 
			{
				//printIndividual(population[i]);
				//veriIndividual(population[i], this->popGeneTypes[i]);

				
				if (doMutation == false)
				{
					this->temp_population[i] = population[i];
					this->tPopGeneTypes[i] = this->popGeneTypes[i];
				}
				else
				{
					this->temp_population[i] = population[match];
					this->tPopGeneTypes[i] = this->popGeneTypes[match];
				}
			}
			else
			{
				//printIndividual(population[match]);
				//veriIndividual(population[match], this->popGeneTypes[match]); // population[match] �̻� ����
				//veriIndividual(population[match], geneTypes[match]); // population[match] �̻� ����
			
				if (doMutation == false)
				{

					this->temp_population[i] = population[match];
					this->tPopGeneTypes[i] = this->popGeneTypes[match];
				}
				else
				{
					this->temp_population[i] = population[i];
					this->tPopGeneTypes[i] = this->popGeneTypes[i];
				}
			}		
		
			//this->printIndividual(temp_population[i]);
			//this->veriIndividual(temp_population[i], this->popGeneTypes[i]);
			//this->veriIndividual(temp_population[i], geneTypes[i]);
		}
		else
		{
			geneIdx = 0;

			population[i].clear();
			temp_population[i].clear();
			popGeneTypes[i].clear();
			tPopGeneTypes[i].clear();

			for (int j = 0; j < this->indiLen; j++) 
			{
				Gene gene = std::make_pair(-1, -1); // �������� ù ��°�� �� ��° ���� -1�� �ʱ�ȭ 

				population[i].insert( make_pair (j, gene)); // �� ������ �ش� ��ü�� �� �������� ���� �ʱ�ȭ
			
				temp_population[i].insert( make_pair (j, gene)); // �ӽ� ������ �ش� ��ü�� �������� ���� 0���� �ʱ�ȭ

				this->popGeneTypes[i].insert(std::pair<int, char> (j, '\0' ));			
			}	

			this->writeGroupIDs(root, i, geneIdx);// �׷� id�� �����Ǵ� ������ ��� (key tree parsing)	
			this->writeNodeIDs(i);// i��° individual�� ��� id�� �����Ǵ� ������ ��� (random)

			temp_population[i] = population[i];
			this->tPopGeneTypes[i] = this->popGeneTypes[i];		
		}
		
	}

	// (2014.06.17) �� ������ ���ݿ� �ش��ϴ� ��ü�� ���� ����
	//for (int i = Population_Size /2; i < Population_Size; i++)
	
}

void GA::crossover() // ���õ� ��ü���� ����
{
	int cp1 = 0;
	int cp2 = 0;

	int rand_index = 0;
	
	int geneIdx_offspring1 = 0; // offspring1�� �����ڸ� ����ϱ� ���� �ε��� (������ ��ϵ� ��ġ)
	int geneIdx_offspring2 = 0; //  offspring2�� �����ڸ� ����ϱ� ���� �ε��� (������ ��ϵ� ��ġ)

	// crossing point ���� - root level+1�� �׷� �� �ϳ��� ���Ƿ� �����Ͽ� �׷��� ���� ������ cp1, �� ������ cp2�� ����
	
	//int numNodesInL1 = kmr->getNumNodesAtLevel(root->level+1);
	
	// 1) subtree�� root�� �������� level 1�� kNode �� ����

	int numNodesInL1 = 0; // subtree�� root�� �������� level 1�� �����ϴ� kNode�� ����

	struct _tagKNode* tempKNode = NULL;

	tempKNode = root->leftChild;

	while (tempKNode !=NULL)
	{
		numNodesInL1++; // root level + 1�� �����ϴ� kNode�� ���� ���
		tempKNode = tempKNode->rightSibling;
	}

	// 2) root level + 1 �� �����ϴ� �׷� �� �ϳ��� �����ϰ� ����

	//int numNodesInL1 = kmr->getNumNodesAtLevel(1);
	rand_index = gen3.genRandInt(numNodesInL1);	


	// 3)  cp1, cp2 ���

	if (root->isCluster == true)
	{
		cp1=1;
		cp2= cp1+ numNodesInL1;
	}
	else
	{
		int group_i = 0;
		tempKNode = root->leftChild;

		cp1= 1; // root ����� ������ ����

		while ( tempKNode != NULL)
		{
			if (group_i++ >= rand_index) // group_i�� ���õ� �׷��� �ε����� �����ϸ� �ݺ��� Ż�� - ���� �׷��� kNOde �� ����
			{
				cp2 = cp1 + kmr->getNumKNodes(tempKNode); // ���õ� �׷��� kNode ������ cp2�� ����
				break;
			}

			cp1 += kmr->getNumKNodes(tempKNode); // group_i ��° �׷��� kNode �� ��� - cp1 ����

			tempKNode = tempKNode->rightSibling;
		
		} // ���õ� �׷��� ���� �׷���� kNode ���� ���� -> cp1 ���	
	}

	/*
	int numGenesInBB = 0; // building block�� ���� ������ ����

	
	for (int i = root->level+1; i < kmr->getKTreeHeight(); i++)
	//for (int i = 1; i < kmr->getKTreeHeight(); i++)
	{			
		int numNodesAtLevel = kmr->getNumNodesAtLevel(i); // level (1~ h-1) ���� ��� ��
		numGenesInBB += numNodesAtLevel/numNodesInL1; // �� level�� ��� ���� L1�� ��� ���� ���� ����� ����
	}
	

	cp1 = 1+ rand_index * numGenesInBB;
	cp2 = cp1 + numGenesInBB;
	*/

	// parent 1, 2 ����

	for (int i = 0; i < Population_Size-1; i+=2) // crossover �߻� Ȯ���� 1�̶�� ����
	{
		int parent1 = i;
		int parent2 = i+1;

		//if (parent1==34)
		//{
		//	//cout << "parent1==34" << endl;
		//}


		Chromosome p1 = temp_population[parent1];
		Chromosome p2 = temp_population[parent2];

		//this->printIndividual(p1);
		//this->printIndividual(p2);

		//this->veriIndividual(p1, this->popGeneTypes[parent1]);
		//this->veriIndividual(p2, this->popGeneTypes[parent2]);

		//this->veriIndividual(p1, geneTypes[parent1]);
		//this->veriIndividual(p2, geneTypes[parent2]);
		//this->veriIndividual(parent1);
		//this->veriIndividual(parent2);

		unsigned int j = 0;

		

		// cp1, cp2 �߰�
		/*
		for (int g = 0; g < temp_population[parent1].size(); g++) 
		{
			cp2 = g;

			if (geneTypes[parent1][g] == 'G' && temp_population[parent1][g].first == 1 )
			{
				if ( temp_population[parent1][g].second == rand_index) // index�� ������ �����ϸ� cp1�� �� ����
				{
					cp1 = g;					
				}
				else if (temp_population[parent1][g].second > rand_index) // index�� �������� ũ�� loop Ż��
				{
					break;
				}				
			}				
		}

		if ( cp2 == temp_population[parent1].size()-1) // cp1 ~ (cp2-1) ���� �ʱ�ȭ�� �Ͼ�Ƿ�, cp2�� ��ü���� ���� 1 ���� ��� +1�������� ��
		{
			cp2 = temp_population[parent1].size();
		}
		*/

		// cp1, cp2 �߰� �Ϸ�

		// cp1~cp2 ������ ������ �����ϴ� ������ �ʱ�ȭ
		for (unsigned int g = (unsigned int) cp1; g < (unsigned int) cp2; g++)
		{
			if (this->tPopGeneTypes[parent1][g] == 'N')
			//if (geneTypes[parent1][g] == 'N')
			{
				temp_population[parent1][g] = std::make_pair (-1, -1); // BB ���� ���� gene �� ��忡 �ش��ϴ� gene�� �� ���ʱ�ȭ
				
			}

			if (this->tPopGeneTypes[parent2][g] == 'N')			
			{
				temp_population[parent2][g] = std::make_pair (-1, -1); // BB ���� ���� gene �� ��忡 �ش��ϴ� gene�� �� ���ʱ�ȭ
				
			}
		}			
		

		// offspring 1 ����
		{
			// parent 1�� ���� �������� ���õ� �׷� ���������� �״�� ���� (�̹� parent1�� �����ڰ� ����Ǿ� ����)

			//if (rand_index == numNodes-1);
				//cp2 = population[parent1].size();
			/*
			for (j = 0; j < temp_population[parent1].size(); j++) // cp1, cp2 �߰�
			{
				cp2 = j;

				if (geneTypes[parent1][j] == 'G' && temp_population[parent1][j].first == 1 )
				{
					if ( temp_population[parent1][j].second == rand_index) // index�� ������ �����ϸ� cp1�� �� ����
					{
						cp1 = j;					
					}
					else if (temp_population[parent1][j].second > rand_index) // index�� �������� ũ�� loop Ż��
					{
						break;
					}				
				}				
			}

			for (j = (unsigned int) cp1; j < (unsigned int) cp2; j++)
			{
				if (geneTypes[parent1][j] == 'N')
				{
					temp_population[parent1][j] = std::make_pair (-1, -1); // BB ���� ���� gene �� ��忡 �ش��ϴ� gene�� �� ���ʱ�ȭ
				}
			}			
			*/
			// parent2�� ���ʺ��� ��ĵ - ������ offspring1�� ������� ���� �������̸� offspring1�� cp1~cp2 ������ �� ���� (geneType = 'N') �� ����

			geneIdx_offspring1 = cp1; // offspring1�� ��ϵ� ��ġ �ʱ�ȭ

			for (j=0; j < p2.size(); j++)
			//for (j=0; j < temp_population[parent2].size(); j++)
			//for (j=0; j < population[parent2].size(); j++)
			{
				if ( ( this->tPopGeneTypes[parent2][j] == 'N') && (p2[j].first != -1) ) // ������ Ÿ���� ��������̰� ��� ���� ���� ���
				//if ( ( geneTypes[parent2][j] == 'N') && (p2[j].first != -1) ) // ������ Ÿ���� ��������̰� ��� ���� ���� ���
				//if ( geneTypes[parent2][j] == 'N')// ������ Ÿ���� ��������� ���
				{
					
					bool exist = false;

					// ������ ����� ���������� �˻�

					for (unsigned int k = 0; k < population[parent1].size(); k++)
					{
						if ( this->tPopGeneTypes[parent1][k] =='N')  
						//if ( geneTypes[parent1][k] =='N')  
						{
							if (temp_population[parent1][k].first == p2[j].first)
							//if (temp_population[parent1][k].first == temp_population[parent2][j].first)
							{
								if (temp_population[parent1][k].second == p2[j].second)
								//if (temp_population[parent1][k].second == temp_population[parent2][j].second)
								{
									exist = true;
									break;
								}
							}
						}

						if (k == cp1) // cp1~cp2 ������ ������ �˻��� �ʿ� ���� (parent2�� �����ڸ� ä������, �� �߿��� �ߺ��Ǵ� ���� �����Ƿ�)
						{
							k = cp2;
						}

					} // �˻� ����

					if (exist == false) // ������ ������� ���� �������̸� offspring1�� cp1~cp2 ������ �� ���Կ� ���� �� ����
					{
						while ( (geneIdx_offspring1 >= cp1) && (geneIdx_offspring1 < cp2) )
						{
							if (this->tPopGeneTypes[parent1][geneIdx_offspring1] == 'N')
							//if (geneTypes[parent1][geneIdx_offspring1] == 'N')
							{
								temp_population[parent1][geneIdx_offspring1] = p2[j];
								//temp_population[parent1][geneIdx_offspring1] = temp_population[parent2][j];
								geneIdx_offspring1++; // ������ ��ϵ� ��ġ ����
								break;
							}		

							geneIdx_offspring1++; // ������ ��ϵ� ��ġ ����

							
						}
					} // offspring1�� parent2�� ������ ����
				}
			} // parent2 ��ĵ �Ϸ�
		} // offspring 1 ���� �Ϸ�


		// offspring 2 ����
		{
			// parent 2�� ���� �������� ���õ� �׷� ���������� �״�� ���� (�̹� parent2�� �����ڰ� ����Ǿ� ����)
			
			/*
			for (j = cp1; j < cp2; j++)
			{
				if (geneTypes[parent2][j] == 'N')
				{
					temp_population[parent2][j] = std::make_pair (-1, -1); // BB ���� ���� gene �� ��忡 �ش��ϴ� gene�� �� ���ʱ�ȭ
				}
			}	
			*/

			// parent1�� ���ʺ��� ��ĵ - ������ offspring2�� ������� ���� �������̸� offspring2�� cp1~cp2 ������ �� ���� (geneType = 'N') �� ����

			geneIdx_offspring2 = cp1; // offspring2�� ��ϵ� ��ġ �ʱ�ȭ

			for (j=0; j < p1.size(); j++)
			//for (j=0; j < temp_population[parent1].size(); j++)
			//for (j=0; j < population[parent1].size(); j++)
			{

				if ( (this->tPopGeneTypes[parent1][j] == 'N') && (p1[j].first != -1))
				//if ( geneTypes[parent1][j] == 'N') // ������ Ÿ���� ��������� ���
				{
					bool exist = false;

					// ������ ����� ���������� �˻�

					for (unsigned int k = 0; k < population[parent2].size(); k++)
					{
						if ( this->tPopGeneTypes[parent2][k] =='N')  
						//if ( geneTypes[parent2][k] =='N')  
						{
							if (temp_population[parent2][k].first == p1[j].first)
							//if (temp_population[parent2][k].first == temp_population[parent1][j].first)
							{
								if (temp_population[parent2][k].second == p1[j].second)
								//if (temp_population[parent2][k].second == temp_population[parent1][j].second)
								{
									exist = true;
								}
							}
						}

						if (k == cp1) // cp1~cp2 ������ ������ �˻��� �ʿ� ���� (parent1�� �����ڸ� ä������, �� �߿��� �ߺ��Ǵ� ���� �����Ƿ�)
						{
							k = cp2;
						}


					} // �˻� ����

					if (exist == false) // ������ ������� ���� �������̸� offspring1�� cp1~cp2 ������ �� ���Կ� ���� �� ����
					{
						while ( (geneIdx_offspring2 >= cp1) && (geneIdx_offspring2 < cp2) )
						{
							if (this->tPopGeneTypes[parent2][geneIdx_offspring2] == 'N')
							//if (geneTypes[parent2][geneIdx_offspring2] == 'N')
							{
								temp_population[parent2][geneIdx_offspring2] = p1[j];
								//temp_population[parent2][geneIdx_offspring2] = temp_population[parent1][j];
								geneIdx_offspring2++; // ������ ��ϵ� ��ġ ����
								break;
							}		

							geneIdx_offspring2++; // ������ ��ϵ� ��ġ ����
						}
					} // offspring2�� parent1�� ������ ����
				}
			} // parent1 ��ĵ �Ϸ�

		} // offspring 2 ���� �Ϸ�

		this->veriIndividual(temp_population[parent1], this->tPopGeneTypes[parent1]);
		this->veriIndividual(temp_population[parent2], this->tPopGeneTypes[parent2]);

		//this->veriIndividual(temp_population[parent1], geneTypes[parent1]);
		//this->veriIndividual(temp_population[parent2]);

		//this->veriIndividual(parent1);
		//this->veriIndividual(parent2);

		//this->printIndividual(temp_population[parent1]);
		//this->printIndividual(temp_population[parent2]);

	}
	
}

void GA::mutate() // GA-mutate() �Լ� - mutation point ������ ���
{
	for (int i = 0; i < Population_Size; i++)
	{
		int r = 0;

		r = gen5.genRandInt(100);

		if ( r < Pm *100) // Ȯ�������� ���� (mutation)
		{
				// ���� ��� ID�� �����Ǵ� ������ ���� p1, p2�� ���ϰ� �� ������ ������ ���� ��ȯ (swapping)

			//this->printIndividual(temp_population[i]);

			// test ��
			//if (i==34)
			//{
			//	;
				//cout << "i==34";
			//}
			int p1 = 0;
			int p2 = 0;

			int r1 = 0; // p1, p2�� ���ϱ� ���� ����
			int r2 = 0; 

			//int seekPoint = 0; // i��° ��ü�� ���� Ž�� ������ ��ġ

			int numNTypeGenes = 0; // Ž�� �������� ���� �� gene (type == 'N')�� ����

			r1 = gen4.genRandInt(nodes.size()-1) +1; // �ּ��� 1 �̻��� ���� ������ ��
			r2 = gen4.genRandInt(nodes.size()-1) +1;

			//r1 = gen4.genRandInt(nodes.size());
			//r2 = gen4.genRandInt(nodes.size());


			//r1 = gen4.genRandInt(this->indiLen);
			//r2 = gen4.genRandInt(this->indiLen);

			for (int j = 0; j < indiLen; j++)
			{
				if ( this->tPopGeneTypes[i][j] == 'N')

				//if ( geneTypes[i][j] == 'N')
				{
					numNTypeGenes++;
			
					if (numNTypeGenes == r1)
					{
						p1 = j;
					}
					if (numNTypeGenes == r2)
					{
						p2 = j;
					}
				}
			} // p1, p2 ����

			// p1, p2��ġ�� ������ �� ��ȯ (swapping)
			Gene temp = std::make_pair (0, 0);
			temp = temp_population[i][p1];
			temp_population[i][p1] = temp_population[i][p2];
			temp_population[i][p2] = temp;

			//veriIndividual(this->temp_population[i], this->tPopGeneTypes[i]); // ���� ��ü ���� - mutate �Ŀ��� ��� ����� iD�� �����ϴ� �� ����
		}		
		
	}
}

/*
GA::GA(int indiLen)
{
	this->indiLen = indiLen;
}
*/

void GA::writeGroupIDs(KNode* currentNode, int individual, int gene) // (2014.06.16 ����) key tree�� ��ȸ�ϸ� �׷� id�� �о� ��ü�� �����ڿ� ���
{
	if (currentNode ==NULL)
	{
		return;
	}	

	if (currentNode->leftChild!=NULL) // // ���� ��尡 k-node�̸� �ش� �׷� id�� ����ü�� ���
	{
		this->population[individual][gene].first = currentNode->level;
		this->population[individual][gene].second = currentNode->index;		

		this->popGeneTypes[individual][gene] = 'G'; // genetype = Group
		//this->geneTypes[individual][gene] = 'G'; // genetype = Group
	
		geneIdx++; // ������ ��ϵ� ����ü�� ��ġ�� ����

		writeGroupIDs (currentNode->leftChild, individual, geneIdx); // ���� ����� leftChild�� ���� ��� ȣ��

		if (currentNode->isCluster == true) // ���� ��尡 Ŭ������ Ű ������ �ش��� ���
		//if (currentNode->leftChild->leftChild ==NULL) // ���� ��尡 Ŭ������ Ű ������ �ش��� ���
		{
			int numSlots; // ���� Ŭ�����Ϳ� ���ϴ� �� ������ ���� (���Ե� �� �ִ� �ִ� ���� ��� ��)
			numSlots = 0;

			struct _tagKNode* tKNode;

			tKNode = currentNode->leftChild;

			while (tKNode != NULL)
			{
				numSlots++;
				tKNode = tKNode->rightSibling;
			}

			for (int i =0; i < numSlots; i++)
			{
				this->popGeneTypes[individual][geneIdx++] = 'N'; // genetype = Node
			}

			/*
			int degree = kmr->getNumNodesAtLevel(currentNode->level+1) / kmr->getNumNodesAtLevel(currentNode->level);

			for (int i = 0; i < degree; i++)
			{
				this->popGeneTypes[individual][geneIdx++] = 'N'; // genetype = Node

			}
			*/
			//geneIdx += degree; // ���� level ��� �� ��ŭ geneIdx ���� (�� ���� Ȯ��)
			//geneIdx += MAX_NODES_PER_CLUSTER; // Ŭ�����ʹ� �ִ� ��� �� ��ŭ geneIdx ���� (�� ���� Ȯ��)
		}

		if ((currentNode != this->root) && (currentNode->rightSibling != NULL))
		{
			writeGroupIDs (currentNode->rightSibling, individual, geneIdx);  // ���� ����� rightSibling�� ���� ��� ȣ��
		}		
	}
	

	
}
void GA::writeNodeIDs(int individual) // (2014.06.16 ����) - ����individual�� �����ϰ� ��� id�� ä�� ����
{
	// cell id �������� �������� ���ĵ� ��� ����� ��ĵ, �����ϰ� ���õ� ���� ��ġ�������� ���ʴ�� ��� ID ����
	int kTreeHeight = kmr->getKTreeHeight(); // key tree ���� ȹ��

	map<int, Node*>::iterator it_node;

	int randPos = gen.genRandInt(this->indiLen)+1; // ����ü�� ��� id�� ����� ���� ��ġ ���� (����)
	int currentPos; // ���� ��� ID�� ����� ��ġ (randPos�������� 1�� ����)

	// ���� ���õ� ��ġ�� �����ڰ� �� ������ �ƴϰų� Node type�� �ƴϸ� ��ġ�� 1�� ����, ����ü�� ���� �����ϸ� �ٽ� ���� �������� �̵�

	/*
	while (population[individual][randPos].first != -1 || popGeneTypes[individual][randPos] != 'N') // 
	{
		randPos++;
		randPos %= indiLen;
	}
	*/

	currentPos = randPos;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // ��� id�� �ϳ��� ����ü�� ���
	{
		int id;
		//id = (*it_node).first; // ID ȹ��

		Node* node;
		node = (*it_node).second; // NODE ������ ȹ��
		id = node->id; // (2014.09.22) 

		while ( !(popGeneTypes[individual][currentPos] == 'N' && population[individual][currentPos].first == -1)) // �� ������ ���� ������ ����
		{
			currentPos++;
			currentPos %= indiLen;
		}

			this->population[individual][currentPos].first = kTreeHeight; // ���� ���Կ� ���� ����� level ���
			this->population[individual][currentPos].second = node->id;		  // ���� ���Կ� ���� ����� ID ���
			this->popGeneTypes[individual][currentPos] = 'N'; // genetype = Node					
	
	} // ����ü�� ��� ID ��� �Ϸ�


	/*
	// ��� ����� ó���������� �ϳ��� ��� ID ����, ����ü�� ������ �� ���Կ� ���

	
	map<int, Node*>::iterator it_node;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	{
		int id;
		id = (*it_node).first; // ID ȹ��

		Node* node;
		node = (*it_node).second; // NODE ������ ȹ��

		int randPos = gen.genRandInt(this->indiLen)+1; // �ش� ����� id�� ����� ��ġ ���� (����)

		// �ش� ����� id ���

		  // chromosome ��ĵ (������ ����)
		  
		int currentPos = 0;

		int numMoves = 0; // �� ���� ���� �̵��� Ƚ�� 

		while(numMoves < randPos) // �� ���� ���� �̵� Ƚ���� ������ ������ ������ �ݺ�
		{
			currentPos++;

			currentPos = currentPos % (this->indiLen);

			if (population[individual][currentPos].first == -1) // �̵��� ������ �� �����̸� �̵� Ƚ�� +1 ����
			{
				numMoves++;

				if (currentPos == 0)
					cout << "currentPos == 0" ; // do nothing

				if (numMoves >= randPos)
				{
					break;
				}
			}
		}	

		int kTreeHeight = kmr->getKTreeHeight(); // key tree ���� ȹ��
		this->population[individual][currentPos].first = kTreeHeight; // ���� ���Կ� ���� ����� level ���
		this->population[individual][currentPos].second = node->id;		  // ���� ���Կ� ���� ����� ID ���
		this->popGeneTypes[individual][currentPos] = 'N'; // genetype = Node
	}
	*/

	
}

void GA::printIndividual(Chromosome indi)
{
	ofstream out("tst_individual.txt");
	out << " individual: " ;

	for (int i = 0; i< this->indiLen; i++)
	{
		
		//cout << " individual: " ;

		int level =  indi[i].first;
		int id = indi[i].second;

		out << "(" << i << ", (" << level << ", " << id << ")) " ; // �־��� individual ��� 
		//cout << "(" << i << ", (" << level << ", " << id << ")) " << endl ; // �־��� individual ��� 
	}
	out.close();
}

Chromosome GA::findSolution(void)
{
	// findSolution ȣ�� �� �̹� individual length�� ������ �־�� �� - setIndiLen() �Լ� ȣ��

	// clear old data - population, individual, genetypes

	individual.clear();

	for (int i = 0; i < Population_Size; i++)
	{
		this->population[i].clear();
		this->temp_population[i].clear();		
		this->popGeneTypes[i].clear();
		this->tPopGeneTypes[i].clear();

		//this->geneTypes[i].clear();
	}	

	create_population(); // create initial population 

	// (2014.06.17) ������� �̻� ����!

	//this->printPopulation();
	this->veriPopulation();

	int generation = 0;
	char ans = '\0'; // ���븦 ��� ������ ������ ���θ� ����

	//while( true) // infinite loop
	while(generation < Num_Generation) // convergence test
	{
		if (root->isCluster == true) // (2014.10.08) ���� ������ ����, �� ����ȭ ������ ck node�� ���, �ݺ��� Ż��
		{
			break;
		}

		cout << "\ngen no." << generation << endl;
		//cout << " evaluate() " << endl;

		evaluate(); // evaluate the current population
		// (2014.05.06) ������� �̻� ����!

		//cout << " select() " << endl;
				

		select();// select good individuals for the next generation
				
		// (2014.06.17) ������� Ȯ��!
		//cout << " crossover() " << endl;

		crossover();// mix the selected individuals

		
		//cout << " mutate() " << endl;
		//mutate(); // (2014.05.03) select() �Լ��� ���� (����ð� ���� ����) mutate with low probability 

		
		 // move to the next generation (copy the elements of temporary population to the next population)
		for (int indi = 0; indi < Population_Size; indi++)
		{			
			population[indi].clear();
			population[indi] = temp_population[indi];			
		}		
		

		// �� ���� ����
		//this->printPopulation();
		//this->veriPopulation();
		//this->printFitness();
		/*
		if (generation % 10 ==0 )
		{
			cout << " more generation? " << endl;
			cin >> ans;

			if ( ans == 'n') // ���븦 �׸� �����Ϸ��� 'n'�Է�
			{
				break;
			}

		}
		*/
		generation++; 
	}

	
	this->veriPopulation();
	evaluate(); // evaluate the last population
	//this->printPopulation();

	// find the best individual
	/*
	bestIndi = 0;
	//int bestIndi = 0;
	double curFitness = fitness[bestIndi];
	double bestFitness = curFitness;	

	for (int i = 1; i < Population_Size; i++) 
	{
		curFitness = fitness[i];

		if (curFitness !=0 && curFitness < bestFitness)
		{
			bestIndi = i;
			bestFitness = curFitness;
		}
	}
	*/

	return population[bestIndi];	// return the best individual
}

map<int, char> GA::getSolGeneType() // (2014.04.28) ����� ���� genetype ��ȯ
{
	return this->popGeneTypes[bestIndi];
	//return this->geneTypes[bestIndi];
}

void GA::printPopulation() // �� ���� �� ��ü ���, ���/�ִ� fitness value ���
{
	//bestIndi = 0;
	double avg_fitness = 0;
	
	//double best_fitness = fitness[bestIndi];
	//double best_fitness = 0;

	ofstream out("tst_population.txt");

	for (int i = 0; i < Population_Size; i++) 
	{
		out << "individual " << i << ": ";

		for (int j = 0; j < this->indiLen; j++)
		{
			out << "( " << j << ", ";

			if (this->popGeneTypes[i][j] == 'G')
			//if (geneTypes[i][j] == 'G')
			{
				int level = population[i][j].first;
				int index = population[i][j].second;

				out << "G(" << level << ", " << index << ") )" ;
			}
			else if (this->popGeneTypes[i][j] == 'N')
			//else if (geneTypes[i][j] == 'N')
			{
				int nID = population[i][j].second;
				
				if (nID >= 0)
				{
					out << "N" << nID << "(" << fullSetNodes[nID]->x << ", " << fullSetNodes[nID]->y << ") ) ";
					//out << "N" << nID << "(" << nodes[nID]->x << ", " << nodes[nID]->y << ") ) ";
				}

				else
					out << "N" << population[i][j].second << " ) ";

				//out << "N" << population[i][j].second << " ) ";

			}
		}

		out << "\n" << endl;

		avg_fitness += fitness[i];

		/*
		if (fitness[i] < best_fitness)
		{
			best_fitness = fitness[i];
			bestIndi = i;
		}
		*/
	}	

	avg_fitness /= Population_Size;

	out << "\nbest fitness: " << fitness[bestIndi] << ", individual: " << bestIndi << endl;
	out << "avg. fitness: " << avg_fitness << endl;

	out.close();

}

void GA::veriPopulation() // �� ���� �� ��ü���� ������ ���� ������ �����ϴ� �� ����
{
	// �� ��ü�� ��� ����� ID�� �����ϴ°�?
	// ��� ����� ó���������� �ϳ��� ��� ID ����, ����ü�� ������ �� ���Կ� ���

	for (int i = 0; i < Population_Size; i++)
	{
		veriIndividual ( temp_population[i], this->tPopGeneTypes[i]);

		//veriIndividual( temp_population[i]);
		//veriIndividual(i); // i ��° ��ü ����
	}
}

void GA::veriIndividual(map<int, std::pair<int, int>> indi, map<int, char> geneTypes) // (2014.06.16 ����)
//void GA::veriIndividual(int indi)
{
	//Chromosome individual = population[indi]; // ���� ��ü ȹ��
	Chromosome individual = indi; // ���� ��ü ȹ��

	std::map <int, bool> exist; // <��� ID, ���� ���� (T/F) > - ��尡 �����ϸ� �ش� ��� ID�� �����Ǵ� bool ������ ���� true�� ����

	map<int, Node*>::iterator it_node;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	{
		int nID;
		Node* node;
		node = (*it_node).second;
		nID = node->id; // ID ȹ��	
		//nID = (*it_node).first; // ID ȹ��		

		exist[nID] = false;
	}


	/*
	for (unsigned int n = Node::numNodesEvctd ; n < Node::totNumNodes; n++) // �� ��� ID�� ���� ���θ� ��Ÿ���� bool ������ ���� false�� �ʱ�ȭ
	//for (unsigned int n = 0 ; n < nodes.size(); n++) // �� ��� ID�� ���� ���θ� ��Ÿ���� bool ������ ���� false�� �ʱ�ȭ
	{
		Node* pNode = nodes[n];

		int nID = pNode->id;

		exist[nID] = false;
	}
	*/
	for (int j = 0; j < this->indiLen; j++) // ��ü�� ���Ե� ��� ID�� �����ϴ� bool ������ ���� true�� ����
	{
		if ((geneTypes[j] == 'N') && (individual[j].first != -1))
		//if ( geneTypes[indi][j] == 'N')
		//if (( geneTypes[indi][j] == 'N') && (individual[j].first != -1))
		{
			int nID = individual[j].second;
			exist[nID] = true; 
		}
	}

	ofstream out("tst_nonexist.txt");
	

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	{
		int nID;
		Node* node;
		node = (*it_node).second;
		nID = node->id; // ID ȹ��	
		//nID = (*it_node).first; // ID ȹ��		

		if ( exist[nID] == false)
		{
			out << nID << ", ";
		}

	}

	out << endl;

	out.close();

	bool complete = true;



	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	{
		int nID;
		Node* node;
		node = (*it_node).second;
		nID = node->id; // ID ȹ��	
		//nID = (*it_node).first; // ID ȹ��		

		if ( exist[nID] == false)
		{
			complete = false;
			break;
		}
	}	

	if (complete == false)
	{
		cout << " error: GA�� ��ü�� ��� ���� ����� id�� �������� ����";
		this->printIndividual(individual);
		exit(-1);
	}
}

bool GA::IsFiniteNumber(double x) // �� ����� ������ �Ǽ����� �˻� (NaN ���� �˻�)
{
	 return (x <= DBL_MAX && x >= -DBL_MAX); 
}

void GA::printFitness() // �� ������ ���/�ִ� ���յ� ���
{
	double avg_fitness = 0;
	double best_fitness = fitness[0];

	for (int i = 0; i < Population_Size; i++) 
	{
		avg_fitness += fitness[i];

		if ( fitness[i]!=0 && fitness[i] < best_fitness)
		{
			best_fitness = fitness[i];
		}
	}

	avg_fitness /= Population_Size;

	cout << "\nbest fitness: " << best_fitness << ", individual: " << bestIndi << endl;
	//cout << "\n best fitness: " << best_fitness << endl;
	cout << " avg. fitness: " << avg_fitness << endl;

}

void GA::testSolution() // (2014.06.18) ����� �� �м� (level, index�� ���� �� �׷��� �򰡰��, �����߽����κ��� �Ÿ��� ���/ǥ������ ���)
{
	int kTreeHeight;

	int startingLevel = 0;

	kTreeHeight = kmr->getKTreeHeight();

	ofstream out("tst_solution.txt");

	// (2014.10.07) testF ȣ�� ���� ����
	Chromosome indi = population[this->bestIndi];

	for (int locus = 0; locus < indi.size(); locus++) // ���� �� ��ĵ �� �� �׷� ��
	{
		int level = indi[locus].first;
		int index = indi[locus].second;

		if (level > 0)
		{
			testF(out, level, index);
		}
	} // ���� �� �� ���� �Ϸ�

	/*
	if ( root->level == 0)
		{
			startingLevel = 1;
		}
		else
		{
			startingLevel = root->level;
		}
		
	for (int j = startingLevel; j < kTreeHeight-1; j++)
	//for (int j= root->level; j <  kTreeHeight-1; j++)  // solution ��ü�� root level~ (h-2) level�� �׷� ��
	//for (int j=1; j <  kTreeHeight-1; j++)  // solution ��ü�� 1~ (h-2) level�� �׷� ��
	{
		int numNodesAtLevel = kmr->getNumNodesAtLevel(j) ;

		for (int k= 0; k < numNodesAtLevel; k++) // �־��� �׷쿡 ���� test_f() �Լ� ȣ��
		{	
			testF(out, j, k);		
		}
	}
	*/
	out.close();	
}

void GA::quicksort( int start, int finish) // (2014.09.02) ��� ����� cellID �������� �������� ����
{
	if (start < finish)
	{
		int q = partition( start, finish); // �迭 ������
		//int q = partition( candis, start, finish); // �迭 ������

		quicksort(start, q-1); // ���� ���� ����
		quicksort(q+1, finish); // ���� ���� ����	
	}
}

int GA::partition( int start, int finish) // (2014.09.02) quicksort�� �ʿ��� ���� �Լ�
{
	double val1 = 0;  // j��° ����� cellID
	double val2= 0;	// finish ��ġ�� �ִ� �����  cellID

	int i = start - 1;

	int j = 0;

	for ( j = start; j < finish; j++) // ���� ������ ���� ��ҵ�� ū ��ҵ�� ����
	{
		
		val1 = nodes[j]->cell;
		val2 = nodes[finish]->cell;

		//val1 = eval_result[j].second;		
		//val2 = eval_result[finish].second;
		

		if (val1< val2)
		{
			i = i +1;

			Node* temp = nodes[i];
			nodes[i] = nodes[j];
			nodes[j] = temp;

			/*
			Node* temp = candis[i];
			candis[i] = candis[j];
			candis[j] = temp;

			pair<int, double> t = eval_result[i];
			//double t = eval_result[i].second;
			eval_result[i] = eval_result[j];
			eval_result[j] = t;
			*/

		}		
	}

	Node* temp = nodes[i+1];
	nodes[i+1] = nodes[finish];
	nodes[finish] = temp;

	/*
	Node* temp = candis[i+1];
	candis[i+1] = candis[finish];
	candis[finish] = temp;

	pair<int, double> t = eval_result[i+1];
	//double t = eval_result[i+1];
	eval_result[i+1] = eval_result[finish];
	eval_result[finish] = t;
	*/

	return (i+1);
}

/*
Chromosome GA::exec(void)
{
	// exec ȣ�� �� �̹� individual length�� ������ �־�� �� - setIndiLen() �Լ� ȣ��

	// clear old data - population, individual, genetypes

	individual.clear();

	for (int i = 0; i < Population_Size; i++)
	{
		this->population[i].clear();
		this->temp_population[i].clear();		
		this->geneTypes[i].clear();
	}	

	create_population(); // create initial population

	int generation = 0;

	while(generation < Num_Generation) // convergence test
	{

		evaluate(); // evaluate the current population

		select();// select good individuals for the next generation

		crossover();// mix the selected individuals

		mutate();// mutate with low probability

		 // move to the next generation (copy the elements of temporary population to the next population)
		for (int indi = 0; indi < Population_Size; indi++)
		{			
			population[indi].clear();
			population[indi] = temp_population[indi];			
		}		

		generation++; 
	}

	evaluate(); // evaluate the last population

	// find the best individual

	int bestIndi = 0;
	int curFitness = fitness[bestIndi];
	int bestFitness = curFitness;	

	for (int i = 1; i < Population_Size; i++) 
	{
		curFitness = fitness[i];

		if (curFitness > bestFitness)
		{
			bestIndi = i;
			bestFitness = curFitness;
		}
	}

	return population[bestIndi];	// return the best individual
}
*/
GA::GA(void)
{
	indiLen = 0;
	root = NULL;

	gen.setSeed(1);

	gen2.setSeed(1);

	gen3.setSeed(1);

	gen4.setSeed(1);

	gen5.setSeed(1);
}

GA::~GA(void)
{

	
}
