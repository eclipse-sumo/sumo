#ifndef MMLayer_h
#define MMLayer_h

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

#include <utils/common/StdDefs.h>

class MMLayer {
public:
    MMLayer() { }
    virtual ~MMLayer() { }
    virtual void simulationStep( SUMOTime start, SUMOTime step) = 0;

};

#endif
