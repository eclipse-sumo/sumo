#ifndef Command_SaveTLCoupledDet_h
#define Command_SaveTLCoupledDet_h

#include <string>
#include <fstream>
#include "Action.h"
#include <microsim/MSNet.h>
#include <helpers/DiscreteCommand.h>

class MSNet;
class FileWriter;
class MSTrafficLightLogic;
class MSDetectorFileOutput;

class Command_SaveTLCoupledDet : public DiscreteCommand
{
public:
    Command_SaveTLCoupledDet(MSTrafficLightLogic *tll, MSDetectorFileOutput *dtf,
        unsigned int begin, const std::string &file);
    ~Command_SaveTLCoupledDet();
    bool execute();
private:
    std::ofstream myFile;
    MSTrafficLightLogic *myLogic;
    MSDetectorFileOutput *myDetector;
    unsigned int myStartTime;
};


#endif
