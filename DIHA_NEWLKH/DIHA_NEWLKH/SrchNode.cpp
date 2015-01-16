
// 2014.04.15
// ������
// SrchNode_KTS.cpp
// A* Ž���� ���� Ž�� ��� ����

#include "SrchNode.h"

double SrchNode::getG() const
{
	return this->g;
}

void SrchNode::setG() 
{
	this->g = parent->getG() + this->getLinkCost(parent); // '�θ� ����� ���' + '�θ� ���� ���� ����� ���� ���'
}

void SrchNode::setG(double g) 
{
	this->g = g;
}


double SrchNode::getF() const
{
	return this->f;
}

double SrchNode::getH() const
{
	return this->h;
}

void SrchNode::setF()
{
	this->f = g + h; 
}

// Ž�� ���� �Լ�

SrchNode* SrchNode::getParent()
{
	return this->parent;
}

void SrchNode::setParent (SrchNode* newParent)
{
	this->parent = newParent;
}

// 

void SrchNode::addChild (SrchNode* s) // add child of the current node
{
	children.push_back(s);
}

int SrchNode::getNumChld()
{
	return this->children.size();
}


list<SrchNode*>::iterator SrchNode::getChildren()
{
	return this->children.begin();
}


bool SrchNode::isAncestorOf (SrchNode* n) // return true if the current node is an ancestor of the given node n
{
	bool res = false;

	SrchNode* temp = n->parent;

	while (temp != NULL)
	{
		if (temp == this)
		{
			res = true;
			break;
		}

		temp = temp->parent;
	}

	return res;
}
//
	// ���� ���� ��� ���� �Լ�
SrchNode** SrchNode::getNxtStates() // return iterator for possible next states list
{
	return this->nxtStates;
}

int SrchNode::getNumNxtSts() // return number of possible next states	
{
	return this->numNxtSts;
}

SrchNode::SrchNode()
{
	g=0;
	h=0;
	f=0;

	parent = NULL;
	nxtStates = NULL;
	numNxtSts = 0;
}

SrchNode::~SrchNode()
{
	delete []nxtStates;
}