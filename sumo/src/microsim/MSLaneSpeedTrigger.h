#ifndef MSLaneSpeedTrigger_h
#define MSLaneSpeedTrigger_h

#include <string>
#include <helpers/Command.h>
#include "MSTriggeredXMLReader.h"
#include "MSTrigger.h"

class MSNet;
class MSLane;

class MSLaneSpeedTrigger : public MSTriggeredXMLReader,
                           public MSTrigger {
private:
    MSLane &_destLane;
    double _currentSpeed;
public:
    MSLaneSpeedTrigger(const std::string &id,
        MSNet &net, MSLane &destLane, 
        const std::string &aXMLFilename);
    ~MSLaneSpeedTrigger();
    void init(MSNet &net);
    void processNext();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
};

#endif

