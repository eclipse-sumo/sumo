#ifndef GUIDialog_AppSettings_h
#define GUIDialog_AppSettings_h

#include <fx.h>

class GUIDialog_AppSettings : public FXDialogBox
{
    FXDECLARE(GUIDialog_AppSettings)
public:
    GUIDialog_AppSettings( FXMainWindow* parent);
    ~GUIDialog_AppSettings();
    long onCmdOk(FXObject*,FXSelector,void*);
    long onCmdCancel(FXObject*,FXSelector,void*);
    long onCmdQuitOnEnd(FXObject*,FXSelector,void*);
    long onCmdSurpressEnd(FXObject*,FXSelector,void*);
    long onCmdAllowAggregated(FXObject*,FXSelector,void*);
    long onCmdAllowAggregatedFloating(FXObject*,FXSelector,void*);
    long onUpdAllowAggregatedFloating(FXObject *sender,
        FXSelector, void *ptr);

public:
private:
    bool myAppQuitOnEnd;
    bool mySurpressEnd;
    bool myAllowAggregated;
    bool myAllowAggregatedFloating;

protected:
    GUIDialog_AppSettings() { }
};

#endif
