# -*- coding: Latin-1 -*-
"""
@file    GenerateTaxiRoutesMain.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-17

Main of GenerateTaxiRoutes.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

from GenerateTaxiRoutes import *

def main():        
    print "start program"
    readFCD()
    writeRoutes()    
    print "end"

#start the program
main()