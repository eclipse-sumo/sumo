#ifndef GUIThreadFactory_h
#define GUIThreadFactory_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


class GUIApplicationWindow;
class MFXInterThreadEventClient;
class GUILoadThread;
class GUIRunThread;
#include <utils/foxtools/FXRealSpinDial.h>

#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>


class GUIThreadFactory {
public:
    GUIThreadFactory();
    virtual ~GUIThreadFactory();
    virtual GUILoadThread *buildLoadThread(MFXInterThreadEventClient *mw,
        MFXEventQue &eq, FXEX::FXThreadEvent &ev);

    virtual GUIRunThread *buildRunThread(MFXInterThreadEventClient *mw,
        FXRealSpinDial &simDelay, MFXEventQue &eq, FXEX::FXThreadEvent &ev);


};

#endif
