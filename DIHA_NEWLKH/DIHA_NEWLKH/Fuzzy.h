// Fuzzy.h: interface for the Fuzzy class.
//
//////////////////////////////////////////////////////////////////////
#define ECONSUMP	0 // energy consumption of a candidate node
#define DIST	1 //  distance to the BS of a candidate node
#define NUMPKEY 2 // number of pairwise keys of a candidate node


#if !defined(AFX_FUZZY_H__E12415C0_AF08_4260_89EF_69482793AFAF__INCLUDED_)
#define AFX_FUZZY_H__E12415C0_AF08_4260_89EF_69482793AFAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Fuzzy  
{
public:
	int child;
	int model;
	int think(int eCnsmptn,int dist,int numPKeys);
	int initialize();
	int round(double d);
	Fuzzy();
	virtual ~Fuzzy();

private:
};

#endif // !defined(AFX_FUZZY_H__E12415C0_AF08_4260_89EF_69482793AFAF__INCLUDED_)