#ifndef Command_SaveTLSState_h
#define Command_SaveTLSState_h

#include <string>
#include <fstream>
#include "Action.h"
#include <microsim/MSNet.h>
#include <helpers/Command.h>

class MSNet;
class FileWriter;
class MSTrafficLightLogic;

class Command_SaveTLSState : public Command
{
public:
    Command_SaveTLSState(/*MSNet &net, */const std::string &tlsid, const std::string &file);
    ~Command_SaveTLSState();
    MSNet::Time execute(/*MSNet &n*/);
private:
    std::ofstream myFile;
    MSTrafficLightLogic *myLogic;
    MSNet::Time myExecTime;
};


#endif
