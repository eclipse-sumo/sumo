#ifndef GUIIconSubSys_h
#define GUIIconSubSys_h

#include <fx.h>
#include "GUIIcons.h"

class GUIIconSubSys {
public:
    static void init(FXApp *a);
    static FXIcon *getIcon(GUIIcon which);
    static void close();
private:
    GUIIconSubSys(FXApp *a);
    ~GUIIconSubSys();

private:
    static GUIIconSubSys *myInstance;

private:
    FXIcon *myIcons[ICON_MAX];

};


#endif
