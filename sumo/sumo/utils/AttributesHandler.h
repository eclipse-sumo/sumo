#ifndef AttributesHandler_h
#define AttributesHandler_h

#include <sax2/Attributes.hpp>
#include <string>
#include <map>

class AttributesHandler {
private:
   typedef std::map<int, unsigned short*> TagMap;
   TagMap _tags;
public:
   AttributesHandler();
   ~AttributesHandler();
   void add(int id, const std::string &name);
   bool getBool(const Attributes &attrs, int id) const;
   int getInt(const Attributes &attrs, int id) const;
   std::string getString(const Attributes &attrs, int id) const;
   long getLong(const Attributes &attrs, int id) const;
   float getFloat(const Attributes &attrs, int id) const;
   char *getCharP(const Attributes &attrs, int id) const;
   void check(int id) const;
   unsigned short *convert(const std::string &name) const;
};

#endif
