#ifndef GLObjectDblPassConnector_h
#define GLObjectDblPassConnector_h

#include <microsim/logging/DoublePassingConnector.h>
#include <gui/GUIGlObject.h>

class GUIGlObject;

class GLObjectDblPassConnector : public DoublePassingConnector
{
public:
    GLObjectDblPassConnector(GUIGlObject &o,
        DoubleValueSource *source,
        DoubleValueRetriever *retriever)
        : DoublePassingConnector(source, retriever), myObject(o) { }
    virtual ~GLObjectDblPassConnector() { }
    virtual bool updateEachTimestep( void ) {
        if(!myObject.active()) {
            return false;
        }
        return DoublePassingConnector::updateEachTimestep();
    }
private:
    GUIGlObject &myObject;
};

#endif
