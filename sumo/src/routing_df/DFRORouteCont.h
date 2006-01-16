#ifndef DFRORouteCont_h
#define DFRORouteCont_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <vector>
#include <map>
#include "DFRORouteDesc.h"

class DFRORouteCont {
public:
    DFRORouteCont();
    ~DFRORouteCont();
    void addRouteDesc(const DFRORouteDesc &desc);
    bool readFrom(const std::string &file);
    bool save(const std::string &file);
    bool computed() const;

protected:
    std::map<ROEdge*, std::vector<DFRORouteDesc> > myRoutes;

};

#endif
