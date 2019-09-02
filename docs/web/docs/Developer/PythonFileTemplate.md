---
title: Developer/PythonFileTemplate
permalink: /Developer/PythonFileTemplate/
---

```py
#!/usr/bin/env python # Leave this one out for non executable python files
# -*- coding: utf-8 -*- # optional encoding line, use if non-ASCII characters are in the code
# Eclipse SUMO, Simulation of Urban MObility; see <https://eclipse.org/sumo>
# Copyright (C) <YEAR OF CREATION>-<CURRENT YEAR> German Aerospace Center (DLR) and others.
# <You can add an additional copyright line for your institution here>
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# <http://www.eclipse.org/legal/epl-v20.html>
# SPDX-License-Identifier: EPL-2.0

# @file    <FILENAME>
# @author  <AUTHOR'S NAME, ONE SEPARATE LINE FOR EACH AUTHOR>
# @author  <AUTHOR'S NAME 2>
# @date    <FILE CREATION DATE>
# @version $Id: $

"""
<A BRIEF DESCRIPTION OF THE FILE'S CONTENTS>

<more documentation including examples>
"""

from __future__ import print_function
from __future__ import absolute_import

import os  # imports one per line
import sys
import ...

# use the following when you depend on sumolib or traci
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
# the noqa is needed to tell the style checker that it is OK to have an import which is not at the top of the file
```