// 2014.04.15
// 문수영
// params.h
// 실험에 필요한 환경 변수 선언

#ifndef _PARAMS_H
#define _PARAMS_H

#include <cmath>

// 난수 생성 관련

const int LCM_a = 1664525; // linear congruential method - 상수 a
const int LCM_c = 1013904223; // linear congruential method - 상수 c

//const int LCM_a = 1013904223; // linear congruential method - 상수 a
//const int LCM_c = 1664525; // linear congruential method - 상수 c

// 실험 주기 관련

const int NUM_EVENTS = 30000; // (한 라운드 당) 이벤트 발생 횟수 
const int NUM_ROUNDS = 10; // 라운드 횟수

//const double Exit_Cond = 0.05; // 종료 조건 (cutoff 비율이 이 값에 도달하면 시뮬레이션 종료)
//const double Exit_Cond = 0.1; // 종료 조건 (cutoff 비율이 이 값에 도달하면 시뮬레이션 종료)
//const double Exit_Cond = 0.15; // (2014.11.30) 종료 조건
const double Exit_Cond = 0.2; // 종료 조건 (cutoff 비율이 이 값에 도달하면 시뮬레이션 종료)
//const double Exit_Cond = 0.5; // 종료 조건 (cutoff 비율이 이 값에 도달하면 시뮬레이션 종료)
//const int Exit_Cond = 50; // 종료 조건 (cutoff 비율이 이 값에 도달하면 시뮬레이션 종료)

// 에너지 소모 관련
const double txEnrg = 16.25; // 1바이트 전송 시 에너지 소모량

const double rxEnrg = 12.5; // 1바이트 수신 시 에너지 소모량

const double hashEnrg = 15; // 해쉬 연산 (혹은 MAC 연산)을 위한 에너지 소모량

// const double fuzzyEnrg =  ; // fuzzy 연산에 소모되는 에너지 

const int TREQ_MSG_LEN = 4; // (2014.05.10) TREQ 메시지 길이

// const int TRES_MSG_LEN = 10; // TRES 메시지 길이

const int DAT_MSG_LEN = 24; // 기본 데이터 메시지 길이 (without MACs ) (bytes) 

const int BLOOM_FILTER_LEN = 8; // (2014.08.21) bloom filter의 길이 (SEF 논문 참조)

//const double INIT_ENRGY = 200000; // 노드의 초기 에너지 (0.5J = 500 milli Joule = 500 * 1000 micro Joule)
const double INIT_ENRGY = 300000; // 노드의 초기 에너지 (0.5J = 500 milli Joule = 500 * 1000 micro Joule)
//const double INIT_ENRGY = 500000; // 노드의 초기 에너지 (0.5J = 500 milli Joule = 500 * 1000 micro Joule)
//const double INIT_ENRGY = 1000000; // 노드의 초기 에너지 (1J = 1000 milli Joule = 1000 * 1000 micro Joule)

const double eTh = INIT_ENRGY * 0.05; // 노드의 에너지 고갈 여부를 판단하기 위한 임계값
//const double eTh = DAT_MSG_LEN * 12.5; // 노드의 에너지 고갈 여부를 판단하기 위한 임계값

const int ID_LEN = 2; // 노드 id 길이 (16bit = 2 byte로 가정)

const int PMAC_LEN = 1; // PAIR MAC 길이 (1byte로 가정)

// 보안 프로토콜 관련

const int INTERLEAVING_T = 2; // IHA와 DIHA에서 사용되는 시스템 파라미터 t
//const int INTERLEAVING_T = 3; // IHA와 DIHA에서 사용되는 시스템 파라미터 t

const int BS_HELLO_LEN = DAT_MSG_LEN + (INTERLEAVING_T+1) * ID_LEN;
const int CLSTR_ACK_LEN = BS_HELLO_LEN;

const double REDUCED_ID_LEN = 0.125; // (1bit = 1/8 byte) 축소된 노드 ID (각 홉마다 최대 2개의 후보 노드를 가질 경우)

const double MAC_REQ_LEN = DAT_MSG_LEN + REDUCED_ID_LEN;
const double MAC_RES_LEN = DAT_MSG_LEN + PMAC_LEN; 


// 네트워크 구조 관련

const int FIELD_WIDTH = 600;
const int FIELD_HEIGHT = 600; 

//const int FIELD_WIDTH = 500;
//const int FIELD_HEIGHT = 500; 

//const int FIELD_WIDTH = 1100;
//const int FIELD_HEIGHT = 1100; 

//const int FIELD_WIDTH = 800;
//const int FIELD_HEIGHT = 800; 

