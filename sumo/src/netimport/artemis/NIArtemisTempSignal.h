#ifndef NIArtemisTempSignal_h
#define NIArtemisTempSignal_h

#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>

class NIArtemisTempSignal {
public:
    static void addPhase(const std::string &nodeid, 
        char phaseName, double perc, int start);
    static void addConnectionPhases(const Connection &c, 
        char startPhase, char endPhase, const std::string &group);
    static void close();
private:

    class PhaseDescription {
    public:
        PhaseDescription(double perc, size_t start);
        ~PhaseDescription();
    private:
        double myPercentage;
        size_t myBegin;
    };


    typedef std::map<char, PhaseDescription*> NameToDescMap;
    typedef std::map<std::string, NameToDescMap> NodeToDescsMap;

    static NodeToDescsMap myDescDict;


    class ConnectionPhases {
    public:
        ConnectionPhases(char startPhase, char endPhase);
        ~ConnectionPhases();
    private:
        char myStartPhase, myEndPhase;
    };

    typedef std::map<Connection, ConnectionPhases*> ConnectionToPhases;

    static ConnectionToPhases myPhasesDict;

};

#endif
