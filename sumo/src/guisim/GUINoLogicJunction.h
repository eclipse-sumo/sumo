#ifndef GUINoLogicJunction_H
#define GUINoLogicJunction_H
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <bitset>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNoLogicJunction.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUINoLogicJunction
    : public MSNoLogicJunction
{
public:
    /// Destructor.
    ~GUINoLogicJunction();

    /** Use this constructor only. */
    GUINoLogicJunction( const std::string &id, double x, double y,
        InLaneCont* in, const Position2DVector &shape);

private:
    Position2DVector myShape;

private:
    /// Default constructor.
    GUINoLogicJunction();

    /// Copy constructor.
    GUINoLogicJunction( const GUINoLogicJunction& );

    /// Assignment operator.
    GUINoLogicJunction& operator=( const GUINoLogicJunction& );

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUINoLogicJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
