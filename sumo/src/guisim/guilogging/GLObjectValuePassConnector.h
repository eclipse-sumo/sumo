#ifndef GLObjectValuePassConnector_h
#define GLObjectValuePassConnector_h

#include <gui/GUIGlObject.h>
#include <microsim/MSUpdateEachTimestep.h>
#include <helpers/ValueSource.h>
#include <helpers/ValueRetriever.h>

class GUIGlObject;

template<typename T>
class GLObjectValuePassConnector :
    public MSUpdateEachTimestep<GLObjectValuePassConnector<T> >
{
public:
    GLObjectValuePassConnector(GUIGlObject &o,
        ValueSource<T> *source,
        ValueRetriever<T> *retriever)
        : myObject(o), mySource(source), myRetriever(retriever)
        { }
    virtual ~GLObjectValuePassConnector() { }
    virtual bool updateEachTimestep( void ) {
        if(!myObject.active()) {
            return false;
        }
        myRetriever->addValue(mySource->getValue());
        return true;
    }
private:
    GUIGlObject &myObject;
    ValueSource<T> *mySource;
    ValueRetriever<T> *myRetriever;
};

#endif
