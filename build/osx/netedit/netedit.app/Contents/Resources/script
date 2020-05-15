#!/bin/zsh 

# ****************************************************************************
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
# ****************************************************************************
# @file    sumo-gui-launcher.zsh
# @author  Robert Hilbrich
# @date    Thu, 14 May 2020
#
# Launcher script for OSX application bundle for sumo-gui
# ****************************************************************************/

emulate -LR zsh

# Go through a list of possible config files, which may contain SUMO_HOME declaration
FILE_LIST=('~/.profile' '~/.bashrc' '~/.zshrc')

for FILE in ${FILE_LIST[*]}; do
    if [ -z "$SUMO_HOME" ]; then
        echo "Loading $FILE"
        source $FILE 2> /dev/null
    else
        echo "Found \$SUMO_HOME"
        break
    fi
done

if [ -z "$SUMO_HOME" ]; then
    echo "\$SUMO_HOME not found - aborting"
    exit -1
else
    echo "Starting: $SUMO_HOME/bin/netedit"
    $SUMO_HOME/bin/netedit
fi

