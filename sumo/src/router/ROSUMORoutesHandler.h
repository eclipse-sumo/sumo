#ifndef ROSumoRoutesHandler_h
#define ROSumoRoutesHandler_h

#include <string>
#include "ROTypedXMLRoutesLoader.h"
#include <utils/xml/AttributesHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>

class RONet;

class ROSumoRoutesHandler : public ROTypedXMLRoutesLoader {
private:
    /** the processing step
        (the vehicle types and routes must be extracted before the vehicles
        can be extracted) */
    /// the name of the current route
    std::string _currentRoute;
    /// the current step
    size_t _step;
public:
    ROSumoRoutesHandler(RONet &net, const std::string &file="");
    ~ROSumoRoutesHandler();
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
    void startRoute(const Attributes &attrs);
    void startVehicle(const Attributes &attrs);
    void startVehType(const Attributes &attrs);
    float getFloatReporting(const Attributes &attrs, AttrEnum attr,
        const std::string &id, const std::string &name) ;
private:
    /// we made the copy constructor invalid
    ROSumoRoutesHandler(const ROSumoRoutesHandler &src);
    /// we made the assignment operator invalid
    ROSumoRoutesHandler &operator=(const ROSumoRoutesHandler &src);
};

#endif
