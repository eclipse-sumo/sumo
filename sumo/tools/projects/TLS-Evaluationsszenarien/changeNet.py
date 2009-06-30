import xml.dom.minidom as dom

def changePhaseDurations(file, greenWE, yellowWE, greenNS, yellowNS):

    tree = dom.parse(file)
    
    net = tree.getElementsByTagName('net')[0]
    tlLogic = net.getElementsByTagName('tl-logic')[0]
    phases = tlLogic.getElementsByTagName('phase')
    
    phases[0].setAttribute('duration',str(greenNS))
    phases[1].setAttribute('duration',str(yellowNS))
    phases[2].setAttribute('duration',str(greenWE))
    phases[3].setAttribute('duration',str(yellowWE))
    
    writer = open(file, 'w')
    tree.writexml(writer)
    
# type = static, actuated
def setTLType(file, type):
    tree = dom.parse(file)
    
    net = tree.getElementsByTagName('net')[0]
    tlLogic = net.getElementsByTagName('tl-logic')[0]
    tlLogic.setAttribute('type',type)
    
    writer = open(file, 'w')
    tree.writexml(writer)
    
def setPhaseMinMax(file, minWE, maxWE, minNS, maxNS):
    tree = dom.parse(file)
    
    net = tree.getElementsByTagName('net')[0]
    tlLogic = net.getElementsByTagName('tl-logic')[0]
    phases = tlLogic.getElementsByTagName('phase')
    
    phases[0].setAttribute('duration',str(minNS))
    phases[0].setAttribute('min',str(minNS))
    phases[0].setAttribute('max',str(maxNS))
    phases[2].setAttribute('duration',str(minWE))
    phases[2].setAttribute('min',str(minWE))
    phases[2].setAttribute('max',str(maxWE))
    
    writer = open(file, 'w')
    tree.writexml(writer)
    
def setDetFreq(file, N):
    tree = dom.parse(file)
    
    dets = (tree.getElementsByTagName('additional')[0]).getElementsByTagName('e1-detector')
    
    for det in dets:
        det.setAttribute('freq',str(N))
        
    writer = open(file, 'w')
    tree.writexml(writer)