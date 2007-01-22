#ifndef MSEdgeContinuations_h
#define MSEdgeContinuations_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <map>
#include <vector>

class MSEdge;

class MSEdgeContinuations {
public:
    MSEdgeContinuations();
    ~MSEdgeContinuations();
    void add(MSEdge *to, MSEdge *from);
    const std::vector<MSEdge*> &getInFrontOfEdge(const MSEdge &toEdge) const;
    bool hasFurther(const MSEdge &toEdge) const;

private:
    std::map<MSEdge*, std::vector<MSEdge*> > myContinuations;
};

#endif
