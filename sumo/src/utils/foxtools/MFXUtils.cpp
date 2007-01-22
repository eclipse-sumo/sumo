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

#include "MFXUtils.h"

void
MFXUtils::deleteChildren(FXWindow *w)
{
    while(w->numChildren()!=0) {
        FXWindow *child = w->childAtIndex(0);
        delete child;
    }
}

