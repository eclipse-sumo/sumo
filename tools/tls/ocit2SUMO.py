#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    ocit2SUMO.py
# @author  Jakob Erdmann Schaefer
# @date    2018-09-19
# @version $Id$

"""
Create additional file with tls programs from ocit xml
SUMO link indices must exist as comment (Bemerkung) in each signal group (Signalgruppe)
"""
from __future__ import absolute_import
from __future__ import print_function
import os, sys
from collections import defaultdict
from itertools import groupby

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse

YELLOW_DURATION = 3
REDYELLOW_DURATION = 1
PHASE_DURATION = 100

INDEX_SEPARATOR = ';'
SIGNALGROUPS = "SignalgruppeListe"
SIGNALGROUP = "Signalgruppe"
SHORTID = "BezeichnungKurz"
COMMENTS = "Bemerkungen"
COMMENT = "Bemerkung"
PHASELIST = "PhaseListe"
PHASE = "Phase"
PHASE_ELEMENT = "PhasenElementeintrag"
PHASE_SIGNAL = "Signalbild"
SUCCESSOR_LIST = "PhasenfolgeListe"
SUCCESSOR = "Phasenfolge"
TRANSITION_LIST = "PhasenuebergangListe"
TRANSITION = "Phasenuebergang"
SWITCHING = "Schaltinformation"
SWITCHING_DURATION = "Dauer"
SWITCHING_ELEMENT = "PUeElement"
#<StartSignalbild>gruen</StartSignalbild>
SWITCHING_TIME = "Schaltzeit"
SWITCHING_TIME2 = "Schaltzeitpunkt"
SWITCH_FROM = "VonPhase"
SWITCH_TO = "NachPhase"


SIGNALSTATE_SUMOSTATE = {
        'gruen': 'G',
        'gelb': 'y',
        'rot': 'r',
        'rotgelb': 'u',
        'dunkel': 'O',
        'gelbblk': 'o',
        }

SUMOSTATE_COMPLEX = {
        'GO' : 'G',
        'Or' : 'r',
        'Go' : 'g',
        'Gr' : 'G',
        'GOo' : 'g',
        'GGO' : 'G',
        'OOr' : 'r',
        }

ocit, outfile = sys.argv[1:]

# init signal groups
sgIndex = defaultdict(list) # signal group id -> [linkIndex1, linkIndex2, ...]
sgList = list(parse(ocit, SIGNALGROUPS))[0]
maxIndex = -1
for sg in sgList.getChild(SIGNALGROUP):
    ID = sg.getChild(SHORTID)[0].getText() 
    try:
        comments = sg.getChild(COMMENTS)[0]
        comment = comments.getChild(COMMENT)[0]
        if comment.getText() is not None:
            indices = map(int, comment.getText().split(INDEX_SEPARATOR))
            sgIndex[ID] = indices
            maxIndex = max(maxIndex, *indices)
    except:
        pass
print(sgIndex, maxIndex)
defaultState = [''] * (maxIndex + 1)

# read phases
phases = {} # id -> sumoState
phaseList = list(parse(ocit, PHASELIST))[0]
for phase in phaseList.getChild(PHASE):
    ID = phase.getChild(SHORTID)[0].getText() 
    stateList = list(defaultState)
    for sg in phase.getChild(PHASE_ELEMENT):
        groupID = sg.getChild(SIGNALGROUP)[0].getText()
        groupState = SIGNALSTATE_SUMOSTATE[sg.getChild(PHASE_SIGNAL)[0].getText()]
        for index in sgIndex[groupID]:
            stateList[index] += groupState
    for i, state in enumerate(stateList):
        if state == '':
            stateList[i] = 'O'
        elif len(state) > 1:
            canonical = ''.join(sorted(state))
            stateList[i] = SUMOSTATE_COMPLEX.get(canonical, canonical)
    phases[ID] = ''.join(stateList)

print('\n'.join(map(str,phases.items())))
print()

