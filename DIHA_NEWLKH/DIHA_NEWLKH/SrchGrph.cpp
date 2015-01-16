

// SrchGrph.cpp
// 2014.04.24
// ������
// A* Ž���� ���� Ž�� �׷��� Ŭ���� ����

#include "SrchGrph.h"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

bool NodeCompare (const SrchNode* n1, const SrchNode* n2) // �� ��� �� �Լ� (���� ����)
{	
		return (n1->getF() < n2->getF() ) ;	
}

void SrchGrph::setRoot( SrchNode* root)
{
	this->root = root;
}

SrchNode* SrchGrph::getRoot()
{
	return root;
}

void SrchGrph::expand(SrchNode* n)
{
	// ���� ����� �̿� ������ ���
	n->calNxtStates();

	// ���� ����� �̿� ������ ����� Ȯ��
	SrchNode** nxtSts = n->getNxtStates();

	// �� ��Ͽ��� ���� ����� 'ancestor'���� ����

	int numNxtSts = n->getNumNxtSts(); // ������ ���� ������ ���� ȹ��

	int count = 0;

	for (;  ; nxtSts++)
	{
		SrchNode* m = *nxtSts;		

		if ( m->isAncestorOf(n)) // ��� m�� ��� n�� 'ancestor' �̸� ����
			;
		else // ��� m�� ��� n�� �ڽ� ��� ��Ͽ� �߰�
		{
			m->setH();
			n->addChild(m);
		}

		if ( ++count >= numNxtSts) // ������ ���� ���¸� ��� �����ϸ� for loop Ż��
			break;	}	

	list<SrchNode*>::iterator it = n->getChildren();	 

	for (int count =0 ; count < n->getNumChld(); it++, count++) // �� ����� ������ �ϳ��� ��ȸ�ϸ� �Ʒ� ������ �ݺ�
	{
		SrchNode* s = *it; // ���� �˻� ���� �ڽ� ��� s		

		if ( this->isInOpenLst (s) ) // 1) ���� �˻����� ��� s�� OPEN node list�� �̹� �����ϴ� ���	
		{
			// ��� s�� ���� �θ� ��带 ���� ��κ��� ��� n�� ���� ��ΰ� �� ����� ���� ���,

			// s�� g, f �� ���� -> s�� ������ �����͸� ��� n�� ����Ű���� ����

			this->updateCost( n, s);
		}

		else if ( this->isInClsdLst (s) )  // 2) ���� �˻����� ��� s�� CLOSED node list�� �̹� �����ϴ� ���
		{			
			// ��� s�� ���� �θ� ��带 ���� ��κ��� ��� n�� ���� ��ΰ� �� ����� ���� ���,

			// s�� g, f �� ���� -> s�� ������ �����͸� ��� n�� ����Ű���� ����

			// s�� �ڼ� ���鿡 ���� �� ���� �ݺ� (����� ó��)

			updateCostRec( n, s);
		}

		else // 3) �� ���� ���
		{
			// open node list�� s �߰�	 

			this->openList.push_back(s);

			// s�� g, f �� ����
			// s�� ������ �����͸� ��� n�� ����Ű���� ����
			
			s->setParent(n); 
			s->setG();
			s->setF();

		}

	} // end for loop
}

void SrchGrph::updateCost (SrchNode* newParent, SrchNode* current)
{
	// ��� current�� ���� �θ� ��带 ���� ��κ��� ��� n�� ���� ��ΰ� �� ����� ���� ���,

	// current�� g, f �� ���� -> s�� ������ �����͸� ��� n�� ����Ű���� ����

	double newG = newParent->getG() + current->getLinkCost(newParent);

	if ( newG < current->getG() )
	{
		current->setG(newG);
		current->setF();

		// s�� ������ �����͸� ��� n�� ����Ű���� ����
		current->setParent(newParent);
	}
}

void SrchGrph::updateCostRec (SrchNode* newParent, SrchNode* n)
{
	updateCost ( newParent, n);

	int numChild = n->getNumChld();

	if (numChild !=0)
	{
		list<SrchNode*>::iterator it =  n->getChildren();

		int count = 0;

		for (it; ; it++)
		{
			SrchNode* c = *it;

			updateCostRec (n, c);

			if ( ++count >= numChild)
				break;
		}
	}	

}

bool SrchGrph::isInOpenLst (SrchNode* s)
{
	list<SrchNode*>::iterator it = this->openList.begin();

	for (it; it != openList.end(); it++)
	{
		if ( (*it)->isSameState(s) )
			return true;
	}

	return false;
}

bool SrchGrph::isInClsdLst (SrchNode* s)
{
	list<SrchNode*>::iterator it = this->closedList.begin();

	for (it; it != closedList.end(); it++)
	{
		if ( (*it)->isSameState(s) )
			return true;
	}

	return false;
}

