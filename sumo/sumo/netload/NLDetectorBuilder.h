#ifndef NLDetectorBuilder_h
#define NLDetectorBuilder_h

#include <string>
#include "../microsim/MSNet.h"
#include "../microsim/MSDetector.h"

class NLDetectorBuilder {
public:
    /// builds an induct loop
    static MSDetector *buildInductLoop(const std::string &id, 
        const std::string &lane, float pos, long splInterval,
        const std::string &style, const std::string &filename);
private:
    /// converts the name of an output style into it's enumeration value
    static MSDetector::OutputStyle convertStyle(const std::string &id, 
        const std::string &style);
};

#endif
