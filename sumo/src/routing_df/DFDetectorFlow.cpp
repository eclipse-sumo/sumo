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


const FlowDef &
DFDetectorFlows::getFlowDef( const std::string &id, int t ) const
{
	return myCurrFlows.find(id)->second.find(t)->second;
}


const std::map< int, FlowDef > &
DFDetectorFlows::getFlowDefs( const std::string &id ) const
{
	return myCurrFlows.find(id)->second;
}


bool
DFDetectorFlows::knows( const std::string &det_id ) const
{
	return myCurrFlows.find(det_id)!=myCurrFlows.end();
}


bool
DFDetectorFlows::knows( const std::string &det_id, SUMOTime time ) const
{
    if(myCurrFlows.find(det_id)==myCurrFlows.end()) {
        return false;
    }
    const std::map< int, FlowDef > &fd = myCurrFlows.find(det_id)->second;
    return fd.find(time)!=fd.end();
}
