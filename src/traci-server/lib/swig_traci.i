%module swig_traci

// ignore constant conditional expression warnings
%begin %{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
%}

// Add necessary symbols to generated header
%{
#include <traci-server/lib/TraCI.h>
#include <traci-server/lib/TraCI_Simulation.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
%}

// replacing vector instances of standard types, see https://stackoverflow.com/questions/8469138
%include "std_vector.i"
%include "std_string.i"
%template(StringVector) std::vector<std::string>;

// Process symbols in header
%include "TraCI.h"
%include "../TraCIDefs.h"
%include "TraCI_Simulation.h"
