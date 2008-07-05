import os

PREFIX = "park" 
DOUBLE_ROWS = 3
ROW_DIST = 20
SLOTS_PER_ROW = 5
SLOT_WIDTH = 5
SLOT_LENGTH = 9

nodes = open("%s.nod.xml" % PREFIX, "w")
print >> nodes, "<nodes>"
edges = open("%s.edg.xml" % PREFIX, "w")
print >> edges, "<edges>"
print >> nodes, '<node id="in" x="-100" y="0"/>' 
print >> edges, '<edge id="mainin" fromnode="in" tonode="main0" nolanes="2" spread_type="center"/>' 
for row in range(DOUBLE_ROWS+1):
    nodeID = "main%s" % row
    x = row * ROW_DIST
    print >> nodes, '<node id="%s" x="%s" y="0"/>' % (nodeID, x) 
    if row > 0:
        print >> edges, '<edge id="main%sto%s" fromnode="main%s" tonode="main%s" nolanes="2" spread_type="center"/>' % (row-1, row, row-1, row) 
    for slot in range(SLOTS_PER_ROW):
        slotID = "row%sslot%s" % (row, slot)
        print >> nodes, '<node id="%s" x="%s" y="%s"/>' % (slotID, x, (slot+1)*SLOT_WIDTH) 
        print >> edges, '<edge id="row%s%sto%s" fromnode="%s" tonode="%s" nolanes="2" spread_type="center"/>' % (row, nodeID, slot, nodeID, slotID)
        if row < DOUBLE_ROWS:
            print >> nodes, '<node id="%sr" x="%s" y="%s"/>' % (slotID, x+SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
            print >> edges, '<edge id="%srp" fromnode="%s" tonode="%sr" spread_type="center"/>' % (slotID, slotID, slotID)
            print >> edges, '<edge id="%srout" fromnode="%sr" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        if row > 0:
            print >> nodes, '<node id="%sl" x="%s" y="%s"/>' % (slotID, x-SLOT_LENGTH, (slot+1)*SLOT_WIDTH) 
            print >> edges, '<edge id="%slout" fromnode="%sl" tonode="%s" spread_type="center"/>' % (slotID, slotID, slotID)
        nodeID = slotID 
print >> nodes, '<node id="out" x="%s" y="0"/>' % (x+100) 
print >> edges, '<edge id="mainout" fromnode="main%s" tonode="out" nolanes="2" spread_type="center"/>' % row 
print >> nodes, "</nodes>"
nodes.close()
print >> edges, "</edges>"
edges.close()

os.system("netconvert -n %s.nod.xml -e %s.edg.xml -o %s.net.xml" % (PREFIX, PREFIX, PREFIX))
