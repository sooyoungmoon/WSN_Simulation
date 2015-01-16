

// 2014.07.04
// 문수영
// GA.cpp
// 유전 알고리즘 클래스 구현

#include "GA.h"

using std::cout;

void GA::setNodes(map<int, Node*> nodesInput) // (2013.08.28 추가)
{
	int count = 0;
	// 

	nodes.clear();

	map<int, Node*>::iterator it;

	for (it = nodesInput.begin(); it != nodesInput.end(); it++) // 입력 받은 node map의 요소들을 멤버 변수인 nodes map에 복사하되 키 값을 새로 설정 0~ size-1
	{
		Node* node = (*it).second;

		nodes[count] = node;
		count++;
	}// 입력 받은 node map의 요소들을 저장하고 key 값을 새로 설정

	//this->nodes= nodes; // 
}

void GA::setFullSetNodes(map<int, Node*> fullSetNodes)
{
	this->fullSetNodes = fullSetNodes; // 전체 노드 map 설정
}

void GA::setKmr(Kmr* kmr) // 키 관리자 객체 설정
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

void GA::create_population() // 초기 세대 생성
{
	// GA의 멤버 변수인 nodes map의 key 값을 재설정 해준다.

	map<int, Node*>::iterator it;

	/*
	int cnt = 0;

	for (it = nodes.begin(); it != nodes.end(); it++) // node map 의 요소들 순회 - key 재설정 
	{
		(*it).first = cnt;
		cnt++;
	} // node map의 key 재설정 완료
	*/


	quicksort(0, nodes.size()-1); // 노드 목록을 cell id 기준으로 오름차순 정렬
		
	for(int i=0; i < Population_Size; i++)
	{
		/*
		if ( i % 10 ==0)
		{
			cout << "개체 " << i << "~" << i+9 << " 생성 " << endl;
		}
		*/
		geneIdx = 0;

		for (int j = 0; j < this->indiLen; j++) 
		{
			Gene gene = std::make_pair(-1, -1); // 유전자의 첫 번째와 두 번째 값을 -1로 초기화 

			population[i].insert( make_pair (j, gene)); // 현 세대의 해당 개체의 각 유전자의 값을 초기화
			//population[i].insert(std::pair<int, Gene> (j, gene)); // 현 세대의 해당 개체의 각 유전자의 값을 초기화
			temp_population[i].insert( make_pair (j, gene)); // 임시 세대의 해당 개체각 유전자의 값을 0으로 초기화

			this->popGeneTypes[i].insert(std::pair<int, char> (j, '\0' ));
			//this->geneTypes[i].insert(std::pair<int, char> (j, '\0' ));
			//this->geneTypes.insert(std::pair<int, char> (j, '\0' )); // gene type 초기화
		}	
		// 여기까지 확인 (7/4, 11:30)
		this->writeGroupIDs(root, i, geneIdx);// 그룹 id에 대응되는 유전자 기록 (key tree parsing)	
		
		//this->printIndividual(population[i]); // i 번째 individual 출력

		this->writeNodeIDs(i);// i번째 individual의 노드 id에 대응되는 유전자 기록 (random)

		this->printIndividual(population[i]); // i 번째 individual 출력

		temp_population[i] = population[i];
		this->tPopGeneTypes[i] = this->popGeneTypes[i];

		this->veriIndividual(temp_population[i], this->tPopGeneTypes[i]); // // i 번째 individual 검증
		//this->veriIndividual(temp_population[i], geneTypes[i]); // // i 번째 individual 검증
	}

}

void GA::evaluate() // 세대에 속한 개체들을 평가
{	
	bestIndi = -1;
	double best_fitness;

	double eval_res = 0;
	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	double avg_fitness = 0;

	int weight = 0 ; // 현재 (level)에 속한 그룹의 가중치

	map<int, int> weights; // 각 level에 대한 가중치 맵 ( <level, weight> )

	//* 가중치 계산

	int degree = 0; //

	//for (int j = kTreeHeight-1; j >= 1; j--) // ck level 부터 가중치 적용
	for (int j = kTreeHeight-2; j >= 1; j--) //ck level 부터 가중치 적용
	{
		weights[j] = 1; // 현재 level의 가중치를 1로 초기화

		if (j == kTreeHeight-2)
		{
			weights[j] = 1;
		}
		else
		{
			weights[j] = weights[j+1]*Max_Degree;
		}

		/*
		degree = kmr->getNumNodesAtLevel(j+1) / kmr->getNumNodesAtLevel(j); // 현재 level의 degree 계산

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

	//* 가중치 계산 완료


	for (int i =0; i < Population_Size; i++)
	{		

		fitness[i] = 0; // 평가 결과 리셋		
		
		//cout << i << "번째 개체 평가" << endl;
			 
		eval_res = 0;

		// (2014.10.07) i번째 개체 평가 방법 개선

		Chromosome indi = population[i]; // i번째 개체 획득

		int locus;



		for (locus = 0; locus < indi.size(); locus++) // 개체 스캔
		{
			if ( this->popGeneTypes[i][locus] == 'G') // 유전자 타입이 Group인 경우
			{
				int level = indi[locus].first;
				int index = indi[locus].second;

				if (level == 0)
				{
					continue;
				}

				double eval = weights[level] * f (i, level, index); // 평가결과 값 누적 (i번째 개체의 해당 (level, index)의 그룹에 속한 노드들의 밀도)

				if ( this->IsFiniteNumber(eval) == false)
				{
					cout << "\n f() 함수- 반환값이 유한한 실수가 아님";
				}

				eval_res += eval;
			}

		} // for문 완료 - 개체 스캔 및 평가 완료


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
		//for (int j=1; j <  kTreeHeight-1; j++)  // i 번째 개체의 1~ (h-2) level의 그룹 평가
		{			

			// j 번째 level의 노드 수 획득
			numNodesAtLevel = kmr->getNumNodesAtLevel(j);

			if (numNodesAtLevel ==0)
			{
				//return;
				continue; // (2014.09.29 수정) - 특정 level의 kNode 수가 0일 경우 
			}

			for (int k= 0; k < numNodesAtLevel; k++) //각 하위 그룹에 속한 노드들의 무게중심을 기준으로 거리의 표준편차 계산
			{
				double eval = weights[j] * f (i, j, k); // 평가결과 값 누적 (i번째 개체의 j번째 level, k번째 그룹에 속한 노드들의 밀도)
				
				//double eval =  (1.0 / j) * f(i , j, k); // 평가결과 값 누적 (i번째 개체의 j번째 level, k번째 그룹에 속한 노드들의 무게중심으로부터의 거리의 평균 계산)
				//double eval = j * f(i , j, k); // 평가결과 값 누적 (i번째 개체의 j번째 level, k번째 그룹에 속한 노드들의 무게중심으로부터의 거리의 평균 계산)
				
				if ( this->IsFiniteNumber(eval) == false)
				{
					cout << "\n f() 함수- 반환값이 유한한 실수가 아님";
				}
				
				eval_res += eval;

				//eval_res += j * f(i, j, k); // 평가결과 값 누적 (i번째 개체의 j번째 level, k번째 그룹에 속한 노드들의 무게중심으로부터의 거리의 평균 계산)
			}	// j번째 level의 그룹 평가 완료		
		} // 모든 level의 그룹 평가 완료
		*/



		// 평가 결과 기록
		fitness[i] = eval_res;

		if (eval_res == 0)
		{
			eval_res = 0;
		}

		avg_fitness += fitness[i];

		if ( bestIndi < 0) // 변수 값이 초기화 되지 않은 경우
		{
			bestIndi = i;
			best_fitness = fitness[i];
		}
		else if ( fitness[i] > best_fitness) // 적합도가 가장 좋은 (높은) 경우에 해당하는 개체 번호 저장
		//else if ( fitness[i] < best_fitness) // 적합도가 가장 좋은 (낮은) 경우에 해당하는 개체 번호 저장
		{
			bestIndi = i;
			best_fitness = fitness[i];
		}

		


		//fitness[i] = eval_res / numNodesAtLevel;

		//this->veriIndividual(population[i], this->popGeneTypes[i]);
	} // 세대 평가 완료

	if (bestIndi < 0) // (2014.09.29)
	{
		bestIndi = 0;
		best_fitness = fitness[0];
	}

	avg_fitness /= Population_Size;

	cout << "best_fitness: " << best_fitness << endl;
	cout << "avg_fitness: " << avg_fitness << endl;
	
}

