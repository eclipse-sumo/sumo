#ifndef GUIDialog_AboutSUMO_H
#define GUIDialog_AboutSUMO_H

#include <fx.h>

class GUIDialog_AboutSUMO : public FXDialogBox
{
public:
    GUIDialog_AboutSUMO(FXWindow* parent,  const char* name,
        int x, int y);
    ~GUIDialog_AboutSUMO();
    void create();
private:
    FXIcon *myDLRIcon, *myZAIKIcon;
};

#endif
