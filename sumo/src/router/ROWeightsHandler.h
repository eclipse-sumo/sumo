#ifndef ROWeightsHandler_h
#define ROWeightsHandler_h

#include <string>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>

class OptionsCont;
class RONet;
class ROEdge;

class ROWeightsHandler : public GenericSAX2Handler {
private:
    /// the programm settings (options/configuration)
    OptionsCont &_options;
    /// the previously build net to store the weights in
    RONet &_net;
    /// the name of the parsed file
    const std::string &_file;
    /// the name of the value to extract; called scheme for further extensions
    std::string _scheme;
    /// the timestep that is currently being processed
    long _currentTimeBeg;
    long _currentTimeEnd;
    /// the edge the is currently being processed
    ROEdge *_currentEdge;
    /// the attributes handler
    AttributesHandler _attrHandler;
    /// enumeration over the used elements
    enum TagEnum { RO_Tag_interval, RO_Tag_edge, RO_Tag_lane };
    /// enumeration over used attributes
    enum AttrEnum { RO_ATTR_value, RO_ATTR_id, RO_ATTR_beg, RO_ATTR_end };
    /** table of relationships between the attributes in their written
        representation and their enumeration */
    static Tag  _tags[3];
public:
    ROWeightsHandler(OptionsCont &oc, RONet &net, const std::string &file);
    ~ROWeightsHandler();
protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name, const Attributes &attrs);
    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name, const std::string &chars);
    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);
private:
    void parseTimeStep(const Attributes &attrs);
    void parseEdge(const Attributes &attrs);
    void parseLane(const Attributes &attrs);
    /// we made the copy constructor invalid
    ROWeightsHandler(const ROWeightsHandler &src);
    /// we made the assignment operator invalid
    ROWeightsHandler &operator=(const ROWeightsHandler &src);
};

#endif
