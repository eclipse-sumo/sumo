#ifndef NBSUMOHandlerEdges_h
#define NBSUMOHandlerEdges_h

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <netbuild/NLLoadFilter.h>

class NBNode;

class NBSUMOHandlerEdges : public SUMOSAXHandler {
private:
    LoadFilter _loading;
public:
    NBSUMOHandlerEdges(LoadFilter what, bool warn, bool verbose);
    ~NBSUMOHandlerEdges();
protected:
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
    void myEndElement(int element, const std::string &name);
private:
    void addEdge(const Attributes &attrs);
    NBNode *getNode(const Attributes &attrs, unsigned int id,
        const std::string &dir, const std::string &name);
    float getFloatReporting(const Attributes &attrs, AttrEnum id,
        const std::string &name, const std::string &objid);
    int getIntReporting(const Attributes &attrs, AttrEnum id,
        const std::string &name, const std::string &objid);
};

#endif

