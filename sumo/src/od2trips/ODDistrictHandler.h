#ifndef ODDistrictHandler_h
#define ODDistrictHandler_h

#include <string>
#include <utility>
#include <utils/sumoxml/SUMOSAXHandler.h>

class ODDistrict;
class ODDistrictCont;

class ODDistrictHandler : public SUMOSAXHandler {
private:
    ODDistrictCont &_cont;
    ODDistrict *_current;
public:
    ODDistrictHandler(ODDistrictCont &cont, bool warn, bool verbose);
    ~ODDistrictHandler();
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
    void myEndElement(int element, const std::string &name);
    void myCharacters(int element, const std::string &name,
        const std::string &chars);
private:
    void openDistrict(const Attributes &attrs);
    void addSource(const Attributes &attrs);
    void addSink(const Attributes &attrs);
    void closeDistrict();
    std::pair<std::string, double> getValues(const Attributes &attrs,
        const std::string &type);

};

#endif
