#ifndef CellDriverInfoParser_h
#define CellDriverInfoParser_h

#include <fstream>

class RORouteDef;

class CellDriverInfoParser {
public:
    struct Driver {
        int start;
        int age;
        int route[3];
        float p[3];
        float cost[3];
        int lastcost;
        int lastroute;
    };
private:
    bool _useLast;
    bool _intel;
    int _routeNo;
    Driver _driver;
public:
    CellDriverInfoParser(bool useLast, bool intel);
    ~CellDriverInfoParser();
    void parseFrom(std::ifstream &strm);
    int getRouteNo() const;
    int getRouteStart() const;
    int getLast() const;
    double getAlternativeCost(size_t pos) const;
    double getAlternativePropability(size_t pos) const;
    int getRouteNo(size_t pos) const;
    void isIntel(bool value);
    void useLast(bool value);
private:
    int computeRouteNo();
};

#endif
