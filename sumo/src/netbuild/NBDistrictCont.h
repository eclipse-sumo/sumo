#ifndef NBDistrictCont_h
#define NBDistrictCont_h
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <iostream>
#include <string>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBDistrict;
class NBEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class NBDistrictCont {
private:
    /// the type of the dictionary where a node may be found by her id
    typedef std::map<std::string, NBDistrict*> DistrictCont;
    /// the instance of the dictionary
    static DistrictCont _districts;
public:
    /** adds a district to the dictionary;
        returns false if the districts already was in the dictionary */
    static bool insert(NBDistrict *edge);
    /// returns the districts that has the given id
    static NBDistrict *retrieve(const std::string &id);
    /** prints all edges */
    static void writeXML(std::ostream &into);
    /** returns the number of districts inside the container */
    static int size();
    /** deletes all districts */
    static void clear();
    /// reports how many districts were loaded
    static void report(bool verbose);
    /// adds a source to the named district
    static bool addSource(const std::string &dist, NBEdge *source,
        double weight);
    /// adds a destination to the named district
    static bool addSink(const std::string &dist, NBEdge *destination,
        double weight);
private:
    /** invalid copy constructor */
    NBDistrictCont(const NBDistrictCont &s);
    /** invalid assignment operator */
    NBDistrictCont &operator=(const NBDistrictCont &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBDistrictCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
