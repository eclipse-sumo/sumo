from xml.dom import minidom

class XmlElement:
    def __init__(self, entity):
        self.tagText = entity.getAttribute('name')
        self.ref = entity.getAttribute('ref')
        self.type = entity.getAttribute('type')
        self.attributes = {}
        self.children = []

    def __repr__(self):
        childList = [c.tagText for c in self.children]
        return "tagText '%s' ref '%s' type '%s' attrs %s %s"  % (self.tagText, self.ref, self.type, self.attributes, childList)

class XsdStructure():
    def __init__(self, xsdFile):        
        xmlDoc = minidom.parse(open(xsdFile))
        self.root = None
        self._namedElements = {}
        for btEntity in xmlDoc.getElementsByTagName('xsd:element'):
            if btEntity.nodeType == 1 and btEntity.hasAttribute('name'):
                el = self.getElementStructure(btEntity, True)
                self._namedElements[el.tagText] = el
                if self.root is None:
                    self.root = el
        self._namedTypes = {}
        for btEntity in xmlDoc.getElementsByTagName('xsd:complexType'):
            if btEntity.nodeType == 1 and btEntity.hasAttribute('name'):
                el = self.getElementStructure(btEntity)
                self._namedTypes[el.tagText] = el
        self.resolveRefs()
        print self._namedElements
        print self._namedTypes

    def getElementStructure(self, entity, checkNestedType=False):
        eleObj = XmlElement(entity)        
        if checkNestedType:
            nestedTypes = entity.getElementsByTagName('xsd:complexType')
            if nestedTypes:
                entity = nestedTypes[0] # remove xsd:complex-tag
        for aa in entity.childNodes:
            if aa.nodeName =='xsd:attribute':
                for attrss in range(aa.attributes.length):
                    if aa.attributes.item(attrss).nodeName == 'name':
                        eleObj.attributes[aa.attributes.item(attrss).value] = None
                        break
            elif aa.nodeName =='xsd:sequence' or aa.nodeName =='xsd:choice':            
                for aae in aa.getElementsByTagName('xsd:element'):
                    seqObj = XmlElement(aae)
                    eleObj.children.append(seqObj)
        return eleObj

    def resolveRefs(self):
        for ele in self._namedElements.itervalues():
            if ele.type and ele.type in self._namedTypes:
                t = self._namedTypes[ele.type]
                ele.attributes = t.attributes
                ele.children = t.children
        for ele in self._namedElements.itervalues():
            newChildren = []
            for child in ele.children:
                if child.ref:
                    newChildren.append(self._namedElements[child.ref])
                elif child.type and child.type in self._namedTypes:
                    t = self._namedTypes[child.type]
                    child.attributes = t.attributes
                    child.children = t.children
                    newChildren.append(child)
            ele.children = newChildren
