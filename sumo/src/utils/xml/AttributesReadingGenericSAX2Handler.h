#ifndef AttributesReadingGenericSAX2Handler_h
#define AttributesReadingGenericSAX2Handler_h

#include "GenericSAX2Handler.h"
#include "AttributesHandler.h"

class AttributesReadingGenericSAX2Handler : public AttributesHandler,
                                            public GenericSAX2Handler {
public:
    AttributesReadingGenericSAX2Handler() { }
    AttributesReadingGenericSAX2Handler(GenericSAX2Handler::Tag *tags,
        int noTags, AttributesHandler::Attr *attrs, int noAttrs)
        : GenericSAX2Handler(tags, noTags),
        AttributesHandler(attrs, noAttrs) { }
    virtual ~AttributesReadingGenericSAX2Handler() { }
};

#endif
