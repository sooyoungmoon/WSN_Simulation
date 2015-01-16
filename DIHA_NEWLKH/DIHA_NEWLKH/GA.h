

// 2014.04.30
// ������
// GA.h
// ���� �˰��� Ŭ���� ����
// ����: Ű Ʈ�� ��� ��� �׷� ����

#ifndef _GA_H
#define _GA_H

#include <list>
#include <map>
#include "Node.h"
#include "RAND.h"
#include "Kmr.h"
#include "params.h"
#include "typedef.h"


using std::list;
using std::map;
using std::pair;

class Kmr;
class Node;

class GA
{
public:
	
	// ��� ���� ���� ����
	void setNodes(map<int, Node*> nodes); // ��� ����Ʈ �ʱ�ȭ �Լ�
	void setFullSetNodes(map<int, Node*> nodes); // ��� ����Ʈ �ʱ�ȭ �Լ�

	void setKmr(Kmr* kmr); // Ű ������ ��ü ����
	void setIndiLen(int indiLen);// ��ü ���� ����	
	void setRoot(struct _tagKNode* root); // key tree root ����


	// ������ ����, ��
	void create_population(); // ���� ����
	void writeGroupIDs(struct _tagKNode* currentNode, int individual, int gene ); // �׷� id ��� (��� ��ü�� �� ��° �����ڿ� ���?)
	void writeNodeIDs(int index); // ��� id ���

	void evaluate(); // ���� ��
	double f(int individual, int level, int index);// �־��� ��ü���� Ư�� �׷쿡 ���� ������ �����߽�(COG)�� �������� �� �������� �Ÿ��� ǥ������ ��ȯ 
	
	double testF(ofstream & out, int level, int index);// ����� �ؿ� ���Ե� �� �׷� �м� ����� ���Ͽ� ���
	
	int getStartingPos (int level, int index); //  Ư�� �׷� (level, index)�� ���� ��ġ�� ��ȯ (��� ��ü�� ����)

	// GA ����
	void select(); // ��ü ����
	void crossover();// ����
	void mutate(); // ��������
	
	void printIndividual(map<int, std::pair<int, int>> indi); // test �� - individual ���

	// ��� ����
	map<int, std::pair<int, int>> findSolution(); // ������ �� �߰� (i, i-th gene)
	map<int, char> getSolGeneType(); // ����� ���� genetype ��ȯ

	// �׽�Ʈ (debugging)
	void printPopulation(); // �� ���� �� ��ü ���, ���/�ִ� fitness value ���
	void veriPopulation(); // �� ���� �� ��ü���� ������ ���� ������ �����ϴ� �� ����

	void veriIndividual(map<int, std::pair<int, int>> indi, map<int, char> geneTypes); // �־��� ��ü�� ������ ���� ������ �����ϴ� �� ����
	bool IsFiniteNumber(double x); // �� ����� ������ �Ǽ����� �˻� (NaN ���� �˻�)


	//void veriIndividual(int indi); // �־��� ��ü�� ������ ���� ������ �����ϴ� �� ����
	void printFitness(); // �� ������ ���/�ִ� ���յ� ���
	void testSolution(); // ����� ���� ������ ��� (�� Ŭ������ �� ������ ���� �߽����κ����� ��� �Ÿ�)

	// �ʱ� ���� ���� ����
	void quicksort( int start, int finish); // (2014.09.02) ��� ��� ���� (sorting) - cellID �������� �������� ����
	int partition( int start, int finish);	

	//map<int, std::pair<int, int>> exec(); // ��� ����
	/*
	void create_population(char [Population_Size][Num_Items], char temp_population[Population_Size][Num_Items]); // create initial population
	void evaluate(int w[Num_Items], int p[Num_Items], int fitness[Num_Items], char population[Population_Size][Num_Items]); // evaluate a candidate solution
	void select(char population[Population_Size][Num_Items], char temp_population[Population_Size][Num_Items], int fitness[Population_Size]); // select individuals for the next generation
	void crossover(char population[Population_Size][Num_Items]); // crossover
	void mutate(char population[Population_Size][Num_Items]); // mutate
	*/


	GA(void);	
	~GA(void);

private:
	map<int, Node*> nodes; // (�Ϻ�) ��� ��� ( {key , node pointer) }// ����! key ���� �ش� ����� id �� �ٸ� ����

	map<int, Node*> fullSetNodes; // (��Ʈ��ũ) ��ü ��� ���

	Kmr* kmr; // Ű ������ ��ü

	int indiLen; // ����ü�� ���� (������)
	map<int, pair<int, int>> individual; // ��ü (����ü) - (�ε���, ��)
	int geneIdx; // �������� �ε��� (������ ��ϵ� �������� ��ġ)

	map<int, pair<int, int>> population[Population_Size];// ����
	map<int, pair<int, int>> temp_population[Population_Size];// �ӽ� ���� (������ ���� ���)

	//map<int, char> geneTypes[Population_Size]; // �������� Ÿ�� ( 'G': group, 'N': node)	

	map<int, char> popGeneTypes[Population_Size]; // population�� ���� ��ü�� �������� Ÿ�� ( 'G': group, 'N': node)	
	map<int, char> tPopGeneTypes[Population_Size]; // temp_population�� ���� ��ü�� �������� Ÿ�� ( 'G': group, 'N': node)	

	double fitness[Population_Size]; // ���� �� ���
	int bestIndi; // the best individual

	struct _tagKNode* root; // Ű Ʈ���� root
	

	

	// ���� ���� ����
	RAND gen; // �ʱ� ���� ������ ���
	//int seed;	
	RAND gen2; // GA-select() �Լ� - tournament selection�� ���
	//int seed2;
	RAND gen3; // GA-crossover() �Լ� - crossing point ������ ���
	//int seed3; 
	RAND gen4; // GA-mutate() �Լ� - mutation point ������ ���
	// int seed4;
	RAND gen5; // GA-mutate() �Լ� - mutation �߻� ���� ������ ���
	// int seed5;

};

#endif

