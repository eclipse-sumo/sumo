%module libsumo

// ignore constant conditional expression warnings
%begin %{
#ifdef _MSC_VER
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
#include <libsumo/Simulation.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
%}

// Process symbols in header
%include "TraCIDefs.h"
%include "Simulation.h"
