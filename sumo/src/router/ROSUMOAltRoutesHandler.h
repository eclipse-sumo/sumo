#ifndef ROSUMOAltRoutesHandler_h
#define ROSUMOAltRoutesHandler_h

#include <string>
#include "ROTypedXMLRoutesLoader.h"
#include <utils/sumoxml/SUMOXMLDefinitions.h>

class RONet;
class RORouteAlternativesDef;

class ROSUMOAltRoutesHandler : public ROTypedXMLRoutesLoader {
private:
    RORouteAlternativesDef *_currentAlternatives;
    double _cost;
    double _prob;
    double _gawronBeta;
    double _gawronA;
public:
    ROSUMOAltRoutesHandler(RONet &net, double gawronBeta, 
        double gawronA, const std::string &file="");
    ~ROSUMOAltRoutesHandler();
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
    void startAlternative(const Attributes &attrs);
    void startRoute(const Attributes &attrs);
    void startVehicle(const Attributes &attrs);
    void startVehType(const Attributes &attrs);
    float getFloatReporting(const Attributes &attrs, AttrEnum attr,
        const std::string &id, const std::string &name) ;
    void endAlternative();
private:
    /// we made the copy constructor invalid
    ROSUMOAltRoutesHandler(const ROSUMOAltRoutesHandler &src);
    /// we made the assignment operator invalid
    ROSUMOAltRoutesHandler &operator=(const ROSUMOAltRoutesHandler &src);
};

#endif