double GA::f(int individual, int level, int index) // (2014.07.04 수정) 주어진 level과 index에 대응하는 하위 그룹에 속한 노드들의 평가 결과 (무게중심으로부터의 평균 거리) 반환
{
	set<int> nodesInGrp; // 주어진 개체에서 (level, index)에 해당하는 그룹 소속 노드들의 ID 집합

	double res = 0;

	Chromosome indi = population[individual];

	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	int groupPos = 0; // 염색체 상에서 해당 하위 그룹의 시작 위치
 
	int numNodes = 0; // 해당 하위 그룹에 속한 노드들의 개수 
	double avg_X = 0; //				"			x 좌표의 평균
	double avg_Y = 0; //				"			y 좌표의 평균
	double avg_dist = 0; // 무게 중심으로부터 노드들까지의 거리의 평균

	unsigned int i = 0;

	// 해당 하위 그룹의 무게중심 (COG) 계산, 노드 개수 계산
	
	struct _tagKNode* kNode = NULL;
	
	for ( i = 0; i < indi.size(); i++) // 해에 해당하는 개체 스캔
	{
		if ( this->popGeneTypes[individual][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // 주어진 level과 index에 대응되는 그룹 정보를 포함하는 유전자 발견
			{
				kNode = kmr->getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			}
			else if ( indi[i].first == level && indi[i].second > index) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}	
			
			else if (kNode != NULL && indi[i].first < level) // 해당 그룹에 대한 정보 스캔 완료
			//else if ( indi[i].first > level) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}		
			
		} // 유전자 타입이 'G'인 경우 처리
		
		else if ( popGeneTypes[individual][i] == 'N' && kNode != NULL)  // 해당 물리적 그룹 내의 노드 정보를 포함하는 유전자 발견		
		{
			if( kNode->level == level && kNode->index == index) // 해당 물리적 그룹 내의 노드 정보를 포함하는 유전자 발견
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
					nodesInGrp.insert(nodeID); // (2014.08.29) 해당 그룹의 노드 집합 구성
				}
			}			
		}
	}  // for 문 종료 - 스캔 완료 - 해당 하위 그룹의 무게 중심과 노드 집합 계산 완료

	
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

	//* 2014.09.01 평가 함수 개선

	double totalArea = FIELD_WIDTH * FIELD_HEIGHT; // 전 영역의 넓이 구함
	double expectedArea = 0;// 네트워크의 전 영역을 해당 level의 그룹의 개수로 균등하게 나누었을 때의 각 영역의 평균 넓이
	double radius = 0; // expectedArea를 원으로 가정했을 때의 반지름

	int numGroups = kmr->getNumNodesAtLevel(level); // 해당 level의 그룹의 개수 획득
	int numNodesWithinArea=0; // 해당 그룹에 속한 노드들 중, 무게중심으로부터 radius 내에 위치한 노드들의 개수


	if (kNode->isCluster == true)
	{
		radius = TRANS_RANGE;
	}
	else
	{
		expectedArea = totalArea / numGroups;
		radius = sqrt((expectedArea/PI));
	}
	// 해당 하위 그룹의 노드들 중 무게 중심으로부터 radius 내에 위치하는 노드들의 수 계산

	set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // // 해당 하위 그룹의 노드들 중 무게 중심으로부터 radius 내에 위치하는 노드들의 수 계산
	{
		int nodeID = *it_node;
		
		double distance = sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
		//double distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

		if (distance < radius)
		{
			numNodesWithinArea++;
		} 

	} // for문 완료 - 무게중심으로부터 radius 내에 위치하는 노드들의 수 계산

	res = numNodesWithinArea;

	return res;

	//* 2014.09.01 평가 함수 개선 완료

	// 해당 하위 그룹의 노드들의 무게중심을 기준으로 각 노드까지의 거리의 평균 계산
	/*
	set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // 해당 하위 그룹의 노드들의 무게 중심으로부터의 평균 거리 계산
	{
		int nodeID = *it_node;
		avg_dist +=  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

	} // for문 완료- 해당 하위 그룹의 노드들의 무게 중심으로부터의 평균 거리 계산

	if (numNodes !=0)
	{
		avg_dist /= numNodes; 
	}

	// 무게 중심으로부터 거리의 표준편차 계산

	double diff = 0; // 무게 중심으로부터의 거리의 편차 (평균 거리 기준, 절대값)
	double min_diff = -1; // 편차의 최소값

	double variance = 0; // 무게 중심으로부터의 거리의 분산
	double std_deviation = 0; // 무게 중심으로부터의 거리의 표준편차

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++)
	{
		int nodeID = *it_node;

		double distance;
			
		distance =  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));	

		diff = abs (distance -  avg_dist) ;			

		variance += pow ( diff   , 2); // 편차의 제곱을 누적

	} // 

	variance /= numNodes; // 편차의 제곱의 평균

	std_deviation = sqrt (variance); // 표준 편차 계산	

	//res = avg_dist + std_deviation; // (2014.06.18) 평가 기준 변경
	//res = std_deviation;
	res = avg_dist;

	return res;
	*/
}

