#ifndef GUIDialog_SimSettings_h
#define GUIDialog_SimSettings_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <fx.h>

class GUIDialog_SimSettings : public FXDialogBox
{
    FXDECLARE(GUIDialog_SimSettings)
public:
    GUIDialog_SimSettings( FXMainWindow* parent, bool *quitOnEnd,
        bool *surpressEnd, bool *allowFloating);
    ~GUIDialog_SimSettings();
    long onCmdOk(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);
    long onCmdQuitOnEnd(FXObject*,FXSelector,void*);
    long onCmdSurpressEnd(FXObject*,FXSelector,void*);
    long onCmdAllowAggregated(FXObject*,FXSelector,void*);

public:
private:
    bool myAppQuitOnEnd;
    bool mySurpressEnd;
    bool myAllowFloating;
    bool *mySetAppQuitOnEnd;
    bool *mySetSurpressEnd;
    bool *mySetAllowFloating;

protected:
    GUIDialog_SimSettings() { }
};

#endif
