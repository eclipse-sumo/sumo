from xml.dom import minidom

class XmlElement:
    def __init__(self):
        self.tagText = None
        self.attributes = []
        self.children = []

    def __repr__(self):
        retVal = "tagText: %s"  % (self.tagText )
        for ii in self.children:
            retVal +='\n   '
            retVal += ii.__repr__()

        for ii in self.attributes:
            retVal +='\n   '
            retVal += ii.__repr__()
        return retVal

class XmlMultiElement(XmlElement):
    def __init__(self):
        XmlElement.__init__(self);
        self.ref = None

    def __repr__(self):
        retVal = "tagText: %s"  % (self.tagText )
        retVal += "ref: %s" % self.ref
        return retVal

class XmlAttribute:
    def __init__(self):
        self.name = None
        self.use = None
        self.type = None
        self.value = None

    def __repr__(self):
        retVal = "name: %s, use: %s, type: %s, value: %s"  % (self.name, self.use, 
                                                              self.type, self.value )
        return retVal

class XsdStructure():
    def __init__(self, xsdFile):        
        self.domFile = open(xsdFile)
        self.xmlDoc = minidom.parse(self.domFile)
        self.baseElements = []
        self.populateBaseElements()
        self.resolveRefs()

    def cleanStructure(self, entity):
        for childEle in entity.childNodes:
            if childEle.nodeType == 3 and len(childEle.data.strip()) == 0: # 3 == Textnode
                entity.removeChild(childEle)

    def populateBaseElements(self):
        baseElementsSch = []
        for btEntity in self.xmlDoc.getElementsByTagName('xsd:element'):
            if btEntity.nodeType == 1 and btEntity.hasAttribute('name'):
                baseElementsSch.append(btEntity)

        for bb in baseElementsSch:
            self.baseElements.append(self.getElementStructure(bb))

    def getElementStructure(self, entity):
        eleObj = XmlElement()
        eleObj.tagText = entity.getAttribute('name')
        self.cleanStructure(entity) # remove whitespaces
        
        if len(entity.childNodes) > 1:
            print 'something is wrong with the xsd'

        subEntity=entity.childNodes.item(0) # remove xsd:complex-tag

        for aa in subEntity.childNodes:                ## attributes
            if aa.nodeName =='xsd:attribute':
                atObj = XmlAttribute()
                for attrss in range(aa.attributes.length):
                    if aa.attributes.item(attrss).nodeName == 'name':
                        atObj.name = aa.attributes.item(attrss).value
                    elif aa.attributes.item(attrss).nodeName == 'use':
                        atObj.use = aa.attributes.item(attrss).value
                    elif aa.attributes.item(attrss).nodeName == 'type':
                        atObj.type = aa.attributes.item(attrss).value
                eleObj.attributes.append (atObj)

        ### sequences and choices    
            elif aa.nodeName =='xsd:sequence' or aa.nodeName =='xsd:choice':            
                for aae in aa.getElementsByTagName('xsd:element'):
                    seqObj = XmlMultiElement()

                    for attrss in range(aae.attributes.length):
                        if aae.attributes.item(attrss).nodeName == 'minOccurs':
                            seqObj.minOccurs = aae.attributes.item(attrss).value
                        elif aae.attributes.item(attrss).nodeName == 'maxOccurs':
                            seqObj.maxOccurs = aae.attributes.item(attrss).value
                        elif aae.attributes.item(attrss).nodeName == 'ref':
                            seqObj.ref = aae.attributes.item(attrss).value
                    eleObj.children.append (seqObj)
        return eleObj

    def resolveRefs(self):
        for ele in self.baseElements:
            newChildren = []
            for child in ele.children:
                if isinstance(child, XmlMultiElement):
                    newChildren.append(self.getBaseElement(child.ref))
                else:
                    newChildren.append(child)
            ele.children = newChildren

    def getBaseElement(self, name):
        retVal = None
        for ee in self.baseElements:
            if ee.tagText == name:
                retVal = ee
                break
        return retVal
