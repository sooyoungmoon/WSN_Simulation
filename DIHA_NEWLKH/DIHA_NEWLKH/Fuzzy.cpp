// Fuzzy.cpp: implementation of the Fuzzy class.
//
//////////////////////////////////////////////////////////////////////

#include "FFLLAPI.h"
#include "Fuzzy.h"

#include <iostream>

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fuzzy::Fuzzy()
{
	initialize();
}

Fuzzy::~Fuzzy()
{

}

int Fuzzy::initialize()
{
 //cout << "hello!" ;
 model = ffll_new_model();  

    int ret_val = (int)ffll_load_fcl_file(model, "aiwisdom.fcl");  

	if (ret_val < 0)
		{
		cout << "Error Opening aiwisdom.fcl";
		// make sure the "working directory" in "Project | Settings..."
		// is set to the executable's directory if running from the MSVC IDE
		return 1;
		}

    // create a child for the model...
     child = ffll_new_child(model);



 return 0;
}

int Fuzzy::think(int eCnsmptn,int dist,int numPKeys)
{
ffll_set_value(model, child, ECONSUMP, eCnsmptn); 
ffll_set_value(model, child, DIST, dist);
ffll_set_value(model, child, NUMPKEY, numPKeys);

            // get and display the output value
int output = round(ffll_get_output_value(model, child));
 return output;
}

int Fuzzy::round(double d)
{
	if ((int)(d-0.5)==(int) d)
		return (int) d +1;
	else 
		return (int) d;
}


