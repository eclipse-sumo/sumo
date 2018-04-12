%module libsumo

#ifdef SWIGPYTHON
%rename(edge) Edge;
%rename(inductionloop) InductionLoop;
%rename(junction) Junction;
%rename(lane) Lane;
%rename(lanearea) LaneArea;
%rename(multientryexit) MultiEntryExit;
%rename(person) Person;
%rename(poi) POI;
%rename(polygon) Polygon;
%rename(route) Route;
%rename(simulation) Simulation;
%rename(trafficlight) TrafficLight;
%rename(vehicle) Vehicle;
%rename(vehicletype) VehicleType;
#endif

%begin %{
#ifdef _MSC_VER
// ignore constant conditional expression warnings
#pragma warning(disable:4127)
#endif

#include <libsumo/TraCIDefs.h>
%}

// replacing vector instances of standard types, see https://stackoverflow.com/questions/8469138
%include "std_vector.i"
%include "std_string.i"
%template(StringVector) std::vector<std::string>;
%template(TraCIStageVector) std::vector<libsumo::TraCIStage>;

// exception handling
%include "exception.i"

// taken from here https://stackoverflow.com/questions/1394484/how-do-i-propagate-c-exceptions-to-python-in-a-swig-wrapper-library
%exception { 
    try {
        $action
    } catch (libsumo::TraCIException &e) {
        std::string s("TraCI error: "), s2(e.what());
        s = s + s2;
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    } catch (ProcessError &e) {
        std::string s("SUMO error: "), s2(e.what());
        s = s + s2;
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    } catch (...) {
        SWIG_exception(SWIG_RuntimeError, "unknown exception");
    }
}

// Add necessary symbols to generated header
%{
#include <libsumo/Edge.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/Junction.h>
#include <libsumo/LaneArea.h>
#include <libsumo/Lane.h>
#include <libsumo/MultiEntryExit.h>
#include <libsumo/Person.h>
#include <libsumo/POI.h>
#include <libsumo/Polygon.h>
#include <libsumo/Route.h>
#include <libsumo/Simulation.h>
#include <libsumo/TrafficLight.h>
#include <libsumo/Vehicle.h>
#include <libsumo/VehicleType.h>
%}

// Process symbols in header
%include "TraCIDefs.h"
%include "Edge.h"
%include "InductionLoop.h"
%include "Junction.h"
%include "LaneArea.h"
%include "Lane.h"
%include "MultiEntryExit.h"
%include "Person.h"
%include "POI.h"
%include "Polygon.h"
%include "Route.h"
%include "Simulation.h"
%include "TrafficLight.h"
%include "Vehicle.h"
%include "VehicleType.h"
