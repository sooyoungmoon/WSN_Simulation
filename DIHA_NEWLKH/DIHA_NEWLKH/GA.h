

// 2014.04.30
// 문수영
// GA.h
// 유전 알고리즘 클래스 선언
// 목적: 키 트리 기반 노드 그룹 형성

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
	
	// 멤버 변수 설정 관련
	void setNodes(map<int, Node*> nodes); // 노드 리스트 초기화 함수
	void setFullSetNodes(map<int, Node*> nodes); // 노드 리스트 초기화 함수

	void setKmr(Kmr* kmr); // 키 관리자 객체 설정
	void setIndiLen(int indiLen);// 개체 길이 설정	
	void setRoot(struct _tagKNode* root); // key tree root 설정


	// 세대의 생성, 평가
	void create_population(); // 세대 생성
	void writeGroupIDs(struct _tagKNode* currentNode, int individual, int gene ); // 그룹 id 기록 (어느 개체의 몇 번째 유전자에 기록?)
	void writeNodeIDs(int index); // 노드 id 기록

	void evaluate(); // 세대 평가
	double f(int individual, int level, int index);// 주어진 개체에서 특정 그룹에 속한 노드들의 무게중심(COG)을 기준으로 각 노드까지의 거리의 표준편차 반환 
	
	double testF(ofstream & out, int level, int index);// 도출된 해에 포함된 각 그룹 분석 결과를 파일에 기록
	
	int getStartingPos (int level, int index); //  특정 그룹 (level, index)의 시작 위치를 반환 (모든 개체에 공통)

	// GA 연산
	void select(); // 개체 선택
	void crossover();// 교배
	void mutate(); // 돌연변이
	
	void printIndividual(map<int, std::pair<int, int>> indi); // test 용 - individual 출력

	// 결과 도출
	map<int, std::pair<int, int>> findSolution(); // 문제의 해 발견 (i, i-th gene)
	map<int, char> getSolGeneType(); // 도출된 해의 genetype 반환

	// 테스트 (debugging)
	void printPopulation(); // 현 세대 내 개체 출력, 평균/최대 fitness value 출력
	void veriPopulation(); // 현 세대 내 개체들이 문제의 제약 조건을 만족하는 지 검증

	void veriIndividual(map<int, std::pair<int, int>> indi, map<int, char> geneTypes); // 주어진 개체가 문제의 제약 조건을 만족하는 지 검증
	bool IsFiniteNumber(double x); // 평가 결과가 유한한 실수인지 검사 (NaN 여부 검사)


	//void veriIndividual(int indi); // 주어진 개체가 문제의 제약 조건을 만족하는 지 검증
	void printFitness(); // 현 세대의 평균/최대 적합도 출력
	void testSolution(); // 도출된 해의 성능을 출력 (각 클러스터 내 노드들의 무게 중심으로부터의 평균 거리)

	// 초기 세대 생성 관련
	void quicksort( int start, int finish); // (2014.09.02) 노드 목록 정렬 (sorting) - cellID 기준으로 오름차순 정렬
	int partition( int start, int finish);	

	//map<int, std::pair<int, int>> exec(); // 결과 도출
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
	map<int, Node*> nodes; // (일부) 노드 목록 ( {key , node pointer) }// 주의! key 값과 해당 노드의 id 는 다른 값임

	map<int, Node*> fullSetNodes; // (네트워크) 전체 노드 목록

	Kmr* kmr; // 키 관리자 객체

	int indiLen; // 염색체의 길이 (가변적)
	map<int, pair<int, int>> individual; // 개체 (염색체) - (인덱스, 값)
	int geneIdx; // 유전자의 인덱스 (다음에 기록될 유전자의 위치)

	map<int, pair<int, int>> population[Population_Size];// 세대
	map<int, pair<int, int>> temp_population[Population_Size];// 임시 세대 (연산을 위해 사용)

	//map<int, char> geneTypes[Population_Size]; // 유전자의 타입 ( 'G': group, 'N': node)	

	map<int, char> popGeneTypes[Population_Size]; // population에 속한 개체의 유전자의 타입 ( 'G': group, 'N': node)	
	map<int, char> tPopGeneTypes[Population_Size]; // temp_population에 속한 개체의 유전자의 타입 ( 'G': group, 'N': node)	

	double fitness[Population_Size]; // 세대 평가 결과
	int bestIndi; // the best individual

	struct _tagKNode* root; // 키 트리의 root
	

	

	// 난수 생성 관련
	RAND gen; // 초기 세대 생성에 사용
	//int seed;	
	RAND gen2; // GA-select() 함수 - tournament selection에 사용
	//int seed2;
	RAND gen3; // GA-crossover() 함수 - crossing point 결정에 사용
	//int seed3; 
	RAND gen4; // GA-mutate() 함수 - mutation point 결정에 사용
	// int seed4;
	RAND gen5; // GA-mutate() 함수 - mutation 발생 여부 결정에 사용
	// int seed5;

};

#endif

