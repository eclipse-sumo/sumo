#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSEmitControl.h>
#include <netload/NLEdgeControlBuilder.h>
#include <netload/NLJunctionControlBuilder.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include "GUIContainer.h"

using namespace std;


GUIContainer::GUIContainer(NLEdgeControlBuilder * const edgeBuilder)
    : NLContainer(edgeBuilder)
{
}

GUIContainer::~GUIContainer()
{
}


// end of operations
GUINet *
GUIContainer::buildGUINet(MSNet::TimeVector dumpMeanDataIntervalls, 
                          std::string baseNameDumpFiles)
{
    MSEdgeControl *edges = m_pECB->build();
    MSJunctionControl *junctions = m_pJCB->build();
    MSEmitControl *emitters = new MSEmitControl("", m_pVehicles);
    GUINet::initGUINet( m_Id, edges, junctions, emitters,
        m_EventControl,
        m_pDetectors, dumpMeanDataIntervalls, baseNameDumpFiles);
    return static_cast<GUINet*>(GUINet::getInstance());
}


void
GUIContainer::addSrcDestInfo(const std::string &id, const std::string &from,
                             const std::string &to, const std::string &func)
{
    // retrieve the junctions
    MSJunction *fromJ = MSJunction::dictionary(from);
    MSJunction *toJ = MSJunction::dictionary(to);
    if(fromJ==0) {
        throw XMLIdNotKnownException("junction", from);
    }
    if(toJ==0) {
        throw XMLIdNotKnownException("junction", to);
    }
    // get the type
    GUIEdge::EdgeBasicFunction funcEnum = GUIEdge::EDGEFUNCTION_UNKNOWN;
    if(func=="normal") {
        funcEnum = GUIEdge::EDGEFUNCTION_NORMAL;
    }
    if(func=="source") {
        funcEnum = GUIEdge::EDGEFUNCTION_SOURCE;
    }
    if(func=="sink") {
        funcEnum = GUIEdge::EDGEFUNCTION_SINK;
    }
    if(funcEnum<0) {
        throw XMLIdNotKnownException("purpose", func);
    }
    // set the values
    static_cast<GUIEdgeControlBuilder*>(m_pECB)->addSrcDestInfo(id, fromJ, toJ, funcEnum);
}

void
GUIContainer::addLane(const string &id, const bool isDepartLane,
                      const float maxSpeed, const float length,
                      const float changeUrge)
{
    MSEdge *edge = m_pECB->getActiveReference();
    if(/* NLNetBuilder::check && */ edge==0) {
        throw XMLInvalidParentException();
    }
    MSLane *lane = new GUILane(id, maxSpeed, length, edge);
    if(/* NLNetBuilder::check && */ !MSLane::dictionary(id, lane))
        throw XMLIdAlreadyUsedException("Lanes", id);
    else {
        try {
            m_pECB->addLane(lane, isDepartLane);
        } catch (XMLDepartLaneDuplicationException &e) {
            throw e;
        }
    }
}