double GA::testF(ofstream & out, int level, int index) // (2014.07.04 수정)
{
	set<int> nodesInGrp; // 주어진 개체에서 (level, index)에 해당하는 그룹 소속 노드들의 ID 집합

	double res = 0;

	Chromosome indi = population[this->bestIndi];

	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	int groupPos = 0; // 염색체 상에서 해당 하위 그룹의 시작 위치
 
	int numNodes = 0; // 해당 하위 그룹에 속한 노드들의 개수 
	double avg_X = 0; //				"			x 좌표의 평균
	double avg_Y = 0; //				"			y 좌표의 평균
	double avg_dist = 0; // 무게 중심으로부터 노드들까지의 거리의 평균

	unsigned int i = 0;

	int CHID = -1; // CH의 ID
	double minDist = -1; // 무게중심에서 노드까지의 최소 거리

	int nodeID;
	double dist;

	// 해당 하위 그룹의 무게중심 (COG) 계산, 노드 개수 계산

	struct _tagKNode* kNode = NULL;

	out << "level: " << level << ", index = " << index;
	out << " { ";


	for ( i = 0; i < indi.size(); i++) // 해에 해당하는 개체 스캔
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // 주어진 level과 index에 대응되는 그룹 정보를 포함하는 유전자 발견
			{
				kNode = kmr->getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			}
			else if ( indi[i].first == level && indi[i].second > index) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}
			else if (kNode != NULL && indi[i].first < level) // 해당 그룹에 대한 정보 스캔 완료
			//else if ( indi[i].first > level) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}		

		} // 유전자 타입이 'G'인 경우 처리

		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  
		{
			if( kNode->level == level && kNode->index == index) // 해당 물리적 그룹 내의 노드 정보를 포함하는 유전자 발견		
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
					nodesInGrp.insert(nodeID); // (2014.08.29) 노드 ID 그룹에 id 추가
				}
				out << nodeID << "(" << fullSetNodes[nodeID]->x << ", " << fullSetNodes[nodeID]->y << ") " ;
				//out << nodeID << "(" << nodes[nodeID]->x << ", " << nodes[nodeID]->y << ") " ;

			}
		}
	} // for 문 종료 - 스캔 완료 - 해당 하위 그룹의 무게 중심과 노드 개수 계산 완료

	out << " } " ;

	if (numNodes ==0)
	{
		out << endl;
		return 0;		
	}	
	
	avg_X /= numNodes;
	avg_Y /= numNodes;		
		
	//kNode = kmr->getKNode(level, index);

	kNode->COG_X = avg_X; // (2014.06.19) 그룹의 무게중심 정보를 해당 그룹과 연관된 kNode에 저장 
	kNode->COG_Y = avg_Y;

	// 해당 하위 그룹의 노드들의 무게중심을 기준으로 각 노드까지의 거리의 평균 계산

	set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // 해당 하위 그룹의 노드들의 무게 중심으로부터의 평균 거리 계산
	{
		int nodeID = *it_node;
		avg_dist +=  sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
		//avg_dist +=  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

	} // for문 완료- 해당 하위 그룹의 노드들의 무게 중심으로부터의 평균 거리 계산

	if (numNodes !=0)
	{
		avg_dist /= numNodes; // 노드들의 무게중심으로부터의 평균 거리 계산
	}

	/// (2014.09.02) 해당 하위 그룹이 클러스터인 경우 CH 계산

	if (kNode->isCluster == true) // 현재 그룹이 클러스터인 경우, CH 계산
	{


		 for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // 해당 그룹(클러스터)의 노드 목록 순회
		 {
			 nodeID = *it_node;
			 dist = sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
			 //dist = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

			 if ( ( minDist < 0) || ( dist < minDist ) ) // 노드에서 무게중심까지의 최소 거리와 CH ID 갱신
			 {
				 minDist = dist;
				 CHID = nodeID;
			 }
		 } // 해당 그룹의 노드 목록 순회 - CH 계산

		 if (minDist > -1) // kNode에 CH ID 저장
		 {
			 kNode->CHID = CHID;
		 }
		 else
		 {
			 cout << "CHID was not set" << endl;
		 }

	} // if문 - 현재 클러스터의 CH 계산 

	///


	// 무게 중심으로부터 거리의 표준편차 계산

	double diff = 0; // 무게 중심으로부터의 거리의 편차 (평균 거리 기준, 절대값)
	double min_diff = -1; // 편차의 최소값

	double variance = 0; // 무게 중심으로부터의 거리의 분산
	double std_deviation = 0; // 무게 중심으로부터의 거리의 표준편차

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++)
	{
		int nodeID = *it_node;

		double distance;
			
		distance =  sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));	
		//distance =  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));	

		diff = abs (distance -  avg_dist) ;			

		variance += pow ( diff   , 2); // 편차의 제곱을 누적

	} // 

	variance /= numNodes; // 편차의 제곱의 평균

	std_deviation = sqrt (variance); // 표준 편차 계산	

	res = avg_dist + std_deviation; // (2014.06.18) 평가 기준 변경

	out << ", COG: ( " << kNode->COG_X << ", " << kNode->COG_Y << "), avg_dist: " << avg_dist << ", std_deviation: " << std_deviation << endl;
	

	if ( CHID > -1)
	{
		out << "CH ID = " << CHID << endl;
	}

	//* 2014.09.01 평가 함수 개선

	double totalArea = FIELD_WIDTH * FIELD_HEIGHT; // 전 영역의 넓이 구함
	double expectedArea = 0;// 네트워크의 전 영역을 해당 level의 그룹의 개수로 균등하게 나누었을 때의 각 영역의 평균 넓이
	double radius = 0; // expectedArea를 원으로 가정했을 때의 반지름

	int numGroups = kmr->getNumNodesAtLevel(level); // 해당 level의 그룹의 개수 획득
	int numNodesWithinArea=0; // 해당 그룹에 속한 노드들 중, 무게중심으로부터 radius 내에 위치한 노드들의 개수

	expectedArea = totalArea / numGroups;

	radius = sqrt((expectedArea/PI));

	// 해당 하위 그룹의 노드들 중 무게 중심으로부터 radius 내에 위치하는 노드들의 수 계산

	//set<int>::iterator it_node;

	for (it_node = nodesInGrp.begin(); it_node != nodesInGrp.end(); it_node++) // // 해당 하위 그룹의 노드들 중 무게 중심으로부터 radius 내에 위치하는 노드들의 수 계산
	{
		int nodeID = *it_node;
		double distance = sqrt (pow ( (double) fullSetNodes[nodeID]->x - avg_X, 2) + pow ( (double) fullSetNodes[nodeID]->y - avg_Y, 2));
		//double distance = sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));

		if (distance < radius)
		{
			numNodesWithinArea++;
		} 

	} // for문 완료 - 무게중심으로부터 radius 내에 위치하는 노드들의 수 계산

	out << ",  numNodesWithinArea: " << numNodesWithinArea << endl;
	//* 2014.09.01 평가 함수 개선 완료

	return res;

	/*
	double res = 0;

	Chromosome indi = population[this->bestIndi];

	int kTreeHeight = 0;
	kTreeHeight = kmr->getKTreeHeight();

	int groupPos = 0; // 염색체 상에서 해당 하위 그룹의 시작 위치
 
	int numNodes = 0; // 해당 하위 그룹에 속한 노드들의 개수 
	double avg_X = 0; //				"			x 좌표의 평균
	double avg_Y = 0; //				"			y 좌표의 평균
	double avg_dist = 0; // 무게 중심으로부터 노드들까지의 거리의 평균

	unsigned int i = 0;

	groupPos = getStartingPos (level, index);

	out << "level: " << level << ", index = " << index;
	out << " { ";

	// 해당 하위 그룹의 무게중심 (COG) 계산, 노드 개수 계산
	
	struct _tagKNode* kNode = NULL;


	for ( i = 0; i < indi.size(); i++) // 해에 해당하는 개체 스캔
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // 주어진 level과 index에 대응되는 그룹 정보를 포함하는 유전자 발견
			{
				kNode = kmr->getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			}
			else if ( indi[i].first == level && indi[i].second > index) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}
			else if ( indi[i].first > level) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}		

		} // 유전자 타입이 'G'인 경우 처리

		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  
		{
			if( kNode->level == level && kNode->index == index) // 해당 물리적 그룹 내의 노드 정보를 포함하는 유전자 발견		
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
	} // for 문 종료 - 스캔 완료 - 해당 하위 그룹의 무게 중심과 노드 개수 계산 완료

	

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

	kNode->COG_X = avg_X; // (2014.06.19) 그룹의 무게중심 정보를 해당 그룹과 연관된 kNode에 저장 
	kNode->COG_Y = avg_Y;
	

	kNode = NULL; // (2014.08.27) kNode 리셋

	// 해당 하위 그룹의 노드들의 무게중심을 기준으로 각 노드까지의 거리의 평균 계산

	for ( i = 0; i < indi.size(); i++) 
	//for ( i = groupPos; i < indi.size(); i++) 
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // 주어진 level과 index에 대응되는 그룹 정보를 포함하는 유전자 발견
			{
				kNode = kmr->getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			}
			else if ( indi[i].first == level && indi[i].second > index) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}
			else if ( indi[i].first > level) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}		

		} // 유전자 타입이 'G'인 경우 처리
		
		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  	
		{

			if( kNode->level == level && kNode->index == index) // 해당 물리적 그룹 내의 노드 정보를 포함하는 유전자 발견		
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

	// 해당 하위 그룹의 노드들의 무게중심으로부터의 거리의 표준 편차 계산

	double diff = 0; // 무게 중심으로부터의 거리의 편차 (평균 거리 기준, 절대값)
	double min_diff = -1; // 편차의 최소값
	//int idx_CH; // 무게중심으로부터 가장 가까운 노드에 대응되는 unode의 key tree에서의 인덱스 (0, 1, 2, ...) (CH)
	//int CH_ID = 0; // CH의 id

	double variance = 0; // 무게 중심으로부터의 거리의 분산
	double std_deviation = 0; // 무게 중심으로부터의 거리의 표준편차

	kNode = NULL;

	for ( i = 0; i < indi.size(); i++) 
	//for ( i = groupPos; i < indi.size(); i++) 
	{
		if ( this->popGeneTypes[bestIndi][i] == 'G')
		{
			if ( indi[i].first == level && indi[i].second == index) // 주어진 level과 index에 대응되는 그룹 정보를 포함하는 유전자 발견
			{
				kNode = kmr->getKNode(level, index); // 해당 물리적 그룹에 대응되는 kNode로 이동
			}
			else if ( indi[i].first == level && indi[i].second > index) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}
			else if ( indi[i].first > level) // 해당 그룹에 대한 정보 스캔 완료
			{
				break;
			}		

		} // 유전자 타입이 'G'인 경우 처리

		else if ( popGeneTypes[bestIndi][i] == 'N' && indi[i].first != -1 && kNode != NULL)  	
		{

			if( kNode->level == level && kNode->index == index) // 해당 물리적 그룹 내의 노드 정보를 포함하는 유전자 발견		
			{
		
				int nodeID;
			
				nodeID = indi[i].second;
			
				double distance;
			
				distance =  sqrt (pow ( (double) nodes[nodeID]->x - avg_X, 2) + pow ( (double) nodes[nodeID]->y - avg_Y, 2));	

				diff = abs (distance -  avg_dist) ;			

				variance += pow ( diff   , 2); // 편차의 제곱을 누적

			}
		}		
	}

	variance /= numNodes; // 편차의 제곱의 평균

	std_deviation = sqrt (variance); // 표준 편차 계산

	//res = std_deviation;

	out << ", COG: ( " << kNode->COG_X << ", " << kNode->COG_Y << "), avg_dist: " << avg_dist << ", std_deviation: " << std_deviation << endl;

	res = avg_dist + std_deviation; // (2014.06.18) 평가 기준 변경

	
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

	int degree = kmr->getNumNodesAtLevel(level) / kmr->getNumNodesAtLevel(level-1); // 상위 수준의 degree 구함

	int parentLevelIndex = index/ degree; // 상위 수준 그룹의 index 구함

	int indexInParentGroup = index % degree; // 상위 수준 그룹 내에서의 index 구함

	// 현재 그룹에 속한 유전자 개수 구함

	int numNodesInCurLevel = kmr->getNumNodesAtLevel(level);// 현재 level에 속한 노드 수
	int numGenesInGroup = 0; // 현재 에 속한 유전자 개수

	for (int i = level; i < kmr->getKTreeHeight(); i++)
	{			
		int numNodesAtLevel = kmr->getNumNodesAtLevel(i); // level (level~ h-1) 까지 노드 수
		numGenesInGroup += numNodesAtLevel/numNodesInCurLevel; // 각 level의 노드 수를 L1의 노드 수로 나눈 결과를 누적
	}

	int parentGroupPos = getStartingPos(level-1, parentLevelIndex); // 상위 수준 그룹의 시작 위치 반환

	startingPos = parentGroupPos + 1 + indexInParentGroup * numGenesInGroup;

	return startingPos;
	

}

void GA::select() // (2014.06.17 수정) 평가결과가 우수한 개체를 선택 - tournament selection 기반
{
	int match = 0; // 비교 상대

	//map<int, pair<int, int>> t_population[Population_Size];// 임시 세대 (연산을 위해 사용)
	/*
	for (int i = 0; i < Population_Size; i++) // 선택을 위해 기존 세대 정보를 저장
	{
		//t_population[i] = temp_population[i];
		this->veriIndividual(temp_population[i], this->tPopGeneTypes[i]);
	}
	*/
	// (2014.05.06) 여기까지 이상 없음!
	
	//for (int i = 0; i < Population_Size /2 ; i++) // (2014.06.17 수정) 조기수렴 방지를 위해 세대를 구성하는 개체 중 절반만 이전 세대에서 선택하고 나머지는 새로 생성
	for (int i = 0; i < Population_Size; i++) // 현 세대의 i 번째 개체와 임의로 선택된 개체를 비교하여 더 우수한 개체를 선택
	{											// 평가 결과가 작은 개체가 더 우수함 (평균 거리)

		if ( i==bestIndi || rand()%4  > 0) // 3/4의 확률로 이전 세대에서 선택되도록 함 
		//if ( i==bestIndi || rand()%2  > 0) // (2014.06.18)  4/5의 확률로 이전 세대에서 선택되도록 함 - 이전 세대에서 최고의 개체이었던 경우 남겨둠
		{
			temp_population[i].clear();

			//this->temp_population[i] = this->population[i];
			//this->temp_population[i] = this->population[(i+1)%Population_Size];		

			match = gen2.genRandInt(Population_Size);

			bool doMutation = false; 

			int r = 0;

			r = gen5.genRandInt(100);

			if ( r < Pm *100) // 확률적으로 변이 (mutation)
			{
				doMutation = true;
			}

			if (fitness[i] > fitness[match]) // 평가함수 값이 높은 것이 선호됨
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
				//veriIndividual(population[match], this->popGeneTypes[match]); // population[match] 이상 없음
				//veriIndividual(population[match], geneTypes[match]); // population[match] 이상 없음
			
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
				Gene gene = std::make_pair(-1, -1); // 유전자의 첫 번째와 두 번째 값을 -1로 초기화 

				population[i].insert( make_pair (j, gene)); // 현 세대의 해당 개체의 각 유전자의 값을 초기화
			
				temp_population[i].insert( make_pair (j, gene)); // 임시 세대의 해당 개체각 유전자의 값을 0으로 초기화

				this->popGeneTypes[i].insert(std::pair<int, char> (j, '\0' ));			
			}	

			this->writeGroupIDs(root, i, geneIdx);// 그룹 id에 대응되는 유전자 기록 (key tree parsing)	
			this->writeNodeIDs(i);// i번째 individual의 노드 id에 대응되는 유전자 기록 (random)

			temp_population[i] = population[i];
			this->tPopGeneTypes[i] = this->popGeneTypes[i];		
		}
		
	}

	// (2014.06.17) 한 세대의 절반에 해당하는 개체는 새로 생성
	//for (int i = Population_Size /2; i < Population_Size; i++)
	
}

void GA::crossover() // 선택된 개체들을 교배
{
	int cp1 = 0;
	int cp2 = 0;

	int rand_index = 0;
	
	int geneIdx_offspring1 = 0; // offspring1에 유전자를 기록하기 위한 인덱스 (다음에 기록될 위치)
	int geneIdx_offspring2 = 0; //  offspring2에 유전자를 기록하기 위한 인덱스 (다음에 기록될 위치)

	// crossing point 결정 - root level+1의 그룹 중 하나를 임의로 선택하여 그룹의 시작 지점을 cp1, 끝 지점을 cp2로 지정
	
	//int numNodesInL1 = kmr->getNumNodesAtLevel(root->level+1);
	
	// 1) subtree의 root를 기준으로 level 1의 kNode 수 구함

	int numNodesInL1 = 0; // subtree의 root를 기준으로 level 1에 존재하는 kNode의 개수

	struct _tagKNode* tempKNode = NULL;

	tempKNode = root->leftChild;

	while (tempKNode !=NULL)
	{
		numNodesInL1++; // root level + 1에 존재하는 kNode의 개수 계산
		tempKNode = tempKNode->rightSibling;
	}

	// 2) root level + 1 에 존재하는 그룹 중 하나를 랜덤하게 선택

	//int numNodesInL1 = kmr->getNumNodesAtLevel(1);
	rand_index = gen3.genRandInt(numNodesInL1);	


	// 3)  cp1, cp2 계산

	if (root->isCluster == true)
	{
		cp1=1;
		cp2= cp1+ numNodesInL1;
	}
	else
	{
		int group_i = 0;
		tempKNode = root->leftChild;

		cp1= 1; // root 노드의 유전자 포함

		while ( tempKNode != NULL)
		{
			if (group_i++ >= rand_index) // group_i가 선택된 그룹의 인덱스와 동일하면 반복문 탈출 - 이전 그룹의 kNOde 수 누적
			{
				cp2 = cp1 + kmr->getNumKNodes(tempKNode); // 선택된 그룹의 kNode 개수를 cp2에 누적
				break;
			}

			cp1 += kmr->getNumKNodes(tempKNode); // group_i 번째 그룹의 kNode 수 계산 - cp1 누적

			tempKNode = tempKNode->rightSibling;
		
		} // 선택된 그룹의 이전 그룹들의 kNode 개수 누적 -> cp1 계산	
	}

	/*
	int numGenesInBB = 0; // building block에 속한 유전자 개수

	
	for (int i = root->level+1; i < kmr->getKTreeHeight(); i++)
	//for (int i = 1; i < kmr->getKTreeHeight(); i++)
	{			
		int numNodesAtLevel = kmr->getNumNodesAtLevel(i); // level (1~ h-1) 까지 노드 수
		numGenesInBB += numNodesAtLevel/numNodesInL1; // 각 level의 노드 수를 L1의 노드 수로 나눈 결과를 누적
	}
	

	cp1 = 1+ rand_index * numGenesInBB;
	cp2 = cp1 + numGenesInBB;
	*/

	// parent 1, 2 선택

	for (int i = 0; i < Population_Size-1; i+=2) // crossover 발생 확률은 1이라고 가정
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

		

		// cp1, cp2 발견
		/*
		for (int g = 0; g < temp_population[parent1].size(); g++) 
		{
			cp2 = g;

			if (geneTypes[parent1][g] == 'G' && temp_population[parent1][g].first == 1 )
			{
				if ( temp_population[parent1][g].second == rand_index) // index가 난수와 동일하면 cp1에 값 대입
				{
					cp1 = g;					
				}
				else if (temp_population[parent1][g].second > rand_index) // index가 난수보다 크면 loop 탈출
				{
					break;
				}				
			}				
		}

		if ( cp2 == temp_population[parent1].size()-1) // cp1 ~ (cp2-1) 까지 초기화가 일어나므로, cp2가 개체길이 보다 1 작을 경우 +1증가시켜 줌
		{
			cp2 = temp_population[parent1].size();
		}
		*/

		// cp1, cp2 발견 완료

		// cp1~cp2 사이의 영역에 존재하는 유전자 초기화
		for (unsigned int g = (unsigned int) cp1; g < (unsigned int) cp2; g++)
		{
			if (this->tPopGeneTypes[parent1][g] == 'N')
			//if (geneTypes[parent1][g] == 'N')
			{
				temp_population[parent1][g] = std::make_pair (-1, -1); // BB 영역 내의 gene 중 노드에 해당하는 gene의 값 재초기화
				
			}

			if (this->tPopGeneTypes[parent2][g] == 'N')			
			{
				temp_population[parent2][g] = std::make_pair (-1, -1); // BB 영역 내의 gene 중 노드에 해당하는 gene의 값 재초기화
				
			}
		}			
		

		// offspring 1 생성
		{
			// parent 1의 시작 지점부터 선택된 그룹 직전까지는 그대로 유지 (이미 parent1의 유전자가 저장되어 있음)

			//if (rand_index == numNodes-1);
				//cp2 = population[parent1].size();
			/*
			for (j = 0; j < temp_population[parent1].size(); j++) // cp1, cp2 발견
			{
				cp2 = j;

				if (geneTypes[parent1][j] == 'G' && temp_population[parent1][j].first == 1 )
				{
					if ( temp_population[parent1][j].second == rand_index) // index가 난수와 동일하면 cp1에 값 대입
					{
						cp1 = j;					
					}
					else if (temp_population[parent1][j].second > rand_index) // index가 난수보다 크면 loop 탈출
					{
						break;
					}				
				}				
			}

			for (j = (unsigned int) cp1; j < (unsigned int) cp2; j++)
			{
				if (geneTypes[parent1][j] == 'N')
				{
					temp_population[parent1][j] = std::make_pair (-1, -1); // BB 영역 내의 gene 중 노드에 해당하는 gene의 값 재초기화
				}
			}			
			*/
			// parent2의 왼쪽부터 스캔 - 기존에 offspring1에 저장되지 않은 유전자이면 offspring1의 cp1~cp2 사이의 빈 슬롯 (geneType = 'N') 에 저장

			geneIdx_offspring1 = cp1; // offspring1에 기록될 위치 초기화

			for (j=0; j < p2.size(); j++)
			//for (j=0; j < temp_population[parent2].size(); j++)
			//for (j=0; j < population[parent2].size(); j++)
			{
				if ( ( this->tPopGeneTypes[parent2][j] == 'N') && (p2[j].first != -1) ) // 유전자 타입이 센서노드이고 비어 있지 않은 경우
				//if ( ( geneTypes[parent2][j] == 'N') && (p2[j].first != -1) ) // 유전자 타입이 센서노드이고 비어 있지 않은 경우
				//if ( geneTypes[parent2][j] == 'N')// 유전자 타입이 센서노드인 경우
				{
					
					bool exist = false;

					// 기존에 저장된 유전자인지 검사

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

						if (k == cp1) // cp1~cp2 사이의 영역은 검사할 필요 없음 (parent2의 유전자만 채워지며, 이 중에는 중복되는 것이 없으므로)
						{
							k = cp2;
						}

					} // 검사 종료

					if (exist == false) // 기존에 저장되지 않은 유전자이면 offspring1의 cp1~cp2 사이의 빈 슬롯에 복사 및 저장
					{
						while ( (geneIdx_offspring1 >= cp1) && (geneIdx_offspring1 < cp2) )
						{
							if (this->tPopGeneTypes[parent1][geneIdx_offspring1] == 'N')
							//if (geneTypes[parent1][geneIdx_offspring1] == 'N')
							{
								temp_population[parent1][geneIdx_offspring1] = p2[j];
								//temp_population[parent1][geneIdx_offspring1] = temp_population[parent2][j];
								geneIdx_offspring1++; // 다음에 기록될 위치 갱신
								break;
							}		

							geneIdx_offspring1++; // 다음에 기록될 위치 갱신

							
						}
					} // offspring1에 parent2의 유전자 복사
				}
			} // parent2 스캔 완료
		} // offspring 1 생성 완료


		// offspring 2 생성
		{
			// parent 2의 시작 지점부터 선택된 그룹 직전까지는 그대로 유지 (이미 parent2의 유전자가 저장되어 있음)
			
			/*
			for (j = cp1; j < cp2; j++)
			{
				if (geneTypes[parent2][j] == 'N')
				{
					temp_population[parent2][j] = std::make_pair (-1, -1); // BB 영역 내의 gene 중 노드에 해당하는 gene의 값 재초기화
				}
			}	
			*/

			// parent1의 왼쪽부터 스캔 - 기존에 offspring2에 저장되지 않은 유전자이면 offspring2의 cp1~cp2 사이의 빈 슬롯 (geneType = 'N') 에 저장

			geneIdx_offspring2 = cp1; // offspring2에 기록될 위치 초기화

			for (j=0; j < p1.size(); j++)
			//for (j=0; j < temp_population[parent1].size(); j++)
			//for (j=0; j < population[parent1].size(); j++)
			{

				if ( (this->tPopGeneTypes[parent1][j] == 'N') && (p1[j].first != -1))
				//if ( geneTypes[parent1][j] == 'N') // 유전자 타입이 센서노드인 경우
				{
					bool exist = false;

					// 기존에 저장된 유전자인지 검사

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

						if (k == cp1) // cp1~cp2 사이의 영역은 검사할 필요 없음 (parent1의 유전자만 채워지며, 이 중에는 중복되는 것이 없으므로)
						{
							k = cp2;
						}


					} // 검사 종료

					if (exist == false) // 기존에 저장되지 않은 유전자이면 offspring1의 cp1~cp2 사이의 빈 슬롯에 복사 및 저장
					{
						while ( (geneIdx_offspring2 >= cp1) && (geneIdx_offspring2 < cp2) )
						{
							if (this->tPopGeneTypes[parent2][geneIdx_offspring2] == 'N')
							//if (geneTypes[parent2][geneIdx_offspring2] == 'N')
							{
								temp_population[parent2][geneIdx_offspring2] = p1[j];
								//temp_population[parent2][geneIdx_offspring2] = temp_population[parent1][j];
								geneIdx_offspring2++; // 다음에 기록될 위치 갱신
								break;
							}		

							geneIdx_offspring2++; // 다음에 기록될 위치 갱신
						}
					} // offspring2에 parent1의 유전자 복사
				}
			} // parent1 스캔 완료

		} // offspring 2 생성 완료

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

void GA::mutate() // GA-mutate() 함수 - mutation point 결정에 사용
{
	for (int i = 0; i < Population_Size; i++)
	{
		int r = 0;

		r = gen5.genRandInt(100);

		if ( r < Pm *100) // 확률적으로 변이 (mutation)
		{
				// 센서 노드 ID에 대응되는 임의의 지점 p1, p2를 구하고 두 지점의 유전자 값을 교환 (swapping)

			//this->printIndividual(temp_population[i]);

			// test 용
			//if (i==34)
			//{
			//	;
				//cout << "i==34";
			//}
			int p1 = 0;
			int p2 = 0;

			int r1 = 0; // p1, p2를 구하기 위한 난수
			int r2 = 0; 

			//int seekPoint = 0; // i번째 개체의 현재 탐색 유전자 위치

			int numNTypeGenes = 0; // 탐색 과정에서 거쳐 온 gene (type == 'N')의 개수

			r1 = gen4.genRandInt(nodes.size()-1) +1; // 최소한 1 이상의 값을 갖도록 함
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
			} // p1, p2 결정

			// p1, p2위치의 유전자 값 교환 (swapping)
			Gene temp = std::make_pair (0, 0);
			temp = temp_population[i][p1];
			temp_population[i][p1] = temp_population[i][p2];
			temp_population[i][p2] = temp;

			//veriIndividual(this->temp_population[i], this->tPopGeneTypes[i]); // 현재 개체 검증 - mutate 후에도 모든 노드의 iD를 포함하는 지 검증
		}		
		
	}
}

