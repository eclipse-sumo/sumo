#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    rebuild_cscope.sh
# @author  Jakob Erdmann
# @date    2015
# script for building ctags/cscope index files 
# to be run from the root directory

#export FOXDIR=~/programs/fox-1.6.43
export CSTMP=~/tmp/cscope.files
export CSOUT=cscope.out
export TAGSOUT=tags

cd src
rm $CSTMP
rm $CSOUT
rm $TAGSOUT
#find $FOXDIR/src -name '*.cpp' > $CSTMP
#find $FOXDIR/include -name '*.h' >> $CSTMP
find -name '*.cpp' >> $CSTMP
find -name '*.h' >> $CSTMP
#find ../unittest -name '*.cpp' >> $CSTMP
#find ../unittest -name '*.h' >> $CSTMP
cscope -i $CSTMP -b -f $CSOUT
ctags -L $CSTMP -f $TAGSOUT
