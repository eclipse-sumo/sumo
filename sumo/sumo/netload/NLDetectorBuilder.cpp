#include <string>
#include <iostream>
#include "../microsim/MSNet.h"
#include "../microsim/MSDetector.h"
#include "../microsim/MSInductLoop.h"
#include "../utils/UtilExceptions.h"
#include "NLDetectorBuilder.h"

using namespace std;

MSDetector *NLDetectorBuilder::buildInductLoop(const std::string &id, 
        const std::string &lane, float pos, long splInterval,
        const std::string &style, const std::string &filename) {
    // get the output style
    MSDetector::OutputStyle cstyle = convertStyle(style, id);
    // build and check the file
    std::ofstream *file = new std::ofstream(filename.c_str());
    if(!file->good())
        throw InvalidArgument("Could not open output for detector '" + id + "' for writing (file:" + filename + ").");
    // get and check the lane
    MSLane *clane = MSLane::dictionary(lane);
    if(clane==0)
        throw InvalidArgument("The lane with the id '" + lane + "' is not known.");
    return new MSInductLoop(id, clane, pos, splInterval, cstyle, file);
}

MSDetector::OutputStyle NLDetectorBuilder::convertStyle(const std::string &id, const std::string &style) {
    if(style=="GNUPLOT" || style=="GPLOT")
        return MSDetector::GNUPLOT;
    if(style=="CSV")
        return MSDetector::CSV;
    throw InvalidArgument("Unknown output style '" + style + "' while parsing the detector '" + id + "' occured.");
}
 