/*
GA::GA(int indiLen)
{
	this->indiLen = indiLen;
}
*/

void GA::writeGroupIDs(KNode* currentNode, int individual, int gene) // (2014.06.16 수정) key tree를 순회하며 그룹 id를 읽어 개체의 유전자에 기록
{
	if (currentNode ==NULL)
	{
		return;
	}	

	if (currentNode->leftChild!=NULL) // // 현재 노드가 k-node이면 해당 그룹 id를 염색체에 기록
	{
		this->population[individual][gene].first = currentNode->level;
		this->population[individual][gene].second = currentNode->index;		

		this->popGeneTypes[individual][gene] = 'G'; // genetype = Group
		//this->geneTypes[individual][gene] = 'G'; // genetype = Group
	
		geneIdx++; // 다음에 기록될 염색체의 위치를 갱신

		writeGroupIDs (currentNode->leftChild, individual, geneIdx); // 현재 노드의 leftChild에 대해 재귀 호출

		if (currentNode->isCluster == true) // 현재 노드가 클러스터 키 레벨에 해당할 경우
		//if (currentNode->leftChild->leftChild ==NULL) // 현재 노드가 클러스터 키 레벨에 해당할 경우
		{
			int numSlots; // 현재 클러스터에 속하는 빈 슬롯의 개수 (포함될 수 있는 최대 센서 노드 수)
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
			//geneIdx += degree; // 다음 level 노드 수 만큼 geneIdx 증가 (빈 슬롯 확보)
			//geneIdx += MAX_NODES_PER_CLUSTER; // 클러스터당 최대 노드 수 만큼 geneIdx 증가 (빈 슬롯 확보)
		}

		if ((currentNode != this->root) && (currentNode->rightSibling != NULL))
		{
			writeGroupIDs (currentNode->rightSibling, individual, geneIdx);  // 현재 노드의 rightSibling에 대해 재귀 호출
		}		
	}
	

	
}
void GA::writeNodeIDs(int individual) // (2014.06.16 수정) - 현재individual에 랜덤하게 노드 id를 채워 넣음
{
	// cell id 기준으로 오름차순 정렬된 노드 목록을 스캔, 랜덤하게 선택된 시작 위치에서부터 차례대로 노드 ID 저장
	int kTreeHeight = kmr->getKTreeHeight(); // key tree 높이 획득

	map<int, Node*>::iterator it_node;

	int randPos = gen.genRandInt(this->indiLen)+1; // 염색체에 노드 id를 기록할 시작 위치 결정 (랜덤)
	int currentPos; // 현재 노드 ID를 기록할 위치 (randPos에서부터 1씩 증가)

	// 만약 선택된 위치의 유전자가 빈 슬롯이 아니거나 Node type이 아니면 위치를 1씩 증가, 염색체의 끝에 도달하면 다시 시작 지점으로 이동

	/*
	while (population[individual][randPos].first != -1 || popGeneTypes[individual][randPos] != 'N') // 
	{
		randPos++;
		randPos %= indiLen;
	}
	*/

	currentPos = randPos;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++) // 노드 id를 하나씩 염색체에 기록
	{
		int id;
		//id = (*it_node).first; // ID 획득

		Node* node;
		node = (*it_node).second; // NODE 포인터 획득
		id = node->id; // (2014.09.22) 

		while ( !(popGeneTypes[individual][currentPos] == 'N' && population[individual][currentPos].first == -1)) // 빈 슬롯을 만날 때까지 진행
		{
			currentPos++;
			currentPos %= indiLen;
		}

			this->population[individual][currentPos].first = kTreeHeight; // 현재 슬롯에 현재 노드의 level 기록
			this->population[individual][currentPos].second = node->id;		  // 현재 슬롯에 현재 노드의 ID 기록
			this->popGeneTypes[individual][currentPos] = 'N'; // genetype = Node					
	
	} // 염색체에 노드 ID 기록 완료


	/*
	// 노드 목록의 처음에서부터 하나씩 노드 ID 추출, 염색체의 임의의 빈 슬롯에 기록

	
	map<int, Node*>::iterator it_node;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	{
		int id;
		id = (*it_node).first; // ID 획득

		Node* node;
		node = (*it_node).second; // NODE 포인터 획득

		int randPos = gen.genRandInt(this->indiLen)+1; // 해당 노드의 id를 기록할 위치 결정 (랜덤)

		// 해당 노드의 id 기록

		  // chromosome 스캔 (오른쪽 방향)
		  
		int currentPos = 0;

		int numMoves = 0; // 빈 슬롯 간에 이동한 횟수 

		while(numMoves < randPos) // 빈 슬롯 간에 이동 횟수가 난수와 같아질 때까지 반복
		{
			currentPos++;

			currentPos = currentPos % (this->indiLen);

			if (population[individual][currentPos].first == -1) // 이동한 슬롯이 빈 슬롯이면 이동 횟수 +1 증가
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

		int kTreeHeight = kmr->getKTreeHeight(); // key tree 높이 획득
		this->population[individual][currentPos].first = kTreeHeight; // 현재 슬롯에 현재 노드의 level 기록
		this->population[individual][currentPos].second = node->id;		  // 현재 슬롯에 현재 노드의 ID 기록
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

		out << "(" << i << ", (" << level << ", " << id << ")) " ; // 주어진 individual 출력 
		//cout << "(" << i << ", (" << level << ", " << id << ")) " << endl ; // 주어진 individual 출력 
	}
	out.close();
}

Chromosome GA::findSolution(void)
{
	// findSolution 호출 전 이미 individual length를 설정해 주어야 함 - setIndiLen() 함수 호출

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

	// (2014.06.17) 여기까지 이상 없음!

	//this->printPopulation();
	this->veriPopulation();

	int generation = 0;
	char ans = '\0'; // 세대를 계속 생성할 것인지 여부를 결정

	//while( true) // infinite loop
	while(generation < Num_Generation) // convergence test
	{
		if (root->isCluster == true) // (2014.10.08) 성능 개선을 위해, 재 구조화 지점이 ck node일 경우, 반복문 탈출
		{
			break;
		}

		cout << "\ngen no." << generation << endl;
		//cout << " evaluate() " << endl;

		evaluate(); // evaluate the current population
		// (2014.05.06) 여기까지 이상 없음!

		//cout << " select() " << endl;
				

		select();// select good individuals for the next generation
				
		// (2014.06.17) 여기까지 확인!
		//cout << " crossover() " << endl;

		crossover();// mix the selected individuals

		
		//cout << " mutate() " << endl;
		//mutate(); // (2014.05.03) select() 함수와 결합 (실행시간 단축 목적) mutate with low probability 

		
		 // move to the next generation (copy the elements of temporary population to the next population)
		for (int indi = 0; indi < Population_Size; indi++)
		{			
			population[indi].clear();
			population[indi] = temp_population[indi];			
		}		
		

		// 현 세대 검증
		//this->printPopulation();
		//this->veriPopulation();
		//this->printFitness();
		/*
		if (generation % 10 ==0 )
		{
			cout << " more generation? " << endl;
			cin >> ans;

			if ( ans == 'n') // 세대를 그만 생성하려면 'n'입력
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

map<int, char> GA::getSolGeneType() // (2014.04.28) 도출된 해의 genetype 반환
{
	return this->popGeneTypes[bestIndi];
	//return this->geneTypes[bestIndi];
}

void GA::printPopulation() // 현 세대 내 개체 출력, 평균/최대 fitness value 출력
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

void GA::veriPopulation() // 현 세대 내 개체들이 문제의 제약 조건을 만족하는 지 검증
{
	// 각 개체가 모든 노드의 ID를 포함하는가?
	// 노드 목록의 처음에서부터 하나씩 노드 ID 추출, 염색체의 임의의 빈 슬롯에 기록

	for (int i = 0; i < Population_Size; i++)
	{
		veriIndividual ( temp_population[i], this->tPopGeneTypes[i]);

		//veriIndividual( temp_population[i]);
		//veriIndividual(i); // i 번째 개체 검증
	}
}

void GA::veriIndividual(map<int, std::pair<int, int>> indi, map<int, char> geneTypes) // (2014.06.16 수정)
//void GA::veriIndividual(int indi)
{
	//Chromosome individual = population[indi]; // 현재 개체 획득
	Chromosome individual = indi; // 현재 개체 획득

	std::map <int, bool> exist; // <노드 ID, 존재 여부 (T/F) > - 노드가 존재하면 해당 노드 ID에 대응되는 bool 변수의 값을 true로 설정

	map<int, Node*>::iterator it_node;

	for (it_node = nodes.begin(); it_node != nodes.end(); it_node++)
	{
		int nID;
		Node* node;
		node = (*it_node).second;
		nID = node->id; // ID 획득	
		//nID = (*it_node).first; // ID 획득		

		exist[nID] = false;
	}


	/*
	for (unsigned int n = Node::numNodesEvctd ; n < Node::totNumNodes; n++) // 각 노드 ID의 존재 여부를 나타내는 bool 변수의 값을 false로 초기화
	//for (unsigned int n = 0 ; n < nodes.size(); n++) // 각 노드 ID의 존재 여부를 나타내는 bool 변수의 값을 false로 초기화
	{
		Node* pNode = nodes[n];

		int nID = pNode->id;

		exist[nID] = false;
	}
	*/
	for (int j = 0; j < this->indiLen; j++) // 개체에 포함된 노드 ID에 대응하는 bool 변수의 값을 true로 설정
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
		nID = node->id; // ID 획득	
		//nID = (*it_node).first; // ID 획득		

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
		nID = node->id; // ID 획득	
		//nID = (*it_node).first; // ID 획득		

		if ( exist[nID] == false)
		{
			complete = false;
			break;
		}
	}	

	if (complete == false)
	{
		cout << " error: GA의 개체가 모든 센서 노드의 id를 포함하지 않음";
		this->printIndividual(individual);
		exit(-1);
	}
}

