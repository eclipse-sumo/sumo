#!/usr/bin/python
outfile = "sumolympicWalks.rou.xml"
startEdge = "beg"
endEdge = "end"
# Startzeit
departTime = 0.
# Startposition
departPos = -30.
# Anzahl Fussgaenger
numberTrips = 200
# Generiere XML Datei
xml_string = "<routes>\n"
for i in range(numberTrips):
    xml_string += '    <person depart="%f" id="p%d">\n' % (departTime, i)
    xml_string += '        <walk departPos="%f" edges="%s %s"/>\n' % (
        departPos, startEdge, endEdge)
    xml_string += '    </person>\n'
xml_string += "</routes>\n"
with open(outfile, "w") as f:
    f.write(xml_string)
