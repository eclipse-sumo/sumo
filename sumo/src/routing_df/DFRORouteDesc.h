#ifndef DFRORouteDesc_h
#define DFRORouteDesc_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <vector>
#include <string>

class ROEdge;

struct DFRORouteDesc {
    std::vector<ROEdge*> edges2Pass;
    std::string routename;
    float duration;
    float probab;
};


#endif
