#ifndef GUIContainer_h
#define GUIContainer_h

#include <netload/NLContainer.h>
#include <microsim/MSNet.h>
#include <string>

class GUINet;
class NLEdgeControlBuilder;

class GUIContainer : public NLContainer
{
public:
    GUIContainer(NLEdgeControlBuilder * const edgeBuilder);
    ~GUIContainer();
    GUINet *buildGUINet(MSNet::TimeVector dumpMeanDataIntervalls, 
        std::string baseNameDumpFiles);
    /// adds information about the source and the destination junction
    void addSrcDestInfo(const std::string &id, const std::string &from,
        const std::string &to, const std::string &func);
    void addLane(const string &id, const bool isDepartLane,
        const float maxSpeed, const float length, const float changeUrge);
private:
};

#endif
