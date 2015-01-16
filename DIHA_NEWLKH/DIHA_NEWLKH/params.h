// 2014.04.15
// ������
// params.h
// ���迡 �ʿ��� ȯ�� ���� ����

#ifndef _PARAMS_H
#define _PARAMS_H

#include <cmath>

// ���� ���� ����

const int LCM_a = 1664525; // linear congruential method - ��� a
const int LCM_c = 1013904223; // linear congruential method - ��� c

//const int LCM_a = 1013904223; // linear congruential method - ��� a
//const int LCM_c = 1664525; // linear congruential method - ��� c

// ���� �ֱ� ����

const int NUM_EVENTS = 30000; // (�� ���� ��) �̺�Ʈ �߻� Ƚ�� 
const int NUM_ROUNDS = 10; // ���� Ƚ��

//const double Exit_Cond = 0.05; // ���� ���� (cutoff ������ �� ���� �����ϸ� �ùķ��̼� ����)
//const double Exit_Cond = 0.1; // ���� ���� (cutoff ������ �� ���� �����ϸ� �ùķ��̼� ����)
//const double Exit_Cond = 0.15; // (2014.11.30) ���� ����
const double Exit_Cond = 0.2; // ���� ���� (cutoff ������ �� ���� �����ϸ� �ùķ��̼� ����)
//const double Exit_Cond = 0.5; // ���� ���� (cutoff ������ �� ���� �����ϸ� �ùķ��̼� ����)
//const int Exit_Cond = 50; // ���� ���� (cutoff ������ �� ���� �����ϸ� �ùķ��̼� ����)

// ������ �Ҹ� ����
const double txEnrg = 16.25; // 1����Ʈ ���� �� ������ �Ҹ�

const double rxEnrg = 12.5; // 1����Ʈ ���� �� ������ �Ҹ�

const double hashEnrg = 15; // �ؽ� ���� (Ȥ�� MAC ����)�� ���� ������ �Ҹ�

// const double fuzzyEnrg =  ; // fuzzy ���꿡 �Ҹ�Ǵ� ������ 

const int TREQ_MSG_LEN = 4; // (2014.05.10) TREQ �޽��� ����

// const int TRES_MSG_LEN = 10; // TRES �޽��� ����

const int DAT_MSG_LEN = 24; // �⺻ ������ �޽��� ���� (without MACs ) (bytes) 

const int BLOOM_FILTER_LEN = 8; // (2014.08.21) bloom filter�� ���� (SEF �� ����)

//const double INIT_ENRGY = 200000; // ����� �ʱ� ������ (0.5J = 500 milli Joule = 500 * 1000 micro Joule)
const double INIT_ENRGY = 300000; // ����� �ʱ� ������ (0.5J = 500 milli Joule = 500 * 1000 micro Joule)
//const double INIT_ENRGY = 500000; // ����� �ʱ� ������ (0.5J = 500 milli Joule = 500 * 1000 micro Joule)
//const double INIT_ENRGY = 1000000; // ����� �ʱ� ������ (1J = 1000 milli Joule = 1000 * 1000 micro Joule)

const double eTh = INIT_ENRGY * 0.05; // ����� ������ �� ���θ� �Ǵ��ϱ� ���� �Ӱ谪
//const double eTh = DAT_MSG_LEN * 12.5; // ����� ������ �� ���θ� �Ǵ��ϱ� ���� �Ӱ谪

const int ID_LEN = 2; // ��� id ���� (16bit = 2 byte�� ����)

const int PMAC_LEN = 1; // PAIR MAC ���� (1byte�� ����)

// ���� �������� ����

const int INTERLEAVING_T = 2; // IHA�� DIHA���� ���Ǵ� �ý��� �Ķ���� t
//const int INTERLEAVING_T = 3; // IHA�� DIHA���� ���Ǵ� �ý��� �Ķ���� t

const int BS_HELLO_LEN = DAT_MSG_LEN + (INTERLEAVING_T+1) * ID_LEN;
const int CLSTR_ACK_LEN = BS_HELLO_LEN;

const double REDUCED_ID_LEN = 0.125; // (1bit = 1/8 byte) ��ҵ� ��� ID (�� ȩ���� �ִ� 2���� �ĺ� ��带 ���� ���)

const double MAC_REQ_LEN = DAT_MSG_LEN + REDUCED_ID_LEN;
const double MAC_RES_LEN = DAT_MSG_LEN + PMAC_LEN; 


// ��Ʈ��ũ ���� ����

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

