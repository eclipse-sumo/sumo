#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <bitset>
#include <map>
#include "MSNet.h"
#include "MSLane.h"
#include "MSLink.h"
#include "MSRightOfWayJunction.h"
#include "MSEventControl.h"
#include "MSJunctionLogic.h"
#include "MSTrafficLightLogic.h"
#include "MSTrafficLightJunction.h"

using namespace std;

MSTrafficLightJunction::MSTrafficLightJunction(const std::string &id,
                                               double x, double y,
                                               InLaneCont* in,
                                               MSJunctionLogic* logic,
                                               MSTrafficLightLogic *tlLogic,
                                               size_t delay,
                                               size_t initStep,
                                               MSEventControl &ec)
    : MSRightOfWayJunction(id, x, y, in, logic),
    _tlLogic(tlLogic), _step(initStep)
{
    ec.addEvent(new SwitchCommand(*this), delay,
        MSEventControl::ADAPT_AFTER_EXECUTION);
    //setLinkPriorities();
}


MSTrafficLightJunction::~MSTrafficLightJunction()
{
}


MSNet::Time
MSTrafficLightJunction::nextPhase()
{
    // increment the index to the current phase
    _step = _tlLogic->nextStep(_step);
    // reset the link priorities
    setLinkPriorities();
    // set the next event
    return _tlLogic->duration(_step);
}


bool
MSTrafficLightJunction::moveFirstVehicles()
{
    collectRequests(); // Sets myRequest.
    _tlLogic->applyPhase(myRequest, _step);
    // Get myRespond from logic and check for deadlocks.
    assert( myRespond.size() == myInLanes->size() );
    myLogic->respond( myRequest, myRespond );
    assert( myRespond.size() == myInLanes->size() );
    deadlockKiller();

    moveVehicles();
    return true;
}


void
MSTrafficLightJunction::deadlockKiller()
{
    // Check for a deadlock condition ( Request != 0 but respond == 0 )
    vector< bool > nullRequest = vector< bool >( myRequest.size(), false );
    if ( myRequest == nullRequest ) {

        return;
    }

    vector< bool > nullRespond = vector< bool >( myRespond.size(), false );
    if ( myRespond == nullRespond ) {

        // Handle deadlock: Create randomly a deadlock-free request out of
        // myRequest, i.e. a "single bit" request. Then again, send it
        // through myLogic (this is neccessary because we don't have a
        // mapping between requests and lanes.)
        vector< unsigned > trueRequests;
        trueRequests.reserve( myRespond.size() );
        for ( unsigned i = 0; i < myRequest.size(); ++i ) {

            if ( myRequest[i] == true && !_tlLogic->linkClosed(_step, i) ) {

                trueRequests.push_back( i );
                assert( trueRequests.size() <= myRespond.size() );
            }
        }
        // Choose randamly an index out of [0,trueRequests.size()];
        unsigned noLockIndex = static_cast< unsigned > (
            floor( static_cast< double >( rand() ) /
                   static_cast< double >( RAND_MAX ) *
                   trueRequests.size()
                )
            );

        // Create deadlock-free request.
        vector< bool > noLockRequest =
            vector< bool >( myRequest.size(), false );
        noLockRequest[ trueRequests[ noLockIndex ] ] = true;

        // Calculate respond with deadlock-free request.
        myLogic->respond( noLockRequest, myRespond );
    }
    return;
}




void
MSTrafficLightJunction::setLinkPriorities()
{
    size_t pos = 0;
    const std::bitset<64> &linkPriorities = _tlLogic->linkPriorities(_step);
    for(InLaneCont::iterator i=myInLanes->begin(); i!=myInLanes->end(); i++) {
        (*i)->myLane->setLinkPriorities(linkPriorities, pos);
    }
}

bool 
MSTrafficLightJunction::linkClosed(const MSLink *link) const
{
    LinkMap::const_iterator i=_linkMap.find(const_cast<MSLink*>(link));
    return _tlLogic->linkClosed(_step, (*i).second);
}

void 
MSTrafficLightJunction::postloadInit()
{
    size_t pos = 0;
    for(InLaneCont::iterator i=myInLanes->begin(); i!=myInLanes->end(); i++) {
        const MSLane::LinkCont &links = (*i)->myLane->getLinkCont();
        for(MSLane::LinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            _linkMap[(*j)] = pos++;
        }
    }
    setLinkPriorities();
}

//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTrafficLightJunction.icc"
//#endif


// Local Variables:
// mode:C++
// End:
