#ifndef GUIHelpingJunction_H
#define GUIHelpingJunction_H
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <microsim/MSJunction.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIHelpingJunction
    : public MSJunction
{
public:
    static void fill(std::vector<GUIJunctionWrapper*> &list,
        GUIGlObjectStorage &idStorage);

private:
    /// Default constructor.
    GUIHelpingJunction();

    /// Copy constructor.
    GUIHelpingJunction( const GUIHelpingJunction& );

    /// Assignment operator.
    GUIHelpingJunction& operator=( const GUIHelpingJunction& );

    /// Destructor.
    ~GUIHelpingJunction();

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUIHelpingJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
