#ifndef GUIEdgeControlBuilder_h
#define GUIEdgeControlBuilder_h

class MSJunction;

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <netload/NLEdgeControlBuilder.h>
#include <guisim/GUIEdge.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * GUIEdgeControlBuilder
 */
class GUIEdgeControlBuilder : public NLEdgeControlBuilder {

public:
    /** standard constructor; the parameter is a hint for the maximal number
        of lanes inside an edge */
    GUIEdgeControlBuilder(unsigned int storageSize=10);
    /// standard destructor
    ~GUIEdgeControlBuilder();
    /** adds an edge with the given id to the list of edges; this method
        throws an XMLIdAlreadyUsedException when the id was already used for
        another edge */
    void addEdge(const std::string &id);
   /** adds information about the source and the destination edge
        (gui-version only) */
    void addSrcDestInfo(const std::string &id, MSJunction *from,
        MSJunction *to, GUIEdge::EdgeBasicFunction func);
private:
    /** invalid copy constructor */
    GUIEdgeControlBuilder(const GUIEdgeControlBuilder &s);
    /** invalid assignment operator */
    GUIEdgeControlBuilder &operator=(const GUIEdgeControlBuilder &s);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUIEdgeControlBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:





