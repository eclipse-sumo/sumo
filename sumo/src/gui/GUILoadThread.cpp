#include <qthread.h>
#include <iostream>

#include <guisim/GUINet.h>
#include <guinetload/GUINetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <sumo_only/SUMOFrame.h>
#include "QSimulationLoadedEvent.h"
#include "GUIApplicationWindow.h"
#include "GUILoadThread.h"

using namespace std;

GUILoadThread::GUILoadThread(GUIApplicationWindow *mw)
    : _parent(mw)
{
}


GUILoadThread::~GUILoadThread()
{
}

void
GUILoadThread::init(const string &file)
{
    _file = file;
}


void GUILoadThread::run()
{
    OptionsCont *oc = SUMOFrame::getOptions();
    oc->set("c", _file);
    GUINet *net = 0;
    std::ostream *craw = 0;
    try {
        OptionsIO::loadConfiguration(oc);
        GUINetBuilder builder(*oc);
        net = builder.buildGUINet();
        if(net!=0) {
            SUMOFrame::postbuild(*net);
        }
        long simStartTime = oc->getLong("b");
        long simEndTime = oc->getLong("e");
        craw = SUMOFrame::buildRawOutputStream(oc);
        QThread::postEvent( _parent, 
            new QSimulationLoadedEvent(net, craw, simStartTime, simEndTime,
            string(_file)) );
//        _parent->netLoaded(net, craw, simStartTime, simEndTime, string(_file));
    } catch (...) {
        delete net;
        delete craw;
    }
    delete oc;
    exit();
}


