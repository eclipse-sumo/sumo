#include "MFXUtils.h"

void
MFXUtils::deleteChildren(FXWindow *w)
{
    while(w->numChildren()!=0) {
        FXWindow *child = w->childAtIndex(0);
        delete child;
    }
}

