#ifndef NBLogicKeyBuilder_h
#define NBLogicKeyBuilder_h

#include <string>
#include <sstream>
#include "NBContHelper.h"

class NBNode;

class NBLogicKeyBuilder {
public:
    /** chars from 'a' to 'z' to get alphanumerical values instead of
        numerical only */
    static char convert[];
    static std::string buildKey(NBNode *junction,
        const EdgeCont * const edges);
    /// rotates the key by the given amount
    static std::string rotateKey(std::string key, int norot);
    /// permutates the key
    static std::string rotateKey(const std::string &run);
private:
    static void appendEdgeDescription(std::ostringstream &to,
        NBNode *junction, const EdgeCont * const edges,
        EdgeCont::const_iterator &pos);
    static void appendEdgesLaneDescriptions(std::ostringstream &to,
        const EdgeCont * const edges, NBEdge *edge,
        EdgeCont::const_iterator &pos);
    static void appendLaneConnectionDescriptions(std::ostringstream &to,
        const EdgeCont * const edges, const EdgeLaneCont *connected,
        EdgeCont::const_iterator &pos);
    static void appendDetailedConnectionDescription(std::ostringstream &to,
        const EdgeCont * const edges, const EdgeLane &edgelane,
        EdgeCont::const_iterator &pos);
};

#endif

