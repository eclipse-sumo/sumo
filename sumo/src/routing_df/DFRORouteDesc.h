#ifndef DFRORouteDesc_h
#define DFRORouteDesc_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utils/common/SUMOTime.h>

class ROEdge;
class DFDetector;


struct DFRORouteDesc {
    std::vector<ROEdge*> edges2Pass;
    std::string routename;
    float duration;
    float distance;
	std::vector<std::pair<SUMOReal, SUMOReal> > myProbabilities;
	int passedNo;
	const ROEdge *endDetectorEdge;
	const ROEdge *lastDetectorEdge;
    float distance2Last;
    SUMOTime duration2Last;

};


#endif
