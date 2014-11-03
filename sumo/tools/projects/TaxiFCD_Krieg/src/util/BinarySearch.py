# -*- coding: Latin-1 -*-
"""
@file    BinarySearch.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-01
@version $Id$

binary search helper functions.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

def isElmInList(list,elm):   
   maxindex = len(list) - 1   
   index = 0   
   while index <= maxindex:
       middle = index + ((maxindex - index)/2)
       if list[middle] == elm: #if elm is found
           return True
       elif elm < list[middle]:
           maxindex = middle - 1
       else:
           index = middle + 1
   return False

def isElmInListFaster(list,elm):
    """Interpolation search only for integers :-("""   

    links = 0;                  # linke Teilfeldbegrenzung 
    rechts = len(list)- 1;  # rechte Teilfeldbegrenzung
    versch=0;                     # Anzahl verschiedener Elemente
    pos=0;                        # aktuelle Teilungsposition

    # solange der Schlüssel im Bereich liegt (andernfalls ist das gesuchte 
    # Element nicht vorhanden)
    while elm >= list[links] and elm <= list[rechts]: 
        # Aktualisierung der Anzahl der verschiedenen Elemente
        versch = list[rechts] - list[links];
        
        # Berechnung der neuen interpolierten Teilungsposition 
        pos = links + int(((rechts - links+0.0) * (elm - list[links]) / versch));
     
        if elm > list[pos]:             # rechtes Teilintervall 
            links = pos + 1;                       # daten[pos] bereits überprüft
        elif elm < list[pos]:        # linkes Teilintervall
            rechts = pos - 1;                      # daten[pos] bereits überprüft
        else:                                     # Element gefunden
            return True;                          # Position zurückgeben
    return False;                                 # Element nicht gefunden
