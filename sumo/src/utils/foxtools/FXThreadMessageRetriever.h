#ifndef FXThreadMessageRetriever_h
#define FXThreadMessageRetriever_h

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

#include "FXBaseObject.h"

class FXThreadMessageRetriever : public FXEX::FXBaseObject
{
public:
    FXThreadMessageRetriever() { }
    ~FXThreadMessageRetriever() { }

private:
};


#endif
