#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsParser.h>
#include <utils/common/MsgHandler.h>
#include "GUIFrame.h"


void
GUIFrame::fillInitOptions(OptionsCont &oc)
{
    oc.doRegister("max-gl-width", 'w', new Option_Integer(1280));
    oc.doRegister("max-gl-height", 'h', new Option_Integer(1024));
    oc.doRegister("quit-on-end", 'Q', new Option_Bool(false));
    oc.doRegister("surpress-end-info", 'S', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("configuration", 'c', new Option_FileName());
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("allow-floating-aggregated-views", 'F', new Option_Bool(false));
    oc.doRegister("disable-aggregated-views", 'A', new Option_Bool(false));
    oc.doRegister("disable-textures", 'T', new Option_Bool(false));
    oc.doRegister("verbose", 'v', new Option_Bool(false)); // !!!
}


bool
GUIFrame::checkInitOptions(OptionsCont &oc)
{
    // check whether the parameter are ok
    if(oc.getInt("w")<0||oc.getInt("h")<0) {
        MsgHandler::getErrorInstance()->inform(
            "Both the screen's width and the screen's height must be larger than zero.");
        return false;
    }
    return true;
}


