

// SrchGrph.cpp
// 2014.04.24
// 문수영
// A* 탐색을 위한 탐색 그래프 클래스 정의

#include "SrchGrph.h"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

bool NodeCompare (const SrchNode* n1, const SrchNode* n2) // 두 노드 비교 함수 (정렬 목적)
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
	// 현재 노드의 이웃 노드들을 계산
	n->calNxtStates();

	// 현재 노드의 이웃 노드들의 목록을 확보
	SrchNode** nxtSts = n->getNxtStates();

	// 위 목록에서 현재 노드의 'ancestor'들을 제거

	int numNxtSts = n->getNumNxtSts(); // 가능한 다음 상태의 개수 획득

	int count = 0;

	for (;  ; nxtSts++)
	{
		SrchNode* m = *nxtSts;		

		if ( m->isAncestorOf(n)) // 노드 m이 노드 n의 'ancestor' 이면 제외
			;
		else // 노드 m을 노드 n의 자식 노드 목록에 추가
		{
			m->setH();
			n->addChild(m);
		}

		if ( ++count >= numNxtSts) // 가능한 다음 상태를 모두 조사하면 for loop 탈출
			break;	}	

	list<SrchNode*>::iterator it = n->getChildren();	 

	for (int count =0 ; count < n->getNumChld(); it++, count++) // 위 목록의 노드들을 하나씩 순회하며 아래 내용을 반복
	{
		SrchNode* s = *it; // 현재 검사 중인 자식 노드 s		

		if ( this->isInOpenLst (s) ) // 1) 현재 검사중인 노드 s가 OPEN node list에 이미 존재하는 경우	
		{
			// 노드 s의 기존 부모 노드를 통한 경로보다 노드 n을 통한 경로가 더 비용이 적을 경우,

			// s의 g, f 값 갱신 -> s의 역방향 포인터를 노드 n을 가리키도록 변경

			this->updateCost( n, s);
		}

		else if ( this->isInClsdLst (s) )  // 2) 현재 검사중인 노드 s가 CLOSED node list에 이미 존재하는 경우
		{			
			// 노드 s의 기존 부모 노드를 통한 경로보다 노드 n을 통한 경로가 더 비용이 적을 경우,

			// s의 g, f 값 갱신 -> s의 역방향 포인터를 노드 n을 가리키도록 변경

			// s의 자손 노드들에 대해 위 과정 반복 (재귀적 처리)

			updateCostRec( n, s);
		}

		else // 3) 그 밖의 경우
		{
			// open node list에 s 추가	 

			this->openList.push_back(s);

			// s의 g, f 값 갱신
			// s의 역방향 포인터를 노드 n을 가리키도록 변경
			
			s->setParent(n); 
			s->setG();
			s->setF();

		}

	} // end for loop
}

void SrchGrph::updateCost (SrchNode* newParent, SrchNode* current)
{
	// 노드 current의 기존 부모 노드를 통한 경로보다 노드 n을 통한 경로가 더 비용이 적을 경우,

	// current의 g, f 값 갱신 -> s의 역방향 포인터를 노드 n을 가리키도록 변경

	double newG = newParent->getG() + current->getLinkCost(newParent);

	if ( newG < current->getG() )
	{
		current->setG(newG);
		current->setF();

		// s의 역방향 포인터를 노드 n을 가리키도록 변경
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

SrchNode* SrchGrph::doSearch() // A*를 사용한 탐색 과정 구현
{
	// 1) 탐색 그래프 생성, root 노드를 OPEN 목록에 추가
	openList.push_back(this->root);

	// 2) CLOSED 목록 초기화
	// 이미 초기화되어 있음

	// 3) OPEN 목록에 노드가 남아 있지 않으면 탐색 실패, 노드가 남아 있으면 아래 WHILE 문 반복
	while ( !this->openList.empty())
	{
		// 현재 OPEN 목록과 CLOSED 목록 출력
		//this->prntOpenLst();
		//this->prntClsdLst();


		// 4) OPEN 목록에서 첫 번째 노드를 제거 - 노드 n
		SrchNode* n = openList.front(); // 첫 번째 노드 반환

		openList.pop_front(); // OPEN 목록에서 첫 번째 노드 제거
		closedList.push_back(n); // 반환된 노드를 CLOSED 목록에 추가

		// 5) n이 목표 노드와 일치하면 해를 저장하고 반복문 탈출 (break)
		if ( n->isGoalNode() == true)
		{			
			solution = n; // solution 변수 값 대입
			
			//cout << " 탐색 성공! " << endl;

			// root에서 목표 노드까지의 경로 출력
			//this->printPath(n);
			break;
			
		}

		// 6) 노드 n을 확장
		expand(n);

		// 7) OPEN 목록의 노드들은 f 값에 따라 오름차순 정렬
		sortOpenLst();
	}

	return solution; // 해를 찾지 못할 경우 비어 있는 LIST 반환
}

/*
std::list<SrchNode*> SrchGrph::doSearch() // A*를 사용한 탐색 과정 구현
{
	// 1) 탐색 그래프 생성, root 노드를 OPEN 목록에 추가
	openList.push_back(this->root);

	// 2) CLOSED 목록 초기화
	// 이미 초기화되어 있음

	// 3) OPEN 목록에 노드가 남아 있지 않으면 탐색 실패, 노드가 남아 있으면 아래 WHILE 문 반복
	while ( !this->openList.empty())
	{
		// 현재 OPEN 목록과 CLOSED 목록 출력
		this->prntOpenLst();
		this->prntClsdLst();


		// 4) OPEN 목록에서 첫 번째 노드를 제거 - 노드 n
		SrchNode* n = openList.front(); // 첫 번째 노드 반환

		openList.pop_front(); // OPEN 목록에서 첫 번째 노드 제거
		closedList.push_back(n); // 반환된 노드를 CLOSED 목록에 추가

		// 5) n이 목표 노드와 일치하면 해당 경로를 출력하고 함수 종료
		if ( n->isGoalNode() == true)
		{			
			solutionPath = getPath(n); // solution path 반환
			
			//cout << " 탐색 성공! " << endl;

			// root에서 목표 노드까지의 경로 출력
			//this->printPath(n);
			//break;
			
		}

		// 6) 노드 n을 확장
		expand(n);

		// 7) OPEN 목록의 노드들은 f 값에 따라 오름차순 정렬
		sortOpenLst();
	}

	return solutionPath; // 해를 찾지 못할 경우 비어 있는 LIST 반환
}
*/

std::list<SrchNode*> SrchGrph::getPath(SrchNode* n) // 주어진 상태에서 초기 상태까지의 경로 반환
{
	
	SrchNode* temp = n;
	std::list<SrchNode*> path;


	while (temp != NULL) // 주어진 상태에서 초기 상태 (root) 노드까지 탐색, 경로 상의 각 노드를 목록에 추가
	{
		path.push_back(temp);
		temp = temp->getParent();
	}

	return path; // 경로 반환
}

void SrchGrph::sortOpenLst() // OPEN 목록 노드들을 정렬
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
		temp->prntStat(); // 현재 상태 출력
		cout << " ) " ;
		cout << endl;

		temp = temp->getParent(); // 부모 노드로 이동
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
	// 탐색 트리에 속한 노드들의 메모리 해제 필요!
}

