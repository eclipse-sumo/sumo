#ifndef GenericSAX2Handler_h
#define GenericSAX2Handler_h

#include <string>
#include <stack>
#include <map>
#include <sstream>
#include <sax2/Attributes.hpp>
#include <sax2/DefaultHandler.hpp>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * GenericSAX2Handler
 * GenericSAX2Handler is an extended SAX2-DefaultHandler which provides a faster access 
 * to a enumeration of tags through a map
 * This handler has no parsing functionality which is only implemented in the 
 * derived classes, where each class solves a single step of the parsing
 */
class GenericSAX2Handler : public DefaultHandler
{
public:
   struct Tag { const char *name; int value; };
protected:
   typedef std::map<std::string, int> TagMap; 
   bool _errorOccured;
   bool _unknownOccured;
   TagMap _tagMap;
   std::stack<int> _tagTree;
   std::string _characters;
public:
   /** constructor */
   GenericSAX2Handler();
   /** constructor */
   GenericSAX2Handler(const Tag *tags, int tagNo);
   /** destructor */
   ~GenericSAX2Handler();
   /** returns the information whether an error occured during the parsing */
   bool errorOccured() const;
   /** returns the information whether an unknown tag occured */
   bool unknownOccured() const;
   /** the inherited method called when a new tag opens
       this method calls the user-implemented methof myStartElement */
   void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);
   /** the inherited method called when characters occured
       the characters are appended into a private buffer and given to myCharacters
       when the according tag is being closed */
   void characters(const XMLCh* const chars, const unsigned int length);
   /** the inherited method called when a tag is being closed
       this method calls the user-implemented methods myCharacters and and myEndElement */
   void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
   /** called when ignorable whitespaces occure */
   void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
   /** called when the document shall be resetted */
   virtual void resetDocument();
   /** called when a XML-warning occures */
   virtual void warning(const SAXParseException& exception);
   /** called when a XML-error occures */
   virtual void error(const SAXParseException& exception);
   /** called when a XML-fatal error occures */
   virtual void fatalError(const SAXParseException& exception);
protected:
   /** the user-impemlented handler method for an opening tag 
       this tag is only called when tha tag name was supplied by the user */
   virtual void myStartElement(int element, const std::string &name, const Attributes &attrs) = 0;
   /** the user-implemented handler method for characters 
       this tag is only called when tha tag name was supplied by the user */
   virtual void myCharacters(int element, const std::string &name, const std::string &chars) = 0;
   /** the user-implemented handler method for a closing tag
       this tag is only called when tha tag name was supplied by the user */
   virtual void myEndElement(int element, const std::string &name) = 0;
   /** a dump-methods that may be used to avoid "unused attribute"-warnings */
   inline void myStartElementDump(int element, const std::string &name, const Attributes &attrs);
   /** a dump-methods that may be used to avoid "unused attribute"-warnings */
   inline void myCharactersDump(int element, const char *chars);
   /** a dump-methods that may be used to avoid "unused attribute"-warnings */
   inline void myEndElementDump(int element, const std::string &name);
private:
   int convertTag(const std::string &tag) const;
};


#endif
