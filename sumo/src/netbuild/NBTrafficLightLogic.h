#ifndef NBTrafficLightLogic_h
#define NBTrafficLightLogic_h

#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <iostream>

class NBTrafficLightLogic {
private:
    std::string _key;
    size_t _noLinks;
    class PhaseDefinition {
    public:
        size_t              duration;
        std::bitset<64>     driveMask;
        std::bitset<64>     brakeMask;
        PhaseDefinition(size_t durationArg, std::bitset<64> driveMaskArg,
            std::bitset<64> brakeMaskArg) 
            : duration(durationArg), driveMask(driveMaskArg), 
            brakeMask(brakeMaskArg) { }
        ~PhaseDefinition() { }
        bool operator!=(const PhaseDefinition &pd) const {
            return pd.duration != duration || 
                pd.driveMask != driveMask ||
                pd.brakeMask != brakeMask;
        }
    };
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;
    PhaseDefinitionVector _phases;
public:
    NBTrafficLightLogic(const std::string &key, size_t noLinks);
    NBTrafficLightLogic(const NBTrafficLightLogic &s);
    ~NBTrafficLightLogic();
    void addStep(size_t duration, std::bitset<64> driveMask, 
        std::bitset<64> brakeMask);
    void writeXML(std::ostream &into, size_t no) const;
    void _debugWritePhases() const;
    bool equals(const NBTrafficLightLogic &logic) const;
};


#endif
