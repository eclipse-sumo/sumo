#ifndef GUINet_h
#define GUINet_h

#include <string>
#include <utility>
#include <microsim/MSNet.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/Position2D.h>
#include "GUIEdgeGrid.h"

class MSEdgeControl;
class MSJunctionControl;
class MSEmitControl;
class MSEventControl;
class MSPersonControl;

class GUINet : public MSNet
{
public:
    ~GUINet();
    const Boundery &getBoundery() const;
    static void preInitGUINet( MSNet::Time startTimeStep );
    static void initGUINet( std::string id,
        MSEdgeControl* ec, MSJunctionControl* jc, MSEmitControl* emc,
        MSEventControl* evc, MSPersonControl* wpc, DetectorCont* detectors,
        MSNet::TimeVector dumpMeanDataIntervalls, 
        std::string baseNameDumpFiles);
    Position2D getJunctionPosition(const std::string &name) const;
    Position2D getVehiclePosition(const std::string &name) const;
    Boundery getEdgeBoundery(const std::string &name) const;
protected:
    GUINet();
    GUIEdgeGrid _edgeGrid;
    Boundery _boundery;
};

#endif
