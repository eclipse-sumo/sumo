
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <guisim/GUIEdge.h>
#include <microsim/MSJunction.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <netload/NLNetBuilder.h>
#include "GUIEdgeControlBuilder.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIEdgeControlBuilder::GUIEdgeControlBuilder(unsigned int storageSize)
    : NLEdgeControlBuilder(storageSize)
{
}

GUIEdgeControlBuilder::~GUIEdgeControlBuilder()
{
}

void
GUIEdgeControlBuilder::addEdge(const string &id)
{
    MSEdge *edge = new GUIEdge(id);
    if(!MSEdge::dictionary(id, edge)) {
        throw XMLIdAlreadyUsedException("Edge", id);
    }
    m_pEdges->push_back(edge);
}



void
GUIEdgeControlBuilder::addSrcDestInfo(const std::string &id, 
                                      MSJunction *from, MSJunction *to, 
                                      GUIEdge::EdgeBasicFunction func)
{
    GUIEdge *edge = static_cast<GUIEdge*>(MSEdge::dictionary(id));
    if(edge==0) {
        throw XMLIdNotKnownException("edge", id);
    }
    edge->initJunctions(from, to, func);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIEdgeControlBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:




