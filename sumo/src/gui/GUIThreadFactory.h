#ifndef GUIThreadFactory_h
#define GUIThreadFactory_h

class GUIApplicationWindow;
class GUILoadThread;
class GUIRunThread;
#include <utils/foxtools/FXRealSpinDial.h>

#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>


class GUIThreadFactory {
public:
    GUIThreadFactory();
    virtual ~GUIThreadFactory();
    virtual GUILoadThread *buildLoadThread(GUIApplicationWindow *mw,
        MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    virtual GUIRunThread *buildRunThread(GUIApplicationWindow *mw,
        FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);


};

#endif
