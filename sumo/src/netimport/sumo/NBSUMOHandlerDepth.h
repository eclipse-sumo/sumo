#ifndef NBSUMOHandlerDepth_h
#define NBSUMOHandlerDepth_h

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netbuild/NLLoadFilter.h>

class NBSUMOHandlerDepth : public SUMOSAXHandler {
private:
    LoadFilter _loading;
public:
    NBSUMOHandlerDepth(LoadFilter what, bool warn, bool verbose);
    ~NBSUMOHandlerDepth();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
};

#endif

