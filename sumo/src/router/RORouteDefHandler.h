#ifndef RORouteDefHandler_h
#define RORouteDefHandler_h

#include <string>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IDSupplier.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"

class SAX2XMLReader;

class RORouteDefHandler : public ROTypedXMLRoutesLoader {
private:
    /// generates numerical ids
    IDSupplier _idSupplier;
public:
    RORouteDefHandler(RONet &net);
    RORouteDefHandler(RONet &net, const std::string &file);
    ~RORouteDefHandler();
    ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const;
    std::string getDataName() const;
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
    std::string getVehicleID(const Attributes &attrs);
    ROEdge *getEdge(const Attributes &attrs, const std::string &purpose,
        AttrEnum which, const std::string &id);
    std::string getVehicleType(const Attributes &attrs);
    float getOptionalFloat(const Attributes &attrs,
        const std::string &name, AttrEnum which, const std::string &place);
    long getDepartureTime(const Attributes &attrs, const std::string &id);
    std::string getLane(const Attributes &attrs);
private:
    /// we made the copy constructor invalid
    RORouteDefHandler(const RORouteDefHandler &src);
    /// we made the assignment operator invalid
    RORouteDefHandler &operator=(const RORouteDefHandler &src);
};

#endif

