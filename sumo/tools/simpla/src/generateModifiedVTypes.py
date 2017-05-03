#!/usr/bin/python
"""
@author Leonhard Luecken
@date   2017-04-09
"""

import xml.etree.ElementTree as ET
import os, sys
import optparse

# Usage example
# ./generateModifiedVTypes.py -i AIMsubnet/vtypes.add.xml -a "accel=2 decel=1.5 tau=1.3 sigma=0 lcStrategic=1.0 lcCooperative=0.0 lcSpeedGain=0.3 lcKeepRight=0.3 color=\"0,0.5,0\"" --filter="pkw" -o AIMsubnet/pkwPlatoonLeaderVTypes -s _pLeader
# ./generateModifiedVTypes.py -i AIMsubnet/vtypes.add.xml -a "accel=2.3 decel=1.7 tau=0.3 sigma=0 color=\"0,1,0\"" --filter="pkw" -o AIMsubnet/pkwPlatoonFollowerVTypes -s _pFollower
# ./generateModifiedVTypes.py -i AIMsubnet/vtypes.add.xml -a "accel=3.3 decel=1.7 tau=0.4 sigma=0 color=\"0,1,0.7\"" --filter="pkw" -o AIMsubnet/pkwPlatoonCatchupVTypes -s _pCatchup


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-i", "--infile", dest="infile",
                         help="define the file containing the vType definitions (mandatory)")
    optParser.add_option("-o", "--outprefix", dest="outprefix",
                         help="define the output filenames prefixes.", default="modifiedVTypes")
    optParser.add_option("-a", "--attributes", dest="attributes",
                         help="define the vType attributes to be altered." + \
                         "For example, '--attributes=\"accel=1 tau=0.2\"'")
    optParser.add_option("-s", "--vTypeSuffix", dest="typesuffix",
                         help="define the suffix for the names of the modified vTypes (must be non-empty).", 
                         default="_mod")
    optParser.add_option("-f", "--filter", dest="filterString",
                         help="define a filter (substring to look for in the vType's id) for original type names that shall be considered (other vTypes are skipped).", 
                         default="")
    optParser.add_option("--add-original", action="store_true", dest="addOriginal",
                         help="define whether the output file should contain the original vType definitions as well.")
    (options, args) = optParser.parse_args(args=args)
    
    print str(options)
    
    if options.infile is None:
        print("No infile given. Use '--help' for usage instructions")
        sys.exit(1)
    if options.attributes is None:
        print("No attributes given. Use '--help' for usage instructions")
        sys.exit(1)
    if options.typesuffix == "":
        print("Type suffix must be non-empty. Use '--help' for usage instructions")
        sys.exit(1)
    
    return options


class TagWriter(object):
    def __init__(self, filename):
        self._fn = filename
        self._file = self.open()
        self._indent = ""
        
    def open(self):
        return open(self._fn, "w")
    
    def close(self):
        self._file.close()
    
    def openTag(self, tag, attrs):
        s = self._indent + "<" + tag + " "
        for k,v in attrs.iteritems():
            s += k + '="' + str(v) + '" '
        s += ">\n"
        self._indent = self._indent + 4*" "
        self._file.write(s)
    
    def writeOneLineTag(self, tag, attrs):
        s = self._indent + "<" + tag + " "
        for k,v in attrs.iteritems():
            s += k + '="' + str(v) + '" '
        s += " />\n"
        self._file.write(s)
        
    def closeTag(self, tag):        
        self._indent = self._indent[4:]
        s = self._indent + "</" + tag + ">\n"
        self._file.write(s)
        
    def writeElemList(self, elist):
        for e in elist:
            if len(e.getchildren()) == 0:
                self.writeOneLineTag(e.tag, e.attrib)
            else:
                self.openTag(e.tag, e.attrib)
                self.writeElemList(e.getchildren())
                self.closeTag(e.tag)
        
    

def main(options):
    # xml-infile with vtype defs    
    infile = os.path.realpath(options.infile)
    if not os.path.isfile(infile):
        print ("File '%s' does not refer to an existing file."%infile)
        return False
    
    # xml-outfile
    outfile_xml = os.path.realpath(options.outprefix+".add.xml")
    
    # prefix for selection of vTypes by beginning characters of their id
    filter = options.filterString
    
    # parse attributes
    try:
        attributes = dict([s.split("=") for s in options.attributes.split()])
    except ValueError:
        print("Attributes have to be of the form 'key=value' (enclose string values as \\\"...\\\")")
    for k,v in attributes.iteritems():
        if v[0] in ["'", '"']:
            # string value
            attributes[k] = v.strip("'\"")
        else:
            # numeric value
            attributes[k] = float(v)
    if attributes.has_key("id"):
        attributes.pop("id")
    print("Parsed attributes to modify:\n%s"%str(attributes))
            
    print("Parsing vTypes from file '%s'"%infile)
    it = iter(ET.iterparse(infile, events=("start","end")))
    _, root = it.next()
    
    # store parsed vtype elements (write modified vTypes below)
    vTypeElements = [] 
    vTypeCount = 0
    vTypeTagActive = False
    for event, elem in it:
        if event == "start":
            if vTypeTagActive or elem.tag == "vType":
                # switch on writing
                vTypeTagActive = True
        if event == "end":                         
            if not vTypeTagActive:
                root.clear()
                elem.clear()
            if elem.tag == "vType":
                vTypeTagActive = False
                if elem.attrib["id"].find(filter) >= 0:
                    # filter applies, add vType element
                    vTypeElements.append(elem)
                    vTypeCount += 1
    print("Parsed %d vTypes."%vTypeCount)
    
    # init xml-writer
    tw = TagWriter(outfile_xml)
    tw.openTag(root.tag, root.attrib)    
    # write original vtypes if required
    if options.addOriginal:
        tw.writeElemList(vTypeElements)
    originalVTypeIDs = [e.attrib["id"] for e in vTypeElements]

    # Write modified copies
    for e in vTypeElements:
        # look for each attribute in vType and child attributes (to depth 1!)
        # replace where found and add to vType if not found in childs
        e.attrib["id"] += options.typesuffix
        for k,v in attributes.iteritems():
            replaced = False
            for child in e.getchildren():
                if child.attrib.has_key(k):
                    child.attrib[k] = v
                    replaced = True
            if e.attrib.has_key(k):
                e.attrib[k] = v
                replaced = True
            if not replaced:
                e.attrib[k] = v
                
    tw.writeElemList(vTypeElements)
    tw.closeTag(root.tag)
    tw.close()
    print("Wrote modified vTypes to '%s'."%outfile_xml)
    
    modifiedVTypeIDs = [e.attrib["id"] for e in vTypeElements]
    
    # Generate and save dict for the types
    vtypes = dict(zip(originalVTypeIDs, modifiedVTypeIDs))
    # outfile    
    outfile = os.path.realpath(options.outprefix+".map")
    with open(outfile, "w") as f:
        for k,v in vtypes.iteritems():
            f.write(k+":"+v+"\n")
    print("Wrote vtype-map to '%s'."%outfile)


if __name__ == '__main__':    
    if not main(get_options()):
        sys.exit(1)
