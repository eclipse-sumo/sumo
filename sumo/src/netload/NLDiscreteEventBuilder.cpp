#include "NLDiscreteEventBuilder.h"
#include <utils/xml/AttributesHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <microsim/actions/Command_SaveTLSState.h>
#include <microsim/MSEventControl.h>
#include <utils/common/FileHelpers.h>

using namespace std;

NLDiscreteEventBuilder::NLDiscreteEventBuilder()
    : AttributesHandler(sumoattrs, noSumoAttrs)
{
    myActions["SaveTLSState"] = EV_SAVETLSTATE;
}


NLDiscreteEventBuilder::~NLDiscreteEventBuilder()
{
}


void
NLDiscreteEventBuilder::addAction(const Attributes &attrs,
                                  const std::string &basePath)
{
    string type = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
    // check whether the type was given
    if(type=="") {
        MsgHandler::getErrorInstance()->inform(
            "An action's type is not given.");
        return;
    }
    // get the numerical representation
    KnownActions::iterator i = myActions.find(type);
    if(i==myActions.end()) {
        MsgHandler::getErrorInstance()->inform(
            string("The action type '") + type + string("' is not known."));
        return;
    }
    ActionType at = (*i).second;
    // build the action
    Command *a;
    switch(at) {
    case EV_SAVETLSTATE:
        a = buildSaveTLStateCommand(attrs, basePath);
        break;
    default:
        throw 1;
    }
    if(a==0) {
        return;
    }
    // schedule the action
/*    MSEventControl::getBeginOfTimestepEvents()->addEvent(
        a, 0, MSEventControl::ADAPT_AFTER_EXECUTION);*/
}


Command *
NLDiscreteEventBuilder::buildSaveTLStateCommand(const Attributes &attrs,
                                                const std::string &basePath)
{
    // get the parameter
    string dest = getStringSecure(attrs, SUMO_ATTR_DEST, "");
    string source = getStringSecure(attrs, SUMO_ATTR_SOURCE, "*");
    // check the parameter
    if(dest==""||source=="") {
        MsgHandler::getErrorInstance()->inform(
            "Incomplete description of an 'SaveTLSState'-action occured.");
        return 0;
    }
     if(!FileHelpers::isAbsolute(dest)) {
         dest = FileHelpers::getConfigurationRelative(basePath, dest);
     }
    // build the action
    return new Command_SaveTLSState(source, dest);
}

