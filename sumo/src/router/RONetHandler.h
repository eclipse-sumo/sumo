#ifndef RONetHandler_h
#define RONetHandler_h

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>

class RONet;
class OptionsCont;
class ROEdge;

class RONetHandler : public SUMOSAXHandler {
private:
    /// the options (program settings)
    OptionsCont &_options;
    /// the net to store the information into
    RONet &_net;
    /// the name of the edge/node that is currently processed
    std::string _currentName;
    /// the currently build edge
    ROEdge *_currentEdge;
public:
    RONetHandler(OptionsCont &oc, RONet &net);
    ~RONetHandler();
protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name, 
        const Attributes &attrs);
    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name, 
        const std::string &chars);
    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);
private:
    void parseEdge(const Attributes &attrs);
    void parseLane(const Attributes &attrs);
    void parseJunction(const Attributes &attrs);
    void parseoutedges(const std::string &outedges);
    void parseConnEdge(const Attributes &attrs);
    void preallocateEdges(const std::string &chars);
private:
    /// we made the copy constructor invalid
    RONetHandler(const RONetHandler &src);
    /// we made the assignment operator invalid
    RONetHandler &operator=(const RONetHandler &src);
};

#endif

