#include "Action.h"
#include "Command_SaveTLCoupledDet.h"
#include <microsim/MSNet.h>
//#include <microsim/FileWriter.h>
#include <microsim/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSDetectorFileOutput.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>


using namespace std;


Command_SaveTLCoupledDet::Command_SaveTLCoupledDet(MSTrafficLightLogic *tll,
                                                   MSDetectorFileOutput *dtf,
                                                   unsigned int begin,
                                                   const std::string &file)
    : myLogic(tll), myDetector(dtf), myFile(file.c_str()),
    myStartTime(begin)
{
    if(!myFile.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The file '") + file
            + string("'to save the tl-states into could not be opened."));
        throw ProcessError();
    }
    myFile << myDetector->getXMLHeader()
        << myDetector->getXMLDetectorInfoStart() << endl;
    tll->addSwitchAction(this);
}


Command_SaveTLCoupledDet::~Command_SaveTLCoupledDet()
{
    myFile << myDetector->getXMLDetectorInfoEnd() << endl;
}


bool
Command_SaveTLCoupledDet::execute()
{
    unsigned int end =
        MSNet::getInstance()->getCurrentTimeStep();
    myFile << "<interval start=\"" << myStartTime << "\" stop=\"" << end
        << "\" " << myDetector->getXMLOutput( end-myStartTime )
        << " />" << endl;
    myStartTime = end;
    return true;
}



