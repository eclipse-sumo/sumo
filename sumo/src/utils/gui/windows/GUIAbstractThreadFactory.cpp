#include <utils/foxtools/MFXEventQue.h>
#include <utils/foxtools/FXThreadEvent.h>
#include "GUIAbstractThreadFactory.h"
//#include "GUIMainWindow.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"

GUIAbstractThreadFactory::GUIAbstractThreadFactory()
{
}


GUIAbstractThreadFactory::~GUIAbstractThreadFactory()
{
}


GUILoadThread *
GUIAbstractThreadFactory::buildLoadThread(GUIMainWindow *mw,
                                  MFXEventQue &eq, FXEX::FXThreadEvent &ev)
{
    return new GUILoadThread(mw, eq, ev);
}


GUIRunThread *
GUIAbstractThreadFactory::buildRunThread(GUIMainWindow *mw,
                                 FXRealSpinDial &simDelay, MFXEventQue &eq,
                                 FXEX::FXThreadEvent &ev)
{
    return new GUIRunThread(mw, simDelay, eq, ev);
}

