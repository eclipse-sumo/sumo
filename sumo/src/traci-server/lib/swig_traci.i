%module swig_traci

// Add necessary symbols to generated header
%{
#include <traci-server/lib/TraCI.h>
#include <traci-server/lib/TraCI_Simulation.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
%}

// Process symbols in header
%include "TraCI.h"
%include "../TraCIDefs.h"
%include "TraCI_Simulation.h"
