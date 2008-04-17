# -*- coding: Latin-1 -*-
"""
@file    GenerateTaxiRoutesMain.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-17

Main of GenerateTaxiRoutes.

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from GenerateTaxiRoutes import *

def main():
    """Main"""
    
    
    print "start program"
    readVLS_Edges()  
    readFCD()
    writeRoutes()    
    print "end"

#start the program
main()