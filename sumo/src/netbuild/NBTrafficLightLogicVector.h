#ifndef NBTrafficLightLogicVector_h
#define NBTrafficLightLogicVector_h

#include <vector>
#include <iostream>

class NBTrafficLightLogic;

class NBTrafficLightLogicVector {
private:
    typedef std::vector<NBTrafficLightLogic*> LogicVector;
    LogicVector _cont;
public:
    NBTrafficLightLogicVector();
    ~NBTrafficLightLogicVector();
    void add(NBTrafficLightLogic *logic);
    void add(const NBTrafficLightLogicVector &cont);
    void writeXML(std::ostream &os) const;
    bool contains(NBTrafficLightLogic *logic) const;
};

#endif