bool GA::IsFiniteNumber(double x) // 평가 결과가 유한한 실수인지 검사 (NaN 여부 검사)
{
	 return (x <= DBL_MAX && x >= -DBL_MAX); 
}

void GA::printFitness() // 현 세대의 평균/최대 적합도 출력
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

void GA::testSolution() // (2014.06.18) 도출된 해 분석 (level, index에 따른 각 그룹의 평가결과, 무게중심으로부터 거리의 평균/표준편차 출력)
{
	int kTreeHeight;

	int startingLevel = 0;

	kTreeHeight = kmr->getKTreeHeight();

	ofstream out("tst_solution.txt");

	// (2014.10.07) testF 호출 과정 개선
	Chromosome indi = population[this->bestIndi];

	for (int locus = 0; locus < indi.size(); locus++) // 최종 해 스캔 및 각 그룹 평가
	{
		int level = indi[locus].first;
		int index = indi[locus].second;

		if (level > 0)
		{
			testF(out, level, index);
		}
	} // 최종 해 평가 과정 완료

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
	//for (int j= root->level; j <  kTreeHeight-1; j++)  // solution 개체의 root level~ (h-2) level의 그룹 평가
	//for (int j=1; j <  kTreeHeight-1; j++)  // solution 개체의 1~ (h-2) level의 그룹 평가
	{
		int numNodesAtLevel = kmr->getNumNodesAtLevel(j) ;

		for (int k= 0; k < numNodesAtLevel; k++) // 주어진 그룹에 대해 test_f() 함수 호출
		{	
			testF(out, j, k);		
		}
	}
	*/
	out.close();	
}

void GA::quicksort( int start, int finish) // (2014.09.02) 노드 목록을 cellID 기준으로 오름차순 정렬
{
	if (start < finish)
	{
		int q = partition( start, finish); // 배열 나누기
		//int q = partition( candis, start, finish); // 배열 나누기

		quicksort(start, q-1); // 좌측 분할 정렬
		quicksort(q+1, finish); // 우측 분할 정렬	
	}
}

int GA::partition( int start, int finish) // (2014.09.02) quicksort에 필요한 분할 함수
{
	double val1 = 0;  // j번째 노드의 cellID
	double val2= 0;	// finish 위치에 있는 노드의  cellID

	int i = start - 1;

	int j = 0;

	for ( j = start; j < finish; j++) // 기준 값보다 작은 요소들과 큰 요소들로 분할
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
	// exec 호출 전 이미 individual length를 설정해 주어야 함 - setIndiLen() 함수 호출

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
