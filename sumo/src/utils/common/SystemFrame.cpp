
#include "SystemFrame.h"
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>


bool
SystemFrame::init(bool gui, OptionsCont *oc)
{
    // initialise the output
        // check whether it is a gui-version or not, first
    if(gui) {
        // within gui-based applications, nothing is reported to the console
        MsgHandler::getErrorInstance()->report2cout(false);
        MsgHandler::getWarningInstance()->report2cout(false);
        MsgHandler::getMessageInstance()->report2cout(false);
    } else {
        // within console-based applications, report everything to the console
        MsgHandler::getErrorInstance()->report2cout(true);
        MsgHandler::getWarningInstance()->report2cout(true);
        MsgHandler::getMessageInstance()->report2cout(true);
    }
        // then, check whether be verbose
    if(oc!=0&&!oc->getBool("v")) {
        MsgHandler::getMessageInstance()->report2cout(false);
    }
        // check whether to suppress warnings
    if(oc!=0&&oc->getBool("suppress-warnings")) {
        MsgHandler::getWarningInstance()->report2cout(false);
    }
    // initialise the xml-subsystem
    bool ok = XMLSubSys::init();
    // return the state
    return ok;
}


void
SystemFrame::close(OptionsCont *oc)
{
    // delete messages
    MsgHandler::cleanupOnEnd();
    // close the xml-subsystem
    XMLSubSys::close();
    // delete build program options
    delete oc;
}

