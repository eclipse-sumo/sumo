#ifndef DoublePassingConnector_h
#define DoublePassingConnector_h

#include "DoubleValueSource.h"
#include "DoubleValueRetriever.h"
#include <microsim/MSUpdateEachTimestep.h>

class DoublePassingConnector : 
    public MSUpdateEachTimestep<DoublePassingConnector> {
public:
    DoublePassingConnector(DoubleValueSource *source, 
        DoubleValueRetriever *retriever)
        : mySource(source), myRetriever(retriever) { }
    virtual ~DoublePassingConnector() { }
    virtual bool updateEachTimestep( void ) {
        myRetriever->addValue(mySource->getValue());
        return true;
    }

private:
    DoubleValueSource *mySource;
    DoubleValueRetriever *myRetriever;
};


#endif
