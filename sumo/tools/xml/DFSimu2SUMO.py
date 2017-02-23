#!/usr/bin/python

import numpy as np
import sys, getopt
import csv


def postion(item):
    return item['Endkm']


def runEdges(ifile, odir):
    arrData = np.genfromtxt(ifile,
                            delimiter=',',
                            names=True,
                            dtype="S10,S10,float,float,S10,S10,S10,S10,float,float,float,float,S10,S10,S50,S10,S10,S10",
                            skip_header=1,
                            skip_footer=0)
    print(arrData.dtype.names)
    # edgeId =  arrData['Edge']

    dict = {}

    for i in range(0, len(arrData)):
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
    keys.sort(key=lambda k: keyMap[k], reverse=True)
    nodes = {}
    links = {}
    places = {}

    y = 0
    for key in keys:
        x = 0
        track = dict[key]
        # if (len(track))
        print(track)
        if len(track) > 1:
            track = sorted(track, key=postion)

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
                x = frNode.x
            else:
                frNode = lambda: 0
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
                nxt = (frNodeId.decode('UTF-8') + "_to").encode('UTF-8')
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
            link.speed = edge['V_kmh'] / 3.6
            link.pr = 1
            link.name = link.id
            link.length = edge['Edge_length_m']
            links[link.id] = link

            place = edge['Place_End']
            if place != b'':
                places[place] = link.id

        y += 1000

    with open(odir + "/nodes.xml", 'w') as f:
        f.write('<nodes>\n')
        for key in nodes:
            node = nodes[key]
            f.write('\t<node id="%s" x="%s" y="%s"/>\n' % (node.id.decode('UTF-8'), node.x, node.y))

        f.write('</nodes>')

    with open(odir + "/edges.xml", 'w') as f:
        f.write('<edges>\n')
        for key in links:
            link = links[key]
            f.write('\t<edge id="%s" from="%s" to="%s" speed="%s" priority="%s" name="%s" length="%s"/>\n' % (
                link.id.decode('UTF-8'), link.fr.decode('UTF-8'), link.to.decode('UTF-8'), link.speed, link.pr,
                link.name.decode('UTF-8'), link.length))

        f.write('</edges>')

    return places


def runTimeTable(timetable, outputdir, places):
    datafile = open(timetable, 'r')
    datareader = csv.reader(datafile)

    trainTypes = set()
    trainNames = {}
    trains = []
    currentTrain = lambda: 0
    currentTrain.id = ''
    for row in datareader:
        if row[0] == 'Train':
            trainType = row[2]
            trainTypes.add(trainType)
            trainNames[row[1]] = trainType

        if row[0] == 'Timetable':
            link = ''
            place = row[2].encode('UTF-8')
            if place in places.keys():
                link = places[place].decode('UTF-8')
            else:
                link = row[2]

            if currentTrain.id != row[1]:
                currentTrain = lambda: 0
                currentTrain.id = row[1]
                currentTrain.fr = link
                dep = row[4]
                currentTrain.dep = (int(dep[:-3]) * 60 + int(dep[-2:]))*60 - 26160
                trains.append(currentTrain)
            else:
                currentTrain.to = link
                # currentTrain.fr fr=

    f = open(outputdir + "/trips.xml", 'w')
    f.writelines('<?xml version="1.0"?>\n')
    f.writelines(
        '<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">\n')
    for trainType in trainTypes:
        f.writelines('\t<vType id="%s" vClass="rail" carFollowModel="Rail" trainType="%s"/>\n' % (trainType, trainType))

    trains.sort(key = lambda x : x.dep,reverse=False)

    for train in trains:
        fr = train.fr
        to = train.to
        id = train.id
        dep = train.dep
        type = trainNames[id]
        f.writelines(
            '\t<trip id="%s" depart="%s" departLane="best" from="%s" to="%s" type="%s"/>\n' % (id, dep, fr, to, type))
    f.writelines('</routes>')


def main(argv):
    edges = ''
    outputdir = ''
    timetable = ''

    if len(argv) < 4:
        print(
            'test.py -i <path to DFSimu edges csv file> -t <path to DFSimu time table csv file> -o <path to output path>')
        sys.exit(2)
    try:
        opts, args = getopt.getopt(argv, "hi:o:t:", ["ifile=", "ofile=", "tfile="])
    except getopt.GetoptError:
        print('test.py -i <edges> -t <time table> -o <outputdir>')
        sys.exit(3)
    for opt, arg in opts:
        if opt == '-h':
            print('test.py -i <time table> -t <edges> -o <outputdir>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            edges = arg
        elif opt in ("-o", "--odir"):
            outputdir = arg
        elif opt in ("-t", "--tfile"):
            timetable = arg

    print('Input edge file is "', edges)
    print('Input time table file is "', timetable)
    print('Output directory is "', outputdir)
    places = runEdges(edges, outputdir)
    runTimeTable(timetable, outputdir, places)


if __name__ == "__main__":
    main(sys.argv[1:])
