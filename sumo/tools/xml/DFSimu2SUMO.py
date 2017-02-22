#!/usr/bin/python

import numpy as np
import sys, getopt



def postion(item):
    return item['Endkm']

def run(ifile, odir):
    arrData = np.genfromtxt(ifile,
                            delimiter=',',
                            names=True,
                            dtype="S10,S10,float,float,S10,S10,S10,S10,float,float,float,float,S10,S10,S10,S10,S10,S10",
                            skip_header=1,
                            skip_footer=0)
    print(arrData.dtype.names)
    # edgeId =  arrData['Edge']

    dict = {}

    for i in range(0,len(arrData)):
        track = arrData['Track'][i]
        if not track in dict:
            dict[track] = []
        dict[track].append(arrData[i])


    keyMap = {}

    for key in dict.keys():
        track = dict[key]
        trackW = 0
        for edge in track:
            trackW += edge['Edge_length_m']
        keyMap[key] = trackW

    keys = list(dict.keys())
    keys.sort(key = lambda k: keyMap[k],reverse=True)
    nodes = {}
    links = {}

    y = 0
    for key in keys:
        x = 0
        track = dict[key]
        # if (len(track))
        print(track)
        if len(track) > 1:
            track = sorted(track,key = postion)

        for edge in track:
            frNodeId = edge['Edge']
            prev = edge['PrevEdge']
            if prev == b'':
                prev = edge['Access']
                if prev != b'':
                    frNodeId = prev
            toNodeID = edge['NextEdge']

            frNode = 0
            if frNodeId in nodes:
                frNode = nodes[frNodeId]
            else:
                frNode = lambda:0
                frNode.x = x
                frNode.id = frNodeId
                frNode.y = y
                nodes[frNodeId] = frNode
            x += edge['Edge_length_m']
            nxt = edge['NextEdge']

            if nxt == b'':
                nxtEx = edge['Exit']
                if nxtEx != b'':
                    nxt = nxtEx
                nxt = (frNodeId.decode('UTF-8')+"_to").encode('UTF-8')
                if nxt not in nodes:
                    nxtNode = lambda: 0
                    nxtNode.x = x
                    nxtNode.y = y
                    nxtNode.id = nxt
                    nodes[nxt] = nxtNode
            link = lambda: 0
            link.id = edge['Edge']
            link.fr = frNodeId
            link.to = nxt
            link.speed = edge['V_kmh']/3.6
            link.pr = 1
            link.name = link.id
            links[link.id] = link



        y += 1000

    with open(odir+"/nodes.xml", 'w') as f:
        f.write('<nodes>\n')
        for key in nodes:
            node = nodes[key]
            f.write('\t<node id="%s" x="%s" y="%s"/>\n' % (node.id.decode('UTF-8'), node.x, node.y))

        f.write('</nodes>')

    with open(odir+"/edges.xml", 'w') as f:
        f.write('<edges>\n')
        for key in links:
            link = links[key]
            f.write('\t<edge id="%s" from="%s" to="%s" speed="%s" priority="%s" name="%s"/>\n' % (
                    link.id.decode('UTF-8'), link.fr.decode('UTF-8'), link.to.decode('UTF-8'), link.speed, link.pr, link.name.decode('UTF-8')))

        f.write('</edges>')

def main(argv):
    inputfile = ''
    outputfile = ''

    if len(argv) < 4:
        print('test.py -i <path to DFUSim network csv file> -o <path to output network file>')
        sys.exit(2)
    try:
        opts, args = getopt.getopt(argv, "hi:o:", ["ifile=", "ofile="])
    except getopt.GetoptError:
        print('test.py -i <inputfile> -o <outputdir>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('test.py -i <inputfile> -o <outputdir>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--odir"):
            outputfile = arg
    print('Input file is "', inputfile)
    print('Output directory is "', outputfile)
    run(inputfile, outputfile)


if __name__ == "__main__":
    main(sys.argv[1:])
