/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "DFDetectorFlow.h"


using namespace std;

DFDetectorFlows::DFDetectorFlows()
{
    
}

DFDetectorFlows::~DFDetectorFlows()
{
    
}

void
DFDetectorFlows::addFlow( std::string id, int t, FlowDef fd )
{
	myCurrFlows[id][t] = fd;
}

FlowDef
DFDetectorFlows::getFlowDef( std::string id, int t )
{
	return myCurrFlows[id][t];
}

std::map< int, FlowDef > &
DFDetectorFlows::getFlowDefs( std::string id )
{ 
	return myCurrFlows[id];
}
