#ifndef GUIAbstractThreadFactory_h
#define GUIAbstractThreadFactory_h

class GUIMainWindow;
class GUILoadThread;
class GUIRunThread;
#include <utils/foxtools/FXRealSpinDial.h>

#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>


class GUIAbstractThreadFactory {
public:
    GUIAbstractThreadFactory();
    virtual ~GUIAbstractThreadFactory();
    virtual GUILoadThread *buildLoadThread(GUIMainWindow *mw,
        MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    virtual GUIRunThread *buildRunThread(GUIMainWindow *mw,
        FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);


};

#endif
