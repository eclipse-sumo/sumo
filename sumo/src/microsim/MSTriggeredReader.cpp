#include <string>
#include <microsim/MSNet.h>
#include "PreStartInitialised.h"
#include "MSTriggeredReader.h"



MSTriggeredReader::MSTriggerCommand::MSTriggerCommand(MSTriggeredReader &parent) 
    : _parent(parent) 
{ 
}


MSTriggeredReader::MSTriggerCommand::~MSTriggerCommand( void ) 
{
}


MSNet::Time 
MSTriggeredReader::MSTriggerCommand::execute() {
    _parent.processNext();
    _parent.readNextTriggered();
    return _parent._offset;
}


MSTriggeredReader::MSTriggeredReader(MSNet &net, 
                                     const std::string &filename)
    : PreStartInitialised(net), _filename(filename)
{
}

MSTriggeredReader::~MSTriggeredReader()
{
}


