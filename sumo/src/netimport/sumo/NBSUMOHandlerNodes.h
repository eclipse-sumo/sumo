#ifndef NBSUMOHandlerNodes_h
#define NBSUMOHandlerNodes_h

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <netbuild/NLLoadFilter.h>

class NBSUMOHandlerNodes : public SUMOSAXHandler {
private:
    LoadFilter _loading;
public:
    NBSUMOHandlerNodes(LoadFilter what, bool warn, bool verbose);
    ~NBSUMOHandlerNodes();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
private:
    void addNode(const Attributes &attrs);

};

#endif