const int CLSTR_WIDTH = 50;
const int CLSTR_HEIGHT = 50;

const int CELL_WIDTH = 50; // 노드 배치 시, 가상의 격자 (virtual grid) 상에 존재하는 셀들에 균일한 개수로 나누어 배치
const int CELL_HEIGHT = 50; //
const int NUM_CELL = (FIELD_WIDTH)/(CELL_WIDTH) * (FIELD_HEIGHT)/(CELL_HEIGHT); // cell의 개수
const int CELLS_PER_ROW = (FIELD_WIDTH)/(CELL_WIDTH);

const int NUM_CLSTR = (FIELD_WIDTH)/(CLSTR_WIDTH) * (FIELD_HEIGHT)/(CLSTR_HEIGHT); //  클러스터 개수 (IHA 기준. 제안 기법에서는 가변적)

//const int NUM_NODES = NUM_CELL * (INTERLEAVING_T +1+1) ; // 4 nodes / cell
const int NUM_NODES = NUM_CELL * (INTERLEAVING_T +1) ; // 3 nodes / cell

const int MIN_NNC = (INTERLEAVING_T +1);
const int MAX_NNC = 2*(INTERLEAVING_T +1) -1;

//const int NUM_NODES = 324;
//const int NUM_NODES = 972; 
//const int NUM_NODES = 2000; // 초기 노드 수

//const int TRANS_RANGE = 75; // 노드의 전송 범위
const int TRANS_RANGE = 100; // 노드의 전송 범위 - New_LKH에 의해 구성된 클러스터 크기와 간격을 고려


// (2014.10.06) 변수로 변경 - 실제 추가/제거된 노드 수 반영 
//double Addition_Ratio = 0.5; //노드 추가 비율 (0.1 ~0.9) Addition_Ratio + Eviction_Ratio = 1
//double Eviction_Ratio = 0.5; // 노드 제거 비율 (0.1 ~0.9)

const double Addition_Ratio = 0.5; // 노드 추가 비율 (0.1 ~0.9) Addition_Ratio + Eviction_Ratio = 1
 
const double Eviction_Ratio = 0.5; // 노드 제거 비율 (0.1 ~0.9)

const int BS_X = FIELD_WIDTH / 2; // BS의 좌표
const int BS_Y = FIELD_HEIGHT / 2;

//const int MAX_NODES_PER_CLUSTER = 2* (INTERLEAVING_T+1); // 클러스터 당 최대 노드 수 (2014.04.01)



// 키 분배 관련

  // KTS 관련

	const int Max_Degree = 5; // Key tree 상에서 각 level의 최대 degree

	const int Group_Key_Size = 10; // 그룹 키 길이 (bits)
	const int Max_Key_Value = pow ((double) 2,  Group_Key_Size); // 그룹 키가 가질 수 있는 최대값

  // GA 관련

	const float Pc = 1.0f; // Probability of Crossover Operation
	const float Pm = 0.01f; // Probability of Mutation Operation
	const int Population_Size = 50; // Number of individuals for each generation	
	//const int Population_Size = 50; // Number of individuals for each generation	
	//const int Num_Generation = 30; // number of generations
	const int Num_Generation = 20; // number of generations
	//const int Num_Generation = 20; // number of generations
	//const int Num_Generation = 50; // number of generations
	const double PI = 3.14; // 원주율


// 라우팅 관련

const int MAX_NUM_PARENT_CHS = 3;// 최대 후보 부모 CH 개수


// 공격 관련

const int FTR = 0; // 전체 보고서 중 위조 보고서의 비율 (%)
const int CNR =  (int) ceil((double) 2 / (INTERLEAVING_T+1) * (double)FTR ); // 훼손 노드의 비율 - 각 src 클러스터마다 평균적으로 훼손된 노드 수로 정의 - 위조 보고서 비율에 비례
//const int CNR =  (int) ceil((double) 2 / (INTERLEAVING_T+1) * 100 ); // 훼손 노드의 비율 - 각 src 클러스터마다 평균적으로 훼손된 노드 수로 정의
//const int compNodes =  (int) ceil((double) 2 / (INTERLEAVING_T+1) * 100 ); // 훼손 노드의 비율 - 각 src 클러스터마다 평균적으로 훼손된 노드 수로 정의
//const int compNodes = 2 ; // Src cluster 내에서 훼손된 노드의 수 (1~ INTERLEAVING_T)




/// SEF 관련 파라미터

const int FILTERING_PROB_SEF = 16;


/// SEF 관련 파라미터 (끝)



#endif