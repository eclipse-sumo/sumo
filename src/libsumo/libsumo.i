%module libsumo
#define SWIG_MODULE libsumo
%include "libsumo_typemap.i"

#if defined(SWIGJAVA)
%typemap(javaimports) libsumo::Simulation "import java.lang.Runtime.Version;"
%extend libsumo::Simulation {
%proxycode %{
    public static void preloadLibraries() {
        if (Version.parse(System.getProperty("java.version")).compareTo(Version.parse("21.0.5")) < 0) {
            System.err.println("The recommended minimal Java version is 21.0.5.");
        }
        if (System.getProperty("os.name").startsWith("Windows")) {
            System.loadLibrary("libcrypto");
            System.loadLibrary("libssl");
        }
        System.loadLibrary("libsumojni");
    }
%}
}
#endif

// Add necessary symbols to generated header
%{
#include <libsumo/Edge.h>
#include <libsumo/GUI.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/Junction.h>
#include <libsumo/LaneArea.h>
#include <libsumo/Lane.h>
#include <libsumo/MultiEntryExit.h>
#include <libsumo/POI.h>
#include <libsumo/Polygon.h>
#include <libsumo/Route.h>
#include <libsumo/Simulation.h>
#include <libsumo/TrafficLight.h>
#include <libsumo/VehicleType.h>
#include <libsumo/Vehicle.h>
#include <libsumo/Person.h>
#include <libsumo/Calibrator.h>
#include <libsumo/BusStop.h>
#include <libsumo/ParkingArea.h>
#include <libsumo/ChargingStation.h>
#include <libsumo/OverheadWire.h>
#include <libsumo/Rerouter.h>
#include <libsumo/MeanData.h>
#include <libsumo/VariableSpeedSign.h>
#include <libsumo/RouteProbe.h>
%}

// Process symbols in header
%include "TraCIDefs.h"
%template(TraCILogicVector) std::vector<libsumo::TraCILogic>;
%template(TraCIStageVector) std::vector<libsumo::TraCIStage>;
%template(TraCINextStopDataVector) std::vector<libsumo::TraCINextStopData>;
%template(TraCIReservationVector) std::vector<libsumo::TraCIReservation>;
%template(TraCISignalConstraintVector) std::vector<libsumo::TraCISignalConstraint>;
%template(TraCICollisionVector) std::vector<libsumo::TraCICollision>;
#ifndef SWIGPYTHON
%template(TraCIBestLanesDataVector) std::vector<libsumo::TraCIBestLanesData>;
%template(TraCIConnectionVector) std::vector<libsumo::TraCIConnection>;
%template(TraCIJunctionFoeVector) std::vector<libsumo::TraCIJunctionFoe>;
%template(TraCILinkVector) std::vector<libsumo::TraCILink>;
%template(TraCILinkVectorVector) std::vector< std::vector<libsumo::TraCILink> >;
%template(TraCINextTLSVector) std::vector<libsumo::TraCINextTLSData>;
%template(TraCPositionVector) std::vector<libsumo::TraCIPosition>;
%template(TraCIVehicleDataVector) std::vector<libsumo::TraCIVehicleData>;

%template(TraCIResults) std::map<int, std::shared_ptr<libsumo::TraCIResult> >;
%template(SubscriptionResults) std::map<std::string, std::map<int, std::shared_ptr<libsumo::TraCIResult> > >;
%template(ContextSubscriptionResults) std::map<std::string, std::map<std::string, std::map<int, std::shared_ptr<libsumo::TraCIResult> > > >;
#endif
%include "Edge.h"
%include "GUI.h"
%include "InductionLoop.h"
%include "Junction.h"
%include "LaneArea.h"
%include "Lane.h"
%include "MultiEntryExit.h"
%include "POI.h"
%include "Polygon.h"
%include "Route.h"
#ifdef SWIGPYTHON
// GCC false positive: the "localhost" default temporary is only deleted if SWIG_IsNewObj is set
%typemap(freearg, noblock=1) const std::string& host {
%#if defined(__GNUC__)
%#pragma GCC diagnostic push
%#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
%#endif
if (SWIG_IsNewObj(res$argnum)) delete $1;
%#if defined(__GNUC__)
%#pragma GCC diagnostic pop
%#endif
}
%include "Simulation.h"
%clear const std::string& host;
#else
%include "Simulation.h"
#endif
%include "TraCIConstants.h"
%include "TrafficLight.h"
%include "VehicleType.h"
%include "Vehicle.h"
%include "Person.h"
%include "Calibrator.h"
%include "BusStop.h"
%include "ParkingArea.h"
%include "ChargingStation.h"
%include "OverheadWire.h"
%include "Rerouter.h"
%include "MeanData.h"
%include "VariableSpeedSign.h"
%include "RouteProbe.h"