# read phase transitions
transitions = {} # id -> [sumoState1, sumoState2, ...]
transitionList = list(parse(ocit, TRANSITION_LIST))[0]
for transition in transitionList.getChild(TRANSITION):
    ID = transition.getChild(SHORTID)[0].getText() 
    switching = transition.getChild(SWITCHING)[0]
    duration = int(switching.getChild(SWITCHING_DURATION)[0].getText())
    fromPhase = transition.getChild(SWITCH_FROM)[0].getText()
    toPhase = transition.getChild(SWITCH_TO)[0].getText()
    transitionStates = [['_' + s for s in phases[fromPhase]] for i in range(duration)]
    for sg in switching.getChild(SWITCHING_ELEMENT):
        groupID = sg.getChild(SIGNALGROUP)[0].getText()
        switchObject = sg.getChild(SWITCHING_TIME)[0]
        switchTime = int(switchObject.getChild(SWITCHING_TIME2)[0].getText())
        groupState = SIGNALSTATE_SUMOSTATE[switchObject.getChild(PHASE_SIGNAL)[0].getText()]
        for time in range(switchTime, duration):
            for index in sgIndex[groupID]:
                if transitionStates[time][index][0] == '_':
                    transitionStates[time][index] = groupState
                else:
                    transitionStates[time][index] += groupState
    transitions[(fromPhase, toPhase)] = (ID, transitionStates)
    #print(ID)
    #print("\n".join(map(str,transitionStates)))
print()


# modify transitions 
# - remove '_'
# - translate multi-letter states
for (fromPhase, toPhase), (ID, transition) in transitions.items():
    for stateList in transition:
        for i, state in enumerate(stateList):
            state = state.replace("_", "")
            if len(state) > 1:
                canonical = ''.join(sorted(state))
                state = SUMOSTATE_COMPLEX.get(canonical, canonical)
            stateList[i] = state
    #print(ID)
    #print("\n".join(map(str,transition)))

# - add yellow and red-yellow states
for (fromPhase, toPhase), (ID, transition) in transitions.items():
    statesBefore = phases[fromPhase]
    statesAfter = phases[toPhase]
    for i in range(0, maxIndex):
        stateBefore = statesBefore[i]
        for t in range(0, len(transition)):
            newState = transition[t][i]
            if newState == 'r' and stateBefore in ['g', 'G']:
                for t2 in range(t, t + YELLOW_DURATION):
                    transition[t2][i] = 'y'
                break;
            stateBefore = newState

        stateAfter = statesAfter[i]
        for t in range(len(transition) - 1, -1 , -1):
            oldState = transition[t][i]
            if oldState == 'r' and stateAfter in ['g', 'G']:
                try:
                    transition[t + 1][i] = 'u'
                except:
                    pass
                break;
            stateAfter = oldState

    print(ID)
    print(" ", list(statesBefore), "(before)")
    print("\n".join(["%i %s" % (i, t) for i, t in enumerate(transition)]))
    print(" ", list(statesAfter), "(after)")

# write tls
cycle = sorted(phases.keys())
sumoIndex = {} # phaseID -> sumo phase index
numPhases = 0
with open(outfile, 'w') as outf:
    usedTransitions = set()
    outf.write('<additiona>\n')
    outf.write('   <tlLogic id="fixme" programID="ocit_import">\n')
    for phaseIndex, phaseID in enumerate(cycle):
        outf.write('       <phase duration="%s" state="%s"/> <!-- %s %s -->\n' % (
            PHASE_DURATION, phases[phaseID], numPhases, phaseID))
        sumoIndex[phaseID] = numPhases
        numPhases += 1
        fromTo = (phaseID, cycle[(phaseIndex + 1) % len(cycle)])
        transitionID, transition = transitions[fromTo]
        usedTransitions.add(fromTo)
        groupedTransitions = [(k, list(g)) for k,g  in groupby(transition)]
        for i, (k, g) in enumerate(groupedTransitions):
            try:
                nextPhase = ' next="%s"' % sumoIndex[fromTo[1]] if i == len(groupedTransitions) - 1 else ''
            except:
                nextPhase = ''
            outf.write('       <phase duration="%s"   state="%s"%s/> <!-- %s %s -->\n' % (
                len(g), ''.join(k), nextPhase, numPhases, transitionID))
            numPhases += 1
    for fromTo, (transitionID, transition) in transitions.items():
        if fromTo in usedTransitions:
            continue
        groupedTransitions = [(k, list(g)) for k,g  in groupby(transition)]
        for i, (k, g) in enumerate(groupedTransitions):
            nextPhase = ' next="%s"' % sumoIndex[fromTo[1]] if i == len(groupedTransitions) - 1 else ''
            outf.write('       <phase duration="%s"   state="%s"%s/> <!-- %s %s -->\n' % (
                len(g), ''.join(k), nextPhase, numPhases, transitionID))
            numPhases += 1

    outf.write('   </tlLogi>\n')
    outf.write('</additiona>\n')

# - compactify








