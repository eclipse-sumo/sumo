#ifndef GUIHandlerBuilder_h
#define GUIHandlerBuilder_h
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <netload/NLHandlerBuilder.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * GUIHandlerBuilder
 */
class GUIHandlerBuilder : public NLHandlerBuilder {
public:
    /// standard constructor
    GUIHandlerBuilder(NLContainer &container);
    /// standard destructor
    ~GUIHandlerBuilder();

    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag;
        this method */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);
private:
    /// adds information about the source and the destination junctions
    void addSourceDestinationInformation(const Attributes &attrs);

private:
    /** invalid copy constructor */
    GUIHandlerBuilder(const GUIHandlerBuilder &s);
    /** invalid assignment operator */
    GUIHandlerBuilder &operator=(const GUIHandlerBuilder &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUIHandlerBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
