#include <utility>
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <guisim/GUIEdge.h>
#include "GUIEdgeGrid.h"
#include "GUINet.h"


GUINet::GUINet()
    : MSNet(), _edgeGrid(100, 100)
{
}


GUINet::~GUINet()
{
}


const Boundery &
GUINet::getBoundery() const
{
    return _boundery;
}


void
GUINet::preInitGUINet( MSNet::Time startTimeStep )
{
    MSNet::myInstance = new GUINet();
    MSNet::myInstance->myStep = startTimeStep;
}


void
GUINet::initGUINet( std::string id, MSEdgeControl* ec, MSJunctionControl* jc,
                   MSEmitControl* emc, MSEventControl* evc,
                   DetectorCont* detectors,
                   MSNet::TimeVector dumpMeanDataIntervalls, 
                   std::string baseNameDumpFiles)
{
    MSNet::init(id, ec, jc, emc, evc, detectors, 
        dumpMeanDataIntervalls, baseNameDumpFiles, true);
    GUINet *net = static_cast<GUINet*>(MSNet::getInstance());
    net->_edgeGrid.init();
    net->_boundery = net->_edgeGrid.getBoundery();
}

Position2D 
GUINet::getJunctionPosition(const std::string &name) const
{
    MSJunction *junction = MSJunction::dictionary(name);
    return Position2D(junction->getXCoordinate(), junction->getYCoordinate());
}


Position2D 
GUINet::getVehiclePosition(const std::string &name) const
{
    MSVehicle *vehicle = MSVehicle::dictionary(name);
    const GUIEdge * const edge = 
        static_cast<const GUIEdge * const>(vehicle->getEdge());
    double pos = vehicle->pos();
    return Position2D(
        (edge->fromXPos()+edge->toXPos()) / 2.0,
        (edge->fromYPos()+edge->toYPos()) / 2.0);
}


Boundery 
GUINet::getEdgeBoundery(const std::string &name) const
{
    GUIEdge *edge = static_cast<GUIEdge*>(MSEdge::dictionary(name));
    return Boundery(
        edge->fromXPos(), edge->fromYPos(), edge->toXPos(), edge->toYPos());
}



