

def write(name, elements):
    fdo = open(name, "w")
    fdo.write('<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/additional_file.xsd">\n')
    for e in elements:
        fdo.write(e.toXML())
        fdo.write("\n")
    fdo.write("</additional>\n")
    fdo.close()
    