SrchNode* SrchGrph::doSearch() // A*�� ����� Ž�� ���� ����
{
	// 1) Ž�� �׷��� ����, root ��带 OPEN ��Ͽ� �߰�
	openList.push_back(this->root);

	// 2) CLOSED ��� �ʱ�ȭ
	// �̹� �ʱ�ȭ�Ǿ� ����

	// 3) OPEN ��Ͽ� ��尡 ���� ���� ������ Ž�� ����, ��尡 ���� ������ �Ʒ� WHILE �� �ݺ�
	while ( !this->openList.empty())
	{
		// ���� OPEN ��ϰ� CLOSED ��� ���
		//this->prntOpenLst();
		//this->prntClsdLst();


		// 4) OPEN ��Ͽ��� ù ��° ��带 ���� - ��� n
		SrchNode* n = openList.front(); // ù ��° ��� ��ȯ

		openList.pop_front(); // OPEN ��Ͽ��� ù ��° ��� ����
		closedList.push_back(n); // ��ȯ�� ��带 CLOSED ��Ͽ� �߰�

		// 5) n�� ��ǥ ���� ��ġ�ϸ� �ظ� �����ϰ� �ݺ��� Ż�� (break)
		if ( n->isGoalNode() == true)
		{			
			solution = n; // solution ���� �� ����
			
			//cout << " Ž�� ����! " << endl;

			// root���� ��ǥ �������� ��� ���
			//this->printPath(n);
			break;
			
		}

		// 6) ��� n�� Ȯ��
		expand(n);

		// 7) OPEN ����� ������ f ���� ���� �������� ����
		sortOpenLst();
	}

	return solution; // �ظ� ã�� ���� ��� ��� �ִ� LIST ��ȯ
}

/*
std::list<SrchNode*> SrchGrph::doSearch() // A*�� ����� Ž�� ���� ����
{
	// 1) Ž�� �׷��� ����, root ��带 OPEN ��Ͽ� �߰�
	openList.push_back(this->root);

	// 2) CLOSED ��� �ʱ�ȭ
	// �̹� �ʱ�ȭ�Ǿ� ����

	// 3) OPEN ��Ͽ� ��尡 ���� ���� ������ Ž�� ����, ��尡 ���� ������ �Ʒ� WHILE �� �ݺ�
	while ( !this->openList.empty())
	{
		// ���� OPEN ��ϰ� CLOSED ��� ���
		this->prntOpenLst();
		this->prntClsdLst();


		// 4) OPEN ��Ͽ��� ù ��° ��带 ���� - ��� n
		SrchNode* n = openList.front(); // ù ��° ��� ��ȯ

		openList.pop_front(); // OPEN ��Ͽ��� ù ��° ��� ����
		closedList.push_back(n); // ��ȯ�� ��带 CLOSED ��Ͽ� �߰�

		// 5) n�� ��ǥ ���� ��ġ�ϸ� �ش� ��θ� ����ϰ� �Լ� ����
		if ( n->isGoalNode() == true)
		{			
			solutionPath = getPath(n); // solution path ��ȯ
			
			//cout << " Ž�� ����! " << endl;

			// root���� ��ǥ �������� ��� ���
			//this->printPath(n);
			//break;
			
		}

		// 6) ��� n�� Ȯ��
		expand(n);

		// 7) OPEN ����� ������ f ���� ���� �������� ����
		sortOpenLst();
	}

	return solutionPath; // �ظ� ã�� ���� ��� ��� �ִ� LIST ��ȯ
}
*/

std::list<SrchNode*> SrchGrph::getPath(SrchNode* n) // �־��� ���¿��� �ʱ� ���±����� ��� ��ȯ
{
	
	SrchNode* temp = n;
	std::list<SrchNode*> path;


	while (temp != NULL) // �־��� ���¿��� �ʱ� ���� (root) ������ Ž��, ��� ���� �� ��带 ��Ͽ� �߰�
	{
		path.push_back(temp);
		temp = temp->getParent();
	}

	return path; // ��� ��ȯ
}

void SrchGrph::sortOpenLst() // OPEN ��� ������ ����
{
	this->openList.sort(NodeCompare);
}

void SrchGrph::printPath(SrchNode* n)
{	

	cout << "________________________________" << endl;


	SrchNode* temp = n;

	while (temp!= NULL)
	{
		cout << " ( " ;
		temp->prntStat(); // ���� ���� ���
		cout << " ) " ;
		cout << endl;

		temp = temp->getParent(); // �θ� ���� �̵�
	}	

	cout << "________________________________" << endl;
}

void SrchGrph::prntOpenLst()
{
	list<SrchNode*>::iterator it = this->openList.begin();

	cout << "OPEN list: " << endl;

	for (it; it != openList.end(); it++)
	{
		SrchNode* t = *it;

		cout << "( " ;
		t->prntStat();
		cout << " ) " << endl ;
	}

	cout << endl;
}

void SrchGrph::prntClsdLst()
{
	list<SrchNode*>::iterator it = this->closedList.begin();

	cout << "CLOSED list: " << endl;

	for (it; it != closedList.end(); it++)
	{
		SrchNode* t = *it;

		cout << "( " ;
		t->prntStat();
		cout << " ) " << endl ;
	}

	cout << endl;
}

SrchGrph::SrchGrph()
{

}

SrchGrph::~SrchGrph()
{
	// Ž�� Ʈ���� ���� ������ �޸� ���� �ʿ�!
}

