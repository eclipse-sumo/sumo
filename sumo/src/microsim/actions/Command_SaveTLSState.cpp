#include "Action.h"
#include "Command_SaveTLSState.h"
#include <microsim/MSNet.h>
//#include <microsim/FileWriter.h>
#include <microsim/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>


using namespace std;


Command_SaveTLSState::Command_SaveTLSState(/*MSNet &net, */
                                       const std::string &tlsid,
                                       const std::string &file)
{
/*
    myFileWriter = net.getActionTarget(file);
    net.integrateAction(this);
    */
    myExecTime = MSEventControl::getBeginOfTimestepEvents()->addEvent(this,
        0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myLogic = MSTrafficLightLogic::dictionary(tlsid);
    if(myLogic==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The traffic light logic to save (")
            + tlsid +
            string( ") is not given."));
        throw ProcessError();
    }
    myFile.open(file.c_str());
    if(!myFile.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The file '") + file
            + string("'to save the tl-states into could not be opened."));
        throw ProcessError();
    }
    myFile << "<sumo-output>" << endl;
}


Command_SaveTLSState::~Command_SaveTLSState()
{
    myFile << "</sumo-output>" << endl;
}


MSNet::Time
Command_SaveTLSState::execute()
{
    myFile << "   <tlsstate time=\"" << myExecTime++
        << "\" state=\"" << myLogic->buildStateList() << "\"/>" << endl;
    return 1;
}

