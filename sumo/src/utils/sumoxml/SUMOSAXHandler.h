#ifndef SUMOSAXHandler_h
#define SUMOSAXHandler_h

#include <string>
#include <utils/xml/AttributesReadingGenericSAX2Handler.h>
#include <utils/common/FileErrorReporter.h>


class SUMOSAXHandler : public FileErrorReporter,
                       public AttributesReadingGenericSAX2Handler {
protected:
    /** the information whether warnings should be printed */
    bool  _warn;
    /** the information whether the builder shall be run in verbose mode */
    bool  _verbose;

public:
    SUMOSAXHandler(bool warn, bool verbose);
    ~SUMOSAXHandler();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    /// called on a XML-warning; the error is reported to the SErrorHandler
    void warning(const SAXParseException& exception);
    /// called on a XML-error; the error is reported to the SErrorHandler
    void error(const SAXParseException& exception);
    /// called on a XML-fatal error; the error is reported to the SErrorHandler
    void fatalError(const SAXParseException& exception);

};

#endif

