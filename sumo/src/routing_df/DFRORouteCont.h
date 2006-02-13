#ifndef DFRORouteCont_h
#define DFRORouteCont_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <vector>
#include <map>
#include "DFRORouteDesc.h"

class DFRORouteCont {
public:
    DFRORouteCont();
	DFRORouteCont(const DFRORouteCont &s);
    ~DFRORouteCont();
    void addRouteDesc(DFRORouteDesc *desc);
    bool readFrom(const std::string &file);
    bool save(std::vector<std::string> &saved,
		const std::string &prependix, std::ostream &os/*const std::string &file*/);
    bool computed() const;
	const std::vector<DFRORouteDesc*> &get() const;
    void sortByDistance();

protected:
    class by_distance_sorter {
    public:
        /// constructor
        explicit by_distance_sorter() { }

        int operator() (DFRORouteDesc *p1, DFRORouteDesc *p2) {
            return p1->distance2Last<p2->distance2Last;
        }
    };

    std::vector<DFRORouteDesc*> myRoutes;

};

#endif
