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

// adding dummy init and close for easier traci -> libsumo transfer
%pythoncode %{
def init(port):
    print("Warning! To make your code usable with traci and libsumo, please use traci.start instead of traci.init.")

def close():
    simulation.close()

def start(args):
    simulation.load(args[1:])

def simulationStep(step=0):
    simulation.step(step)
%}

%typemap(out) std::map<int, std::shared_ptr<libsumo::TraCIResult> > {
    $result = PyDict_New();
    for (auto iter = $1.begin(); iter != $1.end(); ++iter) {
        const int theKey = iter->first;
        const libsumo::TraCIResult* const theVal = iter->second.get();
        const libsumo::TraCIDouble* const theDouble = dynamic_cast<const libsumo::TraCIDouble*>(theVal);
        if (theDouble != nullptr) {
            PyDict_SetItem($result, PyInt_FromLong(theKey), PyFloat_FromDouble(theDouble->value));
            continue;
        }
        const libsumo::TraCIInt* const theInt = dynamic_cast<const libsumo::TraCIInt*>(theVal);
        if (theInt != nullptr) {
            PyDict_SetItem($result, PyInt_FromLong(theKey), PyInt_FromLong(theInt->value));
            continue;
        }
        PyObject *value = SWIG_NewPointerObj(SWIG_as_voidptr(theVal), SWIGTYPE_p_libsumo__TraCIResult, 0);
        PyDict_SetItem($result, PyInt_FromLong(theKey), value);
    }
};

#endif

%begin %{
#ifdef _MSC_VER
// ignore constant conditional expression warnings
#pragma warning(disable:4127)
#endif

#include <libsumo/TraCIDefs.h>
%}


// replacing vector instances of standard types, see https://stackoverflow.com/questions/8469138
%include "std_string.i"
%include "std_vector.i"
%template(StringVector) std::vector<std::string>;
%template(TraCIStageVector) std::vector<libsumo::TraCIStage>;

// exception handling
%include "exception.i"

// taken from here https://stackoverflow.com/questions/1394484/how-do-i-propagate-c-exceptions-to-python-in-a-swig-wrapper-library
%exception { 
    try {
        $action
    } catch (libsumo::TraCIException &e) {
        const std::string s = std::string("TraCI error: ") + e.what();
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    } catch (ProcessError &e) {
        const std::string s = std::string("SUMO error: ") + e.what();
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