const int CELL_WIDTH = 50; // ��� ��ġ ��, ������ ���� (virtual grid) �� �����ϴ� ���鿡 ������ ������ ������ ��ġ
const int CELL_HEIGHT = 50; //
const int NUM_CELL = (FIELD_WIDTH)/(CELL_WIDTH) * (FIELD_HEIGHT)/(CELL_HEIGHT); // cell�� ����
const int CELLS_PER_ROW = (FIELD_WIDTH)/(CELL_WIDTH);

const int NUM_CLSTR = (FIELD_WIDTH)/(CLSTR_WIDTH) * (FIELD_HEIGHT)/(CLSTR_HEIGHT); //  Ŭ������ ���� (IHA ����. ���� ��������� ������)

//const int NUM_NODES = NUM_CELL * (INTERLEAVING_T +1+1) ; // 4 nodes / cell
const int NUM_NODES = NUM_CELL * (INTERLEAVING_T +1) ; // 3 nodes / cell

const int MIN_NNC = (INTERLEAVING_T +1);
const int MAX_NNC = 2*(INTERLEAVING_T +1) -1;

//const int NUM_NODES = 324;
//const int NUM_NODES = 972; 
//const int NUM_NODES = 2000; // �ʱ� ��� ��

//const int TRANS_RANGE = 75; // ����� ���� ����
const int TRANS_RANGE = 100; // ����� ���� ���� - New_LKH�� ���� ������ Ŭ������ ũ��� ������ ���


// (2014.10.06) ������ ���� - ���� �߰�/���ŵ� ��� �� �ݿ� 
//double Addition_Ratio = 0.5; //��� �߰� ���� (0.1 ~0.9) Addition_Ratio + Eviction_Ratio = 1
//double Eviction_Ratio = 0.5; // ��� ���� ���� (0.1 ~0.9)

const double Addition_Ratio = 0.5; // ��� �߰� ���� (0.1 ~0.9) Addition_Ratio + Eviction_Ratio = 1
 
const double Eviction_Ratio = 0.5; // ��� ���� ���� (0.1 ~0.9)

const int BS_X = FIELD_WIDTH / 2; // BS�� ��ǥ
const int BS_Y = FIELD_HEIGHT / 2;

//const int MAX_NODES_PER_CLUSTER = 2* (INTERLEAVING_T+1); // Ŭ������ �� �ִ� ��� �� (2014.04.01)



// Ű �й� ����

  // KTS ����

	const int Max_Degree = 5; // Key tree �󿡼� �� level�� �ִ� degree

	const int Group_Key_Size = 10; // �׷� Ű ���� (bits)
	const int Max_Key_Value = pow ((double) 2,  Group_Key_Size); // �׷� Ű�� ���� �� �ִ� �ִ밪

  // GA ����

	const float Pc = 1.0f; // Probability of Crossover Operation
	const float Pm = 0.01f; // Probability of Mutation Operation
	const int Population_Size = 50; // Number of individuals for each generation	
	//const int Population_Size = 50; // Number of individuals for each generation	
	//const int Num_Generation = 30; // number of generations
	const int Num_Generation = 20; // number of generations
	//const int Num_Generation = 20; // number of generations
	//const int Num_Generation = 50; // number of generations
	const double PI = 3.14; // ������


// ����� ����

const int MAX_NUM_PARENT_CHS = 3;// �ִ� �ĺ� �θ� CH ����


// ���� ����

const int FTR = 0; // ��ü ���� �� ���� ������ ���� (%)
const int CNR =  (int) ceil((double) 2 / (INTERLEAVING_T+1) * (double)FTR ); // �Ѽ� ����� ���� - �� src Ŭ�����͸��� ��������� �Ѽյ� ��� ���� ���� - ���� ���� ������ ���
//const int CNR =  (int) ceil((double) 2 / (INTERLEAVING_T+1) * 100 ); // �Ѽ� ����� ���� - �� src Ŭ�����͸��� ��������� �Ѽյ� ��� ���� ����
//const int compNodes =  (int) ceil((double) 2 / (INTERLEAVING_T+1) * 100 ); // �Ѽ� ����� ���� - �� src Ŭ�����͸��� ��������� �Ѽյ� ��� ���� ����
//const int compNodes = 2 ; // Src cluster ������ �Ѽյ� ����� �� (1~ INTERLEAVING_T)




/// SEF ���� �Ķ����

const int FILTERING_PROB_SEF = 16;


/// SEF ���� �Ķ���� (��)



#endif