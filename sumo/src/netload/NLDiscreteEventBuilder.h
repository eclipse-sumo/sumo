#ifndef NLDiscreteEventBuilder_h
#define NLDiscreteEventBuilder_h

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax2/Attributes.hpp>
#include <string>
#include <map>
#include <utils/xml/AttributesHandler.h>


class Command;

/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif

class NLDiscreteEventBuilder : public AttributesHandler {
public:
    enum ActionType {
        EV_SAVETLSTATE
    };

    NLDiscreteEventBuilder();
    ~NLDiscreteEventBuilder();
    void addAction(const Attributes &attrs,
        const std::string &basePath);
private:
    Command *buildSaveTLStateCommand(const Attributes &attrs,
        const std::string &basePath);

protected:
    typedef std::map<std::string, ActionType> KnownActions;
    KnownActions myActions;
};

#endif
