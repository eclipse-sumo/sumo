#ifndef MSTriggeredReader_h
#define MSTriggeredReader_h

#include <string>
#include <microsim/MSNet.h>
#include <helpers/Command.h>
#include "PreStartInitialised.h"

class MSTriggeredReader : public PreStartInitialised {
protected:
    std::string _filename;
    MSNet::Time _offset;
public:
    virtual ~MSTriggeredReader();
protected:
    MSTriggeredReader(MSNet &net, const std::string &filename);
    virtual void init(MSNet &net) = 0;
    virtual void processNext() = 0;
    virtual bool readNextTriggered() = 0;
//    virtual MSNet::Time getNextStepOffset() const;
private:
    class MSTriggerCommand : public Command
    {
    private:
        MSTriggeredReader &_parent;
    public:
        MSTriggerCommand(MSTriggeredReader &parent);
        /// virtual destructor
        virtual ~MSTriggerCommand( void );
        /** Execute the command and return an offset for recurring commands
            or 0 for single-execution commands. */
        virtual MSNet::Time execute();
    };
public:
    friend class MSTriggeredReader::MSTriggerCommand;
};

#endif

