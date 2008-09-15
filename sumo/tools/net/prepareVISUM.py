#!/usr/bin/env python
"""
@file    prepareVISUM.py
@author  Daniel.Krajzewicz@dlr.de
@date    2008-09-01
@version $Id: $


Parses a VISUM-file and writes a modified
 version in "<FILENAME>_mod.net".
Removes all roads which type is not within
 types2import, patches lane number information
 for all other using the values stored in types2import.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""




import sys


types2import = {}
types2import[2] = 1
types2import[10] = 2
types2import[11] = 4
types2import[12] = 3
types2import[13] = 2
types2import[14] = 2
types2import[15] = 3
types2import[16] = 3
types2import[17] = 3
types2import[18] = 3
types2import[19] = 1
types2import[20] = 2
types2import[21] = 3
types2import[22] = 2
types2import[23] = 2
types2import[24] = 1
types2import[25] = 1
types2import[26] = 1
types2import[27] = 2
types2import[28] = 2
types2import[29] = 2
types2import[30] = 3
types2import[31] = 2
types2import[32] = 2
types2import[33] = 2
types2import[34] = 1
types2import[35] = 1
types2import[36] = 1
types2import[37] = 2
types2import[38] = 1
types2import[39] = 3
types2import[40] = 3
types2import[41] = 1
types2import[42] = 1
types2import[43] = 2
types2import[44] = 3
types2import[45] = 2
types2import[46] = 1
types2import[47] = 1
types2import[49] = 3
types2import[50] = 2
types2import[51] = 3
types2import[52] = 3
types2import[53] = 1
types2import[54] = 3
types2import[55] = 3
types2import[56] = 3
types2import[57] = 3
types2import[58] = 3
types2import[59] = 3
types2import[60] = 1
types2import[61] = 1
types2import[62] = 1
types2import[63] = 1
types2import[64] = 1
types2import[65] = 1
types2import[66] = 1
types2import[68] = 2
types2import[69] = 2
types2import[70] = 1
types2import[71] = 2
types2import[72] = 3
types2import[73] = 3
types2import[74] = 3
types2import[75] = 1
types2import[76] = 1
types2import[77] = 3
types2import[78] = 3
types2import[79] = 1
types2import[80] = 1
types2import[81] = 1
types2import[82] = 1
types2import[83] = 1
types2import[84] = 1
types2import[85] = 3
types2import[86] = 3
types2import[87] = 3
types2import[88] = 3
types2import[89] = 3
types2import[90] = 1
types2import[91] = 1
types2import[92] = 1
types2import[93] = 1
types2import[94] = 2
types2import[95] = 1
types2import[96] = 1
types2import[97] = 1
types2import[98] = 2
types2import[99] = 2


fdi = open(sys.argv[1])
fdo = open(sys.argv[1] + "_mod.net", "w")
process = False
skip = False
for line in fdi:
    skip = False
    if line.find("*")==0 or len(line)<2:
        process = False
    if process:
        vals = line.split(";")
        typ = int(vals[5])
        if typ not in types2import:
            skip = True
        else:
            vals[8] = str(types2import[typ])
            line = ";".join(vals)
    if line.find("$STRECKE:")==0:
        process = True
    if not skip:
        fdo.write(line)
fdi.close()
fdo.close()

