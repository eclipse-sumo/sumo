#ifndef NIVissimLoader_h
#define NIVissimLoader_h

#include <string>
#include <map>
#include <utils/common/FileErrorReporter.h>
#include <utils/common/IntVector.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2D.h>
#include "tempstructs/NIVissimExtendedEdgePoint.h"
#include "NIVissimElements.h"

class OptionsCont;

class NIVissimLoader :
        public FileErrorReporter {
public:
    /// constructor
    NIVissimLoader(const std::string &file);

    /// destructor
    ~NIVissimLoader();

    /// loads the vissim file
    void load(OptionsCont &options);

    bool admitContinue(const std::string &tag);

public:
    class VissimSingleTypeParser :
        public FileErrorReporter::Child {
    public:
        /// Constructor
        VissimSingleTypeParser(NIVissimLoader &parent);

        /// Destructor
        virtual ~VissimSingleTypeParser();

        /** @brief Parses a single data type.
            Returns whether no error occured */
        virtual bool parse(std::istream &from) = 0;

    protected:
        /// reads from the stream and returns the lower case version of the read value
        std::string myRead(std::istream &from);

        /// as myRead, but returns "DATAEND" when the current field has ended
        std::string readEndSecure(std::istream &from,
            const std::string &excl="");

        /// overrides the optional label definition; returns the next tag as done by readEndSecure
        std::string overrideOptionalLabel(std::istream &from,
            const std::string &tag="");

        /// returns the 2d-position saved as next within the stream
        Position2D getPosition2D(std::istream &from);

        /** @brief parses a listof vehicle types assigned to the current data field
            One should remeber, that -1 means "all" vehicle types */
        IntVector parseAssignedVehicleTypes(std::istream &from,
            const std::string &next);

        NIVissimExtendedEdgePoint readExtEdgePointDef(std::istream &from);

        /** @brief Reads the structures name
            We cannot use the "<<" operator, as names may contain more than one word
            which are joined using '"'. */
        std::string readName(std::istream &from);

        /** @brief Overreads the named parameter (if) given and skips the rest until "DATAEND"
         */
        bool skipOverreading(std::istream &from, const std::string &name="");

        /// Reads from the stream until the keywor occures
        void readUntil(std::istream &from, const std::string &name);

    private:
        NIVissimLoader &myVissimParent;

    };


    /// definition of a map from color names to color definitions
    typedef std::map<std::string, RGBColor> ColorMap;

private:
    bool readContents(std::istream &strm);
    void postLoadBuild();
    void buildNBStructures();


    /// adds name-to-id - relationships of known elements into myKnownElements
    void insertKnownElements();

    /// adds id-to-parser - relationships of elements to parse into myParsers
    void buildParsers();

private:
    /// Definition of a map from element names to their numerical representation
    typedef std::map<std::string, NIVissimElement> ToElemIDMap;

    /// Map from element names to their numerical representation
    ToElemIDMap myKnownElements;

    /// Definition of a map from an element's numerical id to his parser
    typedef std::map<NIVissimElement, VissimSingleTypeParser*> ToParserMap;

    /// Parsers by element id
    ToParserMap myParsers;

    /// a map from color names to color definitions
    ColorMap myColorMap;

    std::string myLastSecure;
};


#endif

