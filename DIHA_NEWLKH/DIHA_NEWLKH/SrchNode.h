
// 2014.04.04
// ������
// SrchNode.h
// A* Ž���� ���� Ž�� ��� ����

#ifndef _SRCHNODE_H
#define _SRCHNODE_H

#include <iostream>
#include <list>

using namespace std;

class SrchNode
{
public:

	// Ž�� ����� ��� ���� �Լ�
	double getG() const;
	double getH() const;
	double getF() const;
	void setG();
	void setG( double g);	
	virtual void setH() = 0; // pure virtual function (problem-dependent function)
	void setF();

	// Ž�� ���� �Լ�
	SrchNode* getParent();
	void setParent (SrchNode* newParent);
	virtual double getLinkCost (SrchNode* parent) = 0; //  get the link cost between the parent and this node (pure virtual function)
	void addChild (SrchNode* s); // add child of the current node
	int getNumChld();
	list<SrchNode*>::iterator getChildren();
	bool isAncestorOf (SrchNode* n); // return true if the current node is an ancestor of the given node n
	virtual bool isSameState( SrchNode* n) =0; // return true if this node has the same state as node n

	// ���� ���� ��� ���� �Լ�
	virtual void calNxtStates() = 0; // calculate possible next states (pure virtual function)
	SrchNode** getNxtStates(); // return iterator for possible next states list
	int getNumNxtSts(); // return number of possible next states	

	
	// ���� ���� �˻�
	virtual bool isGoalNode() = 0; // return true if the current state is the goal state

	// �׽�Ʈ �Լ�
	virtual void prntStat() = 0; // print the current state	

	

	SrchNode();
	~SrchNode();

protected:
	double g; // lowest cost found so far from node n0 to this node
	double h; // est. cost from this node to goal node
	double f; // est. cost of 'minimal cost path' (from n0 to goal node) through the current node (i.e., g + h)

	SrchNode* parent; // the parent node of this node on the least cost path found so far from n0 to this node
	list<SrchNode*> children; // child nodes of this node on the search graph

	SrchNode** nxtStates; // possible next states list
	int numNxtSts; // number of possible next states 
};

#endif