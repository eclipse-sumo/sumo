#ifndef NBNBConnection_h
#define NBNBConnection_h

#include <string>
#include "NBEdge.h"

class NBNode;

class NBConnection {
public:
    NBConnection(NBEdge *from, NBEdge *to);
    NBConnection(const std::string &fromID, NBEdge *from,
        const std::string &toID, NBEdge *to);
    NBConnection(const NBConnection &c);
    ~NBConnection();
    NBEdge *getFrom() const;
    NBEdge *getTo() const;
    bool replaceFrom(NBEdge *which, NBEdge *by);
    bool replaceTo(NBEdge *which, NBEdge *by);
    bool check();

    friend bool operator<(const NBConnection &c1, const NBConnection &c2);

private:
    NBEdge *checkFrom();
    NBEdge *checkTo();

private:
    NBEdge *myFrom, *myTo;
    std::string myFromID, myToID;
};

#endif

